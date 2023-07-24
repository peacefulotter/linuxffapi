#include <jni.h>        // JNI header provided by JDK
#include <stdio.h>      // C Standard IO Header
#include "HelloJNI.h"   // Generated
 
JNIEXPORT void JNICALL Java_HelloJNI_sayHello(JNIEnv *env, jobject thisObj) {
    printf("Hello World!\n");
    return;
}

struct ff_effect* getCEffectFromJavaEffect(JNIEnv* env, jobject jEffect) 
{
	struct ff_effect* effect = calloc(1, sizeof(struct ff_effect*));

	jclass clazz = (*env)->GetObjectClass(env, jEffect);

	int effectLength = (*env)->GetIntField(env, jEffect, (*env)->GetFieldID(env, clazz, "length", "I"));

    effect->...

    return effect;
}