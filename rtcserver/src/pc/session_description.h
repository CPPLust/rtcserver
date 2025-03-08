
#ifndef  __SESSION_DESCRIPTION_H_
#define  __SESSION_DESCRIPTION_H_

#include <string>

namespace xrtc {

enum class SdpType {
    k_offer = 0,//推流来的是offer
    k_answer = -1, //返回的是answer
};

class SessionDescription {
public:
    SessionDescription(SdpType type);
    ~SessionDescription();
    
    std::string to_string();

private:
    SdpType _sdp_type;
};

} // namespace xrtc

#endif  //__SESSION_DESCRIPTION_H_


