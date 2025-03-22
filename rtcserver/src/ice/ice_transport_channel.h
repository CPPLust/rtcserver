#ifndef  __ICE_TRANSPORT_CHANNEL_H_
#define  __ICE_TRANSPORT_CHANNEL_H_

#include <vector>
#include <string>

#include <rtc_base/third_party/sigslot/sigslot.h>
#include "base/event_loop.h"
#include "ice/ice_def.h"
#include "ice/port_allocator.h"
#include "ice/ice_credentials.h"
#include "ice/candidate.h"
#include "ice/stun.h"

namespace xrtc {

class UDPPort;

class IceTransportChannel : public sigslot::has_slots<> {
public:
    IceTransportChannel(EventLoop* el, PortAllocator* allocator,
            const std::string& transport_name,
            IceCandidateComponent component);
    virtual ~IceTransportChannel();
    
    std::string transport_name() { return _transport_name; }
    IceCandidateComponent component() { return _component; }
    
    void set_ice_params(const IceParameters& ice_params);
    void set_remote_ice_params(const IceParameters& ice_params);
    void gathering_candidate();
    
    std::string to_string();

    sigslot::signal2<IceTransportChannel*, const std::vector<Candidate>&>
        signal_candidate_allocate_done;

private:
    void _on_unknown_address(UDPPort* port,
        const rtc::SocketAddress& addr,
        StunMessage* msg,
        const std::string& remote_ufrag);

private:
    EventLoop* _el;
    std::string _transport_name;
    IceCandidateComponent _component;
    PortAllocator* _allocator;
    IceParameters _ice_params;
    IceParameters _remote_ice_params;
    std::vector<Candidate> _local_candidates;
};

} // namespace xrtc

#endif  //__ICE_TRANSPORT_CHANNEL_H_


