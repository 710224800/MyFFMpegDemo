//
// Created by lyhao on 21-3-2.
//
extern "C"{
#include <libswresample/swresample.h>
}
#include "libavcodec/avcodec.h"
#include "FFResample.h"
#include "XLog.h"
void FFResample::close() {
    ffresample_mux.lock();
    if(actx){
        swr_free(&actx);
    }
    ffresample_mux.unlock();
}

bool FFResample::open(XParameter in, XParameter out) {
    close();
    ffresample_mux.lock();
    //音频重采样上下文初始化
    actx = swr_alloc();
    actx = swr_alloc_set_opts(actx,
            av_get_default_channel_layout(out.channels),
            AV_SAMPLE_FMT_S16, out.sample_rate,
            av_get_default_channel_layout(in.para->channels),
                              (AVSampleFormat)in.para->format, in.para->sample_rate,
                              0, nullptr);
    int re = swr_init(actx);
    if(re != 0){
        XLOGE("swr_init failed!");
        ffresample_mux.unlock();
        return false;
    } else {
        XLOGI("swr_init success!");
    }
    outChannels = in.para->channels;
    outFormat = AV_SAMPLE_FMT_S16;
    ffresample_mux.unlock();
    return true;
}

XData FFResample::resample(XData indata) {
    if(indata.data == nullptr){
        XLOGI("resample indata.data = null");
        return {};
    }
    //XLOGI("resample indata pts is %d", indata.pts);
    ffresample_mux.lock();
    if(actx == nullptr){
        ffresample_mux.unlock();
        return {};
    }
    AVFrame *frame = (AVFrame*) indata.data;

    //输出空间的分配
    XData out;
    int outsize = outChannels * frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    //XLOGI("resample outsize is %d", outsize);
    if(outsize <= 0){
        ffresample_mux.unlock();
        return {};
    }
    out.Alloc(outsize);
    uint8_t *outArr[2] = {0};
    outArr[0] = out.data;
    int len = swr_convert(actx, outArr, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
    if(len <= 0){
        out.Drop();
        ffresample_mux.unlock();
        return {};
    }
    out.pts = indata.pts;
    //XLOGE("swr_convert success = %d", len);
    ffresample_mux.unlock();
    return out;
}