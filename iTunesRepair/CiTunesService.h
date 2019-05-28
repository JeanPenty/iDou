#pragma once

#define iTunesServiceName _T("Apple Mobile Device Service")
#include "CWinServiceManger.h"

class CiTunesService
{
public:
	CiTunesService();
	~CiTunesService();
	bool IsRun();
	bool RunService()
	{
		return m_iTunesService.RunService();
	}
	operator bool()
	{
		return m_iTunesService;
	}
private:
	
	CWinService m_iTunesService;
};

