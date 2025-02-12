/***************************************************************************
 * 
 * Copyright (c) 2022 str2num.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file signaling_server.h
 * @author str2num
 * @version $Revision$ 
 * @brief 
 *  
 **/



#ifndef  __SIGNALING_SERVER_H_
#define  __SIGNALING_SERVER_H_

#include <string>

namespace xrtc {

struct SignalingServerOptions {
    std::string host;
    int port;
    int worker_num;
    int connection_timeout;
};

class SignalingServer {
public:
    SignalingServer();
    ~SignalingServer();
    
    int init(const char* conf_file);

private:
    SignalingServerOptions _options;
};


} // namespace xrtc


#endif  //__SIGNALING_SERVER_H_


