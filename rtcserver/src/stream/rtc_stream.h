
#ifndef  __RTC_STREAM_H_
#define  __RTC_STREAM_H_

#include <string>
#include <memory>

#include <rtc_base/rtc_certificate.h>

#include "base/event_loop.h"
#include "pc/peer_connection.h"

namespace xrtc {

class RtcStream {
public:
	/*
* 实时音视频流的基类，负责单
个流的数据收发和质量控制
⚫ 内部成员peerconnection 维持一个webrtc p2p通信连接
*/
    RtcStream(EventLoop* el, PortAllocator* allocator, uint64_t uid, 
            const std::string& stream_name, bool audio, bool video, 
            uint32_t log_id);
    virtual ~RtcStream();
    
    int start(rtc::RTCCertificate* certificate);
    int set_remote_sdp(const std::string& sdp);

    virtual std::string create_offer() = 0;

protected:
    EventLoop* el;
    uint64_t uid;
    std::string stream_name;
    bool audio;
    bool video;
    uint32_t log_id;
    std::unique_ptr<PeerConnection> pc;
    friend class RtcStreamManager;
};

} // namespace xrtc

#endif  //__RTC_STREAM_H_


