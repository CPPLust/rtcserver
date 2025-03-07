
#include "server/tcp_connection.h"
#include <string.h>
namespace xrtc {

	TcpConnection::TcpConnection(int fd) 
    :fd(fd),
    querybuf(sdsempty())
	{
		memset(ip, 0, 64);
		port = 0;
	}

TcpConnection::~TcpConnection() {
    sdsfree(querybuf);
}

} // namespace xrtc



















