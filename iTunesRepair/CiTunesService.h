#pragma once

#define iTunesServiceName _T("Apple Mobile Device Service")
#include "CWinServiceManger.h"

class CiTunesService
{
public:
	CiTunesService();
	~CiTunesService();
	bool IsRun();
	static void OpeniTunesUrl();
	bool RunService();
	operator bool()
	{
		return m_iTunesService;
	}
	bool GetVer(SStringT&);
private:
	
	CWinService m_iTunesService;
};