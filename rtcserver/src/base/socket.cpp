#if !defined(_WIN32)
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <rtc_base/logging.h>

#include "base/socket.h"


#if defined(_WIN32)
#define  close closesocket
#else
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
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

int generic_accept(int sock, struct sockaddr* sa, socklen_t* len) {
    int fd = -1;

    while (1) {
        fd = accept(sock, sa, len);
        if (-1 == fd) {
            if (EINTR == errno) {
                continue;
            } else {
                RTC_LOG(LS_WARNING) << "tcp accept error: " << strerror(errno)
                    << ", errno: " << errno;
                return -1;
            }
        }

        break;
    }

    return fd;
}

int tcp_accept(int sock, char* host, int* port) {
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);
    int fd = generic_accept(sock, (struct sockaddr*)&sa, &salen);
    if (-1 == fd) {
        return -1;
    }
    
    if (host) {
        if (inet_ntop(AF_INET, &(sa.sin_addr), host, INET_ADDRSTRLEN) == NULL) {
            perror("inet_ntop failed");
            // 错误处理
        }
        else {
            printf("Host IP: %s\n", host);
        }
        //strcpy(host, inet_ntoa(sa.sin_addr));
    }

    if (port) {
        *port = ntohs(sa.sin_port);
    }

    return fd;
}
int sock_setnonblock(int sock) 
{
#ifdef _WIN32
	unsigned long l = 1;
	int n = ioctlsocket(sock, FIONBIO, &l);
	if (n != 0)
	{
		//LogERR("errno = %d reason:%s %d\n", errno, strerror(errno), fd);
		return false;
	}
#else
    int flags = fcntl(sock, F_GETFL);
    if (-1 == flags) {
        RTC_LOG(LS_WARNING) << "fcntl(F_GETFL) error: " << strerror(errno)
            << ", errno: " << errno << ", fd: " << sock;
        return -1;
    }

    if (-1 == fcntl(sock, F_SETFL, flags | O_NONBLOCK)) {
        RTC_LOG(LS_WARNING) << "fcntl(F_SETFL) error: " << strerror(errno)
            << ", errno: " << errno << ", fd: " << sock;
        return -1;
    }
#endif
    return 0;
}

int sock_setnodelay(int sock) {
    int yes = 1;
    int ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*) & yes, sizeof(yes));
    if (-1 == ret) {
        RTC_LOG(LS_WARNING) << "set nodelay error: " << strerror(errno)
            << ", errno: " << errno << ", fd: " << sock;
        return -1;
    }
    return 0;
}
int sock_peer_to_str(int sock, char* ip, int* port) {
    struct sockaddr_in sa;
    socklen_t salen;
    
    int ret = getpeername(sock, (struct sockaddr*)&sa, &salen);
    if (-1 == ret) {
        if (ip) {
            ip[0] = '?';
            ip[1] = '\0';
        }

        if (port) {
            *port = 0;
        }

        return -1;
    }

    if (ip) {
        //strcpy(ip, inet_ntoa(sa.sin_addr));
		if (inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN) == NULL) {
			perror("inet_ntop failed");
			// 错误处理
		}
		else {
			printf("Host IP: %s\n", ip);
		}
    }

    if (port) {
        *port = ntohs(sa.sin_port);
    }

    return 0;
}
int sock_read_data(int sock, char* buf, size_t len) {
#ifdef _WIN32
    int nread = recv(sock, buf, static_cast<int>(len), 0);
    if (nread == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            nread = 0;
        }
        else {
            RTC_LOG(LS_WARNING) << "sock read failed, error: " << strerror(WSAGetLastError())
                << ", errno: " << WSAGetLastError() << ", fd: " << sock;
            return -1;
        }
    }
    else if (nread == 0) {
        RTC_LOG(LS_WARNING) << "connection closed, fd: " << sock;
        return -1;
    }
#else
    int nread = read(sock, buf, len);
    if (-1 == nread) {
        if (EAGAIN == errno || EWOULDBLOCK == errno) {
            nread = 0;
        }
        else {
            RTC_LOG(LS_WARNING) << "sock read failed, error: " << strerror(errno)
                << ", errno: " << errno << ", fd: " << sock;
            return -1;
        }
    }
    else if (0 == nread) {
        RTC_LOG(LS_WARNING) << "connection closed, fd: " << sock;
        return -1;
    }
#endif


    
    return nread;
}
} // namespace xrtc


