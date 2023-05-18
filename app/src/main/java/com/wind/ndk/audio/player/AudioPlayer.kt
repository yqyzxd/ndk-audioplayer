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
class AudioPlayer private constructor() {

    enum class IMPL {
        AUDIO_TRACT, OPENSL_ES
    }

    companion object {

        @JvmStatic
        fun of(impl: IMPL): IAudioPlayer {
            return when (impl) {
                IMPL.AUDIO_TRACT -> AudioTrackPlayer()
                IMPL.OPENSL_ES -> OpenSLESPlayer()
            }
        }
    }






}