﻿#include "xrtc/media/base/xrtc_pusher.h"

#include <rtc_base/logging.h>
#include <rtc_base/task_utils/to_queued_task.h>
#include <rtc_base/string_encode.h>

#include "xrtc/base/xrtc_global.h"
#include "xrtc/media/chain/xrtc_push_stream.h"

namespace xrtc {
   
XRTCPusher::XRTCPusher(IVideoSource* video_source) :
    video_source_(video_source),
    current_thread_(rtc::Thread::Current())
{
}

XRTCPusher::~XRTCPusher() {

}

void XRTCPusher::StartPush(const std::string& url) {
    RTC_LOG(LS_INFO) << "XRTCPusher StartPush call";
    current_thread_->PostTask(webrtc::ToQueuedTask([=]() {
        RTC_LOG(LS_INFO) << "XRTCPusher StartPush PostTask";

        // xrtc://www.str2num.com/push?uid=xxx&streamName=xxx
        url_ = url;
        // 解析url中的协议，根据协议创建具体的处理链条
        std::vector<std::string> fields;
        rtc::tokenize(url_, ':', &fields);
        if (fields.size() < 2) {
            RTC_LOG(LS_WARNING) << "invalid url: " << url;
            if (XRTCGlobal::Instance()->engine_observer()) {
                XRTCGlobal::Instance()->engine_observer()->OnPushFailed(
                    this, XRTCError::kPushInvalidUrlErr);
            }
            return;
        }

        std::string protocol = fields[0];
        if ("xrtc" == protocol) {
            media_chain_ = std::make_unique<XRTCPushStream>(this, video_source_);
            media_chain_->Start();
        }

    }));
}

void XRTCPusher::StopPush() {
    RTC_LOG(LS_INFO) << "XRTCPusher StopPush call";
    current_thread_->PostTask(webrtc::ToQueuedTask([=]() {
        RTC_LOG(LS_INFO) << "XRTCPusher StopPush PostTask";
        if (media_chain_) {
            media_chain_->Stop();
        }
    }));
}

void XRTCPusher::Destroy() {
    RTC_LOG(LS_INFO) << "XRTCPusher Destroy call";
    current_thread_->PostTask(webrtc::ToQueuedTask([=]() {
        RTC_LOG(LS_INFO) << "XRTCPusher Destroy PostTask";
        delete this;
    }));
}

} // namespace xrtc