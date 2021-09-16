#include <jni.h>
#include "json.h"

#ifndef _Included_aida_types
#define _Included_aida_types
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Defines the permissible types of fields
 */
typedef enum
{
	AIDA_NO_TYPE,
	AIDA_BOOLEAN_TYPE,
	AIDA_BYTE_TYPE,
	AIDA_SHORT_TYPE,
	AIDA_INTEGER_TYPE,
	AIDA_LONG_TYPE,
	AIDA_FLOAT_TYPE,
	AIDA_DOUBLE_TYPE,
	AIDA_STRING_TYPE,
	AIDA_BOOLEAN_ARRAY_TYPE,
	AIDA_BYTE_ARRAY_TYPE,
	AIDA_SHORT_ARRAY_TYPE,
	AIDA_INTEGER_ARRAY_TYPE,
	AIDA_LONG_ARRAY_TYPE,
	AIDA_FLOAT_ARRAY_TYPE,
	AIDA_DOUBLE_ARRAY_TYPE,
	AIDA_STRING_ARRAY_TYPE,
	AIDA_TABLE_TYPE,
	AIDA_JSON_TYPE
} Type;

/**
 * Convert Type to string
 * @param type type
 * @return string
 */
jstring toTypeString(JNIEnv* env, Type type);

/**
 * Defines the layout of TABLES.
 */
typedef enum
{
	AIDA_NO_LAYOUT,
	AIDA_COLUMN_MAJOR_LAYOUT,  // Each top level array entry is a column containing row data
	AIDA_ROW_MAJOR_LAYOUT      // Each top level array entry is a row containing column data
} Layout;

/**
 * Definition of a field in a channel configuration
 *  name:           the name of the field
 *  label:          the field label
 *  description:    the description
 *  units;          the units e.g. cm, mm, nm ...
 */
typedef struct
{
	char* name;
	char* label;
	char* description;
	char* units;
} Field;

/**
 * Definition of the Channel Configuration structure
 *  type:           the type to be returned in this field
 *  description:    the description of the field
 *  layout:         for TABLES only, this determines whether you'll be returning data COLUMN_MAJOR or ROW_MAJOR
 *  fieldCount:     the number of fields
 *  fields:         field descriptions.  Here we limit the number of fields that can be described to 10.  If you
 *                  want more then use yaml configuration files on the java side
 */
typedef struct
{
	Type type;
	char* description;
	Layout layout;
	int fieldCount;
	Field* fields;
} Config;

typedef struct
{
	char* name;
	char* value;
} Argument;

typedef union
{
	char* stringValue;
	json_value* jsonValue;
} ValueContents;

typedef struct
{
	Type type;    // AIDA_STRING_TYPE or AIDA_JSON_TYPE
	ValueContents value;
} Value;

typedef struct
{
	int argumentCount;
	Argument* arguments;
} Arguments;

/**
 * Table structure
 *  columnCount:    number of columns in table
 *  rowCount:       number of rows in table
 *  types:          the scalar type of each column, one of BOOLEAN, BYTE, SHORT, INTEGER, LONG, FLOAT, DOUBLE, or STRING
 *  ppData:         the data.  Array of [rows][columns]
 */
typedef struct
{
	int columnCount;
	int rowCount;
	Type* types;
	void** ppData;
} Table;

/**
 * An array of data
 */
typedef struct
{
	int count;
	void* items;
} Array;

/**
 * An array of data
 */
typedef struct
{
	int count;
	char** items;
} StringArray;

/**
 * Convert Layout to string
 * @param layout layout
 * @return string
 */
jstring toLayoutString(JNIEnv* env, Layout layout);

Table* initTable(JNIEnv* env, Table* table);
void tableFloatColumn(Table* table, int columnNumber);
void tableLongColumn(Table* table, int columnNumber);
void tableBooleanColumn(Table* table, int columnNumber);
void tableByteColumn(Table *table, int columnNumber);
void tableShortColumn(Table *table, int columnNumber);
void tableIntegerColumn(Table *table, int columnNumber);
void tableDoubleColumn(Table *table, int columnNumber);
void tableStringColumn(Table* table, int columnNumber, int maxStringLength);

#ifdef __cplusplus
}
#endif
#endif

