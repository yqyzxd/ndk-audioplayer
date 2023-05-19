//
// Created by wind on 2023/5/19.
//
#include "com_wind_ndk_audio_player_AudioTrackPlayer.h"
#include "audio_track/audio_track_player.h"
#include <jni.h>

JNIEXPORT jlong
JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativeInit(JNIEnv *env, jobject
thiz) {
    auto player = new AudioTrackPlayer();
    return reinterpret_cast<jlong>(player);

}

JNIEXPORT void JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativeSetDataSource(JNIEnv
                                                                    *env,
                                                                    jobject thiz, jlong ptr,
                                                                    jstring source
) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    const char *dataSource = env->GetStringUTFChars(source, 0);
    player->setDataSource(dataSource);
    env->ReleaseStringUTFChars(source, dataSource);

}

JNIEXPORT void JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativePrepare(JNIEnv
                                                              *env,
                                                              jobject thiz, jlong
                                                              ptr) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    player->prepare();
}

JNIEXPORT void JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativeStart(JNIEnv
                                                            *env,
                                                            jobject thiz, jlong
                                                            ptr) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    player->start();
}

JNIEXPORT jobject
JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativeGetMetadata(JNIEnv *env, jobject
thiz,
                                                                  jlong ptr
) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    AudioMetadata *metadata = player->getMetadata();
    jclass jmetaDataClass = env->FindClass("com/wind/ndk/audio/player/Metadata");
    jmethodID constructMethodId = env->GetMethodID(jmetaDataClass, "<init>", "(IIII)V");

    return env->NewObject(jmetaDataClass, constructMethodId, metadata->sampleRateInHz,
                          metadata->bitRate, metadata->audioFormat, metadata->channelConfig);

}

JNIEXPORT void JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativePause(JNIEnv
                                                            *env,
                                                            jobject thiz, jlong
                                                            ptr) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    player->pause();
}

JNIEXPORT jint
JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativeReadSamples(JNIEnv *env, jobject
thiz,
                                                                  jlong ptr, jshortArray shortArray,
                                                                  jint
                                                                  size) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    jshort *data = env->GetShortArrayElements(shortArray, 0);
    int actualSize = player->readSamples(data, size);
    env->ReleaseShortArrayElements(shortArray, data, 0);
    return actualSize;
}

JNIEXPORT void JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativeStop(JNIEnv
                                                           *env,
                                                           jobject thiz, jlong
                                                           ptr) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    player->stop();
}

JNIEXPORT void JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativeRelease(JNIEnv
                                                              *env,
                                                              jobject thiz, jlong
                                                              ptr) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    player->release();
    delete player;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_wind_ndk_audio_player_AudioTrackPlayer_nativeGetMinBufferSize(JNIEnv *env, jobject thiz,
                                                                       jlong ptr) {
    AudioTrackPlayer *player = reinterpret_cast<AudioTrackPlayer *>(ptr);
    return player->getMinBufferSize();
}