//
// Created by lyhao on 21-1-20.
//

#include "FFDecode.h"
#include "XLog.h"

extern "C"{
#include "libavcodec/avcodec.h"
}

bool FFDecode::open(XParameter para) {
    if(para.para == nullptr){
        return false;
    }
    AVCodecParameters *p = para.para;
    // 查找解码器
    AVCodec *cd = avcodec_find_decoder(p->codec_id);
    if(cd == nullptr){
        XLOGE("avcodec_find_decoder %d failed", p->codec_id);
        return false;
    }
    // 创建解码上下文，并复制参数
    codecContext = avcodec_alloc_context3(cd);
    avcodec_parameters_to_context(codecContext, p);

    // 打开解码器
    int re = avcodec_open2(codecContext, 0, 0);
    if(re != 0){
        char buf[1024] = {0};
        av_strerror(re,buf,sizeof(buf)-1);
        XLOGE("avcodec_open2 failed = %s",buf);
        return false;
    }
    XLOGI("avcodec_open2 success!");
    return true;
}