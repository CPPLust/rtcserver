
#include <rtc_base/logging.h>
#include "server/signaling_worker.h"
#include "server/rtc_worker.h"

namespace xrtc {

void rtc_worker_recv_notify(EventLoop* /*el*/, IOWatcher* /*w*/, int fd, 
        int /*events*/, void* data)
{
    int msg;
#if defined(_WIN32)
	struct  sockaddr_in from;
	socklen_t len = sizeof(from);
	int readlen = ::recvfrom(fd, (char*)&msg, sizeof(int), 0, (struct sockaddr*)&from, &len);
	if (readlen != sizeof(int)) {
		RTC_LOG(LS_WARNING) << "read from fd error: " << strerror(errno)
			<< ", errno: " << errno;
		return;
	}
#else
	if (read(fd, &msg, sizeof(int)) != sizeof(int))
	{
		RTC_LOG(LS_WARNING) << "read from pipe error:" << strerror(errno)
			<< ", errno: " << errno;

		return;
	}

#endif

    RtcWorker* worker = (RtcWorker*)data;
    worker->_process_notify(msg);
}

RtcWorker::RtcWorker(int worker_id, const RtcServerOptions& options) :
    _options(options),
    _worker_id(worker_id),
    _el(new EventLoop(this))
{
}

RtcWorker::~RtcWorker() {
    if (_el) {
        delete _el;
        _el = nullptr;
    }

    if (_thread) {
        delete _thread;
        _thread = nullptr;
    }
}

int RtcWorker::init() {
	/* int fds[2];
	 if (pipe(fds)) {
		 RTC_LOG(LS_WARNING) << "create pipe error: " << strerror(errno)
			 << ", errno: " << errno;
		 return -1;
	 }

	 _notify_recv_fd = fds[0];
	 _notify_send_fd = fds[1];*/

   

#if defined(_WIN32)
	int port = 17000 + _worker_id;
	_notify_recv_fd = create_udp_server("127.0.0.1", port);
	if (_notify_recv_fd < 0)
	{
		RTC_LOG(LS_WARNING) << "create udp error: " << strerror(errno)
			<< ", errno: " << errno;
		return -1;
	}
	notify_addr_in.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &notify_addr_in.sin_addr);
	notify_addr_in.sin_port = htons(port);

#else
	int fds[2];
	if (pipe(fds)) {
		RTC_LOG(LS_WARNING) << "create pipe error: " << strerror(errno)
			<< ", errno: " << errno;
		return -1;
	}

	_notify_recv_fd = fds[0];
	_notify_send_fd = fds[1];
#endif
	_pipe_watcher = _el->create_io_event(rtc_worker_recv_notify, this);
	_el->start_io_event(_pipe_watcher, _notify_recv_fd, EventLoop::READ);

    return 0;
}

bool RtcWorker::start() {
    if (_thread) {
        RTC_LOG(LS_WARNING) << "rtc worker already start, worker_id: " << _worker_id;
        return false;
    }
    
    _thread = new std::thread([=]() {
        RTC_LOG(LS_INFO) << "rtc worker event loop start, worker_id: " << _worker_id;
        _el->start();
        RTC_LOG(LS_INFO) << "rtc worker event loop stop, worker_id: " << _worker_id;
    });

    return true;
}

void RtcWorker::stop() {
    notify(QUIT);
}

int RtcWorker::notify(int msg) {
    //int written = write(_notify_send_fd, &msg, sizeof(int));
   // return written == sizeof(int) ? 0 : -1;

#if defined(_WIN32)
	int written = ::sendto(_notify_recv_fd, (const char*)&msg, sizeof(int), 0, (struct sockaddr*)&notify_addr_in, sizeof(notify_addr_in));
	return written == sizeof(int) ? 0 : -1;
#else
	int written = write(_notify_send_fd, &msg, sizeof(int));
	return written == sizeof(int) ? 0 : -1;
#endif
}

void RtcWorker::join() {
    if (_thread && _thread->joinable()) {
        _thread->join();
    }
}

void RtcWorker::push_msg(std::shared_ptr<RtcMsg> msg) {
    _q_msg.produce(msg);
}

bool RtcWorker::pop_msg(std::shared_ptr<RtcMsg>* msg) {
    return _q_msg.consume(msg);
}

int RtcWorker::send_rtc_msg(std::shared_ptr<RtcMsg> msg) {
    // 将消息投递到worker的队列
    push_msg(msg);
    return notify(RTC_MSG);
}
void RtcWorker::_stop() {
    if (!_thread) {
        RTC_LOG(LS_WARNING) << "rtc worker not running, worker_id: " << _worker_id;
        return;
    }

    _el->delete_io_event(_pipe_watcher);
    _el->stop();
    //close(_notify_recv_fd);
   // close(_notify_send_fd);
#if defined(_WIN32)
	//close(_notify_recv_fd);
	close_socket(_notify_recv_fd);
#else
	close_socket(_notify_recv_fd);
	close_socket(_notify_send_fd);
#endif
}
void RtcWorker::_process_push(std::shared_ptr<RtcMsg> msg) {
    std::string offer = "offer";
    msg->sdp = offer;

    SignalingWorker* worker = (SignalingWorker*)(msg->worker);
    if (worker) {
        worker->send_rtc_msg(msg);
    }
}

void RtcWorker::_process_rtc_msg() {
    std::shared_ptr<RtcMsg> msg;
    if (!pop_msg(&msg)) {
        return;
    }

    RTC_LOG(LS_INFO) << "cmdno[" << msg->cmdno << "] uid[" << msg->uid
        << "] stream_name[" << msg->stream_name << "] audio[" << msg->audio
        << "] video[" << msg->video << "] log_id[" << msg->log_id
        << "] rtc worker receive msg, worker_id: " 
        << _worker_id;

    switch (msg->cmdno) {
        case CMDNO_PUSH:
            _process_push(msg);
            break;
        default:
            RTC_LOG(LS_WARNING) << "unknown cmdno: " << msg->cmdno 
                << ", log_id: " << msg->log_id;
            break;
    }
}
void RtcWorker::_process_notify(int msg) {
    switch (msg) {
        case QUIT:
            _stop();
            break;
        case RTC_MSG:
            _process_rtc_msg();
            break;
        default:
            RTC_LOG(LS_WARNING) << "unknown msg: " << msg;
            break;
    }
}


} // namespace xrtc


