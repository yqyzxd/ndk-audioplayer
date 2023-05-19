//
// Created by wind on 2023/5/19.
//

#ifndef NDK_AUDIOPLAYER_AUDIO_TRACK_PLAYER_H
#define NDK_AUDIOPLAYER_AUDIO_TRACK_PLAYER_H


#include "../audio_decoder/audio_decoder.h"

#define QUEUE_MIN_SIZE 20
#define QUEUE_MAX_SIZE 30

class AudioTrackPlayer:Thread {

public:
    AudioTrackPlayer();
    ~AudioTrackPlayer();

    int setDataSource(const char* source);
    int prepare();

    void start();
    void pause();
    void stop();
    void release();
    //线程函数
    void run() override;

    static void onAudioFrameAvailableCallback(void* ctx,AudioFrame* frame);
    void onAudioFrameAvailable(AudioFrame* frame);

    //readSamples的size参数可以从这里获取
    int getMinBufferSize();
    int readSamples(short *data, int size);

    void getMetadata();

private:
    AudioDecoder* mDecoder;
    BlockingQueue<AudioFrame*>* mQueue;
    //当前未读完的AudioFrame
    AudioFrame* mCurAudioFrame;
    //当前AudioFrame读到了哪个位置
    int mCurReadCursor;

    Lock* mLock;
    Condition* mCondition;

    bool mQuit;
};


#endif //NDK_AUDIOPLAYER_AUDIO_TRACK_PLAYER_H
