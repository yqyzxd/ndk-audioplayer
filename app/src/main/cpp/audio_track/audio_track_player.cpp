//
// Created by wind on 2023/5/19.
//

#include "audio_track_player.h"
#define LOG_TAG "AudioTrackPlayer"
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
    return 0;
}

int AudioTrackPlayer::prepare() {
    mLock = new Lock();
    mCondition = mLock->newCondition();
    mQueue = new LinkedBlockingQueue<AudioFrame *>();

    int ret = mDecoder->prepare();
    if (ret < 0) {
        //todo 向java层抛出异常
    }

    return ret;
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
            LOGI("before run await");
            mCondition->await();
            LOGI("after run await");
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
    if (!mQuit){
        mLock->lock();
        LOGI("enter stop");
        mQuit = true;
        mDecoder->removeOnAudioFrameAvailableCallback();
        //清空队列
        mQueue->flush();
        mCondition->signal();
        mLock->unlock();
        //join方法不能放在lock里面，否则引起死锁了
        join();

    }

}

void AudioTrackPlayer::release() {
    LOGI("enter release");
    if (mDecoder== nullptr){
        LOGI("mDecoder ==nullptr");
    }
    if (mDecoder != nullptr) {
        mDecoder->dealloc();
        delete mDecoder;
        mDecoder = nullptr;
    }
    if (mDecoder== nullptr){
        LOGI("mDecoder ==nullptr");
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
    LOGI("enter readSamples");
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
    LOGI("readSamples size:%d,actualSize:%d",size,actualSize);
    //memcpy 的第三个参数是字节个数，因为这里是short，占用2个自己，所以要乘以2
    memcpy(data, (short *)audioFrame->data, actualSize*2);

    if (mQueue->size()<QUEUE_MIN_SIZE){
        mLock->lock();
        LOGI("mQueue->size()<QUEUE_MIN_SIZE signal");
        mCondition->signal();
        mLock->unlock();
    }
    return actualSize;
}

int AudioTrackPlayer::getMinBufferSize() {
    if (mDecoder!= nullptr){
        return mDecoder->getAudioFrameSize();
    }
    return 0;
}

AudioMetadata* AudioTrackPlayer::getMetadata() {
    if (mDecoder!= nullptr){
       return mDecoder->getMetadata();
    }
    return nullptr;

}
