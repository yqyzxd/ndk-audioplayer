//
// Created by wind on 2023/5/19.
//

#include "audio_track_player.h"

void AudioTrackPlayer::onAudioFrameAvailableCallback(void *ctx, AudioFrame *frame) {
    AudioTrackPlayer *player = static_cast<AudioTrackPlayer *>(ctx);
    player->onAudioFrameAvailable(frame);
}

AudioTrackPlayer::AudioTrackPlayer() {

    mDecoder = new AudioDecoder();
    mQueue = nullptr;
    mLock = nullptr;
    mCondition = nullptr;
    mCurAudioFrame = nullptr;
}

AudioTrackPlayer::~AudioTrackPlayer() {

}

int AudioTrackPlayer::setDataSource(const char *source) {
    mDecoder->setDataSource(source);

}

int AudioTrackPlayer::prepare() {
    mLock = new Lock();
    mCondition = mLock->newCondition();
    mQueue = new LinkedBlockingQueue<AudioFrame *>();

    int ret = mDecoder->prepare();
    if (ret < 0) {
        //todo 向java层抛出异常
    }
}

void AudioTrackPlayer::start() {
    mQuit = false;
    //设置回调
    mDecoder->setOnAudioFrameAvailableCallback(onAudioFrameAvailableCallback, this);
    Thread::start();
}

void AudioTrackPlayer::run() {
    while (!mQuit) {
        mLock->lock();
        mDecoder->decode();

        //check AudioFrameQueue's size to determining if need continue decode
        if (mQueue->size() >= QUEUE_MIN_SIZE) {
            mCondition->await();
        }

        mLock->unlock();
    }
}

void AudioTrackPlayer::onAudioFrameAvailable(AudioFrame *frame) {
    mQueue->put(frame);
}

void AudioTrackPlayer::pause() {

}

void AudioTrackPlayer::stop() {
    mLock->lock();
    mQuit = true;
    mDecoder->removeOnAudioFrameAvailableCallback();
    mCondition->signal();
    join();
    mLock->unlock();
    //清空队列
    mQueue->flush();
}

void AudioTrackPlayer::release() {
    if (mDecoder != nullptr) {
        mDecoder->dealloc();
        delete mDecoder;
        mDecoder = nullptr;
    }
    if (mQueue) {
        delete mQueue;
        mQueue = nullptr;
    }
    if (mCondition) {
        delete mCondition;
        mCondition = nullptr;
    }
    if (mLock) {
        delete mLock;
        mLock = nullptr;
    }
}

/*
 * 读取解码出来的pcm音频数据
 * 方案一：每次读取一个AudioFrame中的数据，如果readSamples的size小于AudioFrame的size，那么多余的将会被抛弃。优点是实现简单
 * 方案二：每次按readSamples的size进行读取，不舍弃AudioFrame中的数据。缺点是实现稍复杂
 *
 * 目前实现为方案一，为了防止数据抛弃的情况，提供一个getMinBufferSize给客户端，返回值为一个AudioFrame中数据的大小。
 *
 */
int AudioTrackPlayer::readSamples(short *data, int size) {
    AudioFrame *audioFrame;
    int ret = mQueue->take(&audioFrame);
    if (ret < 0) {
        LOGI("mQueue->take error");
        return ret;
    }
    if (audioFrame == nullptr || audioFrame->data == nullptr) {
        return -1;
    }


    if (audioFrame->size > size) {
        LOGE("the size of readSamples function's parameter is too small");
    }

    int actualSize = audioFrame->size > size ? size : audioFrame->size;

    memcpy(data, audioFrame->data, actualSize * sizeof(short));

    return actualSize;
}

int AudioTrackPlayer::getMinBufferSize() {
    if (mDecoder!= nullptr){
        return mDecoder->getAudioFrameSize();
    }
    return 0;
}

void AudioTrackPlayer::getMetadata() {
    if (mDecoder!= nullptr){

    }

}
