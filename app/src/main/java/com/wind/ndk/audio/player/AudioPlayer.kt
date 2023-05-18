package com.wind.ndk.audio.player

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
class AudioPlayer :IAudioPlayer{

    private var mPtr:Long=0

    init {
        mPtr= nativeInit()
    }

    override fun setDataSource(source: String) {
        nativeSetDataSource(mPtr,source)
    }

    override fun prepare() {
        nativePrepare(mPtr)
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



    private external fun nativeInit(): Long
    private external fun nativeSetDataSource(ptr: Long, source: String)
    private external fun nativePrepare(ptr: Long)
    private external fun nativeStart(ptr: Long)
    private external fun nativePause(ptr: Long)
    private external fun nativeStop(ptr: Long)
    private external fun nativeRelease(ptr: Long)
    private external fun nativeGetMetadata(ptr: Long): Metadata

}