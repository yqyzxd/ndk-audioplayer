package com.wind.ndk.audio.player

import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioTrack
import kotlin.concurrent.thread

/**
 * FileName: AudioPlayer
 * Author: wind
 * Date: 2023/5/18 15:49
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
class AudioTrackPlayer : IAudioPlayer {

    private var mPtr: Long = 0
    private var mMetadata: Metadata? = null
    private var mAudioTrack: AudioTrack? = null

    //finish player
    private var mStop = false

    //pause player
    private var mPause = true

    private val mLock = Object()

    init {
        mPtr = nativeInit()
    }

    override fun setDataSource(source: String) {
        nativeSetDataSource(mPtr, source)
    }

    override fun prepare() {
        //从c++层获取音频的metadata
        mMetadata = getMetadata()
        initAudioTrack()
        nativePrepare(mPtr)
    }

    private fun initAudioTrack() {
        mPause = true
        mStop = false
        mMetadata?.apply {

            val attributes = AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_MEDIA)
                .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                .build()
            val channelMask =
                if (channelConfig == 1) AudioFormat.CHANNEL_IN_MONO else AudioFormat.CHANNEL_IN_STEREO

            val encoding = audioFormat

            val bufferSizeInBytes =
                AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat)
            val audioFormat = AudioFormat.Builder()
                .setSampleRate(sampleRateInHz)
                .setChannelMask(channelMask)
                .setEncoding(encoding)
                .build()
            mAudioTrack = AudioTrack
                .Builder()
                .setAudioAttributes(attributes)
                .setAudioFormat(audioFormat)
                .setTransferMode(AudioTrack.MODE_STREAM)
                .setBufferSizeInBytes(bufferSizeInBytes)
                .build()

            //start play thread
            thread { runPlay() }

        }

    }

    private fun runPlay() {
        while (!mStop) {
            synchronized(lock = mLock) {
                if (mPause) {
                    mLock.wait()
                }

                //read pcm from c++
                val size = 0//todo calculate size
                val data = ShortArray(size)
                val actualSize = readSamples(data)

                mAudioTrack?.apply {
                    write(data, 0, actualSize)
                }

            }
        }
    }

    override fun start() {
        synchronized(lock = mLock) {
            nativeStart(mPtr)
            mPause = false
            mLock.notify()
        }
    }

    override fun pause() {
        synchronized(lock = mLock) {
            mPause = true
            nativePause(mPtr)
        }
    }

    override fun stop() {
        mStop = true
        nativeStop(mPtr)

    }

    override fun release() {
        nativeRelease(mPtr)
    }

    override fun getMetadata(): Metadata {
        return nativeGetMetadata(mPtr)
    }

    private fun readSamples(data: ShortArray): Int {
        return nativeReadSamples(data, data.size)
    }

    private external fun nativeReadSamples(data: ShortArray, size: Int): Int

    private external fun nativeInit(): Long
    private external fun nativeSetDataSource(ptr: Long, source: String)
    private external fun nativePrepare(ptr: Long)
    private external fun nativeStart(ptr: Long)
    private external fun nativePause(ptr: Long)
    private external fun nativeStop(ptr: Long)
    private external fun nativeRelease(ptr: Long)
    private external fun nativeGetMetadata(ptr: Long): Metadata

}