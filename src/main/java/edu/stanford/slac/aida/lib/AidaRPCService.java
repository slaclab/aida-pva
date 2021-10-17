package edu.stanford.slac.aida.lib;

import edu.stanford.slac.aida.lib.model.*;
import edu.stanford.slac.except.*;
import org.epics.nt.NTURI;
import org.epics.pvaccess.server.rpc.RPCRequestException;
import org.epics.pvaccess.server.rpc.RPCService;
import org.epics.pvdata.pv.PVField;
import org.epics.pvdata.pv.PVString;
import org.epics.pvdata.pv.PVStructure;

import java.util.ArrayList;
import java.util.List;

import static edu.stanford.slac.aida.lib.model.AidaType.*;
import static edu.stanford.slac.aida.lib.util.AidaPVHelper.*;
import static org.epics.pvdata.pv.Status.StatusType.ERROR;

public class AidaRPCService implements RPCService {
    private final ChannelProvider aidaChannelProvider;

    public AidaRPCService(ChannelProvider aidaChannelProvider) {
        this.aidaChannelProvider = aidaChannelProvider;
    }

    /**
     * Callback when a channel is called
     *
     * @param pvUri the uri passed to the channel containing the name, query, and arguments
     * @return the result of the call
     * @throws RPCRequestException              if any error occurs formulating the request or decoding the response
     * @throws AidaInternalException            if any error occurs because of an implementation error in aida server code
     * @throws MissingRequiredArgumentException when a required argument was not supplied
     * @throws UnableToGetDataException         when server fails to retrieve data
     * @throws UnableToSetDataException         when server fails to set data
     * @throws UnsupportedChannelException      when server does not yet support the specified channel.
     *                                          Usually caused when channel matches a pattern specified in the channels.yml file
     *                                          but is not yet supported in the service implementation
     */
    public PVStructure request(PVStructure pvUri) throws RPCRequestException, UnableToGetDataException, UnsupportedChannelException, UnableToSetDataException, AidaInternalException, MissingRequiredArgumentException {
        PVStructure retVal;
        try {
            // Check that the parameter is always a normative type
            String type = pvUri.getStructure().getID();
            if (!NTURI.is_a(pvUri.getStructure())) {
                String msg = "Unable to get data, unexpected request type: " + type;
                throw new RPCRequestException(ERROR, msg);
            }

            // Retrieve the PV name
            PVString pvPathField = pvUri.getStringField("path");
            if (pvPathField == null) {
                throw new RPCRequestException(ERROR, "unable to determine the channel from the request specified: " + pvUri);
            }
            String channelName = pvPathField.get();
            if (channelName == null || channelName.length() == 0) {
                throw new RPCRequestException(ERROR, "unable to determine the channel from the request specified: <blank>");
            }

            // Retrieve arguments, if any given to this RPC PV channel.
            PVStructure pvUriQuery = pvUri.getStructureField("query");
            List<AidaArgument> arguments = getArguments(pvUriQuery);

            // Make sure that only one request occurs at a time because
            // the implementations are not thread safe.
            synchronized (this) {
                retVal = request(channelName, arguments);
            }
        } catch (RPCRequestException e) {
            throw e;
        } catch (UnableToGetDataException e) {
            throw new RPCRequestException(ERROR, e.getMessage(), e);
        } catch (UnsupportedChannelException e) {
            throw new RPCRequestException(ERROR, e.getMessage(), e);
        } catch (UnableToSetDataException e) {
            throw new RPCRequestException(ERROR, e.getMessage(), e);
        } catch (AidaInternalException e) {
            throw new RPCRequestException(ERROR, e.getMessage(), e);
        } catch (MissingRequiredArgumentException e) {
            throw new RPCRequestException(ERROR, e.getMessage(), e);
        } catch (Throwable e) {
            throw new RPCRequestException(ERROR, e.getMessage(), e);
        }

        return retVal;
    }

    /**
     * Make request to the specified channel with the uri and arguments specified
     * and return the NT_TABLE of results.
     *
     * @param channelName   channel name
     * @param argumentsList arguments if any
     * @return the structure containing the results.
     * @throws AidaInternalException            if any error occurs because of an implementation error in aida server code
     * @throws MissingRequiredArgumentException when a required argument was not supplied
     * @throws UnableToGetDataException         when server fails to retrieve data
     * @throws UnableToSetDataException         when server fails to set data
     * @throws UnsupportedChannelException      when server does not yet support the specified channel.
     *                                          Usually caused when channel matches a pattern specified in the channels.yml file
     *                                          but is not yet supported in the service implementation
     */
    private PVStructure request(String channelName, List<AidaArgument> argumentsList) throws UnableToGetDataException, UnsupportedChannelException, UnableToSetDataException, AidaInternalException, MissingRequiredArgumentException {
        AidaChannelConfig getterConfig = aidaChannelProvider.getGetterConfig(channelName);
        AidaChannelConfig setterConfig = aidaChannelProvider.getSetterConfig(channelName);

        AidaType aidaGetterType = getterConfig == null ? NONE : getterConfig.getType();
        AidaType aidaSetterType = setterConfig == null ? NONE : setterConfig.getType();

        // Get special arguments `TYPE` and `VALUE` used to determine which APIs will be called
        String typeArgument = null;
        AidaArgument valueArgument = null;
        boolean isSetterRequest = false;
        for (AidaArgument argument : argumentsList) {
            String argumentName = argument.getName();
            if (argumentName.equalsIgnoreCase("TYPE")) {
                typeArgument = argument.getValue().toUpperCase();
            } else if (argumentName.equalsIgnoreCase("VALUE")) {
                valueArgument = argument;
                isSetterRequest = true;
            }
        }

        // See if the request type is supported for the channel
        checkThatRequestedOperationIsSupported(channelName, argumentsList, isSetterRequest, aidaGetterType, aidaSetterType);

        if (typeArgument == null) { // If no TYPE has been specified then see if it was mandatory
            checkIfMissingTypeArgumentIsMandatory(channelName, argumentsList, isSetterRequest, aidaGetterType, aidaSetterType);
        } else { // If a TYPE has been specified then override the Config's type with the specified one if it is compatible
            aidaSetterType = checkIfTypeArgumentIsCompatible(channelName, argumentsList, isSetterRequest, aidaGetterType, aidaSetterType, typeArgument);
        }

        // Verify that, if the request is a table request, the configuration defines fields
        checkThatConfigDefinesFieldsForTableRequests(aidaGetterType, aidaSetterType, getterConfig, setterConfig);

        // Validate that the given arguments are allowed for this operation on this channel
        validateArguments(channelName, argumentsList, isSetterRequest, getterConfig, setterConfig);

        // If channelName contains a service (<service>::channelName) then remove the service part before calling the service implementation
        // This is a special measure put in place to disambiguate some channels that are used by more than one Service.
        // It allows for channels to be prefixed by "<serviceName>::".  This is done in the channel configuration file and also
        // by clients accessing the service.  But the Native Channel Provider will not know about this prefix so we need to
        // remove it before passing the request on to the Native Channel Provider
        channelName = removeServicePrefixIfPresent(channelName);

        // If the client has specified the channel with the legacy formatted channel name we need to change it to the new format before
        // passing it to the Native Channel Provider which will be expecting only new format names
        channelName = ensureNewFormatChannelName(channelName);

        // Display the log entry that indicated the request that is being passed to the Native Channel Provider with its parameters and its expected return type
        logRequest(channelName, argumentsList, isSetterRequest, aidaGetterType, aidaSetterType);

        // Make an arguments object to pass to requests
        AidaArguments arguments = new AidaArguments(argumentsList);

        // Call entry point based on return type
        return callNativeChannelProvider(channelName, arguments, isSetterRequest, aidaGetterType, aidaSetterType, getterConfig, setterConfig);
    }

    /**
     * Call the correct entry point based on the selected channel, the channel configuration, the type of operation requested (get/set)
     *
     * @param channelName     the name of the channel
     * @param arguments       the request's arguments
     * @param isSetterRequest true if this is a set operation
     * @param aidaGetterType  the return type of the getter if this is a get operation
     * @param aidaSetterType  the return type of the setter if this is a set operation
     * @param getterConfig    the getter configuration
     * @param setterConfig    the setter configuration
     * @return the PVStructure returned from the native call
     * @throws UnsupportedChannelException if operation is invalid for channel
     */
    private PVStructure callNativeChannelProvider(String channelName, AidaArguments arguments, boolean isSetterRequest, AidaType aidaGetterType, AidaType aidaSetterType, AidaChannelConfig getterConfig, AidaChannelConfig setterConfig) throws UnsupportedChannelException {
        if (isSetterRequest) {  // Setter requests
            if (aidaSetterType.equals(VOID)) { // Returning VOID
                this.aidaChannelProvider.setValue(channelName, arguments);

            } else if (aidaSetterType.equals(TABLE)) { // Returning TABLES
                return asNtTable(this.aidaChannelProvider.setValueWithResponse(channelName, arguments), setterConfig);
            }

        } else { // Getter requests
            AidaType metaType = aidaGetterType.metaType();

            if (metaType == SCALAR) { // Returning SCALAR
                return asScalar(this.aidaChannelProvider.requestScalar(channelName, arguments, aidaGetterType));

            } else if (metaType == SCALAR_ARRAY) { // Returning SCALAR_ARRAY
                return asScalarArray(this.aidaChannelProvider.requestScalarArray(channelName, arguments, aidaGetterType));

            } else { // Returning TABLE
                return asNtTable(this.aidaChannelProvider.requestTable(channelName, arguments), getterConfig);
            }
        }
        return NT_SCALAR_EMPTY_STRUCTURE;
    }

    /**
     * If channelName contains a service (<service>::channelName) then remove the service part before calling the service implementation
     * This is a special measure put in place to disambiguate some channels that are used by more than one Service.
     * It allows for channels to be prefixed by "<serviceName>::".  This is done in the channel configuration file and also
     * by clients accessing the service.  But the Native Channel Provider will not know about this prefix so we need to
     * remove it before passing the request on to the Native Channel Provider
     *
     * @param channelName the channel name to check for prefix
     * @return the channel name without the prefix if it was present
     */
    private String removeServicePrefixIfPresent(String channelName) {
        int servicePrefix = channelName.indexOf("::");
        if (servicePrefix != -1) {
            channelName = channelName.substring(servicePrefix + 2);
        }
        return channelName;
    }

    /**
     * If the client has specified the channel with the legacy formatted channel name we need to change it to the new format before
     * passing it to the Native Channel Provider which will be expecting only new format names
     *
     * @param channelName the channel name
     * @return the channel name in the new format
     */
    private String ensureNewFormatChannelName(String channelName) {
        int indexOfLastLegacySeparator = channelName.lastIndexOf("//");
        if (indexOfLastLegacySeparator != -1) {
            channelName = channelName.substring(0, indexOfLastLegacySeparator) + ":" + channelName.substring(indexOfLastLegacySeparator + 2);
        }
        return channelName;
    }

    /**
     * See if the request type is supported for the channel
     *
     * @param channelName     the channel name
     * @param argumentsList   the list of arguments
     * @param isSetterRequest is this a set/get operation?
     * @param aidaGetterType  the getter type
     * @param aidaSetterType  the setter type
     * @throws UnsupportedChannelException if the requested operation is not supported
     */
    private void checkThatRequestedOperationIsSupported(String channelName, List<AidaArgument> argumentsList, boolean isSetterRequest, AidaType aidaGetterType, AidaType aidaSetterType) throws UnsupportedChannelException {
        if (isSetterRequest && NONE.equals(aidaSetterType)) {
            throw new UnsupportedChannelTypeException(channelName + argumentsList + ".  Set requests are not supported for this channel");
        } else if (!isSetterRequest && NONE.equals(aidaGetterType)) {
            throw new UnsupportedChannelTypeException(channelName + argumentsList + ".  Get requests are not supported for this channel");
        }
    }

    /**
     * Check that the configuration defines fields for table requests so that we will know how to create the {@link org.epics.pvdata.pv.PVStructure} to return the result in
     *
     * @param aidaGetterType the getter type
     * @param aidaSetterType the setter type
     * @param getterConfig   the getter configuration
     * @param setterConfig   the setter configuration
     * @throws AidaInternalException if the configuration file has been mis-configured
     */
    private void checkThatConfigDefinesFieldsForTableRequests(AidaType aidaGetterType, AidaType aidaSetterType, AidaChannelConfig getterConfig, AidaChannelConfig setterConfig) throws AidaInternalException {
        if (aidaSetterType == TABLE && (setterConfig == null || setterConfig.getFields() == null)) { // Make sure that fields have been defined if the requested return type is TABLE
            throw new AidaInternalException("Table channel configured without defining fields");
        } else if (aidaGetterType == TABLE && getterConfig.getFields() == null) { // Make sure that fields have been defined if the requested return type is TABLE
            throw new AidaInternalException("Table channel configured without defining fields");
        }
    }

    /**
     * Get the {@link AidaType }  of the specified TYPE argument but check that it is compatible with the operation requested on the channel
     *
     * @param channelName     the channel name
     * @param argumentsList   the list of arguments
     * @param isSetterRequest is this a set/get operation?
     * @param aidaGetterType  the getter type
     * @param aidaSetterType  the setter type
     * @param typeArgument    the specified TYPE argument
     * @return the {@link AidaType} of the specified TYPE argument
     * @throws UnsupportedChannelException if not compatible
     */
    private AidaType checkIfTypeArgumentIsCompatible(String channelName, List<AidaArgument> argumentsList, boolean isSetterRequest, AidaType aidaGetterType, AidaType aidaSetterType, String typeArgument) throws UnsupportedChannelException {
        AidaType specifiedAidaType = AidaType.valueOf(typeArgument);

        // Check that it matches the type class from the channels file
        // if channel accepts any type or if the specified type is TABLE (which matches any configuration)
        // or if the specified type is in the class of types allowed by the channel
        if (isSetterRequest) {
            if (!isSpecifiedTypeCompatibleWithConfiguredType(specifiedAidaType, aidaSetterType)) {
                throw new UnsupportedChannelTypeException(channelName + argumentsList + ".  The type specified by the 'Type' parameter must be either VOID or TABLE for setters, but you specified " + specifiedAidaType);
            }
        } else {
            if (!isSpecifiedTypeCompatibleWithConfiguredType(specifiedAidaType, aidaGetterType)) {
                throw new UnsupportedChannelTypeException(channelName + argumentsList + ".  The type specified by the 'Type' parameter must be a " + aidaGetterType + ", but you specified " + specifiedAidaType);
            }
        }
        aidaSetterType = specifiedAidaType;
        return aidaSetterType;
    }

    /**
     * When a TYPE parameter has not been specified, check to see if it was indeed required and raise an exception if so
     *
     * @param channelName     the channel name
     * @param argumentsList   the arguments list
     * @param isSetterRequest is this a set/get request?
     * @param aidaGetterType  the getter type
     * @param aidaSetterType  the setter type
     * @throws UnsupportedChannelException if the TYPE parameter was required but not provided
     */
    private void checkIfMissingTypeArgumentIsMandatory(String channelName, List<AidaArgument> argumentsList, boolean isSetterRequest, AidaType aidaGetterType, AidaType aidaSetterType) throws UnsupportedChannelException {
        if (isSetterRequest) {  // If a meta-type is set as the setter type, but the client request didn't explicitly specify a TYPE argument then error out
            if (aidaSetterType == ANY) {
                throw new UnsupportedChannelTypeException(channelName + argumentsList + ".  The 'Type' parameter must be set 'VOID' or 'TABLE' but you didn't specify one");
            }
        } else { // If a meta-type is set as the getter type, but the client request didn't explicitly specify a TYPE argument then error out
            switch (aidaGetterType) {
                case SCALAR:
                case SCALAR_ARRAY:
                case ANY:
                    throw new UnsupportedChannelTypeException(channelName + argumentsList + ".  The 'Type' parameter must be set to a type corresponding to " + aidaGetterType + ", but you didn't specify one");
            }
        }
    }

    /**
     * Validate arguments:  Check that the given arguments are valid for the operation on the channel that has been requested
     *
     * @param channelName     the channel
     * @param argumentsList   the list of arguments
     * @param isSetterRequest true if this is a setter request
     * @param getterConfig    the getter configuration
     * @param setterConfig    the setter configuration
     * @throws UnsupportedChannelException if any argument is not valid
     */
    private void validateArguments(String channelName, List<AidaArgument> argumentsList, boolean isSetterRequest, AidaChannelConfig getterConfig, AidaChannelConfig setterConfig) throws UnsupportedChannelException {
        for (AidaArgument argument : argumentsList) {
            String argumentName = argument.getName().toUpperCase();

            if (!argumentName.equals("TYPE") && !argumentName.equals("VALUE")) { // Ignore TYPE and VALUE arguments because they are validated elsewhere
                if (isSetterRequest) {
                    if (!setterConfig.getArguments().contains(argumentName)) { // if the given argument name is not in the acceptable set of argument names then error out
                        throw new UnsupportedChannelException(channelName + ":  " + argumentName + " is not a valid argument for set requests to this channel. Valid arguments are: " + setterConfig.getArguments());
                    }
                } else {
                    if (!getterConfig.getArguments().contains(argumentName)) {  // if the given argument name is not in the acceptable set of argument names then error out
                        throw new UnsupportedChannelException(channelName + ":  " + argumentName + " is not a valid argument for get requests to this channel. Valid arguments are: " + getterConfig.getArguments());
                    }
                }
            }
        }
    }

    /**
     * Display the log entry that indicated the request that is being passed to the
     * Native Channel Provider with its parameters and its expected return type
     *
     * @param channelName     the channel name
     * @param argumentsList   the arguments
     * @param isSetterRequest is this a set/get operation
     * @param aidaGetterType  the getter type
     * @param aidaSetterType  the setter type
     */
    private void logRequest(String channelName, List<AidaArgument> argumentsList, boolean isSetterRequest, AidaType aidaGetterType, AidaType aidaSetterType) {
        if (isSetterRequest) {
            String channelSetterType = ntTypeOf(aidaSetterType); // Normative Type
            System.out.println("AIDA SetValue: " + channelName + argumentsList + " => " + aidaSetterType + (channelSetterType == null ? "" : ("::" + channelSetterType)));
        } else {
            String channelGetterType = ntTypeOf(aidaGetterType); // Normative Type
            System.out.println("AIDA GetValue: " + channelName + argumentsList + " => " + aidaGetterType + (channelGetterType == null ? "" : ("::" + channelGetterType)));
        }
    }

    /**
     * Get the arguments for the specified request.  Returns the list of {@link AidaArgument} for the
     * given Normative Type query {@link PVStructure}
     *
     * @param pvUriQuery the given Normative Type query {@link PVStructure}
     * @return the list of {@link AidaArgument}
     * @throws RPCRequestException if there is a problem reading the arguments
     */
    private List<AidaArgument> getArguments(PVStructure pvUriQuery) throws RPCRequestException {
        List<AidaArgument> arguments = new ArrayList<AidaArgument>();
        if (pvUriQuery != null) {
            PVField[] pvFields = pvUriQuery.getPVFields();
            for (PVField field : pvFields) {
                arguments.add(getArgument(field));
            }
        }
        return arguments;
    }

    /**
     * Convert a {@link PVField} into an {@link AidaArgument}
     *
     * @param field the {@link PVField} to convert into an {@link AidaArgument}
     * @return the {@link AidaArgument}
     * @throws RPCRequestException if something bad happens
     */
    private AidaArgument getArgument(PVField field) throws RPCRequestException {
        String name = field.getFieldName();
        if (name == null) {
            throw new RPCRequestException(ERROR, "Invalid argument name: <blank>");
        }

        // To store list of all floating point numbers found in this argument.  They are
        // lists because the arguments can be structures that contain many floating points
        // in various elements.  When extracting those elements the full json path to
        // the element in the structure is saved
        List<FloatArgument> floatArgumentList = new ArrayList<FloatArgument>();
        List<DoubleArgument> doubleArgumentList = new ArrayList<DoubleArgument>();

        // Convert all arguments to string (json or otherwise)
        // But collect all doubles and floats unchanged in ieee format
        String value = fieldToString(field, name, floatArgumentList, doubleArgumentList);

        if (value == null) {
            throw new RPCRequestException(ERROR, "Invalid argument value: <blank>");
        }

        return new AidaArgument(name, value, floatArgumentList, doubleArgumentList);
    }
}
