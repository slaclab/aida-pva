/** @file
 *  @brief Klystron Native Provider implementation.
 *     **MEMBER**=SLCLIBS:AIDA_PVALIB
 *     **ATTRIBUTES**=JNI,LIBR_NOGLOBAL
 */
#include "aida_pva.h"
#include "AIDASLCKLYS_SERVER.h"

static int getKlystronStatus(JNIEnv* env, const char* uri, Arguments arguments, short* klys_status);
static int klystronStatus(JNIEnv* env, char slcName[30], char* beam_c, char* dgrp_c, short* klys_status);
static int klystronStatusDontThrow(JNIEnv* env, char slcName[30], char* beam_c, char* dgrp_c, short* klys_status);
static int
klystronStatusImpl(JNIEnv* env, char slcName[30], char* beam_c, char* dgrp_c, short* klys_status, bool onErrorThrow);
static Table setActivateValue(JNIEnv* env, const char* uri, Arguments arguments, Value value);
static Table setPdesValue(JNIEnv* env, const char* uri, Arguments arguments, Value value,
		char* pmu, char* secn);
static Table setKphrValue(JNIEnv* env, const char* uri, Arguments arguments, Value value,
		char* pmu, char* secn);
static Table setPdesOrKphrValue(JNIEnv* env, const char* uri, Arguments arguments, Value value,
		char* trim, char* pmu, char* secn);
static void setPconOrAconValue(JNIEnv* env, Arguments arguments, Value value, char* pmu, char* secn);
static int getStandardArgs(JNIEnv* env, Arguments arguments, char** beam_c, char** dgrp_c);
static int getDeviceList(JNIEnv* env, const char* uri, Arguments arguments, char*** devices, int* nDevices);
static bool getKlystronStatuses(JNIEnv* env, char* const* devices, int nDevices,
		char* beam_c, char* dgrp_c,
		short* status,
		bool* isSuccessFull,
		bool* isInAccelerateState,
		bool* isInStandByState,
		bool* isInBadState,
		bool* isSledTuned,
		bool* isSleded,
		bool* isPampl,
		bool* isPphas);

/**
 * The standard attribute suffix for status requests
 */
#define STD_ATTRIBUTE   ":TACT"

/**
 * The length of the standard attribute suffix for status requests
 */
#define STD_ATTRIBUTE_LEN strlen(STD_ATTRIBUTE)

// API Stubs
REQUEST_STUB_BOOLEAN
REQUEST_STUB_BYTE
REQUEST_STUB_INTEGER
REQUEST_STUB_FLOAT
REQUEST_STUB_DOUBLE
REQUEST_STUB_BOOLEAN_ARRAY
REQUEST_STUB_BYTE_ARRAY
REQUEST_STUB_SHORT_ARRAY
REQUEST_STUB_INTEGER_ARRAY
REQUEST_STUB_LONG_ARRAY
REQUEST_STUB_FLOAT_ARRAY
REQUEST_STUB_DOUBLE_ARRAY
REQUEST_STUB_STRING_ARRAY

/**
 * Initialise the service
 * @param env to be used to throw exceptions using aidaThrow() and aidaThrowNonOsException()
 * @throws ServerInitialisationException if the service fails to initialise
 */
void aidaServiceInit(JNIEnv* env)
{
	vmsstat_t status;

	if (!$VMS_STATUS_SUCCESS(status = init("AIDA_SLCKLYS", false))) {
		aidaThrow(env, status, SERVER_INITIALISATION_EXCEPTION, "initialising Klystron Service");
		return;
	}

	printf("Aida Klystron Service Initialised\n");
}

/**
 * Get a short
 *
 * @param uri the uri
 * @param env to be used to throw exceptions using aidaThrow() and aidaThrowNonOsException()
 * @param arguments the arguments
 * @return the short
 */
short aidaRequestShort(JNIEnv* env, const char* uri, Arguments arguments)
{
	short klystronStatus;
	if (getKlystronStatus(env, uri, arguments, &klystronStatus)) {
		return -1;
	}
	return klystronStatus;
}

/**
 * Get a long
 *
 * @param env to be used to throw exceptions using aidaThrow() and aidaThrowNonOsException()
 * @param uri the uri
 * @param arguments the arguments
 * @return the long
 */
long aidaRequestLong(JNIEnv* env, const char* uri, Arguments arguments)
{
	short klystronStatus;
	if (getKlystronStatus(env, uri, arguments, &klystronStatus)) {
		return -1;
	}
	return (long)klystronStatus;
}

/**
 * Get a string.  Allocate memory for string and it will be freed for you by framework
 *
 * @param env to be used to throw exceptions using aidaThrow() and aidaThrowNonOsException()
 * @param uri the uri
 * @param arguments the arguments
 * @return the string
 */
char* aidaRequestString(JNIEnv* env, const char* uri, Arguments arguments)
{
	short klystronStatus;
	if (getKlystronStatus(env, uri, arguments, &klystronStatus)) {
		return NULL;
	}

	if (klystronStatus) {
		return ALLOCATE_STRING(env, "activated", "string");
	} else {
		return ALLOCATE_STRING(env, "deactivated", "string");
	}
}

/**
 * Get a table of data containing full klystron device status:
 *
 * This is the entry point for getting the klystron device status for one or more klystrons
 *
 * For one klystron the `uri` is `KLYS:<micr><unit>:TACT`
 * For multiple the `uri` is `KLYSTRONGET:TACT` and a mandatory parameter `DEVICES` contains an array of klystrons to query
 * e.g. `DEVICES=["KLYS:LI31:31", "KLYS:LI31:32"]`
 *
 * If all of the queries fail then an exception is raised.
 * Otherwise the following fields are returned:
 * 		`name`		STRING		device name `<micr>:<unit>`
 * 		`opstat`	BOOLEAN		operation status (true = success, false = failure)
 * 		`status`	SHORT		klystron status
 * 		`accel`		BOOLEAN		is in accelerate state
 * 		`standby`	BOOLEAN		is in standby mode
 * 		`bad`		BOOLEAN		is bad?
 * 		`sled`		BOOLEAN		true is sled
 * 		`sleded`	BOOLEAN		true if sleded
 * 		`pampl`		BOOLEAN		true if pampl
 * 		`pphas`		BOOLEAN		true if pphas
 */
Table aidaRequestTable(JNIEnv* env, const char* uri, Arguments arguments)
{
	TRACK_ALLOCATED_MEMORY

	// Blank table
	Table table;
	table.columnCount = 0;

	// Query parameters
	char** devices;
	int nDevices;
	char* beam_c;
	char* dgrp_c;

	// Get the list of klystron devices to query
	if (getDeviceList(env, uri, arguments, &devices, &nDevices)) {
		return table;
	}
	TRACK_MEMORY(devices)

	// Get the standard arguments
	if (getStandardArgs(env, arguments, &beam_c, &dgrp_c)) {
		ON_EXCEPTION_FREE_MEMORY_AND_RETURN_(table)
	}
	TRACK_MEMORY(beam_c)
	TRACK_MEMORY(dgrp_c)

	// Query results variables
	short status[nDevices];
	bool isSuccessFull[nDevices];
	bool isInAccelerateState[nDevices];
	bool isInStandByState[nDevices];
	bool isInBadState[nDevices];
	bool isSledTuned[nDevices];
	bool isSleded[nDevices];
	bool isPampl[nDevices];
	bool isPphas[nDevices];

	// Get the status for each klystron into the query results variables
	if (getKlystronStatuses(env, devices, nDevices, beam_c, dgrp_c,
			status, isSuccessFull, isInAccelerateState,
			isInStandByState, isInBadState, isSledTuned, isSleded, isPampl, isPphas)) {
		// Queries have failed for all klystron devices so raise an error
		aidaThrowNonOsException(env, UNABLE_TO_GET_DATA_EXCEPTION, "Failed to get any Klystron Device Statuses");
		FREE_MEMORY
		return table;
	}

	// Free up standard args (don't need them anymore)
	FREE_TRACKED_MEMORY(beam_c);
	FREE_TRACKED_MEMORY(dgrp_c);

	// Allocate a table of 10 columns
	table = tableCreate(env, nDevices, 10);
	ON_EXCEPTION_FREE_MEMORY_AND_RETURN_(table)

	// Add the klystron names as the first column
	tableAddStringColumn(env, &table, devices);
	ON_EXCEPTION_FREE_MEMORY_AND_RETURN_(table)

	// Free all allocated memory
	FREE_MEMORY

	// Add remaining columns to table
	tableAddColumn(env, &table, AIDA_BOOLEAN_TYPE, &isSuccessFull, false);
	ON_EXCEPTION_RETURN_(table)
	tableAddColumn(env, &table, AIDA_SHORT_TYPE, &status, false);
	ON_EXCEPTION_RETURN_(table)
	tableAddColumn(env, &table, AIDA_BOOLEAN_TYPE, &isInAccelerateState, false);
	ON_EXCEPTION_RETURN_(table)
	tableAddColumn(env, &table, AIDA_BOOLEAN_TYPE, isInStandByState, false);
	ON_EXCEPTION_RETURN_(table)
	tableAddColumn(env, &table, AIDA_BOOLEAN_TYPE, &isInBadState, false);
	ON_EXCEPTION_RETURN_(table)
	tableAddColumn(env, &table, AIDA_BOOLEAN_TYPE, &isSledTuned, false);
	ON_EXCEPTION_RETURN_(table)
	tableAddColumn(env, &table, AIDA_BOOLEAN_TYPE, &isSleded, false);
	ON_EXCEPTION_RETURN_(table)
	tableAddColumn(env, &table, AIDA_BOOLEAN_TYPE, &isPampl, false);
	ON_EXCEPTION_RETURN_(table)
	tableAddColumn(env, &table, AIDA_BOOLEAN_TYPE, &isPphas, false);

	return table;
}

/**
 * Get status for each of the given list of klystrons and return status in the given arrays
 * @param env
 * @param devices names of klystron devices to query
 * @param nDevices the number of devices
 * @param beam_c the beam code
 * @param dgrp_c the dgroup
 * @param status the pre-allocated space for the status code
 * @param isSuccessFull the pre-allocated space for the query success status
 * @param isInAccelerateState the pre-allocated space for the is In Accelerate state
 * @param isInStandByState the pre-allocated space for the is in standby state
 * @param isInBadState the pre-allocated space for the is in bad state
 * @param isSledTuned the pre-allocated space for the is sled tuned
 * @param isSleded the pre-allocated space for the  is sledded state
 * @param isPampl the pre-allocated space for the is pampl
 * @param isPphas the pre-allocated space for the is pphas
 * @return true if all of the attempts to get status fail, otherwise false
 */
static bool getKlystronStatuses(JNIEnv* env, char* const* devices, int nDevices,
		char* beam_c, char* dgrp_c,
		short* status,
		bool* isSuccessFull,
		bool* isInAccelerateState,
		bool* isInStandByState,
		bool* isInBadState,
		bool* isSledTuned,
		bool* isSleded,
		bool* isPampl,
		bool* isPphas)
{
	bool allFail = true;

	// For each klystron device, get the status
	for (int i = 0; i < nDevices; i++) {
		char device[strlen(devices[i]) + STD_ATTRIBUTE_LEN + 1];
		sprintf(device, "%s%s", devices[i], STD_ATTRIBUTE);

		TO_SLC_NAME(device, slcName)

		if (!klystronStatusDontThrow(env, slcName, beam_c, dgrp_c, &status[i])) {
			allFail = false; // At least one has not failed

			// Successful status query
			isSuccessFull[i] = true;
			isInAccelerateState[i] = (short)(status[i] & LINKLYSTA_ACCEL) ? true : false;
			isInStandByState[i] = (short)(status[i] & LINKLYSTA_STANDBY) ? true : false;
			isInBadState[i] = (short)(status[i] & LINKLYSTA_BAD) ? true : false;
			isSledTuned[i] = (short)(status[i] & LINKLYSTA_SLED_TUNED) ? true : false;
			isSleded[i] = (short)(status[i] & LINKLYSTA_SLEDED) ? true : false;
			isPampl[i] = (short)(status[i] & LINKLYSTA_PAMPL) ? true : false;
			isPphas[i] = (short)(status[i] & LINKLYSTA_PPHAS) ? true : false;
		} else {
			// Unsuccessful status query
			isSuccessFull[i] = false;
			isInAccelerateState[i] = false;
			isInStandByState[i] = false;
			isInBadState[i] = false;
			isSledTuned[i] = false;
			isSleded[i] = false;
			isPampl[i] = false;
			isPphas[i] = false;
		}
	}
	return allFail;
}

/**
 * Get the list of klystron devices that we need to query.
 * If this is a multi-get query then we need to get the list from the mandatory devices argument
 * Otherwise we use the uri to extract the single device and return it in a singleton array.
 *
 * @param env
 * @param uri
 * @param arguments
 * @param devices
 * @param nDevices
 * @return
 */
static int getDeviceList(JNIEnv* env, const char* uri, Arguments arguments, char*** devices, int* nDevices)
{
	if (!strcasecmp(uri, "KLYSTRONGET:TACT")) {
		// Get the array of klystrons given by the devices argument
		if (ascanf(env, &arguments, "%sa", "devices", devices, nDevices)) {
			return EXIT_FAILURE;
		}
	} else {
		// Otherwise, extract the klystron device name from the uri
		size_t deviceNameLen = strlen(uri) - STD_ATTRIBUTE_LEN, deviceNameSpaceOffset = sizeof(char*);

		// Allocate memory.  Make sure that the allocated memory matches the pattern used by ascanf.
		// e.g. 1 block of memory that contains the pointer followed by the string.
		ALLOCATE_MEMORY_AND_ON_ERROR_RETURN_(env, *devices, deviceNameSpaceOffset + deviceNameLen + 1,
				"Klystron Device Name List", EXIT_FAILURE);
		**devices = (char*)(((char*)*devices) + deviceNameSpaceOffset); // point array element to string space

		// Copy the name.
		strncpy(**devices, uri, deviceNameLen); // copy from uri
		(**devices)[deviceNameLen] = '\0';      // null terminate

		// Return the array
		*nDevices = 1; // one device in this array
	}
	return EXIT_SUCCESS;
}

/**
 * Set a value
 *
 * @param env to be used to throw exceptions using aidaThrow() and aidaThrowNonOsException()
 * @param uri the uri
 * @param arguments the arguments
 * @param value to set
 */
void aidaSetValue(JNIEnv* env, const char* uri, Arguments arguments, Value value)
{
	if (!DPSLCKLYS_ACCESSENABLED()) {
		aidaThrowNonOsException(env, UNABLE_TO_SET_DATA_EXCEPTION,
				"Aida access to klystron operations is not currently enabled");
		return;
	}

	PMU_STRING_FROM_URI(pmu_str, uri)

	if (endsWith(uri, "PCON")) {
		setPconOrAconValue(env, arguments, value, pmu_str, "PCON");
	} else if (endsWith(uri, "ACON")) {
		setPconOrAconValue(env, arguments, value, pmu_str, "ACON");
	} else {
		aidaThrowNonOsException(env, UNSUPPORTED_CHANNEL_EXCEPTION, uri);
		return;
	}
}

/**
 * Set a value and return a table as a response
 *
 * @param env to be used to throw exceptions using aidaThrow() and aidaThrowNonOsException()
 * @param uri the uri
 * @param arguments the arguments
 * @param value to set
 * @return a table
 */
Table aidaSetValueWithResponse(JNIEnv* env, const char* uri, Arguments arguments, Value value)
{
	if (!DPSLCKLYS_ACCESSENABLED()) {
		aidaThrowNonOsException(env, UNABLE_TO_SET_DATA_EXCEPTION,
				"Aida access to klystron operations is not currently enabled");
		RETURN_NULL_TABLE
	}

	PMU_STRING_FROM_URI(pmu_str, uri)

	if (endsWith(uri, "TACT")) {
		return setActivateValue(env, uri, arguments, value);
	} else if (endsWith(uri, "PDES")) {
		return setPdesValue(env, uri, arguments, value, pmu_str, "PDES");
	} else if (endsWith(uri, "KPHR")) {
		return setKphrValue(env, uri, arguments, value, pmu_str, "KPHR");
	} else {
		aidaThrowNonOsException(env, UNSUPPORTED_CHANNEL_EXCEPTION, uri);
		RETURN_NULL_TABLE
	}
}

static void setPconOrAconValue(JNIEnv* env, Arguments arguments, Value value, char* pmu, char* secn)
{
	if (value.type != AIDA_STRING_TYPE) {
		aidaThrowNonOsException(env, MISSING_REQUIRED_ARGUMENT_EXCEPTION, "Missing value to PCON or ACON value");
		return;
	}

	float floatValue;
	if (avscanf(env, &arguments, &value, "%f", "value", &floatValue)) {
		return;
	}

	// Set the value
	vmsstat_t status;
	CONVERT_TO_VMS_FLOAT(&floatValue, 1)

	status = DPSLCKLYS_SETCONFIG(pmu, &floatValue, secn);
	if (!SUCCESS(status)) {
		aidaThrow(env, status, MISSING_REQUIRED_ARGUMENT_EXCEPTION, NULL);
	}
}

static Table setKphrValue(JNIEnv* env, const char* uri, Arguments arguments, Value value, char* pmu, char* secn)
{
	return setPdesOrKphrValue(env, uri, arguments, value, NULL, pmu, secn);
}

static Table setPdesValue(JNIEnv* env, const char* uri, Arguments arguments, Value value, char* pmu, char* secn)
{
	unsigned char trim = true;

	if (ascanf(env, &arguments, "%ob", "TRIM", &trim)) {
		RETURN_NULL_TABLE
	}
	return setPdesOrKphrValue(env, uri, arguments, value, trim ? "YES" : "NO", pmu, secn);
}

static Table
setPdesOrKphrValue(JNIEnv* env, const char* uri, Arguments arguments, Value value, char* trim, char* pmu, char* secn)
{
	// Get the value to set
	float secnValue;
	if (avscanf(env, &arguments, &value, "%f", "value", &secnValue)) {
		RETURN_NULL_TABLE;
	}

	// Set the value
	float phas_value;  /* Returned in ieee format */
	CONVERT_TO_VMS_FLOAT(&secnValue, 1)
	vmsstat_t status = DPSLCKLYS_SETTRIMPHASE(pmu, secn, &secnValue, trim, &phas_value);
	if (!SUCCESS(status)) {
		aidaThrow(env, status, UNABLE_TO_SET_DATA_EXCEPTION, "Error setting value");
		RETURN_NULL_TABLE;
	}

	Table table = tableCreate(env, 1, 1);
	ON_EXCEPTION_RETURN_(table)
	tableAddSingleRowFloatColumn(env, &table, phas_value, false);

	return table;
}

static Table setActivateValue(JNIEnv* env, const char* uri, Arguments arguments, Value value)
{
	// Keep track of stuff to free
	TRACK_ALLOCATED_MEMORY

	// Get the arguments
	char* beam_c;
	char* dgrp_c = NULL;
	unsigned char isActivateOperation;

	if (avscanf(env, &arguments, &value, "%b %s %os",
			"value", &isActivateOperation,
			"beam", &beam_c,
			"dgrp", &dgrp_c
	)) {
		RETURN_NULL_TABLE
	}
	PMU_STRING_FROM_URI(shortSlcName, uri)

	TRACK_MEMORY(beam_c)
	TRACK_MEMORY(dgrp_c)

	// Get the status
	short klys_status;
	int callStatus = klystronStatus(env, shortSlcName, beam_c, dgrp_c, &klys_status);
	if (callStatus) {
		FREE_MEMORY
		RETURN_NULL_TABLE
	}

	short isInAccelerateState = (short)(klys_status & LINKLYSTA_ACCEL);
	short isInStandByState = (short)(klys_status & LINKLYSTA_STANDBY);

	// If trying to activate but not in standby state
	if (isActivateOperation && !isInStandByState) {
		aidaThrowNonOsException(env, UNABLE_TO_SET_DATA_EXCEPTION,
				"Cannot reactivate klystron when not in standby state");
		FREE_MEMORY
		RETURN_NULL_TABLE
	}

	// If trying to deactivate but not in accelerate state
	if (!isActivateOperation && !isInAccelerateState) {
		aidaThrowNonOsException(env, UNABLE_TO_SET_DATA_EXCEPTION,
				"Cannot deactivate klystron when not in accelerate state");
		FREE_MEMORY
		RETURN_NULL_TABLE
	}

	// Set the new value
	int status = DPSLCKLYS_SETDEACTORREACT(shortSlcName, isActivateOperation, beam_c);
	if (!SUCCESS(status)) {
		FREE_MEMORY
		aidaThrow(env, status, UNABLE_TO_SET_DATA_EXCEPTION, "Could not set activation state");
		RETURN_NULL_TABLE
	}

	// Get the status to return it
	callStatus = klystronStatus(env, shortSlcName, beam_c, dgrp_c, &klys_status);
	FREE_MEMORY
	if (callStatus) {
		RETURN_NULL_TABLE
	}

	// Create table for return value
	Table table = tableCreate(env, 1, 1);
	ON_EXCEPTION_RETURN_(table)
	tableAddSingleRowShortColumn(env, &table, klys_status);

	return table;
}

/**
 * Get Status
 *
 * @param env
 * @param uri
 * @param arguments
 * @param klys_status
 * @return
 */
static int getKlystronStatus(JNIEnv* env, const char* uri, Arguments arguments, short* klys_status)
{
	TRACK_ALLOCATED_MEMORY

	char* beam_c;
	char* dgrp_c;

	if (getStandardArgs(env, arguments, &beam_c, &dgrp_c)) {
		return EXIT_FAILURE;
	}
	TRACK_MEMORY(beam_c)
	TRACK_MEMORY(dgrp_c)

	TO_SLC_NAME(uri, slcName)
	int returnStatus = klystronStatus(env, slcName, beam_c, dgrp_c, klys_status);

	FREE_MEMORY

	return returnStatus;
}

/**
 * Get the standard arguments required for klystron status requests
 * @param arguments the arguments to parse
 * @param beam_c  the extracted beam code
 * @param dgrp_c  the extracted dgrp, NULL if not specified
 * @return `EXIT_SUCCESS` if no errors occurred, otherwise `EXIT_FAILURE`
 */
static int getStandardArgs(JNIEnv* env, Arguments arguments, char** beam_c, char** dgrp_c)
{
	*dgrp_c = NULL; // default
	return ascanf(env, &arguments, "%s %os",
			"beam", beam_c,
			"dgrp", dgrp_c
	);
}

/**
 * Get Status, and throw an exception on errors
 *
 * @param env
 * @param uri
 * @param arguments
 * @param klys_status
 * @return
 */
static int klystronStatus(JNIEnv* env, char slcName[30], char* beam_c, char* dgrp_c, short* klys_status)
{
	return klystronStatusImpl(env, slcName, beam_c, dgrp_c, klys_status, true);
}

/**
 * Get Status and don't throw an exception on errors
 *
 * @param env
 * @param uri
 * @param arguments
 * @param klys_status
 * @return
 */
static int klystronStatusDontThrow(JNIEnv* env, char slcName[30], char* beam_c, char* dgrp_c, short* klys_status)
{
	return klystronStatusImpl(env, slcName, beam_c, dgrp_c, klys_status, false);
}

/**
 * Get Status implementation
 *
 * @param env
 * @param uri
 * @param arguments
 * @param klys_status
 * @param onErrorThrow
 * @return
 */
static int
klystronStatusImpl(JNIEnv* env, char slcName[30], char* beam_c, char* dgrp_c, short* klys_status, bool onErrorThrow)
{
	vmsstat_t status = DPSLCKLYS_GETSTATUS(slcName, beam_c, dgrp_c ? dgrp_c : "LIN_KLYS", klys_status);
	if (!SUCCESS(status)) {
		if (onErrorThrow) {
			aidaThrowNonOsException(env, UNABLE_TO_GET_DATA_EXCEPTION, "failed to get klystron status");
		}
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
