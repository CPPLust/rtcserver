#ifndef  __ICE_AGENT_H_
#define  __ICE_AGENT_H_

#include <vector>

#include "base/event_loop.h"
#include "ice/ice_transport_channel.h"

//��ҪΪ��candadite
namespace xrtc {

class IceAgent {
public:
    IceAgent(EventLoop* el);
    ~IceAgent();
    
    //����channel�ķ���  transport_name �����video/audio
    //component �����rtp/rtcp
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
    //���е�channel
    std::vector<IceTransportChannel*> _channels;
};

} // namespace xrtc

#endif  //__ICE_AGENT_H_


