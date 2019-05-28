#include "stdafx.h"
#include "CWinServiceManger.h"


CWinServiceManger::CWinServiceManger()
{
}


CWinServiceManger::~CWinServiceManger()
{
	SALF_CLOSE_HANDLE(m_SMHandle);
}

bool CWinServiceManger::OpenServiceManger()
{
	return OpenServiceManger(NULL, NULL, SC_MANAGER_ALL_ACCESS);
}

bool CWinServiceManger::OpenServiceManger(LPCWSTR lpMachineName, LPCWSTR lpDatabaseName, DWORD dwDesiredAccess)
{
	SASSERT(m_SMHandle==NULL);
	m_SMHandle = ::OpenSCManager(lpMachineName, lpDatabaseName, dwDesiredAccess);
	return m_SMHandle != NULL;
}

void CWinServiceManger::CloseServiceManger()
{
	SALF_CLOSE_HANDLE(m_SMHandle);
}

CWinService CWinServiceManger::OpenService(LPCTSTR lpServiceName, DWORD dwDesiredAccess)
{
	return ::OpenService(m_SMHandle, lpServiceName, dwDesiredAccess);
}

bool CWinServiceManger::ServiceExists(LPCTSTR ServiceName, const DWORD dwServiceType, const DWORD dwServiceState)
{
	if (ServiceName == NULL)
		return false;
	LPENUM_SERVICE_STATUS pServices = NULL;
	DWORD dwByteNeed = 0;
	DWORD dwServiceReturn = 0;
	LPDWORD lpResumeHandle = NULL;
	//��һ�ε��ã�������������ΪNULL������������С����Ϊ0
	::EnumServicesStatus(m_SMHandle, dwServiceType, dwServiceState, pServices, 0, &dwByteNeed, &dwServiceReturn, lpResumeHandle);
	//�������Ϊ��������С����
	bool bRet = false;
	if (ERROR_MORE_DATA == GetLastError())
	{
		//���滺��������ʵ��С
		DWORD dwRealNeed = dwByteNeed;
		//�����һ����Ϊ�˱����ַ���ĩβ��0
		pServices = (LPENUM_SERVICE_STATUS)new char[dwRealNeed + 1];
		SASSERT(NULL != pServices);
		ZeroMemory(pServices, dwRealNeed + 1);
		if (::EnumServicesStatus(m_SMHandle, dwServiceType, dwServiceState, pServices, dwRealNeed + 1, &dwByteNeed, &dwServiceReturn, lpResumeHandle))
		{
			for (int i = 0; i < dwServiceReturn; i++)
			{
				if (_tcscmp(ServiceName, pServices[i].lpServiceName) == 0)
				{
					bRet = true;
					break;
				}
			}
		}
		delete pServices;
	}
	return bRet;
}

CWinService::CWinService(SC_HANDLE handle)
{
	m_ServiceHandle = handle;
}

CWinService::~CWinService()
{
	SALF_CLOSE_HANDLE(m_ServiceHandle);
}
