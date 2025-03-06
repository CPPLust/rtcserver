
#include "server/tcp_connection.h"
#include <string.h>
namespace xrtc {

	TcpConnection::TcpConnection(int fd) 
			: fd(fd)
			, querybuf(new CNetStreamMaker())
	{
		memset(ip, 0, 64);
		port = 0;
	}

TcpConnection::~TcpConnection() {
	if (querybuf)
	{
		delete querybuf;
		querybuf = NULL;
	}
}

} // namespace xrtc



















