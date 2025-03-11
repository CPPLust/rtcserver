#ifndef  __ICE_CREDENTIALS_H_
#define  __ICE_CREDENTIALS_H_

#include <string>

namespace xrtc {

class IceParameters {
public:

    IceParameters() = default;
    IceParameters(const std::string& ufrag, const std::string& pwd) :
        ice_ufrag(ufrag), ice_pwd(pwd) {}

    std::string ice_ufrag; //ice实际探测过程中使用的用户名  在ifc 5245
    std::string ice_pwd; //密码
};

class IceCredentials {
public:
    static IceParameters create_random_ice_credentials();
};

} // namespace xrtc


#endif  //__ICE_CREDENTIALS_H_


