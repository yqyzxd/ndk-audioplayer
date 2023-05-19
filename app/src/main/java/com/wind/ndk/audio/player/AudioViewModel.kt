package com.wind.ndk.audio.player

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider

/**
 * FileName: AudioViewModel
 * Author: wind
 * Date: 2023/5/19 09:12
 * Description:
 * Path:
 * History:
 *  <author> <time> <version> <desc>
 *
 */
class AudioViewModel(
    private val mAudioPlayer:IAudioPlayer
) : ViewModel() {
    fun onClickPlayAudioTrack(source:String) {
        Log.e("AudioViewModel","onClickPlayAudioTrack")
        mAudioPlayer.setDataSource(source)
        mAudioPlayer.prepare()
        mAudioPlayer.start()
    }

    fun onClickStopAudioTrack() {
        mAudioPlayer.stop()
        mAudioPlayer.release()
    }


    override fun onCleared() {
        super.onCleared()
        Log.e("AudioViewModel","onCleared")
        mAudioPlayer.stop()
        mAudioPlayer.release()
    }

    companion object {
        val Factory : ViewModelProvider.Factory=object :ViewModelProvider.Factory{
            @Suppress("UNCHECKED_CAST")
            override fun <T : ViewModel> create(modelClass: Class<T>): T {
                return AudioViewModel(AudioPlayer.of(AudioPlayer.IMPL.AUDIO_TRACT)) as T
            }
        }
    }



}