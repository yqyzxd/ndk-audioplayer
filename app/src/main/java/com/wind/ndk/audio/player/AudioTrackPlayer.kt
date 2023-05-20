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
 * ffmpeg samplefmt.h定义的audioFormat
 *
 * enum AVSampleFormat {
AV_SAMPLE_FMT_NONE = -1,
AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
AV_SAMPLE_FMT_S16,         ///< signed 16 bits
AV_SAMPLE_FMT_S32,         ///< signed 32 bits
AV_SAMPLE_FMT_FLT,         ///< float
AV_SAMPLE_FMT_DBL,         ///< double

AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
AV_SAMPLE_FMT_FLTP,        ///< float, planar
AV_SAMPLE_FMT_DBLP,        ///< double, planar
AV_SAMPLE_FMT_S64,         ///< signed 64 bits
AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};
 *
 *
 *
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

    private var mReleased=false;

    private val mLock = Object()

    init {
        mPtr = nativeInit()
    }

    override fun setDataSource(source: String) {
        nativeSetDataSource(mPtr, source)
    }

    override fun prepare() {
        //从c++层获取音频的metadata
        nativePrepare(mPtr)
        mMetadata = getMetadata()
        initAudioTrack()

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

            var encoding=AudioFormat.ENCODING_DEFAULT
            if (audioFormat == 1){//ffmpeg中 AV_SAMPLE_FMT_S16为1
                encoding = AudioFormat.ENCODING_PCM_16BIT
            }


            val bufferSizeInBytes =
                AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, encoding)
            val format = AudioFormat.Builder()
                .setSampleRate(sampleRateInHz)
                .setChannelMask(channelMask)
                .setEncoding(encoding)
                .build()
            mAudioTrack = AudioTrack
                .Builder()
                .setAudioAttributes(attributes)
                .setAudioFormat(format)
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
                if (mStop){
                    return
                }
                //read pcm from c++
                val size = getMinBufferSize()
                val data = ShortArray(size)
                val actualSize = readSamples(data)
                mAudioTrack?.write(data, 0, actualSize)


            }
        }
    }

    override fun start() {
        synchronized(lock = mLock) {
            mPause = false
            nativeStart(mPtr)
            mAudioTrack?.play()
            mLock.notify()

        }
    }

    override fun pause() {
        synchronized(lock = mLock) {
            mPause = true
            nativePause(mPtr)
            mAudioTrack?.pause()
        }
    }

    override fun stop() {
        synchronized(lock = mLock){
            if (!mStop){
                mStop = true
                mLock.notify()
                mAudioTrack?.stop()
                nativeStop(mPtr)
            }

        }


    }

    override fun release() {
        if (!mStop){
            stop()
        }
        if (!mReleased){
            mReleased=true
            mAudioTrack?.release()
            nativeRelease(mPtr)
        }

    }

    override fun getMetadata(): Metadata {
        return nativeGetMetadata(mPtr)
    }

    private fun readSamples(data: ShortArray): Int {
        return nativeReadSamples(mPtr,data, data.size)
    }

    private fun getMinBufferSize():Int{
        return nativeGetMinBufferSize(mPtr)
    }
    private external fun nativeReadSamples(ptr: Long, data: ShortArray, size: Int): Int

    private external fun nativeInit(): Long
    private external fun nativeSetDataSource(ptr: Long, source: String)
    private external fun nativePrepare(ptr: Long)
    private external fun nativeStart(ptr: Long)
    private external fun nativePause(ptr: Long)
    private external fun nativeStop(ptr: Long)
    private external fun nativeRelease(ptr: Long)
    private external fun nativeGetMetadata(ptr: Long): Metadata

    private external fun nativeGetMinBufferSize(ptr:Long):Int


    companion object{
        init {
            System.loadLibrary("player")
        }
    }

}