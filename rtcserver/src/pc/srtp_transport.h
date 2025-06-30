
#ifndef  __SRTP_TRANSPORT_H_
#define  __SRTP_TRANSPORT_H_

namespace xrtc {

class SrtpTransport {
public:
    SrtpTransport(bool rtcp_mux_enabled);
    virtual ~SrtpTransport() = default;

private:
    bool _rtcp_mux_enabled;
};

} // namespace xrtc

#endif  //__SRTP_TRANSPORT_H_


