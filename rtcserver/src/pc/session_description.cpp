

#include <sstream>

#include "pc/session_description.h"

namespace xrtc {

//跟浏览器打交道必须加密
const char k_media_protocol_dtls_savpf[] = "UDP/TLS/RTP/SAVPF";

const char k_meida_protocol_savpf[] = "RTP/SAVPF";
AudioContentDescription::AudioContentDescription() {
    //添加音频的编解码信息
    auto codec = std::make_shared<AudioCodecInfo>();
    codec->id = 111;
    codec->name = "opus";
    codec->clockrate = 48000;
    codec->channels = 2;
    //Transport-wide Congestion Control  传输拥塞控制
    // add feedback param
    codec->feedback_param.push_back(FeedbackParam("transport-cc"));

    _codecs.push_back(codec);
}
VideoContentDescription::VideoContentDescription() {
    auto codec = std::make_shared<VideoCodecInfo>();
    codec->id = 107;
    codec->name = "H264";
    codec->clockrate = 90000;
    _codecs.push_back(codec);

    // add feedback param
    //基于接收端估计的最大比特率的反馈机制
    codec->feedback_param.push_back(FeedbackParam("goog-remb"));
    codec->feedback_param.push_back(FeedbackParam("transport-cc"));
    //ccm Codec Control Message Fast Image Refresh  i帧请求
    codec->feedback_param.push_back(FeedbackParam("ccm", "fir"));

    codec->feedback_param.push_back(FeedbackParam("nack"));
    //Picture Loss Indication
    codec->feedback_param.push_back(FeedbackParam("nack", "pli"));
    //fir 和 pli 的区别在于fir 必须发I帧， pli是告诉我丢了一些帧， 建议你发i帧
	
    //重传包 一种新的类型
    auto rtx_codec = std::make_shared<VideoCodecInfo>();
    rtx_codec->id = 99;
    rtx_codec->name = "rtx";
    rtx_codec->clockrate = 90000;
    _codecs.push_back(rtx_codec);
}
bool ContentGroup::has_content_name(const std::string& content_name) {
    for (auto name : _content_names) {
        if (name == content_name) {
            return true;
        }
    }

    return false;
}
void ContentGroup::add_content_name(const std::string& content_name) {
    if (!has_content_name(content_name)) {
        _content_names.push_back(content_name);
    }
}
SessionDescription::SessionDescription(SdpType type) :
    _sdp_type(type)
{
}

SessionDescription::~SessionDescription() {

}

void SessionDescription::add_content(std::shared_ptr<MediaContentDescription> content) {
    _contents.push_back(content);
}
void SessionDescription::add_group(const ContentGroup& group) {
    _content_groups.push_back(group);
}
std::vector<const ContentGroup*> SessionDescription::get_group_by_name(
        const std::string& name) const
{
    std::vector<const ContentGroup*> content_groups;
    for (const ContentGroup& group : _content_groups) {
        if (group.semantics() == name) {
            content_groups.push_back(&group);
        }
    }

    return content_groups;
}
static void add_rtcp_fb_line(std::shared_ptr<CodecInfo> codec,
        std::stringstream& ss)
{
    for (auto param : codec->feedback_param) {
        ss << "a=rtcp-fb:" << codec->id << " " << param.id();
        if (!param.param().empty()) {
            ss << " " << param.param();
        }
        ss << "\r\n";
    }
}
static void build_rtp_map(std::shared_ptr<MediaContentDescription> content,
        std::stringstream& ss)
{
    for (auto codec : content->get_codecs()) {
        ss << "a=rtpmap:" << codec->id << " " << codec->name << "/" << codec->clockrate;
        if (MediaType::MEDIA_TYPE_AUDIO == content->type()) {
            auto audio_codec = codec->as_audio();
            ss << "/" << audio_codec->channels;
        }
        ss << "\r\n";

        add_rtcp_fb_line(codec, ss); //每个m行都要增加rtcp feedback， 也就是每个codec
        //add_fmtp_line(codec, ss);
    }
}
std::string SessionDescription::to_string() {
    std::stringstream ss;
    // version 这是版本
    ss << "v=0\r\n";
	// session origin
	// RFC 4566
	// o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
	ss << "o=- 0 2 IN IP4 127.0.0.1\r\n";
	// session name
	ss << "s=-\r\n";
	// time description
	ss << "t=0 0\r\n";
    
    // BUNDLE
    //a=group:BUNDLE audio video
    std::vector<const ContentGroup*> content_group = get_group_by_name("BUNDLE");
    if (!content_group.empty()) {
        ss << "a=group:BUNDLE";
        for (auto group : content_group) {
            for (auto content_name : group->content_names()) {
                ss << " " << content_name;
            }
        }
        //最后追加一个\r\n
        ss << "\r\n";
    }
    ss << "a=msid-semantic: WMS\r\n";

    for (auto content : _contents) {
        // RFC 4566
        // m=<media> <port> <proto> <fmt>
        std::string fmt;
        for (auto codec : content->get_codecs()) {
            fmt.append(" ");
            fmt.append(std::to_string(codec->id));
        }
        //m=audio 9 UDP/TLS/RTP/SAVPF 111
        //m=video 9 UDP/TLS/RTP/SAVPF 107 99
        ss << "m=" << content->mid() << " 9 " << k_media_protocol_dtls_savpf
            << fmt << "\r\n";

        ss << "c=IN IP4 0.0.0.0\r\n";
        ss << "a=rtcp:9 IN IP4 0.0.0.0\r\n";

        build_rtp_map(content, ss);
    }





    return ss.str();
}

} // namespace xrtc




