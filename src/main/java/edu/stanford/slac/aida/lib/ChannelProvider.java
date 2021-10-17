/*
 * Copyright (c) 2004 by Cosylab
 *
 * The full license specifying the redistribution, modification, usage and other
 * rights and obligations is included with the distribution of this project in
 * the file "LICENSE-CAJ". If the license is not included visit Cosylab web site,
 * <http://www.cosylab.com>.
 *
 * THIS SOFTWARE IS PROVIDED AS-IS WITHOUT WARRANTY OF ANY KIND, NOT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY. THE AUTHOR OF THIS SOFTWARE, ASSUMES
 * _NO_ RESPONSIBILITY FOR ANY CONSEQUENCE RESULTING FROM THE USE, MODIFICATION,
 * OR REDISTRIBUTION OF THIS SOFTWARE.
 */

package edu.stanford.slac.aida.lib;

import edu.stanford.slac.aida.lib.model.*;
import edu.stanford.slac.aida.lib.util.AidaPva;
import slac.aida.NativeChannelProvider;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Set;

import static edu.stanford.slac.aida.lib.util.AidaStringUtils.lessStrings;

/**
 * Implementation of a Channel Provider.
 * A Channel Provider is responsible for communicating with the Native Channel Provider
 * to get and receive information from the AIDA-PVA network.
 * <p>
 * This class encodes generic functionality common to all Channel Providers on the AIDA-PVA network.
 * Specific customisations should be coded in the {@link edu.stanford.slac.aida.impl.AidaChannelProvider}
 * class.
 * <p>
 * This class automatically calls the initialisation of the underlying Native Provider
 * and shows the AIDA-PVA banner.
 * <p>
 * It also handles the five main classes of requests routing them to the correct underlying
 * - {@link slac.aida.NativeChannelProvider} entry point:
 * - {@link ChannelProvider#requestScalar(String, AidaArguments, AidaType)}
 * - {@link ChannelProvider#requestScalarArray(String, AidaArguments, AidaType)}
 * - {@link ChannelProvider#requestTable(String, AidaArguments)}
 * - {@link ChannelProvider#setValue(String, AidaArguments)}
 * - {@link ChannelProvider#setValueWithResponse(String, AidaArguments)}
 */
public abstract class ChannelProvider extends NativeChannelProvider {
    private final AidaProvider aidaProvider;

    /**
     * Constructor for the channel provider that reads the configuration from environment variables and
     * configuration files.
     * <p>
     * Uses {@link ChannelProviderFactory#create(ChannelProvider)} to check
     * the `aida.pva.channels.filename` property or the `AIDA_PVA_CHANNELS_FILENAME`
     * environment variable to load the yaml configuration file
     * <p>
     * Lists the abbreviated list of hosted channels to the console.
     * <p>
     * Calls the {@link ChannelProvider#logHostedChannels()}
     */
    public ChannelProvider() {
        // Aida server Banner on standard output
        System.out.println(AidaPva.banner());

        // Create the channel based on environment and configuration files
        this.aidaProvider = ChannelProviderFactory.create(this);

        // Log the configuration of the channel we just created
        logHostedChannels();

        // Call initialisation for the channel provider
        aidaServiceInit();
    }

    /**
     * Handles scalar requests by calling the appropriate Native Method.  This can return :-
     * - scalars e.g. Boolean, Integer, Byte, Long, Float, Double, ...
     *
     * @param channelName request channel name
     * @param arguments   request arguments
     * @return object
     */
    public Object requestScalar(String channelName, AidaArguments arguments, AidaType aidaType) {
        switch (aidaType) {
            case BOOLEAN:
                return aidaRequestBoolean(channelName, arguments);
            case BYTE:
                return aidaRequestByte(channelName, arguments);
            case SHORT:
                return aidaRequestShort(channelName, arguments);
            case INTEGER:
                return aidaRequestInteger(channelName, arguments);
            case LONG:
                return aidaRequestLong(channelName, arguments);
            case FLOAT:
                return aidaRequestFloat(channelName, arguments);
            case DOUBLE:
                return aidaRequestDouble(channelName, arguments);
            case STRING:
                return aidaRequestString(channelName, arguments);
        }
        return null;
    }

    /**
     * Handles scalar array requests by calling the appropriate Native Method.  This can return :-
     * - collections of scalars e.g. List<Boolean>, List<Integer>, List<Byte>, List<Long>, List<Float>, List<Double>,  ...
     *
     * @param channelName request channel name
     * @param arguments   request arguments
     * @param aidaType    the scalar type underpinning this scalar array
     * @return List of scalar object
     */
    public List<?> requestScalarArray(String channelName, AidaArguments arguments, AidaType aidaType) {
        switch (aidaType) {
            case BOOLEAN_ARRAY:
                List<Boolean> tList = new ArrayList<Boolean>();
                for (Boolean b : aidaRequestBooleanArray(channelName, arguments)) {
                    tList.add(b);
                }
                return tList;
            case BYTE_ARRAY:
                List<Byte> bList = new ArrayList<Byte>();
                for (Byte b : aidaRequestByteArray(channelName, arguments)) {
                    bList.add(b);
                }
                return bList;
            case SHORT_ARRAY:
                List<Short> sList = new ArrayList<Short>();
                for (Short s : aidaRequestShortArray(channelName, arguments)) {
                    sList.add(s);
                }
                return sList;
            case INTEGER_ARRAY:
                List<Integer> iList = new ArrayList<Integer>();
                for (Integer i : aidaRequestIntegerArray(channelName, arguments)) {
                    iList.add(i);
                }
                return iList;
            case LONG_ARRAY:
                List<Long> lList = new ArrayList<Long>();
                for (Long l : aidaRequestLongArray(channelName, arguments)) {
                    lList.add(l);
                }
                return lList;
            case FLOAT_ARRAY:
                List<Float> fList = new ArrayList<Float>();
                for (Float f : aidaRequestFloatArray(channelName, arguments)) {
                    fList.add(f);
                }
                return fList;
            case DOUBLE_ARRAY:
                List<Double> dList = new ArrayList<Double>();
                for (Double d : aidaRequestDoubleArray(channelName, arguments)) {
                    dList.add(d);
                }
                return dList;
            case STRING_ARRAY:
                return Arrays.asList(aidaRequestStringArray(channelName, arguments));
        }
        return null;
    }

    /**
     * Handles tables by calling the appropriate Native Method.
     *
     * @param channelName channel name
     * @param arguments   arguments
     * @return return list of lists
     */
    public List<List<Object>> requestTable(String channelName, AidaArguments arguments) {
        return aidaRequestTable(channelName, arguments).asList();
    }

    /**
     * Handles setting values with a void return by calling the appropriate Native Method.
     *
     * @param channelName channel name
     * @param arguments   arguments
     *                    contains {@code value} that specifies the value as a literal string or in json if it is a table
     */
    public void setValue(String channelName, AidaArguments arguments) {
        aidaSetValue(channelName, arguments);
    }

    /**
     * Handles setting values with a void return by calling the appropriate Native Method.
     *
     * @param channelName channel name
     * @param arguments   arguments
     *                    contains {@code value} that specifies the value as a literal string or in json if it is a table
     * @return return list of lists
     */
    public List<List<Object>> setValueWithResponse(String channelName, AidaArguments arguments) {
        return aidaSetValueWithResponse(channelName, arguments).asList();
    }

    /**
     * Get Setter configuration
     *
     * @param channelName the channel name to retrieve configuration
     * @return the configuration
     */
    public AidaChannelConfig getSetterConfig(String channelName) {
        AidaChannel channel = this.aidaProvider.getAidaChannel(channelName);
        return (channel == null) ? null : channel.getSetterConfig();
    }

    /**
     * Get Getter configuration
     *
     * @param channelName the channel name to retrieve configuration
     * @return the configuration
     */
    public AidaChannelConfig getGetterConfig(String channelName) {
        AidaChannel channel = this.aidaProvider.getAidaChannel(channelName);
        return (channel == null) ? null : channel.getGetterConfig();
    }

    /**
     * Get the name of this channel provider
     *
     * @return name of channel provider
     */
    public String getProviderName() {
        return this.aidaProvider.getName();
    }

    /**
     * Get the names of the channels hosted by this channel provider
     *
     * @return channel names
     */
    public Set<String> getChannelNames() {
        return this.aidaProvider.getChannelNames();
    }

    /**
     * Log the list of channels being served
     */
    private void logHostedChannels() {
        System.out.println("\nAIDA-pva Channel Provider : " + getProviderName());
        System.out.println("Channels hosted: \n  " + lessStrings(this.aidaProvider.getChannelNames()) + "\n");
    }
}
