#include "stream/rtc_stream_manager.h"
#include "stream/push_stream.h"


namespace xrtc {

RtcStreamManager::RtcStreamManager(EventLoop* el) :_el(el)
{

}

RtcStreamManager::~RtcStreamManager() {
}

PushStream* RtcStreamManager::find_push_stream(const std::string& stream_name) {
    auto iter = _push_streams.find(stream_name);
    if (iter != _push_streams.end()) {
        return iter->second;
    }

    return nullptr;
}
int RtcStreamManager::create_push_stream(uint64_t uid, const std::string& stream_name,
        bool audio, bool video, uint32_t log_id,
        std::string& offer)
{
    PushStream* stream = find_push_stream(stream_name);
    if (stream) {
        _push_streams.erase(stream_name);
        delete stream;
    }

    stream = new PushStream(_el, uid, stream_name,
            audio, video, log_id);
    offer = stream->create_offer();
    return 0;
}

} // namespace xrtc


