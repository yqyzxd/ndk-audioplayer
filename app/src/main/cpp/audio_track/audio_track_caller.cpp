//
// Created by wind on 2023/5/20.
//

#include "audio_track_caller.h"

#define LOG_TAG "AudioTrackCaller"
AudioTrackCaller::AudioTrackCaller(JavaVM *vm, jobject jobj): JNICaller(vm) {
    this->jobj=jobj;
}
AudioTrackCaller::~AudioTrackCaller() {
    JNIEnv* env;
    if (attachCurrentThread(&env,0)!=JNI_OK){
        LOGE("AttachCurrentThread failed");
    }
    env->DeleteGlobalRef(jobj);
    if (detachCurrentThread()!=JNI_OK){
        LOGE("DetachCurrentThread failed");
    }
}


void AudioTrackCaller::onCompleted() {
    JNIEnv* env;
    if (attachCurrentThread(&env,0)!=JNI_OK){
        LOGE("AttachCurrentThread failed");
    }

    jclass jaudioTrackClass=env->GetObjectClass(jobj);
    jmethodID  jcompletedMethodID=env->GetMethodID(jaudioTrackClass,"completedFromNative","()V");
    env->CallVoidMethod(jobj,jcompletedMethodID);

    if (detachCurrentThread()!=JNI_OK){
        LOGE("DetachCurrentThread failed");
    }

}