package com.wind.ndk.audio.player

/**
 * FileName: IAudioPlayer
 * Author: wind
 * Date: 2023/5/18 15:53
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
interface IAudioPlayer {

    fun setDataSource(source: String)
    fun prepare()
    fun start()
    fun pause()
    fun stop()
    fun release()
    fun getMetadata(): Metadata
}

/**
 * @param audioFormat the format in which the audio data is represented.
 *   See {@link AudioFormat#ENCODING_PCM_16BIT} and
 *   {@link AudioFormat#ENCODING_PCM_8BIT},
 *   and {@link AudioFormat#ENCODING_PCM_FLOAT}
 */
data class Metadata(
    val sampleRateInHz: Int,
    val bitRate: Int,
    //val audioFormat: Int,
    val bitsPerSample:Int,
    val channelConfig: Int
)