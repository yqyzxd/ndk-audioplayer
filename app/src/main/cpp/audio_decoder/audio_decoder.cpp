//
// Created by wind on 2023/5/19.
//

#include "audio_decoder.h"


#define LOG_TAG "AudioDecoder"

AudioDecoder::AudioDecoder() {
    mSource = nullptr;
    mAVFormatContext = nullptr;
    mAVCodecContext = nullptr;
    mSwrContext= nullptr;
    mSwrBuffer= nullptr;
    mSwrBufferSize= 0;

}

AudioDecoder::~AudioDecoder() {
    if (mSource != nullptr) {
        delete[] mSource;
        mSource = nullptr;
    }


}

int AudioDecoder::prepare() {
    LOGI("prepare");
    mAVFormatContext = avformat_alloc_context();
    AVInputFormat *iformat = av_find_input_format("mp3");
    int ret = avformat_open_input(&mAVFormatContext, mSource, iformat, nullptr);
    if (ret < 0) {
        LOGI("avformat_open_input error ret:%d,%s", ret, av_err2str(ret));
        return -1;
    }

    ret = avformat_find_stream_info(mAVFormatContext, nullptr);
    if (ret < 0) {
        LOGI("avformat_find_stream_info error");
        return -1;
    }
    AVStream *audioStream = nullptr;
    for (int i = 0; i < mAVFormatContext->nb_streams; i++) {
        AVStream *stream = mAVFormatContext->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = stream;
            break;
        }
    }

    if (audioStream == nullptr) {
        LOGI("could not find audio stream");
        return -1;
    }

    AVCodec *avCodec = avcodec_find_decoder(audioStream->codecpar->codec_id);
    mAVCodecContext = avcodec_alloc_context3(avCodec);
    ret = avcodec_open2(mAVCodecContext, avCodec, nullptr);
    if (ret < 0) {
        LOGI("avcodec_open2 error");
        return -1;
    }
    ret = avcodec_parameters_to_context(mAVCodecContext, audioStream->codecpar);
    if (ret < 0) {
        LOGI("avcodec_parameters_to_context error");
        return -1;
    }

    //判断是否需要resample
    if (!isTargetSampleFmt()) {

        mSwrContext = swr_alloc_set_opts(nullptr,
                                         av_get_default_channel_layout(TARGET_NB_CHANNELS),
                                         TARGET_SAMPLE_FMT, mAVCodecContext->sample_rate,
                                         av_get_default_channel_layout(mAVCodecContext->channels),
                                         mAVCodecContext->sample_fmt, mAVCodecContext->sample_rate,
                                         0, 0
        );
        if (mSwrContext == nullptr) {
            LOGI("swr_alloc_set_opts error");
            return -1;
        }
        ret = swr_init(mSwrContext);
        if (ret < 0) {
            swr_free(&mSwrContext);
            LOGI("swr_init error");
            avcodec_close(mAVCodecContext);
            return -1;
        }
    }
    //分配AVFrame
    mAVFrame = av_frame_alloc();
    mAVPacket = av_packet_alloc();

    return 0;
}

bool AudioDecoder::isTargetSampleFmt() {
    LOGI("mAVCodecContext->sample_fmt:%d",mAVCodecContext->sample_fmt);
    return mAVCodecContext->sample_fmt == TARGET_SAMPLE_FMT && mAVCodecContext->channels == TARGET_NB_CHANNELS;
}


int AudioDecoder::decode() {

    int ret = av_read_frame(mAVFormatContext, mAVPacket);
    if (ret < 0) {
        LOGI("av_read_frame error:%d,%s", ret, av_err2str(ret));
        return ret;
    }

    ret = avcodec_send_packet(mAVCodecContext, mAVPacket);
    while (ret >= 0) {
        ret = avcodec_receive_frame(mAVCodecContext, mAVFrame);
        if (ret >= 0) {
            int dstSize;
            int dstSizeInShort;
            uint8_t *dstData;

            if (mSwrContext) {
                //需要转换
                //uint8_t ***audio_data, int *linesize, int nb_channels,
                //int nb_samples, enum AVSampleFormat sample_fmt, int align

                int dst_nb_channels = TARGET_NB_CHANNELS;
                int dst_nb_samples = mAVFrame->nb_samples;

                //av_samples_get_buffer_size  = dst_nb_channels * dst_nb_samples* TARGET_SAMPLE_FMT
                dstSize = av_samples_get_buffer_size(nullptr, dst_nb_channels, dst_nb_samples,
                                                     TARGET_SAMPLE_FMT, 1);
                //LOGI("av_samples_get_buffer_size ：%d",dstSize);
                if (mSwrBuffer== nullptr || dstSize!=mSwrBufferSize){
                    mSwrBufferSize=dstSize;
                    mSwrBuffer= realloc(mSwrBuffer,mSwrBufferSize);
                }
                /*
                * @param s         allocated Swr context, with parameters set
                * @param out       output buffers, only the first one need be set in case of packed audio
                * @param out_count amount of space available for output in samples per channel
                * @param in        input buffers, only the first one need to be set in case of packed audio
                * @param in_count  number of input samples available in one channel
                *
                * @return number of samples output per channel, negative value on error
                */
                uint8_t *out[2] ={static_cast<uint8_t *>(mSwrBuffer), nullptr};
                ret = swr_convert(mSwrContext, out, dst_nb_samples,
                                  (const uint8_t **) mAVFrame->data, mAVFrame->nb_samples);
                mNbSamples = ret;
                //LOGI("swr_convert return samples per channel:%d",ret);
                if (ret < 0) {
                    LOGI("swr_convert error");
                    return -1;
                }
                dstData= static_cast<uint8_t *>(mSwrBuffer);
                if (dstSize < 0) {
                    LOGI("av_samples_get_buffer_size error");
                    return -1;
                }

            } else {
                //音频只有第0个位置有数据
                dstData = mAVFrame->data[0];
                mNbSamples=mAVFrame->nb_samples;
            }
            dstSizeInShort=mNbSamples*TARGET_NB_CHANNELS;
            //解码成功
            AudioFrame *audioFrame = new AudioFrame();

            //因为format是AV_SAMPLE_FMT_S16，所以用short类型
            audioFrame->data = (short*)dstData;
            //short个数 注意memcpy时要乘以2，因为memcpy是字节个数
            audioFrame->size = dstSizeInShort;



            if (mOnAudioFrameAvailableCallback != nullptr) {
                mOnAudioFrameAvailableCallback(ctx, audioFrame);
            }

        }
    }
    av_packet_unref(mAVPacket);
    return 0;


}

void AudioDecoder::setOnAudioFrameAvailableCallback(Callback callback, void *ctx) {
    this->mOnAudioFrameAvailableCallback = callback;
    this->ctx = ctx;
}

void AudioDecoder::removeOnAudioFrameAvailableCallback() {
    mOnAudioFrameAvailableCallback = nullptr;
}

void AudioDecoder::setDataSource(const char *source) {
    int length = strlen(source);
    mSource = new char[length + 1];
    strcpy(mSource, source);
}

void AudioDecoder::dealloc() {
    LOGI("enter dealloc");
    if (mAVCodecContext != nullptr) {
        LOGI("before avcodec_close ");
        avcodec_close(mAVCodecContext);
        LOGI("before avcodec_free_context ");
        avcodec_free_context(&mAVCodecContext);
        mAVCodecContext = nullptr;
    }

    if (mSwrContext != nullptr) {
        LOGI("before swr_free ");
        swr_free(&mSwrContext);
        LOGI("after swr_free ");
        mSwrContext = nullptr;
    }
    if (mAVFrame != nullptr) {
        LOGI("before av_frame_free ");
        av_frame_free(&mAVFrame);
        LOGI("after av_frame_free ");
        mAVFrame = nullptr;
    }
    if (mAVPacket != nullptr) {
        LOGI("before av_packet_free ");
        av_packet_free(&mAVPacket);
        LOGI("after av_packet_free ");
        mAVPacket = nullptr;
    }
    if (mAVFormatContext != nullptr) {
        LOGI("before avformat_close_input ");
        avformat_close_input(&mAVFormatContext);
        LOGI("before avformat_free_context ");
        avformat_free_context(mAVFormatContext);
        mAVFormatContext = nullptr;
    }
    LOGI("exit dealloc");
}

int AudioDecoder::getAudioFrameSize() {
    int size= mNbSamples*TARGET_NB_CHANNELS;
    LOGI("getAudioFrameSize:%d", size);
    return  size;
}

AudioMetadata *AudioDecoder::getMetadata() {
    AudioMetadata *metadata = new AudioMetadata();
    metadata->sampleRateInHz = mAVCodecContext->sample_rate;
    metadata->bitRate = mAVCodecContext->bit_rate;
    metadata->bitsPerSample = 16;
    metadata->channelConfig = TARGET_NB_CHANNELS;

    LOGI("sampleRateInHz:%d,bitRate:%d,audioFormat:%d,channelConfig:%d", metadata->sampleRateInHz,
         metadata->bitRate, metadata->bitsPerSample, metadata->channelConfig);
    return metadata;
}
