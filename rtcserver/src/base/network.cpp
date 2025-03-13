

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
    ULONG outBufLen = 15000;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    DWORD dwRetVal;

    do {
        if (pAddresses)
        {
            free(pAddresses);
            pAddresses = NULL;
        }
       
        pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);
        if (pAddresses == NULL) {
            RTC_LOG(LS_WARNING) << "Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n";
            return -1;
        }
        dwRetVal = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
    } while (dwRetVal == ERROR_BUFFER_OVERFLOW);

    if (dwRetVal != NO_ERROR) {
        RTC_LOG(LS_WARNING) << "Call to GetAdaptersAddresses failed with error: " << dwRetVal << "\n";
        free(pAddresses);
        return -1;
    }

    PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
    while (pCurrAddresses) {
        PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
        while (pUnicast != NULL) {
            SOCKADDR_IN* sockaddr_ipv4 = (SOCKADDR_IN*)pUnicast->Address.lpSockaddr;
            rtc::IPAddress ip_address(sockaddr_ipv4->sin_addr.S_un.S_addr);

            if (rtc::IPIsPrivateNetwork(ip_address) || rtc::IPIsLoopback(ip_address)) {
                continue;
            }
            //std::string networkname = std::string(pCurrAddresses->FriendlyName);
            std::string networkname;
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, &pCurrAddresses->FriendlyName[0], -1, NULL, 0, NULL, NULL);
            networkname.resize(size_needed - 1); // exclude null terminator
            WideCharToMultiByte(CP_UTF8, 0, &pCurrAddresses->FriendlyName[0], -1, &networkname[0], size_needed, NULL, NULL);

            Network* network = new Network(networkname, ip_address);

            RTC_LOG(LS_INFO) << "gathered network interface: " << network->to_string();

            _network_list.push_back(network);

            pUnicast = pUnicast->Next;
        }
        pCurrAddresses = pCurrAddresses->Next;
    }
    free(pAddresses);
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


