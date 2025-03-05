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
    
    //��ȡ���ݰ���״̬
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
    //��һ�ο�����Ҫ����ͷ,֮�����ͷ�����ݲ���֪������ֵ
    size_t bytes_expected = XHEAD_SIZE;
    //�Ѿ���������ݳ���
    size_t bytes_processed = 0;
    
    //��ǰ��ȡ��״̬
    int current_state = STATE_HEAD;
    unsigned long last_interaction = 0;
};

} // namespace xrtc

#endif  //__TCP_CONNECTION_H_


