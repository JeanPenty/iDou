#pragma once


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

