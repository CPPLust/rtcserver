#ifndef  __ICE_AGENT_H_
#define  __ICE_AGENT_H_

#include <vector>

#include "base/event_loop.h"
#include "ice/ice_transport_channel.h"

//主要为了candadite
namespace xrtc {

class IceAgent {
public:
    IceAgent(EventLoop* el);
    ~IceAgent();
    
    //创建channel的方法  transport_name 是针对video/audio
    //component 是针对rtp/rtcp
    //rfc5245
    bool create_channel(EventLoop* el, const std::string& transport_name,
            IceCandidateComponent component);
    
    IceTransportChannel* get_channel(const std::string& transport_name,
            IceCandidateComponent component);
    void gathering_candidate();

private:
    std::vector<IceTransportChannel*>::iterator _get_channel(
            const std::string& transport_name,
            IceCandidateComponent component);

private:
    EventLoop* _el;
    //所有的channel
    std::vector<IceTransportChannel*> _channels;
};

} // namespace xrtc

#endif  //__ICE_AGENT_H_


