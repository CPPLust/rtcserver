
#ifndef  __SESSION_DESCRIPTION_H_
#define  __SESSION_DESCRIPTION_H_

#include <string>

namespace xrtc {

enum class SdpType {
    k_offer = 0,//����������offer
    k_answer = -1, //���ص���answer
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


