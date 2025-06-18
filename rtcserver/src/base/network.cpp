

#include <rtc_base/logging.h>
#include "base/network.h"
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#else
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace xrtc {

NetworkManager::NetworkManager() = default;

NetworkManager::~NetworkManager() {
    for (auto network : _network_list) {
        delete network;
    }

    _network_list.clear();
}

int NetworkManager::create_networks() {
#ifdef _WIN32

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    const char* addr = "192.168.10.71";
    if (addr && inet_pton(AF_INET, addr, &sa.sin_addr) == 0) {
        RTC_LOG(LS_WARNING) << "invalid address";;
        return -1;
    }
    rtc::IPAddress ip_address(sa.sin_addr);
    Network* network = new Network("wlan0", ip_address);

    RTC_LOG(LS_INFO) << "gathered network interface: " << network->to_string();

    _network_list.push_back(network);

#else
    struct ifaddrs* interface;
    int err = getifaddrs(&interface);
    if (err != 0) {
        RTC_LOG(LS_WARNING) << "getifaddrs error: " << strerror(errno) 
            << ", errno: " << errno;
        return -1;
    }
    
    for (auto cur = interface; cur != nullptr; cur = cur->ifa_next) {
        if (cur->ifa_addr->sa_family != AF_INET) {
            continue;
        }

        struct sockaddr_in* addr = (struct sockaddr_in*)(cur->ifa_addr);
        rtc::IPAddress ip_address(addr->sin_addr);
        
        if (rtc::IPIsPrivateNetwork(ip_address) || rtc::IPIsLoopback(ip_address)) {
            continue;
        }
        
        Network* network = new Network(cur->ifa_name, ip_address);

        RTC_LOG(LS_INFO) << "gathered network interface: " << network->to_string();

        _network_list.push_back(network);
    }
    
    freeifaddrs(interface);
#endif
    return 0;
}

} // namespace xrtc


