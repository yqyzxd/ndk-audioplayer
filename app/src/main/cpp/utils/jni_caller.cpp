//
// Created by wind on 2023/5/20.
//

#include "jni_caller.h"
JNICaller::JNICaller(JavaVM *javaVm) {
    this->mJavaVm=javaVm;
}

JNICaller::~JNICaller() {

}

bool JNICaller::isAttachedCurrentThread(JNIEnv **env) {
    if (mJavaVm->GetEnv((void**)env,JNI_VERSION_1_4)!=JNI_OK){
        return false;
    }
    return true;
}

int JNICaller::attachCurrentThread(JNIEnv **env, void *thr_args) {
    mAttached=isAttachedCurrentThread(env);
    if (!mAttached){
        return mJavaVm->AttachCurrentThread(env,thr_args);
    }
    return JNI_OK;
}

int JNICaller::detachCurrentThread() {
    if (!mAttached){//AttachCurrentThread之前没有Attach，所有需要Detach
       return mJavaVm->DetachCurrentThread();
    }
    return JNI_OK;
}