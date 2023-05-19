//
// Created by wind on 2023/5/19.
//

#ifndef NDK_AUDIOPLAYER_AUDIO_DECODER_H
#define NDK_AUDIOPLAYER_AUDIO_DECODER_H
#include "../utils/log.h"
#include "../utils/thread.h"
#include "audio_frame.h"
#include "../libs/blocking_queue/blocking_queue.h"
#include "audio_metadata.h"

extern "C"{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};
#define TARGET_SAMPLE_FMT AV_SAMPLE_FMT_S16
#define TARGET_NB_CHANNELS 2

typedef void (*Callback) (void*,AudioFrame*);

class AudioDecoder{
public:
    AudioDecoder();
    ~AudioDecoder();


    int prepare();
    int decode();

    void setOnAudioFrameAvailableCallback(Callback callback,void* ctx);
    void removeOnAudioFrameAvailableCallback();
    void setDataSource(const char *source);

    void dealloc();

    int getAudioFrameSize();

    AudioMetadata* getMetadata();

private:
    char* mSource;
    AVFormatContext* mAVFormatContext;
    AVCodecContext* mAVCodecContext;
    AVFrame* mAVFrame;//解码后的数据
    AVPacket *mAVPacket;
    SwrContext* mSwrContext;
    //每个AVFrame含有多少个samples
    int mNbSamples;


    Callback mOnAudioFrameAvailableCallback;
    void* ctx;




private:
    bool isTargetSampleFmt();
};


#endif //NDK_AUDIOPLAYER_AUDIO_DECODER_H
