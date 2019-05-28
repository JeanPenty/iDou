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
		//获取当前的状态
		BOOL bRet = ::QueryServiceStatus(m_ServiceHandle, &ServiceStatus);
		if (bRet)
		{
			//正在恢复则不处理
			if (SERVICE_CONTINUE_PENDING == ServiceStatus.dwCurrentState)
				return true;
			//停止则尝试启动
			if (SERVICE_STOPPED == ServiceStatus.dwCurrentState)
				return ::StartService(m_ServiceHandle, 0, NULL);
			//暂停则继续
			if (SERVICE_PAUSED == ServiceStatus.dwCurrentState)
				return ::ControlService(m_ServiceHandle, SERVICE_CONTROL_CONTINUE, &ServiceStatus);
		}
		return false;
	}
	bool ServiceIsRun()
	{
		SERVICE_STATUS ServiceStatus = { 0 };
		//获取当前的状态
		BOOL bRet = ::QueryServiceStatus(m_ServiceHandle, &ServiceStatus);
		/*SERVICE_RUNNING			已启动			
			SERVICE_STOPPED			已停止
			SERVICE_PAUSED			已暂停
			SERVICE_CONTINUE_PENDING		正在恢复
			SERVICE_PAUSE_PENDING			正在暂停
			SERVICE_START_PENDING			正在启动			
			SERVICE_STOP_PENDING			正在停止*/
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
	//打开本机注册表所有权限打开服务管理器
	bool OpenServiceManger();
	bool OpenServiceManger(LPCWSTR  lpMachineName, LPCWSTR lpDatabaseName, DWORD dwDesiredAccess);
	void CloseServiceManger();
	CWinService OpenService(LPCTSTR lpServiceName, DWORD dwDesiredAccess = SERVICE_ALL_ACCESS);
	bool ServiceExists(LPCTSTR ServiceName, const DWORD dwServiceType, const DWORD dwServiceState);
protected:

private:
	SC_HANDLE m_SMHandle = NULL;

};

