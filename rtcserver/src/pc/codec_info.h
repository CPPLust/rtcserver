#ifndef  __CODEC_INFO_H_
#define  __CODEC_INFO_H_

#include <string>

namespace xrtc {

class CodecInfo {
public:
    int id; //id  webrtc pt  payload type id
    std::string name; //����
    int clockrate; //Ƶ��
};

class AudioCodecInfo : public CodecInfo {
public:
    //��Ƶ���е�������
    int channels;
};

class VideoCodecInfo : public CodecInfo {
};

} // namespace xrtc

#endif  //__CODEC_INFO_H_


