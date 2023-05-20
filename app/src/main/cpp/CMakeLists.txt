cmake_minimum_required(VERSION 3.22.1)


project("player")

set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/thirdparty")
set(FFMPEG_LIBS "${THIRD_PARTY}/prebuilt/${CMAKE_ANDROID_ARCH_ABI}/")
include_directories(${THIRD_PARTY}/ffmpeg/include/)
#链接ffmpeng
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -L${FFMPEG_LIBS}")

include_directories(libs/lock)
add_subdirectory(libs/lock)

add_library(
        player


        SHARED


        com_wind_ndk_audio_player_AudioTrackPlayer.cpp
        audio_track/audio_track_player.cpp
        audio_decoder/audio_decoder.cpp
        utils/thread.cpp
        audio_decoder/audio_decoder.cpp
        audio_track/audio_track_caller.cpp
        utils/jni_caller.cpp

)


target_link_libraries(
        player

        # system lib
        log
        android
        z

        #third party lib
        fdk-aac
        x264
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale

        lock

)
