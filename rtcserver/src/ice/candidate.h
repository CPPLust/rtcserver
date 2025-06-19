
#ifndef  __ICE_CANDIDATE_H_
#define  __ICE_CANDIDATE_H_

#include <string>

#include <rtc_base/socket_address.h>

#include "ice/ice_def.h"
namespace xrtc {

class Candidate {
public:
    uint32_t get_priority(uint32_t type_preference,
            int network_adapter_preference,
            int relay_preference);
    std::string to_string() const;

public:
    //是rtp还是rtcp
    IceCandidateComponent component; 
    //udp or tcp
    std::string protocol;
    //地址和端口
    rtc::SocketAddress address;
    //端口 address里也有port
    int port = 0;
    uint32_t priority;
    std::string username;
    std::string password;
    std::string type;
    /*
    * Foundation 是一个字符串，用于标识一组具有相同网络属性的候选地址。
    它的主要作用是在 ICE 协商过程中，帮助对等端（peer）识别哪些候选地址是基于相同的网络接口或路径生成的。
    具有相同 foundation 的候选地址通常意味着它们共享相似的网络特性，例如使用相同的 IP 地址、协议（UDP/TCP）等。
    */
    std::string foundation;
};

} // namespace xrtc

#endif  //__ICE_CANDIDATE_H_


