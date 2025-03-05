#ifndef  __TCP_CONNECTION_H_
#define  __TCP_CONNECTION_H_

#if defined(_WIN32)
#include <rtc_base/pc/sds.h>
#else
#include <rtc_base/yuan/sds.h>
#endif

#include "base/xhead.h"
#include "base/event_loop.h"

namespace xrtc {

class TcpConnection {
public:
    TcpConnection(int fd);
    ~TcpConnection();

public:
    int fd;
    char ip[64];
    int port;
    IOWatcher* io_watcher = nullptr;
    sds querybuf;
    size_t bytes_expected = XHEAD_SIZE;
    size_t bytes_processed = 0;
};

} // namespace xrtc

#endif  //__TCP_CONNECTION_H_


