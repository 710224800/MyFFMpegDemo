
#include "XLog.h"
#include "FFDemux.h"
extern "C"{
#include <libavformat/avformat.h>
}

static double r2d(AVRational r){
    return r.num == 0 || r.den == 0 ? 0. : (double) r.num / (double) r.den;
}

//打开文件，或者流媒体
bool FFDemux::open(const char *url){
    XLOGI("open file %s begin", url);
    //打开url
    int re = avformat_open_input(&avFormatContext, url, 0, 0);
    if(re != 0){
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("FFDemux open %s failed because %s", url, buf);
        return false;
    }
    XLOGI("FFDemux open %s success", url);
    //读取文件信息
    re = avformat_find_stream_info(avFormatContext, 0);
    if(re != 0){
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("avformat_find_stream_info %s failed because %s", url, buf);
        return false;
    }
    this->totalMs = avFormatContext->duration / (AV_TIME_BASE / 1000);
    XLOGI("totalMs = %lld", this->totalMs);
//    getVPara();
//    getAPara();
    return true;
}

//获取视频参数
XParameter FFDemux::getVPara() {
    if(avFormatContext == nullptr){
        XLOGE("getVPara failed avFormatContext == nullptr");
        return {};
    }
    int re = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO,-1, -1, nullptr, 0);
    if(re < 0){
        XLOGE("av_find_best_stream TYPE_VIDEO failed!");
        return {};
    }
    videoStream = re;
    XParameter parameter;
    parameter.para = avFormatContext->streams[re]->codecpar;
    return parameter;
}

XParameter FFDemux::getAPara() {
    if(avFormatContext == nullptr){
        XLOGE("getAPara avFormatContext == nullptr");
        return {};
    }
    int re = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if(re < 0){
        XLOGE("av_find_best_stream TYPE_AUDIO failed!");
        return {};
    }
    audioStream = re;
    XParameter parameter;
    parameter.para = avFormatContext->streams[re]->codecpar;
    parameter.channels = avFormatContext->streams[re]->codecpar->channels;
    parameter.sample_rate = avFormatContext->streams[re]->codecpar->sample_rate;
    return parameter;
}

//读取一帧数据，数据由调用者清理
XData FFDemux::read(){
    if(avFormatContext == nullptr){
        XLOGE("Read avFormatContext == nullptr");
        return {};
    }

    XData data;
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(avFormatContext, pkt);
    if(re != 0){
        av_packet_free(&pkt);
        XLOGE("av_read_frame error re = %d", re);
        if(AVERROR_EOF == re){ //到 文件 结尾
            data.size = -9999;
            return data;
        }
        return {};
    }
    XLOGI("pack size is %d , pts is %lld", pkt->size, pkt->pts);
    data.data = (unsigned char *) pkt;
    data.size = pkt->size;
    if(pkt->stream_index == audioStream){
        data.isAudio = true;
    } else if(pkt->stream_index == videoStream){
        data.isAudio = false;
    } else {
        av_packet_free(&pkt);
        return {};
    }

    //转换pts
    pkt->pts = pkt->pts * (1000 * r2d(avFormatContext->streams[pkt->stream_index]->time_base));
    pkt->dts = pkt->dts * (1000 * r2d(avFormatContext->streams[pkt->stream_index]->time_base));
    data.pts = (int) pkt->pts;
    return data;
}
static bool isFirst = true;
FFDemux::FFDemux() {
    if(isFirst){
        isFirst = false;
        //注册所有封装器
        av_register_all();

        //注册所有的解码器
        avcodec_register_all();

        //初始化网络
        avformat_network_init();
        XLOGI("register ffmpeg!");
    }
}