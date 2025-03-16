
#ifndef  __XRTCSERVER_DEF_H_
#define  __XRTCSERVER_DEF_H_



#define CMDNO_PUSH      1 // 开始推流
#define CMDNO_PULL      2 // 开始拉流
#define CMDNO_ANSWER    3 // 发送answer
#define CMDNO_STOPPUSH 4 // 停止推流
#define CMDNO_STOPPULL 5 // 停止拉流

#define MAX_RES_BUF 4096

#include <stdint.h>
#include <string.h>

namespace xrtc {

struct RtcMsg {
    int cmdno = -1;
    uint64_t uid = 0;
    std::string stream_name;
    std::string stream_type;
    int audio = 0;
    int video = 0;
    uint32_t log_id = 0;
    void* worker = nullptr;  //对应的要使用的signaling worker
    void* conn = nullptr;    //哪个tcp的传输通道
    int fd = 0;              //它的目的就是回传时检查tcpconnection 和 conn一起考虑
    std::string sdp;        //创建回填的offer sdp
    int err_no = 0;          //如果有错误返回错误
    void* certificate = nullptr;
};

} // namespace xrtc


#endif  //__XRTCSERVER_DEF_H_


