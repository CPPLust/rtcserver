#ifndef  __PEER_CONNECTION_H_
#define  __PEER_CONNECTION_H_

#include <string>
#include <memory>

#include <rtc_base/rtc_certificate.h>

#include "base/event_loop.h"
#include "pc/session_description.h"

namespace xrtc {

struct RTCOfferAnswerOptions {
    bool send_audio = true;
    bool send_video = true;
    bool recv_audio = true;
    bool recv_video = true;
	//由用户自己去决定是否使用bundle机制，实际就是通道复用
    bool use_rtp_mux = true;
	//是否rtp和rtcp一个通道
    bool use_rtcp_mux = true;
    bool dtls_on = true;
};
class PeerConnection {
public:
    PeerConnection(EventLoop* el);
    ~PeerConnection();
    
    int init(rtc::RTCCertificate* certificate);
    std::string create_offer(const RTCOfferAnswerOptions& options);

private:
    EventLoop* _el;

    std::unique_ptr<SessionDescription> _local_desc;
    rtc::RTCCertificate* _certificate = nullptr;
};

} // namespace xrtc

#endif  //__PEER_CONNECTION_H_


