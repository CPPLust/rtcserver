#include <rtc_base/logging.h>
#include <yaml-cpp/yaml.h>

#include "server/signaling_server.h"
#include "base/socket.h"



namespace xrtc {

void signaling_server_recv_notify(EventLoop* el, IOWatcher* w, int fd, int events, void* data) {
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
    SignalingServer* server = (SignalingServer*)data;
    server->_process_notify(msg);
}


void accept_new_conn(EventLoop* el, IOWatcher* w, int fd, int events, void* data) {
	RTC_LOG(LS_INFO) << "+++++++++++++++==accept_new_conn ";
}

SignalingServer::SignalingServer() : _el(new EventLoop(this)) {
}

SignalingServer::~SignalingServer() {
}

int SignalingServer::init(const char* conf_file) {
    if (!conf_file) {
        RTC_LOG(LS_WARNING) << "signaling server conf_file is null";
        return -1;
    }

    try {
        YAML::Node config = YAML::LoadFile(conf_file);
        RTC_LOG(LS_INFO) << "signaling server options:\n" << config;

        _options.host = config["host"].as<std::string>();
        _options.port = config["port"].as<int>();
        _options.worker_num = config["worker_num"].as<int>();
        _options.connection_timeout = config["connection_timeout"].as<int>();

    } catch (YAML::Exception e) {
        RTC_LOG(LS_WARNING) << "catch a YAML exception, line:" << e.mark.line + 1
            << ", column: " << e.mark.column + 1 << ", error: " << e.msg;
        return -1;
    }
    
#if defined(_WIN32)
    _notify_send_fd = create_udp_server("127.0.0.1",18000);
	if (_notify_send_fd < 0)
	{
		RTC_LOG(LS_WARNING) << "create udp error: " << strerror(errno)
			<< ", errno: " << errno;
		return -1;
	}
    notify_addr_in.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &notify_addr_in.sin_addr);
    notify_addr_in.sin_port = htons(18000);
	// ��recv_fd��ӵ��¼�ѭ�������й���
	_pipe_watcher = _el->create_io_event(signaling_server_recv_notify, this);
	_el->start_io_event(_pipe_watcher, _notify_send_fd, EventLoop::READ);
#else
	//�����ܵ�
	int fds[2];
	if (pipe(fds))
	{
		RTC_LOG(LS_WARNING) << "create pipe error: " << strerror(errno) << ", errno: " << errno;
		return -1;
	}

	_notify_recv_fd = fds[0];
	_notify_send_fd = fds[1];
	//��fd��ӵ��¼�ѭ�����н��й���
	_pipe_watcher = _el->create_io_event(signaling_server_recv_notify, this);
	_el->start_io_event(_pipe_watcher, _notify_recv_fd, EventLoop::READ);
#endif
	
  
    // ����tcp server
    _listen_fd = create_tcp_server(_options.host.c_str(), _options.port); 
    _io_watcher = _el->create_io_event(accept_new_conn, this); 
    _el->start_io_event(_io_watcher, _listen_fd, EventLoop::READ); 

    return 0;
}

bool SignalingServer::start() {
    if (_thread) {
        RTC_LOG(LS_WARNING) << "signalling server already start";
        return false;
    }

    _thread = new std::thread([=]() {
        RTC_LOG(LS_INFO) << "signaling server event loop run";
        _el->start();
        RTC_LOG(LS_INFO) << "signaling server event loop stop";
    });

    return true;
}

void SignalingServer::stop() {
    notify(SignalingServer::QUIT);
}

int SignalingServer::notify(int msg) {
#if defined(_WIN32)
    int written = ::sendto(_notify_send_fd, (const char*)&msg, sizeof(int), 0, (struct sockaddr*)&notify_addr_in, sizeof(notify_addr_in));
    return written == sizeof(int) ? 0 : -1;
#else
	int written = write(_notify_send_fd, &msg, sizeof(int));
	return written == sizeof(int) ? 0 : -1;
#endif
    return 0;
}

void SignalingServer::_process_notify(int msg) {
    switch (msg) {
        case QUIT:
            _stop();
            break;
        default:
            RTC_LOG(LS_WARNING) << "unknown msg: " << msg;
            break;
    }
}

void SignalingServer::_stop() {
    if (!_thread) {
        RTC_LOG(LS_WARNING) << "signaling server not running";
        return;
    }

    _el->delete_io_event(_pipe_watcher);
    _el->delete_io_event(_io_watcher);
    _el->stop();

#if defined(_WIN32)
	//close(_notify_recv_fd);
	close_socket(_notify_send_fd);
#else
    close_socket(_notify_recv_fd);
	close_socket(_notify_send_fd);
#endif
    
    close_socket(_listen_fd);

    RTC_LOG(LS_INFO) << "signaling server stop";
}

void SignalingServer::join() {
    if (_thread && _thread->joinable()) {
        _thread->join();
    }
}
} // namespace xrtc




