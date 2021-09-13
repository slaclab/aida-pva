#include <string.h>
#include <stsdef.h>               /* $VMS.. status manipulation */
#include <ssdef.h>                /* SS$_NORMAL and other VMS general codes */
#include <descrip.h>              /*  for definition of $DESCRIPTOR  */
#include <ctype.h>                /* isalnum, ispunct */
#include <stdlib.h>

#include "aida_types.h"
#include "json.h"
#include "aida_server_helper.h"

/**
 * To log any non-OS exceptions and throw back to java
 *
 * The exception is formatted in a standard way with the optionally supplied message
 * The exception is always assumed to be from the edu.stanford.slac.aida.exception package
 *
 * @param env
 * @param exception
 * @param message
 */
void aidaThrowNonOsException(JNIEnv* env, char* exception, const char* message) {
	fprintf(stderr, "AIDA Exception: %s: %s\n", exception, message);
	aidaThrow(env, 1, exception, message);
}

/**
 * To log any exceptions and throw back to java
 *
 * The exception is formatted in a standard way using the VMS status code and its associated message
 * and the optionally supplied message
 * The exception is always assumed to be from the edu.stanford.slac.aida.exception package
 *
 * @param env
 * @param status
 * @param exception
 * @param message
 */
void aidaThrow(JNIEnv* env, int4u status, char* exception, const char* message)
{
	// Clear any exception that may be in the process of being thrown (unlikely)
	if ((*env)->ExceptionCheck(env)) {
		(*env)->ExceptionClear(env);
	}

 	char vmsErrorMessage[BUFSIZ] = { '\0' };
	$DESCRIPTOR(MESSAGE, vmsErrorMessage);
	struct dsc$descriptor errorMessageDescriptor = { BUFSIZ, DSC$K_DTYPE_T, DSC$K_CLASS_S, (char*)&vmsErrorMessage };

	//	Get the message text associated with the VMS message code. if the cause is an OS error
	if (!SUCCESS(status)) {
		ERRTRANSLATE(&status, &errorMessageDescriptor);
		strncat(errorMessageDescriptor.dsc$a_pointer, "; ",
				MIN(strlen("; "), BUFSIZ - strlen(errorMessageDescriptor.dsc$a_pointer)));
	}

	// Add exception
	strncat(errorMessageDescriptor.dsc$a_pointer, exception,
			MIN(strlen(exception), BUFSIZ - strlen(errorMessageDescriptor.dsc$a_pointer)));

	// If a message is specified then append it to the vms message string
	if (message) {
		strncat(errorMessageDescriptor.dsc$a_pointer, "; ",
				MIN(strlen("; "), BUFSIZ - strlen(errorMessageDescriptor.dsc$a_pointer)));
		strncat(errorMessageDescriptor.dsc$a_pointer, message,
				MIN(strlen(message), BUFSIZ - strlen(errorMessageDescriptor.dsc$a_pointer)));
	}

	// Log error message to cm log
	issue_err(errorMessageDescriptor.dsc$a_pointer);

	// Create the fully qualified java class name of the exception to throw
	char classToCreate[BUFSIZ] = "edu/stanford/slac/aida/exception/";
	strcat (classToCreate, exception);

	// Create the java exception class
	jclass exceptionClass;
	exceptionClass = (*env)->FindClass(env, classToCreate);
	if (!exceptionClass) {
		fprintf(stderr, "FATAL: Failed to create object of class: %s\n", classToCreate);
		exit(status);
	}

	// 	Throw the given exception to Java server code, giving the
	//	VMS error text and supplied message as the exception text.
	(*env)->ThrowNew(env, exceptionClass, errorMessageDescriptor.dsc$a_pointer);
}

/**
 * Check if a string ends with another string
 * @param str
 * @param suffix
 * @return true if string ends with suffix
 */
int endsWith(const char *str, char *suffix)
{
	if (!str || !suffix) {
		return 0;
	}
	size_t lenstr = strlen(str);
	size_t lenSuffix = strlen(suffix);
	if (lenSuffix >  lenstr)
		return 0;
	return !strncasecmp(str + lenstr - lenSuffix, suffix, lenSuffix);
}

/**
 * Check if a string starts with another string
 * @param str
 * @param prefix
 * @returns true if string starts with prefix
 */
int startsWith(const char *str, char *prefix)
{
	if (!str || !prefix) {
		return 0;
	}
	size_t lenstr = strlen(str);
	size_t lenPrefix = strlen(prefix);
	if (lenPrefix >  lenstr)
		return 0;
	return !strncasecmp(str, prefix, lenPrefix);
}

/**
 * Get a named argument
 * @param arguments arguments
 * @param name name
 * @return Argument
 */
Argument getArgument(Arguments arguments, char* name)
{
	Argument noArgument;
	memset(&noArgument, 0, sizeof(Argument));

	for (int i = 0; i < arguments.argumentCount; i++) {
		Argument argument = arguments.arguments[i];
		if (!strcasecmp(argument.name, name)) {
			if (strlen(argument.value) > 0) {
				return argument;
			}
		}
	}
	return noArgument;
}

/**
 * Get boolean argument
 * @param argument argument
 * @return boolean
 */
int getBooleanArgument(Argument argument)
{
	return strlen(argument.value) &&
			strcasecmp(argument.value, "false") != 0 &&
			strcmp(argument.value, "0") != 0;
}

/**
 * Get byte argument
 * @param argument argument
 * @return byte
 */
char getByteArgument(Argument argument)
{
	char item = 0;
	int scannedValue = 0;

	if (!strncmp("0x", argument.value, 2)) {
		sscanf(argument.value, "%x", &scannedValue);
	} else {
		sscanf(argument.value, "%d", &scannedValue);
	}
	item = (char)scannedValue;

	return item;
}

/**
 * Get short argument
 * @param argument argument
 * @return short
 */
short getShortArgument(Argument argument)
{
	short item = 0;
	sscanf(argument.value, "%hi", &item);
	return item;
}

/**
 * Get integer argument
 * @param argument argument
 * @return int
 */
int getIntegerArgument(Argument argument)
{
	int item = 0;
	sscanf(argument.value, "%d", &item);
	return item;
}

/**
 * Get long argument
 * @param argument argument
 * @return long
 */
long getLongArgument(Argument argument)
{
	long item = 0;
	sscanf(argument.value, "%ld", &item);
	return item;
}

/**
 * Get float argument
 * @param argument argument
 * @return float
 */
float getFloatArgument(Argument argument)
{
	float item = 0;
	sscanf(argument.value, "%f", &item);
	return item;
}

/**
 * Get double argument
 * @param argument argument
 * @return double
 */
double getDoubleArgument(Argument argument)
{
	double item = 0.0;
	sscanf(argument.value, "%lf", &item);
	return item;
}

/**
 * Print a value to standard output
 * @param env
 * @param value
 */
void printValue(JNIEnv* env, Value value) {
	if ( value.type == AIDA_STRING_TYPE ) {
		printf("%s\n", value.value.stringValue);
	} else if ( value.type == AIDA_JSON_TYPE ) {
		process_value(value.value.jsonValue, 0);
	}
}

