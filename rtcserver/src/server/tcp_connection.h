#ifndef  __TCP_CONNECTION_H_
#define  __TCP_CONNECTION_H_

#if defined(_WIN32)
#include <rtc_base/pc/sds.h>
#else
#include <rtc_base/yuan/sds.h>
#endif
#include "base/NetStreamMaker.h"

#include "base/xhead.h"
#include "base/event_loop.h"
#include <rtc_base/slice.h>
#include <list>

namespace xrtc {

class TcpConnection {
public:
    
    //读取数据包的状态
    enum {
        STATE_HEAD = 0,
        STATE_BODY = 1
    };

    TcpConnection(int fd);
    ~TcpConnection();

public:
    int fd;
    char ip[64];
    int port;
    IOWatcher* io_watcher = nullptr;
    TimerWatcher* timer_watcher = nullptr;
    sds querybuf;

    //第一次可能需要读个头,之后出现头部数据才能知道期望值
    size_t bytes_expected = XHEAD_SIZE;
    //已经处理的数据长度
    size_t bytes_processed = 0;
    
    //当前读取的状态
    int current_state = STATE_HEAD;
    //最后一次收取的时间
    unsigned long last_interaction = 0;
    std::list<rtc::Slice> reply_list;
    size_t cur_resp_pos = 0;
};

} // namespace xrtc

#endif  //__TCP_CONNECTION_H_


