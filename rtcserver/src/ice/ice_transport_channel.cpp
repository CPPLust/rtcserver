﻿#include <rtc_base/logging.h>
#include "ice/udp_port.h"
#include "ice/ice_transport_channel.h"
#include "ice/ice_connection.h"

namespace xrtc {

IceTransportChannel::IceTransportChannel(EventLoop* el, 
        PortAllocator* allocator,
        const std::string& transport_name,
        IceCandidateComponent component) :
    _el(el),
    _transport_name(transport_name),
    _component(component),
    _allocator(allocator)
{
    RTC_LOG(LS_INFO) << "ice transport channel created, transport_name: " << _transport_name
        << ", component: " << _component;
}

IceTransportChannel::~IceTransportChannel() {
}

void IceTransportChannel::set_ice_params(const IceParameters& ice_params) {
    RTC_LOG(LS_INFO) << "set ICE param, transport_name: " << _transport_name
        << ", component: " << _component
        << ", ufrag: " << ice_params.ice_ufrag
        << ", pwd: " << ice_params.ice_pwd;
    _ice_params = ice_params;
}

void IceTransportChannel::set_remote_ice_params(const IceParameters& ice_params) {
    RTC_LOG(LS_INFO) << "set remote ICE param, transport_name: " << _transport_name
        << ", component: " << _component
        << ", ufrag: " << ice_params.ice_ufrag
        << ", pwd: " << ice_params.ice_pwd;
    _remote_ice_params = ice_params;
}

void IceTransportChannel::gathering_candidate() {
    if (_ice_params.ice_ufrag.empty() || _ice_params.ice_pwd.empty()) {
        RTC_LOG(LS_WARNING) << "cannot gathering candidate because ICE param is empty"
            << ", transport_name: " << _transport_name
            << ", component: " << _component
            << ", ufrag: " << _ice_params.ice_ufrag
            << ", pwd: " << _ice_params.ice_pwd;
        return;
    }
    auto network_list = _allocator->get_networks();
    if (network_list.empty()) {
        RTC_LOG(LS_WARNING) << "cannot gathering candidate because network_list is empty"
            << ", transport_name: " << _transport_name
            << ", component: " << _component;
        return;
    }

    for (auto network : network_list) {
        UDPPort* port = new UDPPort(_el, _transport_name, _component, _ice_params);
        port->signal_unknown_address.connect(this,
                &IceTransportChannel::_on_unknown_address);

        Candidate c;
        int ret = port->create_ice_candidate(network, _allocator->min_port(), 
                _allocator->max_port(), c);
        if (ret != 0) {
            continue;
        }
        _local_candidates.push_back(c);
    }
    signal_candidate_allocate_done(this, _local_candidates);
}

void IceTransportChannel::_on_unknown_address(UDPPort* port,
        const rtc::SocketAddress& addr,
        StunMessage* msg,
        const std::string& remote_ufrag)
{
    const StunUInt32Attribute* priority_attr = msg->get_uint32(STUN_ATTR_PRIORITY);
    if (!priority_attr) {
        RTC_LOG(LS_WARNING) << to_string() << ": priority not found in the"
            << " binding request message, remote_addr: " << addr.ToString();
        port->send_binding_error_response(msg, addr, STUN_ERROR_BAD_REQUEST,
                STUN_ERROR_REASON_BAD_REQUEST);
        return;
    }

    uint32_t remote_priority = priority_attr->value();
    Candidate remote_candidate;
    remote_candidate.component = _component;
    remote_candidate.protocol = "udp";
    remote_candidate.address = addr;
    remote_candidate.username = remote_ufrag;
    remote_candidate.password = _remote_ice_params.ice_pwd;  //这个是sdp交换过来的
    remote_candidate.priority = remote_priority;
    remote_candidate.type = PRFLX_PORT_TYPE;

    RTC_LOG(LS_INFO) << to_string() << ": create peer reflexive candidate: "
        << remote_candidate.to_string();

    //这个conn 关联了本地的connection 和远端的connection
    IceConnection* conn = port->create_connection(remote_candidate);
    if (!conn) {
        RTC_LOG(LS_WARNING) << to_string() << ": create connection from "
            << " peer reflexive candidate error, remote_addr: "
            << addr.ToString();
        port->send_binding_error_response(msg, addr, STUN_ERROR_SERVER_ERROR,
                STUN_ERROR_REASON_SERVER_ERROR);
        return;
    }

    //对端的外网地址 也叫反射地址
    RTC_LOG(LS_INFO) << to_string() << ": create connection from "
        << " peer reflexive candidate success, remote_addr: "
        << addr.ToString();

    conn->handle_stun_binding_request(msg);
}

std::string IceTransportChannel::to_string() {
    std::stringstream ss;
    ss << "Channel[" << this << ":" << _transport_name << ":" << _component
        << "]";
    return ss.str();
}
} // namespace xrtc


