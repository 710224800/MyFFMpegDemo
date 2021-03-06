
#include "XLog.h"
#include "FFDemux.h"
extern "C"{
#include <libavformat/avformat.h>
}

static double r2d(AVRational r){
    return r.num == 0 || r.den == 0 ? 0. : (double) r.num / (double) r.den;
}
void FFDemux::close() {
    ffdemux_mux.lock();
    if(avFormatContext){
        avformat_close_input(&avFormatContext);
    }
    ffdemux_mux.unlock();
}
bool FFDemux::seek(double pos) {
    if(pos<0 || pos>0){
        XLOGE("seek value must 0.0-1.0");
        return false;
    }
    bool re = false;
    ffdemux_mux.lock();
    if(!avFormatContext){
        ffdemux_mux.unlock();
        return false;
    }
    //清理读取的缓冲
    avformat_flush(avFormatContext);
    long long seekPts = 0;
    seekPts = avFormatContext->streams[videoStream]->duration * pos;

    //往后跳转到关键帧
    re = av_seek_frame(avFormatContext, videoStream, seekPts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
    ffdemux_mux.unlock();
    return re;
}
//打开文件，或者流媒体
bool FFDemux::open(const char *url){
    XLOGI("open file %s begin", url);
    close();
    ffdemux_mux.lock();
    //打开url
    int re = avformat_open_input(&avFormatContext, url, 0, 0);
    if(re != 0){
        ffdemux_mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("FFDemux open %s failed because %s", url, buf);
        return false;
    }
    XLOGI("FFDemux open %s success", url);
    //读取文件信息
    re = avformat_find_stream_info(avFormatContext, 0);
    if(re != 0){
        ffdemux_mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("avformat_find_stream_info %s failed because %s", url, buf);
        return false;
    }
    this->totalMs = avFormatContext->duration / (AV_TIME_BASE / 1000);
    XLOGI("totalMs = %lld", this->totalMs);
    ffdemux_mux.unlock();
//    getVPara();
//    getAPara();
    return true;
}

//获取视频参数
XParameter FFDemux::getVPara() {
    ffdemux_mux.lock();
    if(avFormatContext == nullptr){
        ffdemux_mux.unlock();
        XLOGE("getVPara failed avFormatContext == nullptr");
        return {};
    }
    int re = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO,-1, -1, nullptr, 0);
    if(re < 0){
        ffdemux_mux.unlock();
        XLOGE("av_find_best_stream TYPE_VIDEO failed!");
        return {};
    }
    videoStream = re;
    XParameter parameter;
    parameter.para = avFormatContext->streams[re]->codecpar;
    ffdemux_mux.unlock();
    return parameter;
}

XParameter FFDemux::getAPara() {
    ffdemux_mux.lock();
    if(avFormatContext == nullptr){
        ffdemux_mux.unlock();
        XLOGE("getAPara avFormatContext == nullptr");
        return {};
    }
    int re = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if(re < 0){
        ffdemux_mux.unlock();
        XLOGE("av_find_best_stream TYPE_AUDIO failed!");
        return {};
    }
    audioStream = re;
    XParameter parameter;
    parameter.para = avFormatContext->streams[re]->codecpar;
    parameter.channels = avFormatContext->streams[re]->codecpar->channels;
    parameter.sample_rate = avFormatContext->streams[re]->codecpar->sample_rate;
    ffdemux_mux.unlock();
    return parameter;
}

//读取一帧数据，数据由调用者清理
XData FFDemux::read(){
    ffdemux_mux.lock();
    if(avFormatContext == nullptr){
        ffdemux_mux.unlock();
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
            ffdemux_mux.unlock();
            return data;
        }
        ffdemux_mux.unlock();
        return {};
    }
    data.data = (unsigned char *) pkt;
    data.size = pkt->size;
    if(pkt->stream_index == audioStream){
        data.isAudio = true;
    } else if(pkt->stream_index == videoStream){
        data.isAudio = false;
    } else {
        av_packet_free(&pkt);
        ffdemux_mux.unlock();
        return {};
    }
    if (!data.isAudio) {
        XLOGI("isAudio= %d pack size is %d , pts is %lld", data.isAudio, pkt->size, pkt->pts);
    }

    //转换pts 为毫秒
    pkt->pts = pkt->pts * (1000 * r2d(avFormatContext->streams[pkt->stream_index]->time_base));
    pkt->dts = pkt->dts * (1000 * r2d(avFormatContext->streams[pkt->stream_index]->time_base));
    data.pts = (int) pkt->pts;
    ffdemux_mux.unlock();
    if (!data.isAudio) {
        XLOGI("isAudio= %d pack size is %d , pts2 is %lld", data.isAudio, pkt->size, pkt->pts);
    }
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