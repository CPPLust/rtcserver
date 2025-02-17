
#ifndef  __BASE_SOCKET_H_
#define  __BASE_SOCKET_H_

#if defined(_WIN32)
#include <WS2tcpip.h>
#include <WinSock2.h>
#else
#include <sys/socket.h>
#endif

namespace xrtc {

class WinsockInitializer {
    public:
        WinsockInitializer() {
#if defined(_WIN32)
            // 初始化Winsock
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                RTC_LOG(LS_ERROR) << "Failed to initialize Winsock";
                exit(1);
            }
#endif
        }

        ~WinsockInitializer() {
#if defined(_WIN32)
            // 清理Winsock
            WSACleanup();
#endif
        }
#if defined(_WIN32)
    private:
        WSADATA wsaData;
#endif
    };

int create_tcp_server(const char* addr, int port);



} // namespace xrtc

#endif  //__BASE_SOCKET_H_


