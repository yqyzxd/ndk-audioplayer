//
// Created by wind on 2023/5/19.
//

#ifndef NDK_AUDIOPLAYER_AUDIO_FRAME_H
#define NDK_AUDIOPLAYER_AUDIO_FRAME_H

#define LOG_TAG "AudioFrame"
typedef struct  AudioFrame {

    void* data;
    int size;

    ~AudioFrame(){
        if (data!= nullptr){
            delete[] data; //因为知道data是数组，所以delete[]
            data= nullptr;
        }
    }
} AudioFrame;


#endif //NDK_AUDIOPLAYER_AUDIO_FRAME_H
