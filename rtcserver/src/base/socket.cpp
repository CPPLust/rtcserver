#if !defined(_WIN32)
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <rtc_base/logging.h>

#include "base/socket.h"


#if defined(_WIN32)
#define  close closesocket
#endif

namespace xrtc {




int create_tcp_server(const char* addr, int port) {
    // 1. 创建socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        RTC_LOG(LS_WARNING) << "create socket error, errno: " << errno
            << ", error: " << strerror(errno);
        return -1;
    }

    // 2. 设置SO_REUSEADDR
    int on = 1;
#if defined(_WIN32)
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*) & on, sizeof(on));
#else
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif
    if (-1 == ret) {
        RTC_LOG(LS_WARNING) << "setsockopt SO_REUSEADDR error, errno: " << errno
            << ", error: " << strerror(errno);
		close_socket(sock);
        return -1;
    }

    // 3. 创建addr
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (addr && inet_pton(AF_INET,addr, &sa.sin_addr) == 0) {
        RTC_LOG(LS_WARNING) << "invalid address";;
		close_socket(sock);
        return -1;
    }
    
    // 4. bind
    ret = bind(sock, (struct sockaddr*)&sa, sizeof(sa));
    if (-1 == ret) {
        RTC_LOG(LS_WARNING) << "bind error, errno: " << errno
            << ", error: " << strerror(errno);
		close_socket(sock);
        return -1;
    }

    // 5. listen
    ret = listen(sock, 4095);
    if (-1 == ret) {
        RTC_LOG(LS_WARNING) << "listen error, errno: " << errno
            << ", error: " << strerror(errno);
		close_socket(sock);
        return -1;
    }

    return sock;
}
bool SetNonblocking(int fd)
{
#ifdef _WIN32
	unsigned long l = 1;
	int n = ioctlsocket(fd, FIONBIO, &l);
	if (n != 0)
	{
		//LogERR("errno = %d reason:%s %d\n", errno, strerror(errno), fd);
		return false;
	}
#else
	int get = fcntl(fd, F_GETFL);
	if (::fcntl(fd, F_SETFL, get | O_NONBLOCK) == -1)
	{
		//LogERR("errno = %d reason:%s %d\n", errno, strerror(errno), fd);
		return false;
	}

	//recv timeout
	struct timeval timeout = { 3, 0 };//3s
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#endif

#ifdef IOS
	int set = 1;
	setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif

	return true;
}

int create_udp_server(const char* addr, int port)
{
	// 1. 创建socket
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sock) {
		RTC_LOG(LS_WARNING) << "create socket error, errno: " << errno
			<< ", error: " << strerror(errno);
		return -1;
	}


	if (!SetNonblocking(sock))
	{
		//LogERR("Create socket, SetNonblocking fail! error code:%u, discrbie:%s", errno, strerror(errno));
		close_socket(sock);
		return false;
	}

	

	// 3. 创建addr
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	if (addr && inet_pton(AF_INET, addr, &sa.sin_addr) == 0) {
		RTC_LOG(LS_WARNING) << "invalid address";;
		close_socket(sock);
		return -1;
	}

	// 4. bind
	int ret = bind(sock, (struct sockaddr*)&sa, sizeof(sa));
	if (-1 == ret) {
		RTC_LOG(LS_WARNING) << "bind error, errno: " << errno
			<< ", error: " << strerror(errno);
		close_socket(sock);
		return -1;
	}

	return sock;
}

} // namespace xrtc


