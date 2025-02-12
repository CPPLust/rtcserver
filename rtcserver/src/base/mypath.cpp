
#include <stdio.h>
#include <iostream>

#include "base/mypath.h"

#ifdef  _WIN32
#include <windows.h>

#endif

namespace xrtc {

std::string get_bin_path()
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
	char path[512] = { 0 };
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


} // namespace xrtc


















