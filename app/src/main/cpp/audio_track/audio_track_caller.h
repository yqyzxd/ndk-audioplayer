//
// Created by wind on 2023/5/20.
//

#ifndef NDK_AUDIOPLAYER_AUDIO_TRACK_CALLER_H
#define NDK_AUDIOPLAYER_AUDIO_TRACK_CALLER_H

#include <jni.h>
#include "../utils/log.h"
#include "../utils/jni_caller.h"

class AudioTrackCaller:public JNICaller {

public:
    AudioTrackCaller(JavaVM* vm,jobject jobj);
    ~AudioTrackCaller();


    void onCompleted();
private:
    jobject jobj;



};


#endif //NDK_AUDIOPLAYER_AUDIO_TRACK_CALLER_H
