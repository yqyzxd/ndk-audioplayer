//
// Created by wind on 2023/5/19.
//

#ifndef NDK_AUDIOPLAYER_AUDIO_METADATA_H
#define NDK_AUDIOPLAYER_AUDIO_METADATA_H


typedef struct AudioMetadata {

    int sampleRateInHz;
    int bitRate;
    int audioFormat;
    int channelConfig;
} AudioMetadata;

#endif //NDK_AUDIOPLAYER_AUDIO_METADATA_H
