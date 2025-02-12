
#include "utils/comm_lib.h"

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

#include <sys/types.h> 
#if !defined(ANDROID)
#include <sys/timeb.h>
#endif
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#if defined(MACOS)
#include <mach-o/dyld.h>
#endif

bool  debug_fake_encode = false;
//for time 
static const int64_t kNumMillisecsPerSec = 1000;
static const int64_t kNumMicrosecsPerSec = 1000 * 1000;
static const int64_t kNumNanosecsPerSec = 1000 * 1000 * 1000;

static const int64_t kNumMicrosecsPerMillisec = kNumMicrosecsPerSec / kNumMillisecsPerSec;
static const int64_t kNumNanosecsPerMillisec = kNumNanosecsPerSec / kNumMillisecsPerSec;

std::string RtcEngine_GetModuleDirectoryPath()
{
	std::string strPath;

#ifdef WIN32
	char szPath[1024] = { 0 };
	if (::GetModuleFileName(NULL, szPath, 1024) >= 0)
	{
		strPath = szPath;
		size_t nPos = strPath.rfind("\\");
		if (std::string::npos == nPos)
		{
			return std::string("");
		}
		if (0 == nPos && strPath.size() > 0)
		{
			nPos++;
		}
		return strPath.substr(0, nPos);
	}
#endif

#if defined(OS_LINUX) 
	char szPath[1024] = { 0 };
	memset(szPath, '\0', 1024);
	int nCount = readlink("/proc/self/exe", szPath, 1024);

	if (nCount < 0 || nCount >= 1024)
	{
		printf("failed\n");
	}

	strPath = szPath;
	size_t nPos = strPath.rfind("/");
	if (std::string::npos == nPos)
	{
		return std::string("");
	}
	if (0 == nPos && strPath.size() > 0)
	{
		nPos++;
	}
	return strPath.substr(0, nPos);
#endif

#if defined(__OHOS__)
	return strPath;
#endif

#ifdef MACOS
    char path[512] = {0};
    unsigned size = 512;
    _NSGetExecutablePath(path, &size);
    path[size] = '\0';
    printf("The path is: %s\n", path);
    
    strPath = path;
    size_t nPos = strPath.rfind("/");
    if (std::string::npos == nPos)
    {
        return std::string("");
    }
    if (0 == nPos && strPath.size() > 0)
    {
        nPos++;
    }
    return strPath.substr(0, nPos);
#endif


	return strPath;
}

int RtcEngine_SRCreatDir(const char *pDir)
{
	if (NULL == pDir)
	{
		return 0;
	}

	int iRet = 0;
	char* pszDir = NULL;

	int iLen = strlen(pDir);

	//分配空间
	pszDir = new char[iLen + 2];
	memset(pszDir, 0, (iLen + 2) * sizeof(char));
	memcpy(pszDir, pDir, iLen * sizeof(char));


	//1. 先格式化格式
#if defined(_WIN32)
	for (int i = 0; i < iLen; i++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\\';
		}
	}
#endif

	//在末尾加/
	if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/')
	{
#if defined(_WIN32)
		pszDir[iLen] = '\\';
#else
		pszDir[iLen] = '/';
#endif
		pszDir[iLen + 1] = '\0';
	}



	// 创建中间目录  
	for (int i = 1; i <= iLen + 1; i++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\0';
			//如果不存在,创建  

			if (ACCESS(pszDir, 0) == -1) //不存在
			{
				iRet = MKDIR(pszDir);
				if (iRet == -1) //成功返回0 失败返回-1
				{
					printf("Create Dir Error....directory already exists or not\n");
					//return -1;
				}
			}
			//支持linux,将所有\换成/  
#if defined(_WIN32)
			pszDir[i] = '\\';
#else
			pszDir[i] = '/';
#endif
		}
	}


	delete[] pszDir;
	return iRet;
}

 uint32_t myGetCurrentProcessId()  {
#if defined(_WIN32)
	return ::GetCurrentProcessId();
#elif defined(OS_LINUX) || defined(__OHOS__)
	return ::getpid();
#else
	return 0;
#endif
}

static char g_curr_time[50];
/* 获取当前时间 */
char *SRE_GetCurrTime()
{
	memset(g_curr_time, 0, 50);
#if defined(OS_LINUX) || defined(ANDROID) || defined(IOS) || defined(MACOS) || defined(__OHOS__)
	struct timeval val;
	struct tm *ptm = NULL;

	tsk_gettimeofday(&val, NULL);
	ptm = localtime(&val.tv_sec);
	sprintf(g_curr_time,
		"%04d-%02d-%02d-%02d.%02d.%02d-%03ld",
		ptm->tm_year + 1900,
		ptm->tm_mon + 1,
		ptm->tm_mday,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		val.tv_usec / 1000);

#endif

#if defined(WIN32)
	struct timeb tp;
	struct tm * tm = NULL;
	ftime(&tp);
	tm = localtime(&(tp.time));
	sprintf(g_curr_time, "%d-%02d-%02d-%02d.%02d.%02d-%03d",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec, tp.millitm);
#endif  
	return g_curr_time;

}
extern char *SRE_GetCurrTimeWithDay(int * day)
{
	memset(g_curr_time, 0, 50);
#if defined(OS_LINUX) || defined(ANDROID) || defined(IOS) || defined(MACOS) || defined(__OHOS__)
	struct timeval val;
	struct tm *ptm = NULL;

	tsk_gettimeofday(&val, NULL);
	ptm = localtime(&val.tv_sec);
	sprintf(g_curr_time,
		"%04d-%02d-%02d-%02d.%02d.%02d-%03ld",
		ptm->tm_year + 1900,
		ptm->tm_mon + 1,
		ptm->tm_mday,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		val.tv_usec / 1000);
	*day = ptm->tm_mday;
#endif

#if defined(WIN32)
	struct timeb tp;
	struct tm * tm = NULL;
	ftime(&tp);
	tm = localtime(&(tp.time));
	sprintf(g_curr_time, "%d-%02d-%02d-%02d.%02d.%02d-%03d",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec, tp.millitm);

	*day = tm->tm_mday;
#endif  
	return g_curr_time;
}

char *SRE_GetCurrTimeFilename()
{
	memset(g_curr_time, 0, 50);
	//srengine_log-2023-12-13-14.18.01-162
#if defined(OS_LINUX) || defined(ANDROID) || defined(IOS) || defined(MACOS) || defined(__OHOS__)
	struct timeval val;
	struct tm *ptm = NULL;

	tsk_gettimeofday(&val, NULL);
	ptm = localtime(&val.tv_sec);
	sprintf(g_curr_time,
		"%u-%04d-%02d-%02d-%02d.%02d.%02d-%03ld",
		myGetCurrentProcessId(),
		ptm->tm_year + 1900,
		ptm->tm_mon + 1,
		ptm->tm_mday,
		ptm->tm_hour,
		ptm->tm_min,
		ptm->tm_sec,
		val.tv_usec / 1000);

#endif

#if defined(WIN32)
	struct timeb tp;
	struct tm * tm = NULL;
	ftime(&tp);
	tm = localtime(&(tp.time));
	sprintf(g_curr_time, "%u-%04d-%02d-%02d-%02d.%02d.%02d-%03d"
		,myGetCurrentProcessId()
		,tm->tm_year + 1900
		, tm->tm_mon + 1
		, tm->tm_mday
		,tm->tm_hour
		, tm->tm_min
		, tm->tm_sec
		, tp.millitm
		);
#endif  
	return g_curr_time;

}

int SRE_GetCurrTimeDay()
{
	//srengine_log-2023-12-13-14.18.01-162
#if defined(OS_LINUX) || defined(ANDROID) || defined(IOS) || defined(MACOS) || defined(__OHOS__)
	struct timeval val;
	struct tm *ptm = NULL;

	tsk_gettimeofday(&val, NULL);
	ptm = localtime(&val.tv_sec);

	return ptm->tm_mday;
#endif

#if defined(WIN32)
	struct timeb tp;
	struct tm * tm = NULL;
	ftime(&tp);
	tm = localtime(&(tp.time));

	return tm->tm_mday;
#endif  
	return 0;
}

int __free_string(char **str)
{
	if (str != NULL) {
		tsk_free((void **)str);
	}
	*str = NULL;
	return 0;
}

std::string get_vector_stirng(const std::vector<std::string > & vStr, const std::string& separator)
{
	std::string retStr;

	for (auto itor = vStr.begin(); itor != vStr.end(); itor++)
	{
		retStr.append(*itor);
		retStr.append(separator);
	}

	if (retStr.size() > 0)
	{
		retStr = retStr.substr(0, retStr.size() - 1);
	}

	return retStr;
}

void  SRengine_SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;

	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}

	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}
void  SRengine_SplitString3(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos;
	std::string tmp = s;
	pos = tmp.find_last_of(c);

	int  i = 2;

	while (i > 0 && std::string::npos != pos)
	{
		std::string tmp1 = tmp.substr(pos + 1);
		tmp = tmp.substr(0, pos);

		v.push_back(tmp1);

		i = i - 1;
		if (i == 0)
		{
			v.push_back(tmp);
			break;
		}

		pos = tmp.find_last_of(c);
	}


	/*
	pos1 = 0;

	int i = 3;


	if (pos1 != s.length())
	v.push_back(s.substr(pos1));*/
}

bool IsV6Addr(const char* ip)
{
	struct sockaddr_storage addr;
	if (1 == inet_pton(AF_INET6, ip, (struct sockaddr_in6*)&addr))
	{
		return true;
	}
	return false;
}

void  SRengine_SplitString2(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos;
	std::string tmp = s;
	pos = tmp.find_last_of(c);

	int  i = 1;

	while (i > 0 && std::string::npos != pos)
	{
		std::string tmp1 = tmp.substr(pos + 1);
		tmp = tmp.substr(0, pos);

		v.push_back(tmp1);

		i = i - 1;
		if (i == 0)
		{
			v.push_back(tmp);
			break;
		}

		pos = tmp.find_last_of(c);
	}
}


#if defined(OSX) || defined(IOS)
#include <sys/time.h>
#include <mach/mach_time.h>
#endif
uint64_t TimeNanos() {
    int64_t ticks = 0;
#if defined(OSX) || defined(IOS)
    static mach_timebase_info_data_t timebase;
    if (timebase.denom == 0) {
        // Get the timebase if this is the first time we run.
        // Recommended by Apple's QA1398.
        //VERIFY(KERN_SUCCESS == mach_timebase_info(&timebase));
        KERN_SUCCESS == mach_timebase_info(&timebase);
    }
    // Use timebase to convert absolute time tick units into nanoseconds.
    ticks = mach_absolute_time() * timebase.numer / timebase.denom;
#elif defined(ANDROID) || defined(LINUX)
    struct timespec ts;
    // TODO: Do we need to handle the case when CLOCK_MONOTONIC
    // is not supported?
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ticks = kNumNanosecsPerSec * static_cast<int64_t>(ts.tv_sec) +
    static_cast<int64_t>(ts.tv_nsec);
#elif defined(WIN32)
    //static volatile LONG last_timegettime = 0;
    //static volatile int64_t num_wrap_timegettime = 0;
    //volatile LONG* last_timegettime_ptr = &last_timegettime;
    //DWORD now = timeGetTime();
    //// Atomically update the last gotten time
    //DWORD old = InterlockedExchange(last_timegettime_ptr, now);
    //if (now < old) {
    //    // If now is earlier than old, there may have been a race between
    //    // threads.
    //    // 0x0fffffff ~3.1 days, the code will not take that long to execute
    //    // so it must have been a wrap around.
    //    if (old > 0xf0000000 && now < 0x0fffffff) {
    //        num_wrap_timegettime++;
    //    }
    //}
    //ticks = now + (num_wrap_timegettime << 32);
    //// TODO: Calculate with nanosecond precision.  Otherwise, we're just
    //// wasting a multiply and divide when doing Time() on Windows.
    //ticks = ticks * kNumNanosecsPerMillisec;
#endif
    return ticks;
}

unsigned long SRengine_TimeMS()
{
#if defined(WIN32)
	return GetTickCount();
#elif defined(ANDROID) || defined(LINUX)
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#elif defined(OSX) || defined(IOS)
	//纳秒-->毫秒
	return TimeNanos() / 1000000;
#endif
	return 0;
}

unsigned long SRengine_TimeS()
{
	return  SRengine_TimeMS() / 1000;
}


unsigned long SRE_GetTimeOfDay()			//秒
{
	//这里修改成系统开机到现在的时间
	//GetTickCount返回（retrieve）从操作系统启动所经过（elapsed）的毫秒数
	//return GetTickCount() / 1000;
	return SRengine_TimeS();
	/*struct timeval tv;
	tsk_gettimeofday(&tv, NULL);
	return tv.tv_sec;*/
};

unsigned int StringIpToInt(std::string & ip)
{
	unsigned int ret = 0;
	std::vector<std::string> slitInt;

	SRengine_SplitString(ip, slitInt, ".");

	if (!slitInt.empty())
	{
		ret = (atoi(slitInt[0].c_str()) << 24) | (atoi(slitInt[1].c_str()) << 16) | (atoi(slitInt[2].c_str()) << 8) | atoi(slitInt[3].c_str());
	}

	return ret;
}


bool isCheckReJoin(int iRet)
{
	switch (static_cast<JoinErrorCode>(iRet)) {
	case JoinErrorCode::G_NoMcCode:
		return true;
	case JoinErrorCode::G_NoCasCode:
		return true;
	case JoinErrorCode::G_NoMasterCASCode:
		return true;
	case JoinErrorCode::G_JLCode:
		return true;
	case JoinErrorCode::G_CasBusy:
		return true;
	case JoinErrorCode::G_TokenExpired:
		return true;
	case JoinErrorCode::G_DEVICE_IS_ALLOCATING:
		return true;
	case JoinErrorCode::G_CACHE_IS_CLEANING:
		return true;
	default:
		return false;
	}
	return false;
}