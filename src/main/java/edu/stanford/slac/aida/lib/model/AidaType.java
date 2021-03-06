/*
 * @file
 * This class encapsulates an AIDA-PVA type.
 */
package edu.stanford.slac.aida.lib.model;

import java.util.List;

/**
 * This class encapsulates an AIDA-PVA type.
 * <p>
 * All the data types that can be used in AIDA-PVA are represented here.
 */
public enum AidaType {
    /**
     * API Not valid
     */
    NONE,               ///< API Not valid. Means that this getter or setter is not supported

    /**
     * API functionality supported by another service
     * When used in a config file this requires an accompanying PREFIX tag containing the prefix to append to the request before redirecting
     */
    ALIAS,               ///< The functionality for this API is split between two channel providers depending on the operation.  For the operation where this is specified, the functionality is implemented in a provider with the given prefix

    /**
     * No return value
     */
    VOID,               ///< No return value. Means that this setter does not return a value (only valid for setters)

    /*
     * Scalar return values
     */
    BOOLEAN,            ///< Scalar return value. Getter returns a boolean
    BYTE,               ///< Scalar return value. Getter returns a byte
    SHORT,              ///< Scalar return value. Getter returns a short
    INTEGER,            ///< Scalar return value. Getter returns an integer
    LONG,               ///< Scalar return value. Getter returns a long
    FLOAT,              ///< Scalar return value. Getter returns a float
    DOUBLE,             ///< Scalar return value. Getter returns a double
    STRING,             ///< Scalar return value. Getter returns a string

    /*
     * Scalar Array return values
     */
    BOOLEAN_ARRAY,      ///< Scalar Array return value. Getter returns a boolean array
    BYTE_ARRAY,         ///< Scalar Array return value. Getter returns a byte array
    SHORT_ARRAY,        ///< Scalar Array return value. Getter returns a short array
    INTEGER_ARRAY,      ///< Scalar Array return value. Getter returns an integer array
    LONG_ARRAY,         ///< Scalar Array return value. Getter returns a long array
    FLOAT_ARRAY,        ///< Scalar Array return value. Getter returns a float array
    DOUBLE_ARRAY,       ///< Scalar Array return value. Getter returns a double array
    STRING_ARRAY,       ///< Scalar Array return value. Getter returns a string array

    /*
     * Table Array return values
     */
    TABLE,              ///< Table Array return values. Getter or setter returns a table

    /**
     * Used for channel configuration only.
     * Getter or setter returns any type defined by the mandatory accompanying TYPE argument, for setters this can only be VOID or TABLE
     * Cannot be used as the value of a TYPE parameter.
     *
     * When a channel is this meta-type, all requests for that channel must be presented with TYPE parameter.
     *
     * You may set TABLE as the value of the mandatory TYPE parameter so in the configuration,
     * you must also provide field definitions if the Channel Provider will support TABLE requests.
     */
    ANY,
    /**
     * Used for channel configuration only.
     * Constrains the TYPE parameter to be set to any scalar type or TABLE
     * Cannot be used as the value of a TYPE parameter.
     *
     * When a channel is this meta-type, all requests for that channel must be presented with TYPE parameter.
     *
     * You may set TABLE as the value of the mandatory TYPE parameter so in the configuration,
     * you must also provide field definitions if the Channel Provider will support TABLE requests.
     */
    SCALAR,
    /**
     * Used for channel configuration only.
     * Constrains the TYPE parameter to be set to any scalar array type or TABLE
     * Cannot be used as the value of a TYPE parameter.
     *
     * When a channel is this meta-type, all requests for that channel must be presented with TYPE parameter.
     *
     * You may set TABLE as the value of the mandatory TYPE parameter so in the configuration,
     * you must also provide field definitions if the Channel Provider will support TABLE requests.
     */
    SCALAR_ARRAY;

    /**
     * Given a Object this method will return the corresponding AIDA-PVA type.
     *
     * @param value java object to check
     * @return the corresponding AIDA-PVA type or null if the type is not supported by AIDA-PVA.
     */
    public static AidaType aidaTypeOf(Object value) {
        if (value instanceof Boolean) {
            return BOOLEAN;
        }
        if (value instanceof Byte) {
            return BYTE;
        }
        if (value instanceof Short) {
            return SHORT;
        }
        if (value instanceof Integer) {
            return INTEGER;
        }
        if (value instanceof Long) {
            return LONG;
        }
        if (value instanceof Float) {
            return FLOAT;
        }
        if (value instanceof Double) {
            return DOUBLE;
        }
        if (value instanceof String) {
            return STRING;
        }
        return null;
    }

    /**
     * Given a List of homogenous objects this method will return the corresponding AIDA-PVA type of the List.
     *
     * @param values list of homogenous objects
     * @return the corresponding AIDA-PVA type of the List or null if the type is not supported
     */
    public static AidaType aidaTypeOf(List<?> values) {
        if (values == null || values.isEmpty()) {
            return null;
        }
        return arrayTypeOf(aidaTypeOf(values.get(0)));
    }

    /**
     * Get the meta-type of this aida type
     *
     * @return the meta type SCALAR, SCALAR_ARRAY, or AHY
     */
    public AidaType metaType() {
        switch (this) {
            case BOOLEAN:
            case BYTE:
            case DOUBLE:
            case FLOAT:
            case INTEGER:
            case LONG:
            case SHORT:
            case STRING:
                return SCALAR;
            case BYTE_ARRAY:
            case BOOLEAN_ARRAY:
            case DOUBLE_ARRAY:
            case FLOAT_ARRAY:
            case INTEGER_ARRAY:
            case LONG_ARRAY:
            case SHORT_ARRAY:
            case STRING_ARRAY:
                return SCALAR_ARRAY;
            case TABLE:
            default:
                return ANY;
        }
    }

    /**
     * Return the corresponding array type for the given scalar type
     *
     * @param aidaType the given scalar type
     * @return the corresponding array type
     */
    private static AidaType arrayTypeOf(AidaType aidaType) {
        switch (aidaType) {
            case BOOLEAN:
                return BOOLEAN_ARRAY;
            case BYTE:
                return BYTE_ARRAY;
            case DOUBLE:
                return DOUBLE_ARRAY;
            case FLOAT:
                return FLOAT_ARRAY;
            case INTEGER:
                return INTEGER_ARRAY;
            case LONG:
                return LONG_ARRAY;
            case SHORT:
                return SHORT_ARRAY;
            case STRING:
                return STRING_ARRAY;
            case TABLE:
            default:
                return null;
        }
    }

    /**
     * Check if the specified type is compatible with the configured type.
     *
     * @param specifiedAidaType the specified type
     * @param configuredType    the configured type
     * @return true if compatible false if not
     */
    public static boolean isSpecifiedTypeCompatibleWithConfiguredType(AidaType specifiedAidaType, AidaType configuredType) {
        if (configuredType == ANY) {
            return true;
        } else if (configuredType == SCALAR || configuredType == SCALAR_ARRAY) {
            return specifiedAidaType == TABLE || specifiedAidaType.metaType() == configuredType;
        } else
            return specifiedAidaType == configuredType;
    }
}
