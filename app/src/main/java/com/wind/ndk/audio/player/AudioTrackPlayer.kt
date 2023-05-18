package com.wind.ndk.audio.player

import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioTrack
import android.net.rtp.AudioStream
import androidx.activity.compose.rememberLauncherForActivityResult
import kotlinx.coroutines.awaitAll
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import java.io.UnsupportedEncodingException
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
class AudioTrackPlayer :IAudioPlayer{

    private var mPtr:Long=0
    private var mMetadata:Metadata?=null
    private var mAudioTrack:AudioTrack?=null
    //finish player
    private var mStop=false
    //pause player
    private var mPause=true

    private val mLock = java.lang.Object()
    init {
        mPtr= nativeInit()
    }

    override fun setDataSource(source: String) {
        nativeSetDataSource(mPtr,source)
    }

    override fun prepare() {
        //从c++层获取音频的metadata
        mMetadata= getMetadata()
        initAudioTrack()
        nativePrepare(mPtr)
    }

    private fun initAudioTrack() {
        mPause=true
        mStop=false
        mMetadata?.apply {

            val attributes=AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_MEDIA)
                .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                .build()
            val channelMask=if (channelConfig==1) AudioFormat.CHANNEL_IN_MONO else AudioFormat.CHANNEL_IN_STEREO

            val encoding = audioFormat

            val bufferSizeInBytes=AudioTrack.getMinBufferSize(sampleRateInHz,channelConfig,audioFormat)
            val audioFormat=AudioFormat.Builder()
                .setSampleRate(sampleRateInHz)
                .setChannelMask(channelMask)
                .setEncoding(encoding)
                .build()
            mAudioTrack=  AudioTrack
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

    private fun runPlay(){
        while(!mStop){
            synchronized(lock = mLock){
                if (mPause){
                    mLock.wait()
                }

                //read pcm from c++

                //readSamples()



            }
        }
    }

    override fun start() {
       nativeStart(mPtr)
    }

    override fun pause() {
        nativePause(mPtr)
    }

    override fun stop() {
       nativeStop(mPtr)
    }

    override fun release() {
        nativeRelease(mPtr)
    }

    override fun getMetadata(): Metadata {
       return nativeGetMetadata(mPtr)
    }


    private external fun nativeReadSamples(data: ShortArray,size:Int)


    private external fun nativeInit(): Long
    private external fun nativeSetDataSource(ptr: Long, source: String)
    private external fun nativePrepare(ptr: Long)
    private external fun nativeStart(ptr: Long)
    private external fun nativePause(ptr: Long)
    private external fun nativeStop(ptr: Long)
    private external fun nativeRelease(ptr: Long)
    private external fun nativeGetMetadata(ptr: Long): Metadata

}