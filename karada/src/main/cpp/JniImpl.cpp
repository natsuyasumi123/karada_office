
#include "util/LogUtil.h"
#include <MyGLRenderContext.h>
#include <EGLRender.h>
#include "jni.h"

#include<vector>
#define NATIVE_RENDER_CLASS_NAME "com/enjoy/karada/MyNativeRender"
#define NATIVE_BG_RENDER_CLASS_NAME "com/enjoy/karada/egl/NativeEglRender"

#ifdef __cplusplus
extern "C" {
#endif




/*

 * Method:    native_Init
 * Signature: ()
 */
JNIEXPORT void JNICALL native_Init(JNIEnv *env, jobject instance)
{
	MyGLRenderContext::GetInstance();

}

/*

 * Method:    native_UnInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_UnInit(JNIEnv *env, jobject instance)
{
	MyGLRenderContext::DestroyInstance();
}


JNIEXPORT void JNICALL Java_com_enjoy_karada_MyNativeRender_native_1SetImageData
(JNIEnv *env, jobject instance, jint format, jint width, jint height, jbyteArray imageData , jobjectArray maskData, jobjectArray faceData )
{
	int len = env->GetArrayLength (imageData);
	uint8_t* buf = new uint8_t[len];
	env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));
	MyGLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
	delete[] buf;
	env->DeleteLocalRef(imageData);

    jsize rows = env->GetArrayLength(maskData);
    for (jsize i = 0; i < rows; i++) {
        jobject row = env->GetObjectArrayElement(maskData, i);
        jfloatArray floatArray = static_cast<jfloatArray>(row);
        jsize cols = env->GetArrayLength(floatArray);
        jfloat *elements = env->GetFloatArrayElements(floatArray, 0);
        MyGLRenderContext::GetInstance()->setKaradaData(elements , i ,cols * 3 ) ;
        env->ReleaseFloatArrayElements(floatArray, elements, 0);
    }

    rows = env->GetArrayLength(faceData);
    for (jsize i = 0; i < rows; i++) {
        jobject row = env->GetObjectArrayElement(faceData, i);
        jfloatArray floatArray = static_cast<jfloatArray>(row);
        jsize cols = env->GetArrayLength(floatArray);
        jfloat *elements = env->GetFloatArrayElements(floatArray, 0);
        MyGLRenderContext::GetInstance()->setFaceData(elements , i ,cols * 3 ) ;
        env->ReleaseFloatArrayElements(floatArray, elements, 0);
    }
}

/*

 * Method:    native_SetImageDataWithIndex
 * Signature: (IIII[B)V
 */
JNIEXPORT void JNICALL native_SetImageDataWithIndex
		(JNIEnv *env, jobject instance, jint index, jint format, jint width, jint height, jbyteArray imageData)
{
	int len = env->GetArrayLength (imageData);
	uint8_t* buf = new uint8_t[len];
	env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));
	MyGLRenderContext::GetInstance()->SetImageDataWithIndex(index, format, width, height, buf);
	delete[] buf;
	env->DeleteLocalRef(imageData);
}

/*

 * Method:    native_SetParamsInt
 * Signature: (III)V
 */
JNIEXPORT void JNICALL native_SetParamsInt
		(JNIEnv *env, jobject instance, jint paramType, jint value0, jint value1)
{
	MyGLRenderContext::GetInstance()->SetParamsInt(paramType, value0, value1);
}

/*

 * Method:    native_SetParamsFloat
 * Signature: (IFF)V
 */
JNIEXPORT void JNICALL native_SetParamsFloat
		(JNIEnv *env, jobject instance, jint paramType, jfloat value0, jfloat value1)
{
	MyGLRenderContext::GetInstance()->SetParamsFloat(paramType, value0, value1);
}

/*

 * Method:    native_UpdateTransformMatrix
 * Signature: (FFFF)V
 */
JNIEXPORT void JNICALL native_UpdateTransformMatrix(JNIEnv *env, jobject instance, jfloat rotateX, jfloat rotateY, jfloat scaleX, jfloat scaleY)
{
	MyGLRenderContext::GetInstance()->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
}

/*

 * Method:    native_OnSurfaceCreated
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance)
{
	MyGLRenderContext::GetInstance()->OnSurfaceCreated();
}

/*

 * Method:    native_OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_OnSurfaceChanged
(JNIEnv *env, jobject instance, jint width, jint height)
{
	MyGLRenderContext::GetInstance()->OnSurfaceChanged(width, height);

}

/*

 * Method:    native_OnDrawFrame
 * Signature: ()V
 */

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance)
{
	MyGLRenderContext::GetInstance()->OnDrawFrame();

}


/*

 * Method:    native_EglRenderInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_EglRenderInit(JNIEnv *env, jobject instance)
{
	EGLRender::GetInstance()->Init();

}

/*

 * Method:    native_EglRenderSetImageData
 * Signature: ([BII)V
 */
JNIEXPORT void JNICALL native_EglRenderSetImageData(JNIEnv *env, jobject instance, jbyteArray data, jint width, jint height)
{
	int len = env->GetArrayLength (data);
	uint8_t* buf = new uint8_t[len];
	env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));
	EGLRender::GetInstance()->SetImageData(buf, width, height);
	delete[] buf;
	env->DeleteLocalRef(data);


}

/*

 * Method:    native_EglRenderSetIntParams
 * Signature: (II)V
 */
JNIEXPORT void JNICALL native_EglRenderSetIntParams(JNIEnv *env, jobject instance, jint type, jint param)
{
	EGLRender::GetInstance()->SetIntParams(type, param);

}

/*

 * Method:    native_EglRenderDraw
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_EglRenderDraw(JNIEnv *env, jobject instance)
{
	EGLRender::GetInstance()->Draw();
}

/*

 * Method:    natuve_BgRenderUnInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL natuve_BgRenderUnInit(JNIEnv *env, jobject instance)
{
	EGLRender::GetInstance()->UnInit();
}

#ifdef __cplusplus
}
#endif


#include <iostream>
//JNIEXPORT void JNICALL native_Init(JNIEnv *env, jobject instance)
extern "C" JNIEXPORT void JNICALL Java_com_enjoy_karada_MyNativeRender_native_1SetMarkData(
        JNIEnv *env, jobject obj ,
        jobjectArray maskData,
        jobjectArray faceData)
{
	jsize rows = env->GetArrayLength(maskData);

	for (jsize i = 0; i < rows; i++) {
		jobject row = env->GetObjectArrayElement(maskData, i);
		jfloatArray floatArray = static_cast<jfloatArray>(row);
		jsize cols = env->GetArrayLength(floatArray);
		jfloat *elements = env->GetFloatArrayElements(floatArray, 0);
		MyGLRenderContext::GetInstance()->setKaradaData(elements , i ,cols * 3 ) ;
		env->ReleaseFloatArrayElements(floatArray, elements, 0);
	}

    rows = env->GetArrayLength(faceData) ;
	for (jsize i = 0; i < rows; i++) {
		jobject row = env->GetObjectArrayElement(faceData, i);
		jfloatArray floatArray = static_cast<jfloatArray>(row);
		jsize cols = env->GetArrayLength(floatArray);
		jfloat *elements = env->GetFloatArrayElements(floatArray, 0);
		MyGLRenderContext::GetInstance()->setFaceData(elements , i ,cols * 3 ) ;

		env->ReleaseFloatArrayElements(floatArray, elements, 0);
	}
}


extern "C" JNIEXPORT void JNICALL Java_com_enjoy_karada_MyNativeRender_native_1SetOutlineData(
        JNIEnv *env, jobject obj ,
        jbyteArray imageData,jint format, jint width, jint height)
{

    int len = env->GetArrayLength (imageData);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));
    MyGLRenderContext::GetInstance()->SetOutlineData(format, width, height, buf);
    delete[] buf;
    env->DeleteLocalRef(imageData);
}

extern "C" JNIEXPORT void JNICALL Java_com_enjoy_karada_MyNativeRender_native_1SetDegree(
        JNIEnv *env, jobject obj ,
        jfloat degree )
{

    MyGLRenderContext::GetInstance()->setDegree(degree);
}

extern "C" JNIEXPORT void JNICALL Java_com_enjoy_karada_MyNativeRender_native_1addSticker(
        JNIEnv *env, jobject obj ,
        jstring path )
{
    const char* chars = env->GetStringUTFChars(path, nullptr);
    std::string imagePath(chars);
    env->ReleaseStringUTFChars(path, chars);



    MyGLRenderContext::GetInstance()->addSticker(imagePath);
}

extern "C" JNIEXPORT void JNICALL Java_com_enjoy_karada_MyNativeRender_native_1setStickerVertices(
        JNIEnv *env, jobject obj ,
        jfloatArray vertices )
{
    jsize length = env->GetArrayLength(vertices);

    jfloat* tempArray = env->GetFloatArrayElements(vertices, NULL);
    if (tempArray == nullptr) {
        return;
    }
    float* floatArray = new float[length];  // 在堆上分配内存
    std::memcpy(floatArray, tempArray, length * sizeof(float));  // 复制数据
    MyGLRenderContext::GetInstance()->setStickerVertices(floatArray , length);
    env->ReleaseFloatArrayElements(vertices, tempArray, 0);
    delete[] floatArray;
}

static JNINativeMethod g_RenderMethods[] = {
		{"native_Init",                      "()V",       (void *)(native_Init)},
//		{"native_SetMarkData",              "()V",       (void *)(native_SetMarkData)},
		{"native_UnInit",                    "()V",       (void *)(native_UnInit)},
//		{"native_SetImageData",              "(III[B[Ljava/lang/Object)V",  (void *)(native_SetImageData)},
		{"native_SetImageDataWithIndex",     "(IIII[B)V", (void *)(native_SetImageDataWithIndex)},
		{"native_SetParamsInt",              "(III)V",    (void *)(native_SetParamsInt)},
		{"native_SetParamsFloat",            "(IFF)V",    (void *)(native_SetParamsFloat)},
		{"native_UpdateTransformMatrix",     "(FFFF)V",   (void *)(native_UpdateTransformMatrix)},
		{"native_OnSurfaceCreated",          "()V",       (void *)(native_OnSurfaceCreated)},
		{"native_OnSurfaceChanged",          "(II)V",     (void *)(native_OnSurfaceChanged)},
		{"native_OnDrawFrame",               "()V",       (void *)(native_OnDrawFrame)},

};

static JNINativeMethod g_BgRenderMethods[] = {
		{"native_EglRenderInit",          "()V",       (void *)(native_EglRenderInit)},
		{"native_EglRenderSetImageData",  "([BII)V",   (void *)(native_EglRenderSetImageData)},
		{"native_EglRenderSetIntParams",  "(II)V",     (void *)(native_EglRenderSetIntParams)},
		{"native_EglRenderDraw",          "()V",       (void *)(native_EglRenderDraw)},
		{"native_EglRenderUnInit",        "()V",       (void *)(natuve_BgRenderUnInit)},
};

static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum)
{
	LOGCATE("RegisterNativeMethods");
	jclass clazz = env->FindClass(className);
	if (clazz == NULL)
	{
		LOGCATE("RegisterNativeMethods fail. clazz == NULL");
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, methods, methodNum) < 0)
	{
		LOGCATE("RegisterNativeMethods fail");
		return JNI_FALSE;
	}
	return JNI_TRUE;
}

static void UnregisterNativeMethods(JNIEnv *env, const char *className)
{
	LOGCATE("UnregisterNativeMethods");
	jclass clazz = env->FindClass(className);
	if (clazz == NULL)
	{
		LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
		return;
	}
	if (env != NULL)
	{
		env->UnregisterNatives(clazz);
	}
}

// call this func when loading lib
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p)
{
	LOGCATE("===== JNI_OnLoad =====");
	jint jniRet = JNI_ERR;
	JNIEnv *env = NULL;
	if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
	{
		return jniRet;
	}

	jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME, g_RenderMethods,
										sizeof(g_RenderMethods) /
										sizeof(g_RenderMethods[0]));
	if (regRet != JNI_TRUE)
	{
		return JNI_ERR;
	}

	regRet = RegisterNativeMethods(env, NATIVE_BG_RENDER_CLASS_NAME, g_BgRenderMethods,
										sizeof(g_BgRenderMethods) /
										sizeof(g_BgRenderMethods[0]));
	if (regRet != JNI_TRUE)
	{
		return JNI_ERR;
	}

	return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p)
{
	JNIEnv *env = NULL;
	if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
	{
		return;
	}

	UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);

	UnregisterNativeMethods(env, NATIVE_BG_RENDER_CLASS_NAME);
}


