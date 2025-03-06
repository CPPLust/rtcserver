
#ifndef  __XRTCSERVER_DEF_H_
#define  __XRTCSERVER_DEF_H_



#define CMDNO_PUSH      1 // ��ʼ����
#define CMDNO_PULL      2 // ��ʼ����
#define CMDNO_ANSWER    3 // ����answer
#define CMDNO_STOPPUSH 4 // ֹͣ����
#define CMDNO_STOPPULL 5 // ֹͣ����

namespace xrtc {

struct RtcMsg {
    int cmdno = -1;
    uint64_t uid = 0;
    std::string stream_name;
    int audio = 0;
    int video = 0;
};

} // namespace xrtc


#endif  //__XRTCSERVER_DEF_H_


