﻿
#include <rtc_base/logging.h>

#include "stream/rtc_stream.h"

namespace xrtc {

RtcStream::RtcStream(EventLoop* el, PortAllocator* allocator,
        uint64_t uid, const std::string& stream_name,
        bool audio, bool video, uint32_t log_id):
    el(el), uid(uid), stream_name(stream_name), audio(audio),
    video(video), log_id(log_id),
    pc(new PeerConnection(el, allocator))
{
    //底层的消息变化告诉流
    pc->signal_connection_state.connect(this, &RtcStream::_on_connection_state);
}

RtcStream::~RtcStream() {
    pc->destroy();
}

void RtcStream::_on_connection_state(PeerConnection*, PeerConnectionState state) {
    if (_state == state) {
        return;
    }

    RTC_LOG(LS_INFO) << to_string() << ": PeerConnectionState change from " << _state
        << " to " << state;
    _state = state;

    if (_listener) {
        _listener->on_connection_state(this, state);
    }
}

int RtcStream::start(rtc::RTCCertificate* certificate) {
    return pc->init(certificate);
}

int RtcStream::set_remote_sdp(const std::string& sdp) {
    return pc->set_remote_sdp(sdp);
}

std::string RtcStream::to_string() {
    std::stringstream ss;
    ss << "Stream[" << this << "|" << uid << "|" << stream_name << "]";
    return ss.str();
}

} // namespace xrtc


