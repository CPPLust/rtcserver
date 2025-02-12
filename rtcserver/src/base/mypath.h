
#ifndef  __BASE_MYPATH_H_
#define  __BASE_MYPATH_H_

#include <string>

#if defined(_WIN32)
#define  MY_PATH_STRING "\\"
#else
#define  MY_PATH_STRING "/"
#endif

namespace xrtc {

 
    std::string get_bin_path();


} // namespace xrtc


#endif  //__BASE_MYPATH_H_


