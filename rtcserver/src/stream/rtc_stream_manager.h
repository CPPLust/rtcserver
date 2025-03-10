#ifndef  __RTC_STREAM_MANAGER_H_
#define  __RTC_STREAM_MANAGER_H_

#include <string>
#include <unordered_map>

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
            std::string& offer);
    PushStream* find_push_stream(const std::string& stream_name);

private:
    EventLoop* _el;
    std::unordered_map<std::string, PushStream*> _push_streams;
};

} // namespace xrtc


#endif  //__RTC_STREAM_MANAGER_H_


