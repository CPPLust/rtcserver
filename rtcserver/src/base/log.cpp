
#include <iostream>

#include "base/log.h"
#include "base/mypath.h"

#if defined(WEBRTC_POSIX)
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace xrtc {

XrtcLog::XrtcLog(const std::string& log_dir,
        const std::string& log_name,
        const std::string& log_level) 
{
	_log_dir = log_dir;
	_log_name = log_name;
	_log_level = log_level;
	_log_file = log_dir + MY_PATH_STRING + log_name + ".log";
	_log_file_wf = log_dir + MY_PATH_STRING + log_name + ".log.wf";


	_thread = NULL;
	_running = false;
}

XrtcLog::~XrtcLog() {}

void XrtcLog::OnLogMessage(const std::string& message, 
        rtc::LoggingSeverity severity)
{
    //std::cout << "=======OnLogMessage: " << message; 
	if (severity >= rtc::LS_WARNING)
	{
		std::unique_lock<std::mutex> lock(_mtx_wf);
		_log_queue_wf.push(message);
	}
	else
	{
		std::unique_lock<std::mutex> lock(_mtx);
		_log_queue.push(message);

	}
}

void XrtcLog::OnLogMessage(const std::string& /*message*/) {
    // 不需要有逻辑
}

static rtc::LoggingSeverity get_log_severity(const std::string& level) {
    if ("verbose" == level) {
        return rtc::LS_VERBOSE;
    } else if ("info" == level) {
        return rtc::LS_INFO;
    } else if ("warning" == level) {
        return rtc::LS_WARNING;
    } else if ("error" == level) {
        return rtc::LS_ERROR;
    } else if ("none" == level) {
        return rtc::LS_NONE;
    }
    
    return rtc::LS_NONE;
}

int XrtcLog::init() {
    rtc::LogMessage::ConfigureLogging("thread tstamp");
    rtc::LogMessage::SetLogPathPrefix("/src");
    rtc::LogMessage::AddLogToStream(this, get_log_severity(_log_level));


	//openfile
	_out_file.open(_log_file, std::ios::app);
	if (!_out_file.is_open())
	{
		fprintf(stderr, "open log_file[%s] failed\n", _log_file.c_str());
		return -1;
	}


	_out_file_wf.open(_log_file_wf, std::ios::app);
	if (!_out_file_wf.is_open())
	{
		fprintf(stderr, "open log_file_wf[%s] failed\n", _log_file_wf.c_str());
		return -1;
	}

	start();

    return 0;
}

void XrtcLog::set_log_to_stderr(bool on) {
    rtc::LogMessage::SetLogToStderr(on);
}


bool XrtcLog::start() {
	if (_running)
	{
		fprintf(stderr, "log thread already running\n");
		return false;
	}

	_running = true;

	_thread = new std::thread([=]() {
		struct  stat stat_data;
		std::stringstream ss;
		while (_running)
		{
			if (stat(_log_file.c_str(), &stat_data) < 0)
			{
				_out_file.close();
				_out_file.open(_log_file, std::ios::app);
			}

			if (stat(_log_file_wf.c_str(), &stat_data) < 0)
			{
				_out_file_wf.close();
				_out_file_wf.open(_log_file_wf, std::ios::app);
			}

			bool write_log = false;
			{
				std::unique_lock<std::mutex> lock(_mtx);
				if (!_log_queue.empty())
				{
					write_log = true;
					while (!_log_queue.empty())
					{
						ss << _log_queue.front();
						_log_queue.pop();
					}
				}
			}
			if (write_log) {
				_out_file << ss.str();
				_out_file.flush();
			}
			ss.str("");

			bool write_log_wf = false;
			{
				std::unique_lock<std::mutex> lock(_mtx_wf);
				if (!_log_queue_wf.empty())
				{
					write_log_wf = true;
					while (!_log_queue_wf.empty())
					{
						ss << _log_queue_wf.front();
						_log_queue_wf.pop();
					}
				}
			}
			if (write_log_wf) {
				_out_file_wf << ss.str();
				_out_file_wf.flush();
			}

			ss.str("");

			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}
		});

	return 0;

}


void XrtcLog::stop()
{
	_running = false;

	if (_thread)
	{
		if (_thread->joinable())
		{
			_thread->join();
		}

		delete _thread;
		_thread = NULL;
	}

	_out_file.close();
	_out_file_wf.close();
}

void XrtcLog::join()
{
	if (_thread && _thread->joinable())
	{
		_thread->join();
	}
}

} // namespace xrtc


