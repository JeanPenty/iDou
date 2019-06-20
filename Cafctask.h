#pragma once

#include <core/SSingleton.h>
#include <TaskLoop/TaskLoop.h>
#include <map>
#include <com-cfg.h>
#include <string>
#include <include\libimobiledevice\afc.h>

class CAFCThread
{
public:
	CAFCThread(SComMgr& comMgr, LPCSTR udid);
	~CAFCThread();
	void Updata(int taskid,LPCSTR path, bool bAfc2);
	void OpenFile(LPCSTR path, bool bAfc2);
protected:
	void _Updata(int taskid, std::string path, bool bAfc2);
	//void _Updata2(int id, std::string path);
	void _OpenFile(std::string path,bool bAfc2);
	void _InitAFC();

	afc_client_t m_afc2 = NULL;
	afc_client_t m_afc = NULL;
	std::string m_udid;
	CAutoRefPtr<ITaskLoop> m_pTaskLoop;
};

class Cafctask : public SSingleton<Cafctask>
{
public:
	Cafctask();
	~Cafctask();
	void AddDev(LPCSTR udid);
	void RemoveDev(LPCSTR udid);
	UINT Updata(LPCSTR udid, LPCSTR path,bool bAfc2);
	void OpenFile(LPCSTR udid, LPCSTR path, bool bAfc2);
protected:
	//一个设备一个线程
	std::map<std::string, CAFCThread*>	m_threadPool;
	SComMgr								m_comMgr;
};