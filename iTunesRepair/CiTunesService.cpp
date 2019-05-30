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
void CiTunesService::OpeniTunesUrl() 
{//https://support.apple.com/zh_CN/downloads/itunes

}

bool CiTunesService::RunService()
{
	return m_iTunesService.RunService();
}

bool CiTunesService::GetVer(SStringT &outver)
{
	return m_iTunesService.GetServiceVer(outver);

}

