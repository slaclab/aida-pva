/** @file
 *  @brief The Header File for the AIDA-PVA Module functions.
 *       **CMS**=C_INC
 */
#ifndef aida_pva_h
#define aida_pva_h
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>                /* isalnum, ispunct */

#include <stsdef.h>               /* Macros for handling VMS status */
#include <stdbool.h>
#include <stsdef.h>               /* Macros for handling VMS status */
#include <jni.h>

#include "slc_macros.h"
#include "msg_proto.h"            /* for standalone_init */
#include "errtranslate.h"
#include "ref.h"                  /* passing by reference macros */
#include "process_parm.h"

#include "aida_pva_json.h"
#include "aida_pva_types.h"
#include "aida_pva_exceptions.h"
#include "aida_pva_api.h"
#include "aida_pva_convert.h"
#include "aida_pva_memory.h"
#include "aida_pva_uri.h"

/////////////////////////////////
/// Initialisation
/////////////////////////////////

/**
 * Call standalone_init() and set development mode flag.
 * @param processName the name of the process being initialised
 * @param initMessageServices boolean to determine if the message service needs to be initialised
 * @return vms status code
 */
vmsstat_t init(const char* processName, bool initMessageServices);

/////////////////////////////////
/// Exception Handling
/////////////////////////////////

/**
 * To log any exceptions and throw back to java.
 *
 * The #exception is formatted in a standard way using the VMS status code and its associated message
 * and the optionally supplied #message.
 * The #exception is always assumed to be from the edu.stanford.slac.except package
 *
 * @param env the JNI environment.  Used in all functions involving JNI
 * @param status the VMS status code to luck up and display text for.
 * @param exception the string representation of the exception class to throw.
 * @param message the optional message to attach to the exception.  If NULL it is ignored.
 */
void aidaThrow(JNIEnv* env, vmsstat_t status, char* exception, const char* message);

/**
 * To log any non-OS exceptions and throw back to java.
 *
 * The #exception is formatted in a standard way with the optionally supplied #message.
 * The #exception is always assumed to be from the edu.stanford.slac.except package
 *
 * @param env the JNI environment.  Used in all functions involving JNI
 * @param exception the string representation of the exception class to throw.
 * @param message the optional message to attach to the exception.  If NULL it is ignored.
 */
void aidaThrowNonOsException(JNIEnv* env, char* exception, const char* message);

/////////////////////////////////
/// String Handling
/////////////////////////////////

/**
 * Check if a string @p str, ends with another string @p suffix.
 *
 * @param str the string to check.
 * @param suffix the @p suffix to look for at the end of @p str
 * @return `true` if @p str ends with @p suffix.  `false`
 */
int endsWith(const char* str, char* suffix);

/**
 * Check if a string starts with another string.
 *
 * @param str
 * @param prefix
 * @returns true if string starts with prefix
 */
int startsWith(const char* str, char* prefix);

/////////////////////////////////
/// URI Handling for group, secn, pmu and slacName
/////////////////////////////////

/**
 * Get the Display group name from a URI.
 *
 * @param groupName pre-allocated space to store the group name
 * @param uri the new format AIDA PV name
 * @return EXIT_SUCCESS if all goes well EXIT_FAILURE otherwise
 */
int groupNameFromUri(char groupName[], const char* uri);

/**
 * Get secondary from pseudo secondary (containing a colon) number from URI.
 *  e.g. `BD01:BEND:BDES`  => `BEND` as int4u
 *
 * @param uri the new format AIDA PV name
 * @param secn pointer to an int to store the secondary as a number
 */
void secnFromUri(const char* uri, int4u* secn);

/**
 * Get secondary from URI.  Just points into the URI so don't go messing with it.
 *
 * @param uri the new format AIDA PV name
 */
const char* secondaryFromUri(const char* uri);

/**
 * Get primary, micro and unit from a device name.
 *
 * @param env the JNI environment.  Used in all functions involving JNI
 * @param device pre-allocated space to store the device
 * @param primary pre-allocated space to store the primary
 * @param micro pre-allocated space to store the micro
 * @param unit pre-allocated space to store the unit
 */
int pmuFromDeviceName(JNIEnv* env, char* device, char* primary, char* micro, int4u* unit);

/**
 * Get the pmu part of a URI.
 *
 * @param pmuString the pre-allocated space to store the pmu string
 * @param uri the new format AIDA PV name
 */
void pmuStringFromUri(char* pmuString, const char* uri);

/**
 * Convert all URIs to slac names before making queries.
 *
 * @param slcName pointer to space to store the SLC name
 * @param uri the new format AIDA PV name
 */
void uriToSlcName(char slcName[MAX_URI_LEN], const char* uri);

/**
 * Convert the given URI to the legacy AIDA name for low level functions that still require it that way.
 *
 * @param legacyName pointer to space to store the legacy AIDA name
 * @param uri the new format AIDA PV name
 */
void uriLegacyName(char legacyName[MAX_URI_LEN], const char* uri);

/////////////////////////////////
/// Memory Handling
/////////////////////////////////
/**
 * Allocate memory and copy the source to it if specified.  If the null terminate flag is set
 * null terminate the allocate space, at the last position
 *
 * @param env to be used to throw exceptions using aidaThrow() and aidaThrowNonOsException()
 * @param source source of data to copy to newly allocated space, NULL to not copy
 * @param size the amount of space to allocate
 * @param nullTerminate true to null terminate
 * @param message the message to display if anything goes wrong
 * @return the allocated memory
 */
void* allocateMemory(JNIEnv* env, void* source, size_t size, bool nullTerminate, char* message);

/**
 * Free up any memory allocated for the given pv and arguments.
 *
 * @param arguments the given arguments
 */
void releasePvAndArguments(JNIEnv* env, jstring uri, const char* pv, Arguments arguments);

/**
 * Free up any memory allocated the given scalar array.
 *
 * @param array the given scalar array
 */
void releaseArray(Array array);

/**
 * Free up any memory allocated for string arrays.
 *
 * @param array
 */
void releaseStringArray(StringArray array);

/**
 * Free up any memory allocated for the given table.
 *
 * @param table the given tables
 */
void releaseTable(Table table);

/**
 * Release all allocated memory in the given value.
 *
 * @param value the given value'
 */
void releaseValue(Value value);


/////////////////////////////////
/// Table Handling
/////////////////////////////////

/**
 * Make a Table for return to client.  This is the first call that needs to be made to return a Table.
 * This will create a Table with the specified the number of rows and columns.
 * You need to call tableAddColumn(), tableAddStringColumn(), or any of the
 * other special `tableAdd` functions to add
 * columns to the Table before returning it.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param rows           the number of rows to create the Table with.
 * @param columns        the number of columns to create the Table with,
 * @return the newly created Table
 *
 * @see
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 *
 * @paragraph Example
 *
 * Create a two column, two row Table, add data, and return.
 * @code
 * int rows = 2, columns = 2;
 * float xData[rows] = { 1.0f, 2.0f }, yData[rows] = { 7.0f, 8.0f };
 *
 * Table table = tableCreate(env, rows, columns);
 * ON_EXCEPTION_AND_RETURN_(table)
 * tableAddColumn(env, &table, AIDA_FLOAT_TYPE, xData, true);
 * ON_EXCEPTION_AND_RETURN_(table)
 * tableAddColumn(env, &table, AIDA_FLOAT_TYPE, yData, true);
 * ON_EXCEPTION_AND_RETURN_(table)
 * return table;
 * @endcode
 * @note
 * You need to call ON_EXCEPTION_AND_RETURN_(table) after each call to make
 * sure that no exception was raised.
 */
Table tableCreate(JNIEnv* env, int rows, int columns);

/**
 * Add a column of arbitrary type to a Table.  Add the given data to the
 * column assuming that the data has a number of rows that corresponds to the
 * Table's rowCount.  Memory will be allocated for the data of the column so
 * the @p data buffer presented can be freed up after calling this function.
 *
 * @note
 * Don't call this to add strings to the Table.  Use tableAddStringColumn() for that.
 *
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          the Table to add the column to.
 * @param type           the type of this Table column.
 * @param data           the data to add to this column, a buffer of `sizeof(type) * table->rowCount` size.
 * @param ieeeFormat     true if the data provided is already in ieee format.  If the data is not in ieee format,
 *        usually because it has been retrieved from some backend system, this function will convert it to ieee format
 *        unless this parameter is set to true.
 *
 * @see
 * tableCreate(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 *
 * @paragraph Example
 *
 * Create a two column, two row Table, add data, and return.
 * @code
 * int rows = 2, columns = 2;
 * float xData[rows] = { 1.0f, 2.0f }, yData[rows] = { 7.0f, 8.0f };
 *
 * Table table = tableCreate(env, rows, columns);
 * ON_EXCEPTION_AND_RETURN_(table)
 * tableAddColumn(env, &table, AIDA_FLOAT_TYPE, xData, true);
 * ON_EXCEPTION_AND_RETURN_(table)
 * tableAddColumn(env, &table, AIDA_FLOAT_TYPE, yData, true);
 * ON_EXCEPTION_AND_RETURN_(table)
 * return table;
 * @endcode
 * @note
 * You need to call ON_EXCEPTION_AND_RETURN_(table) after each call to make
 * sure that no exception was raised.
 */
void tableAddColumn(JNIEnv* env, Table* table, Type type, void* data, bool ieeeFormat);

/**
 * Add a String column to the given Table.
 * This reads data from a buffer that is itself a list of pointers to strings.
 * We allocate just enough space to store the strings in our Table.  This is allocated
 * in one buffer so there is only one pointer to release.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          the Table to add the string column to.
 * @param data           the data to add to this column, a buffer of `sizeof(char *) * table->rowCount` size.
 *     This will contain the strings to be added to the Table.
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 *
 * @paragraph Example
 *
 * Create a single column, one row Table, add data, and return.
 * @code
 * int rows = 1, columns = 1;
 * char* namesData[rows];
 * namesData[0] = "NAME";
 *
 * Table table = tableCreate(env, rows, columns);
 * ON_EXCEPTION_AND_RETURN_(table)
 * tableAddStringColumn(env, &table, namesData);
 * ON_EXCEPTION_AND_RETURN_(table)
 * return table;
 * @endcode
 * @note
 * You need to call ON_EXCEPTION_AND_RETURN_(table) after each call to make
 * sure that no exception was raised.
 */
void tableAddStringColumn(JNIEnv* env, Table* table, char** data);

/**
 * Add fixed-width string data to a column in the given Table.
 * This reads data from an allocated space that is rows * width with each string occupying width characters
 * Though the strings are null terminated if there is space, there is no guarantee so an exact number of
 * bytes is copied.  Each string in the Table is allocated maximally.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          The Table to add the column to.
 * @param data           The data to add to this column, a pointer to `char` buffer containing
 *                       the fixed length strings.  The strings are arranged in
 *                       contiguous blocks @p width long.
 * @param width 		 the width of the strings
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 */
void tableAddFixedWidthStringColumn(JNIEnv* env, Table* table, char* data, int width);

/**
 * Add a boolean column to a Table with only one row.  This function will allocate
 * the required memory for the single `unsigned char` that is required.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          The Table to add the column to.
 * @param data           The data to add to this column, a pointer to an `unsigned char`.
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 */
void tableAddSingleRowBooleanColumn(JNIEnv* env, Table* table, unsigned char data);

/**
 * Add a byte column to a Table with only one row.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          The Table to add the column to.
 * @param data           The data to add to this column, an `unsigned char`.
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 */
void tableAddSingleRowByteColumn(JNIEnv* env, Table* table, unsigned char data);

/**
 * Add a short column to a Table with only one row.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          The Table to add the column to.
 * @param data           The data to add to this column, a `short`.
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 */
void tableAddSingleRowShortColumn(JNIEnv* env, Table* table, short data);

/**
 * Add a integer column to a Table with only one row.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          the Table to add the column to.
 * @param data           the data to add to this column, an `int`
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 */
void tableAddSingleRowIntegerColumn(JNIEnv* env, Table* table, int data);

/**
 * Add a long column to a Table with only one row.
 *
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          the Table to add the column to.
 * @param data           the data to add to this column.  A `long` value
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 */
void tableAddSingleRowLongColumn(JNIEnv* env, Table* table, long data);

/**
 * Add a float column to a Table with only one row.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          The Table to add the column to.
 * @param data           The data to add to this column, a `float`.
 * @param ieeeFloat	     True if the data is in ieee format, otherwise the given floating point
 * 						 number is converted from VMS to ieee format.
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowDoubleColumn(),
 * tableAddSingleRowStringColumn(),
 */
void tableAddSingleRowFloatColumn(JNIEnv* env, Table* table, float data, bool ieeeFloat);

/**
 * Add a double column to a Table with only one row.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          The Table to add the column to.
 * @param data           The data to add to this column, a `double`.
 * @param ieeeDouble	 True if the data is in ieee format, otherwise the double precision floating point
 * 						 number is converted from VMS to ieee format.
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowStringColumn(),
 */
void tableAddSingleRowDoubleColumn(JNIEnv* env, Table* table, double data, bool ieeeDouble);

/**
 * Add a string column to a Table with only one row.  This is a shortcut
 * Table function that simplifies adding a string to a Table with only one row.
 *
 * @note
 * The framework will release all memory associated with
 * a Table when you return from your API implementation.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI.
 * @param table          the Table to add the column to.
 * @param data           the data to add to this column.  A single string.
 *
 * @see
 * tableCreate(),
 * tableAddColumn(),
 * tableAddStringColumn()
 * tableAddFixedWidthStringColumn(),
 * tableAddSingleRowBooleanColumn(),
 * tableAddSingleRowByteColumn(),
 * tableAddSingleRowShortColumn(),
 * tableAddSingleRowIntegerColumn(),
 * tableAddSingleRowLongColumn(),
 * tableAddSingleRowFloatColumn(),
 * tableAddSingleRowDoubleColumn(),
 */
void tableAddSingleRowStringColumn(JNIEnv* env, Table* table, char* data);

/////////////////////////////////
/// Argument Processing
/////////////////////////////////

/**
 * ascanf(), avscanf()
 *
 * @paragraph Synopsis
 * @code
 *     int ascanf(Arguments *arguments, const char *format, ...);
 *     int avscanf(Arguments *arguments, Value *value, const char *format, ...);
 * @endcode
 *
 * @paragraph Details
 * Reads data from the given @p arguments and stores them according
 * to parameter format into the locations given by the additional arguments,
 * as if scanf() was used, but reading from arguments instead of the standard input (stdin).
 *
 * @see avscanf() for full details.
 *
 * @param env            The JNI environment.  Used in all functions involving JNI
 * @param arguments      Arguments that the function processes as its source to retrieve the data.
 * @param formatString   C string that contains a format string as described above
 * @param ...            Depending on the format string, the function may expect a sequence of additional arguments,
 * 						 containing pairs of names and pointers to allocated storage (except as indicated above),
 * 						 where the interpretation of the extracted data is stored with the appropriate type.
 *                       There should be at least as many pairs of these arguments as the number of values stored
 *                       by the format specifiers.
 *                       Additional arguments are ignored by the function
 * @return `EXIT_SUCCESS` if all required arguments were read and no errors occurred, otherwise `EXIT_FAILURE`
 * @throw MissingRequiredArgumentException if one of the required arguments are missing
*/
int ascanf(JNIEnv* env, Arguments* arguments, const char* formatString, ...);

/**
 * ascanf(), avscanf()
 *
 * @paragraph Synopsis
 * @code
 *     int ascanf(Arguments *arguments, const char *format, ...);
 *     int avscanf(Arguments *arguments, Value *value, const char *format, ...);
 * @endcode
 *
 * @paragraph Details
 * Reads data from the given @p arguments and stores them according
 * to parameter format into the locations given by the additional arguments,
 * as if scanf() was used, but reading from arguments instead of the standard input (stdin).
 *
 * The additional arguments should point to already allocated objects of
 * the type specified by their corresponding format specifier.
 * For strings and arrays only the pointer needs to be pre-allocated.
 *
 * The only space allocated by this function is for the strings or arrays.  So callers
 * should only free strings and arrays.  Even if you provide a default value for a string
 * the pointer will be allocated memory on exit from the function, so even then you need to free it.
 *
 * @note
 * Only the provided pointer needs to be freed as only one allocation is made e.g.
 * @code
 *     Arguments arguments;
 *     int *intArray;
 *     ascanf(arguments "%da, "fooArray", &intArray);
 *     // Do stuff
 *     free(intArray);
 * @endcode
 *
 * String space is allocated as follows:
 * @code
 * +------------------------+----------+----------+----------+---------+
 * | pointers to strings    | string 1 | string 2 | string 3 | ...     |
 * +------------------------+----------+----------+----------+---------+
 * @endcode
 *
 * @paragraph dts Differences to scanf()
 * ___
 * There are a number of differences from scanf() which are best described by example:
 *
 * @paragraph gd General differences
 * -# Scan into simple variable/
 * @code
 *     int n;
 *     ascanf("%d", "NPOS", &n);
 * @endcode
 * You must always provide the name of the variable and the pointer to the place to put the value in pairs
 * -# Optional arguments.
 * Optional arguments are specified with the **o** character before the format character.
 * @code
 *     short flag = 10;  // 10 is the default value
 *     ascanf("%ohd", "flag", &flag);
 * @endcode
 *
 * By default all arguments referenced by format specifications are considered required
 * unless the format specification character is preceded by **o**.
 * For optional arguments the pointer provided must point to the default value.
 * In the case of arrays and strings this will be copied into allocated storage that
 * will need to be freed as normal.
 *
 * @paragraph an Argument names
 * -# You can specify simple argument names to search for.  These will simply find the named argument and extract its value.
 * @code
 *     int simpleInt;
 *     ascanf(&arguments "%d, "simple", &simpleInt);
 * @endcode
 * -# You can also use dot and square brace notation to refer to complex arguments that are either arrays or complex objects.
 *  Values that are buried deep inside the json structures can be referenced in this way. e.g., given a
 *  variable named `json` and presented as:
 * @code
 *     json='{"foo": {"bar": 0}}}'
 * @endcode
 * You can specify the @p name as `"json.foo.bar"` to retrieve the `0` value.
 * -# Also given a variable named `jsonArray` and presented as
 * @code
 *     jsonArray='[{"foo": 10}, {"bar": 20}]'
 * @endcode
 * You can specify the #p name as `"jsonArray[1].bar"` to retrieve the `20` value.
 * -# Finally if you use the @p name `"value"`, then the avscanf() function
 * will use the supplied @p value parameter to get the data for that parameter
 * @code
 *     Arguments arguments;
 *     Value value;
 *     int *intArray;
 *     avscanf(&arguments &value, "%da, "fooArray", &intArray);
 *     // Do stuff
 *     free(intArray);
 * @endcode
 *
 * @paragraph fs Format Specifiers
 * ___
 * @paragraph sfs Type specifiers
 * - **b** : `unsigned char *` - interpret the input as a boolean, then extract a single byte into the corresponding variable.
 *   - The following translate to `true`
 *     - not `0`  - integer, short, long,
 *     - not `0.0` - float, double
 *     - `“true”` - char string
 *     - `“t”`  - char string
 *     - `“yes”` - char string
 *     - `“y”`  - char string
 *     - `!0` - short, int, long, float, or double
 *   - The following translate to `false`
 *     - `0`  - integer, short, long,
 *     - `0.0` - float, double
 *     - `“false”` - char string
 *     - `“f”`- char string
 *     - `“no”`- char string
 *     - `“n”`- char string
 *     - `0` - short, int, long, float, or double
 * - **c** : `char *` - extract a single character into the corresponding variable.
 * - **d** : `int *` - extract an integer into the corresponding variable (see **l** & **h** below).
 * - **f** : `float *` - extract a floating point number (see **l** below).
 * - **s** : `char *` - extract a string of characters into allocated space and point the corresponding variable to it.
 * - **u** : `unsigned int *` - extract an unsigned integer into the corresponding variable (see **l** & **h** below).
 *
 * @paragraph rf Required flag
 * - **o** - optional precede the format with this to indicate that the argument is optional.
 *
 * @paragraph Prefixes
 * - **h** - `short *` - preceding **d** will retrieve a short e.g. "%hd".
 * - **l** - `long *`, double * - preceding **d** will retrieve a long eg. `"%ld"`; preceding **f** will retrieve a double eg. `"%lf"`.
 *
 * @paragraph Suffixes
 * - **a** - extract an array of the preceding type into a block of allocated space and point the corresponding variable to it.
 *   The variable will have an extra level of indirection than the non-array version.
 *   e.g.,
 *   @code
 *       int i;
 *       ascanf(..., "%d", &i);
 *   @endcode
 *   becomes
 *   @code
 *       int *ia, n;
 *       ascanf(..., "%da", &ia, &n);
 *   @endcode
 *   Also, you need to provide an extra parameter for each format containing an **a** suffix to hold the count of array
 *   elements found.  The pointer will point to an `int`.
 *
 *
 * @param env            The JNI environment.  Used in all functions involving JNI
 * @param arguments      Arguments that the function processes as its source to retrieve the data.
 * @param value      	 For the avscanf() form this parameter holds the parsed Value given to the Channel Provider endpoint.
 * @param formatString   C string that contains a format string as described above
 * @param ...            Depending on the format string, the function may expect a sequence of additional arguments,
 * 						 containing pairs of names and pointers to allocated storage (except as indicated above),
 * 						 where the interpretation of the extracted data is stored with the appropriate type.
 *                       There should be at least as many pairs of these arguments as the number of references
 *                       in the format specifiers.
 *                       Additional arguments are ignored by the function
 * @return `EXIT_SUCCESS` if all required arguments were read and no errors occurred, otherwise `EXIT_FAILURE`
 * @throw MissingRequiredArgumentException if one of the required arguments are missing
*/
int avscanf(JNIEnv* env, Arguments* arguments, Value* value, const char* formatString, ...);

#ifdef __cplusplus
}
#endif
#endif

