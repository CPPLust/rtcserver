#ifndef  __CODEC_INFO_H_
#define  __CODEC_INFO_H_

#include <string>

namespace xrtc {

class CodecInfo {
public:
    int id; //id  webrtc pt  payload type id
    std::string name; //名字
    int clockrate; //频率
};

class AudioCodecInfo : public CodecInfo {
public:
    //音频独有的声道数
    int channels;
};

class VideoCodecInfo : public CodecInfo {
};

} // namespace xrtc

#endif  //__CODEC_INFO_H_


