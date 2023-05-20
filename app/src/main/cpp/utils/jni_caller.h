//
// Created by wind on 2023/5/20.
//

#ifndef NDK_AUDIOPLAYER_JNI_CALLER_H
#define NDK_AUDIOPLAYER_JNI_CALLER_H


#include <jni.h>
class JNICaller {

public:
    JNICaller(JavaVM* javaVm);
    ~JNICaller();

    bool isAttachedCurrentThread(JNIEnv** env);
    //若当前已经是java线程，则无需AttachCurrentThread，也无需调用DetachCurrentThread；
    //若当前不是java线程，则必须AttachCurrentThread，也要调用DetachCurrentThread；
    //因此，DetachCurrentThread产出崩溃就只有一种情况：当前是java线程，调用DetachCurrentThread方法。
    int attachCurrentThread(JNIEnv** env, void* thr_args);
    int detachCurrentThread();

private:
    JavaVM* mJavaVm;
    bool mAttached;
};


#endif //NDK_AUDIOPLAYER_JNI_CALLER_H
