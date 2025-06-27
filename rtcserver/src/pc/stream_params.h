


#ifndef  __PC_STREAM_PARAMS_H_
#define  __PC_STREAM_PARAMS_H_

#include <vector>
#include <string>

//ssrc 遵循 rfc5576
namespace xrtc {

struct SsrcGroup {
    SsrcGroup(const std::string& semantics, const std::vector<uint32_t>& ssrcs);

    std::string semantics;
    std::vector<uint32_t> ssrcs;
};

struct StreamParams {
    bool has_ssrc(uint32_t ssrc);

    std::string id; 
    std::vector<uint32_t> ssrcs; //ssrcids
    std::vector<SsrcGroup> ssrc_groups; //相关的组
    std::string cname;
    std::string stream_id;
};

} // namespace xrtc


#endif  //__PC_STREAM_PARAMS_H_


