#include "stdafx.h"
#include "CiTunesService.h"


CiTunesService::CiTunesService()
{
	CWinServiceManger sc;
	if (sc.OpenServiceManger())
	{
		m_iTunesService = sc.OpenService(iTunesServiceName);
	}
}

CiTunesService::~CiTunesService()
{
}

bool CiTunesService::IsRun()
{
	return m_iTunesService.ServiceIsRun();
}
