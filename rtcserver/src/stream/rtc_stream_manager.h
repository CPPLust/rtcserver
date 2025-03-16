#ifndef  __RTC_STREAM_MANAGER_H_
#define  __RTC_STREAM_MANAGER_H_

#include <string>
#include <unordered_map>

#include <rtc_base/rtc_certificate.h>

#include "ice/port_allocator.h"
#include "base/event_loop.h"

namespace xrtc {

//管理所有的推流和拉流会话 
//控制流之间的数据转发 


class PushStream;

class RtcStreamManager {
public:
    RtcStreamManager(EventLoop* el);
    ~RtcStreamManager();
    
    int create_push_stream(uint64_t uid, const std::string& stream_name,
            bool audio, bool video, uint32_t log_id,
            rtc::RTCCertificate* certificate,
            std::string& offer);
    int set_answer(uint64_t uid, const std::string& stream_name,
            const std::string& answer, const std::string& stream_type, 
            uint32_t log_id);

    PushStream* find_push_stream(const std::string& stream_name);

private:
    EventLoop* _el;
    std::unordered_map<std::string, PushStream*> _push_streams;
    std::unique_ptr<PortAllocator> _allocator;
};

} // namespace xrtc


#endif  //__RTC_STREAM_MANAGER_H_


