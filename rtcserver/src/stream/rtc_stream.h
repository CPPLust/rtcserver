
#ifndef  __RTC_STREAM_H_
#define  __RTC_STREAM_H_

#include <string>
#include <memory>

#include <rtc_base/rtc_certificate.h>

#include "base/event_loop.h"
#include "pc/peer_connection.h"

namespace xrtc {

class RtcStream;

enum class RtcStreamType {
    k_push,
    k_pull
};

class RtcStreamListener {
public:
    //监听状态使用
    virtual void on_connection_state(RtcStream* stream, PeerConnectionState state) = 0;
};
	
	/*
* 实时音视频流的基类，负责单
个流的数据收发和质量控制
⚫ 内部成员peerconnection 维持一个webrtc p2p通信连接
*/
class RtcStream : public sigslot::has_slots<> {
public:
    RtcStream(EventLoop* el, PortAllocator* allocator, uint64_t uid, 
            const std::string& stream_name, bool audio, bool video, 
            uint32_t log_id);
    virtual ~RtcStream();
    
    int start(rtc::RTCCertificate* certificate);
    int set_remote_sdp(const std::string& sdp);
    void register_listener(RtcStreamListener* listener) { _listener = listener; }

    virtual std::string create_offer() = 0;
    virtual RtcStreamType stream_type() = 0;
    
    uint64_t get_uid() { return uid; }
    const std::string& get_stream_name() { return stream_name; }
    
    std::string to_string();

private:
    void _on_connection_state(PeerConnection*, PeerConnectionState);
protected:
    EventLoop* el;
    uint64_t uid;
    std::string stream_name;
    bool audio;
    bool video;
    uint32_t log_id;

    PeerConnection* pc;
    PeerConnectionState _state = PeerConnectionState::k_new;
    RtcStreamListener* _listener = nullptr;

    friend class RtcStreamManager;
};

} // namespace xrtc

#endif  //__RTC_STREAM_H_


