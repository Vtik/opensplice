
 /* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_opensplice_dds_dcps_FooDataReaderViewImpl */

#ifndef _Included_org_opensplice_dds_dcps_FooDataReaderViewImpl
#define _Included_org_opensplice_dds_dcps_FooDataReaderViewImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniRead
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;IIII)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniRead
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jint, jint, jint);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniTake
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;IIII)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniTake
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jint, jint, jint);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniReadWCondition
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;ILDDS/ReadCondition;)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniReadWCondition
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jobject);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniTakeWCondition
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;ILDDS/ReadCondition;)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniTakeWCondition
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jobject);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniReadNextSample
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoHolder;)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniReadNextSample
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniTakeNextSample
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoHolder;)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniTakeNextSample
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniReadInstance
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;IJIII)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniReadInstance
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jlong, jint, jint, jint);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniTakeInstance
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;IJIII)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniTakeInstance
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jlong, jint, jint, jint);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniReadNextInstance
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;IJIII)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniReadNextInstance
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jlong, jint, jint, jint);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniTakeNextInstance
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;IJIII)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniTakeNextInstance
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jlong, jint, jint, jint);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniReadNextInstanceWCondition
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;IJLDDS/ReadCondition;)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniReadNextInstanceWCondition
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jlong, jobject);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniTakeNextInstanceWCondition
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;LDDS/SampleInfoSeqHolder;IJLDDS/ReadCondition;)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniTakeNextInstanceWCondition
  (JNIEnv *, jclass, jobject, jlong, jobject, jobject, jint, jlong, jobject);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniGetKeyValue
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;J)I
 */
JNIEXPORT jint JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniGetKeyValue
  (JNIEnv *, jclass, jobject, jlong, jobject, jlong);

/*
 * Class:     org_opensplice_dds_dcps_FooDataReaderViewImpl
 * Method:    jniLookupInstance
 * Signature: (Ljava/lang/Object;JLjava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL Java_org_opensplice_dds_dcps_FooDataReaderViewImpl_jniLookupInstance
  (JNIEnv *, jclass, jobject, jlong, jobject);

#ifdef __cplusplus
}
#endif
#endif