//
// Created by wind on 2023/5/19.
//

#include "audio_decoder.h"


#define LOG_TAG "AudioDecoder"

AudioDecoder::AudioDecoder() {
    mSource= nullptr;
    mAVFormatContext = nullptr;
    mAVCodecContext = nullptr;

}

AudioDecoder::~AudioDecoder() {
    if (mSource!= nullptr){
        delete []mSource;
        mSource= nullptr;
    }


}

int AudioDecoder::prepare() {

    mAVFormatContext = avformat_alloc_context();
    AVInputFormat* iformat=av_find_input_format("mp3");
    int ret = avformat_open_input(&mAVFormatContext, mSource, iformat, nullptr);
    if (ret < 0) {
        LOGI("avformat_open_input error ret:%d,%s",ret,    av_err2str(ret));
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
    return mAVCodecContext->sample_fmt == TARGET_SAMPLE_FMT;
}


int AudioDecoder::decode() {

    int ret = av_read_frame(mAVFormatContext, mAVPacket);
    if (ret < 0) {
        LOGI("av_read_frame error:%d,%s",ret, av_err2str(ret));
        return ret;
    }
    ret = avcodec_send_packet(mAVCodecContext, mAVPacket);
    while (ret >= 0) {
        ret = avcodec_receive_frame(mAVCodecContext, mAVFrame);
        if (ret >= 0) {
            int dstSize;
            uint8_t **dstData;
            mNbSamples=mAVFrame->nb_samples;
            if (mSwrContext) {
                //需要转换
                //uint8_t ***audio_data, int *linesize, int nb_channels,
                //int nb_samples, enum AVSampleFormat sample_fmt, int align
                int dst_linesize;
                int dst_nb_channels = TARGET_NB_CHANNELS;
                int dst_nb_samples = mAVFrame->nb_samples;


                ret = av_samples_alloc_array_and_samples(&dstData, &dst_linesize, dst_nb_channels,
                                                         dst_nb_samples, TARGET_SAMPLE_FMT, 0);
                if (ret < 0) {
                    LOGI("av_samples_alloc_array_and_samples error");
                    return -1;
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
                ret = swr_convert(mSwrContext, dstData, dst_nb_samples,
                                  (const uint8_t **) mAVFrame->data, mAVFrame->nb_samples);
                mNbSamples=ret;
                LOGI("swr_convert return samples per channel:%d",ret);
                if (ret < 0) {
                    LOGI("swr_convert error");
                    return -1;
                }
                dstSize = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels, ret,
                                                     TARGET_SAMPLE_FMT, 1);
                if (dstSize < 0) {
                    LOGI("av_samples_get_buffer_size error");
                    return -1;
                }

            } else {
                dstData = mAVFrame->data;
                dstSize = av_samples_get_buffer_size(mAVFrame->linesize, mAVFrame->channels,
                                                     mAVFrame->nb_samples,
                                                     mAVCodecContext->sample_fmt, 1);
            }

            //解码成功
            AudioFrame *audioFrame = new AudioFrame();
            //音频只有第0个位置有数据
            audioFrame->data = (short*)dstData[0];
            audioFrame->size = dstSize;
            if (mOnAudioFrameAvailableCallback!= nullptr){
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
    mOnAudioFrameAvailableCallback= nullptr;
}

void AudioDecoder::setDataSource(const char *source) {
    int length=strlen(source);
    mSource=new char[length+1];
   strcpy(mSource,source);
}

void AudioDecoder::dealloc() {
    avformat_close_input(&mAVFormatContext);
    avcodec_close(mAVCodecContext);
    avcodec_free_context(&mAVCodecContext);
    if (mSwrContext!= nullptr){
        swr_free(&mSwrContext);
    }
    av_frame_free(&mAVFrame);
    av_packet_free(&mAVPacket);
    avformat_free_context(mAVFormatContext);
}

int AudioDecoder::getAudioFrameSize() {
    if (mNbSamples<=0){
        mNbSamples=1024;
    }
    int size= av_samples_get_buffer_size(nullptr, TARGET_NB_CHANNELS, mNbSamples,
                                         TARGET_SAMPLE_FMT, 1);

    LOGI("getAudioFrameSize:%d",size);
    return size;
}

AudioMetadata*  AudioDecoder::getMetadata() {
    AudioMetadata* metadata=new AudioMetadata();
    metadata->sampleRateInHz=mAVCodecContext->sample_rate;
    metadata->bitRate=mAVCodecContext->bit_rate;
    metadata->audioFormat=TARGET_SAMPLE_FMT;
    metadata->channelConfig=TARGET_NB_CHANNELS;

    LOGI("sampleRateInHz:%d,bitRate:%d,audioFormat:%d,channelConfig:%d",metadata->sampleRateInHz,metadata->bitRate,metadata->audioFormat, metadata->channelConfig);
    return metadata;
}
