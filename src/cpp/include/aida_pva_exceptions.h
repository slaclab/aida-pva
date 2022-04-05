/** @file
 *  @brief The Header File for the exceptions helper MACROS.
 *       **CMS**=C_INC
 */
#ifndef aida_pva_exceptions_h
#define aida_pva_exceptions_h
#ifdef __cplusplus
extern "C" {
#endif

#include "aida_pva.h"
#include "errtranslate.h"

// Supported exceptions
/**
 * Use this string to signal Internal Exceptions in aidaThrow()
 */
#define AIDA_INTERNAL_EXCEPTION "AidaInternalException"
/**
 * Use this string to signal Server Initialisation Exceptions in aidaThrow()
 */
#define SERVER_INITIALISATION_EXCEPTION "ServerInitialisationException"
/**
 * Use this string to signal Exceptions when trying to Get Data in aidaThrow()
 */
#define UNABLE_TO_GET_DATA_EXCEPTION "UnableToGetDataException"
/**
 * Use this string to signal Exceptions when trying to Set Data in aidaThrow()
 */
#define UNABLE_TO_SET_DATA_EXCEPTION "UnableToSetDataException"
/**
 * Use this string to signal Unsupported Channel Exceptions in aidaThrow()
 */
#define UNSUPPORTED_CHANNEL_EXCEPTION "UnsupportedChannelException"
/**
 * Use this string to signal Missing Required Argument Exceptions in aidaThrow()
 */
#define MISSING_REQUIRED_ARGUMENT_EXCEPTION "MissingRequiredArgumentException"

/**
 * The maximum size of the error text message in an aidaThrow() call
 */
#define MAX_ERROR_TEXT_LEN 100

/**
 * Check to see if an exception has been raised,
 * and return the given return value.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @param _r the specified return value.
 * @return This MACRO will return the specified return value from your function if there has been an exception.
 */
#define ON_EXCEPTION_RETURN_(_r) \
    if ((*env)->ExceptionCheck(env)) { \
        return _r; \
    }

/**
 * Check to see if an exception has been raised,
 * then free tracked memory,
 * and return the given return value.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @param _r	the specified return value.
 * @return This MACRO will return the specified return value from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_MEMORY_AND_RETURN_(_r) \
    if ((*env)->ExceptionCheck(env)) { \
        FREE_MEMORY \
        return _r; \
    }

/**
 * Check to see if an exception has been raised,
 * and return void,
 *
 * Uses local variable `env` for checking exception status.
 *
 * @return This MACRO will return from your function if there has been an exception.
 */
#define ON_EXCEPTION_RETURN_VOID \
    if ((*env)->ExceptionCheck(env)) { \
        return; \
    }

/**
 * Check to see if an exception has been raised,
 * free local variable pv if set,
 * free local variable {@link Arguments}
 * and return the given return value.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @param _r the specified return value.
 * @return This MACRO will return the specified return value from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_PV_AND_ARGUMENTS_AND_RETURN_(_r) \
    if ((*env)->ExceptionCheck(env)) { \
        releasePvAndArguments(env, uri, pv, arguments); \
        return _r; \
    }

/**
 * Check to see if an exception has been raised,
 * free local variable pv if set,
 * free local variable {@link Arguments}
 * and return void,
 *
 * Uses local variable `env` for checking exception status.
 *
 * @return This MACRO will return from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_PV_AND_ARGUMENTS_AND_RETURN_VOID \
    if ((*env)->ExceptionCheck(env)) { \
        releasePvAndArguments(env, uri, pv, arguments); \
        return; \
    }

/**
 * Check to see if an exception has been raised,
 * free a local variable `string`,
 * free local variable pv if set,
 * free local variable {@link Arguments}
 * and return the given return value.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @param _r the specified return value.
 * @return This MACRO will return the specified return value from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_STRING_AND_PV_AND_ARGS_AND_RETURN_(_r) \
    if ((*env)->ExceptionCheck(env)) { \
        if ( string ) free(string); \
        releasePvAndArguments(env, uri, pv, arguments); \
        return _r; \
    }

/**
 * Check to see if an exception has been raised,
 * free a local variable `string`,
 * and return the given return value.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @param _r the specified return value.
 * @return This MACRO will return the specified return value from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_STRING_AND_RETURN_(_r) \
    if ((*env)->ExceptionCheck(env)) { \
        if ( string ) free(string); \
        return _r; \
    }

/**
 * Check to see if an exception has been raised,
 * free local variable {@link Array},
 * free local variable pv if set,
 * free local variable {@link Arguments}
 * and return the given return value.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @param _r the specified return value.
 * @return This MACRO will return the specified return value from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_ARRAY_AND_PV_AND_ARGS_AND_RETURN_(_r) \
    if ((*env)->ExceptionCheck(env)) { \
        releaseArray(array); \
        releasePvAndArguments(env, uri, pv, arguments); \
        return _r; \
    }

/**
 * Check to see if an exception has been raised,
 * free local variable {@link Array},
 * free local variable pv if set,
 * free local variable {@link Arguments}
 * and return void.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @return This MACRO will return from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_STRING_ARRAY_AND_PV_AND_ARGS_AND_RETURN_NULL \
    if ((*env)->ExceptionCheck(env)) { \
        releaseStringArray(array); \
        releasePvAndArguments(env, uri, pv, arguments); \
        return NULL; \
    }

/**
 * Check to see if an exception has been raised,
 * free local variable {@link Table},
 * free local variable pv if set,
 * free local variable {@link Arguments},
 * and return the given return value.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @param _r the specified return value.
 * @return This MACRO will return the specified return value from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_TABLE_AND_PV_AND_ARGS_AND_RETURN_(_r) \
    if ((*env)->ExceptionCheck(env)) { \
        releaseTable(table); \
        releasePvAndArguments(env, uri, pv, arguments); \
        return _r; \
    }

/**
 * Check to see if an exception has been raised,
 * free local variable `value`,
 * free local variable pv if set,
 * free local variable {@link Arguments}
 * and return void.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @return This MACRO will return from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_VALUE_AND_PV_AND_ARGS_AND_RETURN_VOID \
    if ((*env)->ExceptionCheck(env)) { \
        releaseValue(value); \
        releasePvAndArguments(env, uri, pv, arguments); \
        return; \
    }

/**
 * Check to see if an exception has been raised,
 * free local variable `value`,
 * free local variable pv if set,
 * free local variable {@link Arguments},
 * and return the given return value.
 *
 * Uses local variable `env` for checking exception status.
 *
 * @param _r the specified return value.
 * @return This MACRO will return the specified return value from your function if there has been an exception.
 */
#define ON_EXCEPTION_FREE_VALUE_AND_PV_AND_ARGS_AND_RETURN_(_r) \
    if ((*env)->ExceptionCheck(env)) { \
        releaseValue(value); \
        releasePvAndArguments(env, uri, pv, arguments); \
        return _r; \
    }

/**
 * Format an error message, throw it in an exception, free any allocated memory and return the error code.
 *
 * @param _exception exception to raise (string)
 * @param _errorText the text of the error to raise
 * @param _ref a string that will be substituted in message with %s
 * @param _r the specified return value
 * @return This MACRO will return the specified return value from your function
 */
#define SPRINTF_ERROR_FREE_MEMORY_AND_RETURN_(_exception, _errorText, _ref, _r) \
{ \
    char error[MAX_ERROR_TEXT_LEN + strlen(_ref)]; \
    sprintf(error, _errorText,  _ref); \
    aidaThrowNonOsException(env, _exception, error); \
    FREE_MEMORY \
    return _r; \
}

/**
 * Throw error message in an exception, free any allocated memory and return the error code.
 *
 * @param _exception exception to raise (string)
 * @param _errorText the text of the error to raise
 * @param _r the specified return value
 * @return This MACRO will return the specified return value from your function
 */
#define PRINT_ERROR_FREE_MEMORY_AND_RETURN_(_exception, _errorText, _r) \
{ \
    aidaThrowNonOsException(env, _exception, _errorText); \
    FREE_MEMORY \
    return _r; \
}

/**
 * Throw unsupported channel exception and return a blank array.
 */
#define UNSUPPORTED_ARRAY_REQUEST \
    aidaThrowNonOsException(env, UNSUPPORTED_CHANNEL_EXCEPTION, uri); \
    Array array; \
    return array;

/**
 * Throw an unsupported channel exception and return an empty string array.
 */
#define UNSUPPORTED_STRING_ARRAY_REQUEST \
    aidaThrowNonOsException(env, UNSUPPORTED_CHANNEL_EXCEPTION, uri); \
    StringArray stringArray; \
    return stringArray;

/**
 * Throw an unsupported channel exception and return an empty table.
 */
#define UNSUPPORTED_TABLE_REQUEST \
    aidaThrowNonOsException(env, UNSUPPORTED_CHANNEL_EXCEPTION, uri); \
    Table table; \
    return table;

#ifdef __cplusplus
}
#endif
#endif

