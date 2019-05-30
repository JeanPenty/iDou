#pragma once
#include <memory>
#include <io.h>
#pragma comment(lib, "Version.lib")

#define SALF_CLOSE_HANDLE(h_SCHandle) if(h_SCHandle)CloseServiceHandle(h_SCHandle);h_SCHandle=NULL;

class CWinService
{
public:
	CWinService() {};
	CWinService(SC_HANDLE handle);
	~CWinService();
	CWinService& operator =(SC_HANDLE handle)
	{
		SALF_CLOSE_HANDLE(m_ServiceHandle);
		m_ServiceHandle = handle;
		return *this;
	}
	CWinService& operator =(CWinService&& handle)
	{
		m_ServiceHandle = handle.m_ServiceHandle;
		handle.m_ServiceHandle = NULL;
		return *this;
	}
	operator bool()
	{
		return m_ServiceHandle != NULL;
	}
	BOOL RunService()
	{
		SERVICE_STATUS ServiceStatus = { 0 };
		//��ȡ��ǰ��״̬
		BOOL bRet = ::QueryServiceStatus(m_ServiceHandle, &ServiceStatus);
		if (bRet)
		{
			//���ڻָ��򲻴���
			if (SERVICE_CONTINUE_PENDING == ServiceStatus.dwCurrentState)
				return true;
			//ֹͣ��������
			if (SERVICE_STOPPED == ServiceStatus.dwCurrentState)
				return ::StartService(m_ServiceHandle, 0, NULL);
			//��ͣ�����
			if (SERVICE_PAUSED == ServiceStatus.dwCurrentState)
				return ::ControlService(m_ServiceHandle, SERVICE_CONTROL_CONTINUE, &ServiceStatus);
		}
		return false;
	}
	bool ServiceIsRun()
	{
		SERVICE_STATUS ServiceStatus = { 0 };
		//��ȡ��ǰ��״̬
		BOOL bRet = ::QueryServiceStatus(m_ServiceHandle, &ServiceStatus);
		/*SERVICE_RUNNING			������
			SERVICE_STOPPED			��ֹͣ
			SERVICE_PAUSED			����ͣ
			SERVICE_CONTINUE_PENDING		���ڻָ�
			SERVICE_PAUSE_PENDING			������ͣ
			SERVICE_START_PENDING			��������
			SERVICE_STOP_PENDING			����ֹͣ*/
		return (bRet && ServiceStatus.dwCurrentState == SERVICE_RUNNING);
	}

	SStringT getProgramVersion(LPCWSTR filepath)
	{
		VS_FIXEDFILEINFO* pVsInfo;
		unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);

		SStringT file = filepath;
		file.Trim(L'\"');
		int iVerInfoSize = GetFileVersionInfoSize(file, NULL);		
		if (iVerInfoSize != 0) {
			std::unique_ptr<wchar_t> pBuf(new wchar_t[iVerInfoSize]);
			if (GetFileVersionInfo(file, 0, iVerInfoSize, pBuf.get())) {
				if (VerQueryValue(pBuf.get(), L"\\", (void**)& pVsInfo, &iFileInfoSize)) {
					return SStringT().Format(L"%d.%d.%d.%d", HIWORD(pVsInfo->dwFileVersionMS),
						LOWORD(pVsInfo->dwFileVersionMS),
						HIWORD(pVsInfo->dwFileVersionLS),
						LOWORD(pVsInfo->dwFileVersionLS));
				}
			}
		}
		return L"";
	}

	bool GetServiceVer(SStringT& outver)
	{
		DWORD dwBuffNeed = 0;
		BOOL bRet = ::QueryServiceConfig(m_ServiceHandle, NULL, 0, &dwBuffNeed);
		if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
		{
			DWORD dwRealNeed = dwBuffNeed;
			std::unique_ptr<QUERY_SERVICE_CONFIG>  QUERY_SERVICE_CONFIGW((LPQUERY_SERVICE_CONFIG)new char[dwRealNeed]);
			bRet = QueryServiceConfig(m_ServiceHandle, QUERY_SERVICE_CONFIGW.get(), dwRealNeed, &dwBuffNeed);
			if (bRet)
			{				
				outver = getProgramVersion(QUERY_SERVICE_CONFIGW.get()->lpBinaryPathName);
			}
		}
		return bRet;
	}
private:
	SC_HANDLE m_ServiceHandle = NULL;
};

class CWinServiceManger
{
public:
	CWinServiceManger();
	~CWinServiceManger();
	//�򿪱���ע�������Ȩ�޴򿪷��������
	bool OpenServiceManger();
	bool OpenServiceManger(LPCWSTR  lpMachineName, LPCWSTR lpDatabaseName, DWORD dwDesiredAccess);
	void CloseServiceManger();
	CWinService OpenService(LPCTSTR lpServiceName, DWORD dwDesiredAccess = SERVICE_ALL_ACCESS);
	bool ServiceExists(LPCTSTR ServiceName, const DWORD dwServiceType, const DWORD dwServiceState);
protected:

private:
	SC_HANDLE m_SMHandle = NULL;

};

