﻿#ifndef  __PEER_CONNECTION_H_
#define  __PEER_CONNECTION_H_

#include <string>
#include <memory>

#include <rtc_base/rtc_certificate.h>

#include "base/event_loop.h"
#include "pc/session_description.h"
#include "pc/transport_controller.h"

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
class PeerConnection : public sigslot::has_slots<> {
public:
    PeerConnection(EventLoop* el, PortAllocator* allocator);
    ~PeerConnection();

    int init(rtc::RTCCertificate* certificate);
    void destroy();
    std::string create_offer(const RTCOfferAnswerOptions& options);
    int set_remote_sdp(const std::string& sdp);
    
    sigslot::signal2<PeerConnection*, PeerConnectionState> signal_connection_state;

private:
   
    void on_candidate_allocate_done(TransportController* transport_controller,
            const std::string& transport_name,
            IceCandidateComponent component,
            const std::vector<Candidate>& candidate);
    
    void _on_connection_state(TransportController*, PeerConnectionState state);

    friend void destroy_timer_cb(EventLoop* el, TimerWatcher* w, void* data);

private:
    EventLoop* _el;
    std::unique_ptr<SessionDescription> _local_desc;
    std::unique_ptr<SessionDescription> _remote_desc;
    rtc::RTCCertificate* _certificate = nullptr;
    std::unique_ptr<TransportController> _transport_controller;
    TimerWatcher* _destroy_timer = nullptr;
};

} // namespace xrtc

#endif  //__PEER_CONNECTION_H_


