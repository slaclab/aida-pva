/**
 * JNI Service interface: DO NOT EDIT THIS FILE
      **MEMBER**=SLCLIBS:AIDASHRLIB
      **ATTRIBUTES**=JNI
 */
#include <jni.h>
#include <stdio.h>
#include "aida_types.h"
#include "../include/reference_server.h"
#include "../include/aida_jni_helper.h"

/*
 * Get channel configuration for the specified channel
 *
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaChannelConfig
 * Signature: (Ljava/lang/String;)Lslac_aida/model/AidaChannelConfig;
 */
JNIEXPORT jobject
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaChannelConfig
		(JNIEnv* env, jobject jthis, jstring channelName)
{
	jobject returnValue = NULL;

	const char* channel = toCString(env, channelName);

	// Call the implementation
	Config config = aidaChannelConfig(channel);

	// marshal results
	returnValue = aidaChannelConfigToJObject(env, config);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestBoolean
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/lang/Boolean;
 */
jboolean
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestBoolean
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jboolean returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	returnValue = aidaRequestBoolean(pv, arguments);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestByte
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/lang/Byte;
 */
JNIEXPORT jbyte
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestByte
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jbyte returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	returnValue = aidaRequestByte(pv, arguments);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestShort
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/lang/Short;
 */
JNIEXPORT jshort
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestShort
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jshort returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	returnValue = aidaRequestShort(pv, arguments);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestInteger
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/lang/Integer;
 */
JNIEXPORT jint
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestInteger
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jint returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	returnValue = aidaRequestInteger(pv, arguments);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestLong
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/lang/Long;
 */
JNIEXPORT jlong
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestLong
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jlong returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	returnValue = aidaRequestLong(pv, arguments);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestFloat
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/lang/Float;
 */
JNIEXPORT jfloat
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestFloat
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jfloat returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	returnValue = aidaRequestFloat(pv, arguments);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestDouble
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/lang/Double;
 */
JNIEXPORT jdouble
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestDouble
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jdouble returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	returnValue = aidaRequestDouble(pv, arguments);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestString
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestString
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jstring returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	returnValue = toJString(env, aidaRequestString(pv, arguments));

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestBooleanArray
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jbooleanArray
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestBooleanArray
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jbooleanArray returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	Array array = aidaRequestBooleanArray(pv, arguments);

	// create result array
	returnValue = toBooleanArray(env, array);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestByteArray
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jbyteArray
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestByteArray
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jbyteArray returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	Array array = aidaRequestByteArray(pv, arguments);

	// create result array
	returnValue =toByteArray(env, array);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestShortArray
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jshortArray
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestShortArray
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jshortArray returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	Array array = aidaRequestShortArray(pv, arguments);

	// create result array
	returnValue = toShortArray(env, array);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestIntegerArray
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jintArray
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestIntegerArray
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jintArray returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	Array array = aidaRequestIntegerArray(pv, arguments);

	// create result array
	returnValue = toIntegerArray(env, array);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestLongArray
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jlongArray
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestLongArray
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jlongArray returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	Array array = aidaRequestLongArray(pv, arguments);

	// create result array
	returnValue = toLongArray(env, array);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestFloatArray
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jfloatArray
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestFloatArray
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jfloatArray returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	Array array = aidaRequestFloatArray(pv, arguments);

	// create result array
	returnValue = toFloatArray(env, array);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestBooleanArray
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jdoubleArray
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestDoubleArray
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jdoubleArray returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	Array array = aidaRequestDoubleArray(pv, arguments);

	// create result array
	returnValue = toDoubleArray(env, array);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_AidaNativeChannelProvider
 * Method:    aidaRequestStringArray
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jobjectArray
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestStringArray
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jobjectArray returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	StringArray array = aidaRequestStringArray(pv, arguments);

	// create result array
	returnValue = toStringArray(env, array);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}

/*
 * Class:     slac_aida_lib_AidaNativeChannelProvider
 * Method:    aidaRequestTable
 * Signature: (Ljava/lang/String;Ljava/util/List;)Ljava/util/List;
 */
JNIEXPORT jobject
JNICALL Java_slac_aida_AidaNativeChannelProvider_aidaRequestTable
		(JNIEnv* env, jobject jthis, jstring uri, jobject args)
{
	jobject returnValue;

	const char* pv = toCString(env, uri);
	Arguments arguments = toArguments(env, args);

	Table table = aidaRequestTable(pv, arguments);

	// create result table
	returnValue = toTable(env, table);

	// Free up arguments list
	releaseArguments(arguments);

	return returnValue;
}


