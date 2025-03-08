
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
    //兜个底  某些情况下可能没有释放
    while (!reply_list.empty()) {
        rtc::Slice reply = reply_list.front();
        free((void*)reply.data());
        reply_list.pop_front();
    }

    reply_list.clear();
}

} // namespace xrtc



















