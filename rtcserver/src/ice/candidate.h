
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
    //��rtp����rtcp
    IceCandidateComponent component; 
    //udp or tcp
    std::string protocol;
    //��ַ�Ͷ˿�
    rtc::SocketAddress address;
    //�˿� address��Ҳ��port
    int port = 0;
    uint32_t priority;
    std::string username;
    std::string password;
    std::string type;
    /*
    * Foundation ��һ���ַ��������ڱ�ʶһ�������ͬ�������Եĺ�ѡ��ַ��
    ������Ҫ�������� ICE Э�̹����У������Եȶˣ�peer��ʶ����Щ��ѡ��ַ�ǻ�����ͬ������ӿڻ�·�����ɵġ�
    ������ͬ foundation �ĺ�ѡ��ַͨ����ζ�����ǹ������Ƶ��������ԣ�����ʹ����ͬ�� IP ��ַ��Э�飨UDP/TCP���ȡ�
    */
    std::string foundation;
};

} // namespace xrtc

#endif  //__ICE_CANDIDATE_H_


