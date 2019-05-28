#pragma once

#include <interface/sipcobj-i.h>
#include "protocol.h"
#include <commgr2.h>




class CiTunesIPCSvrConnect : public SOUI::TObjRefImpl<SOUI::IIpcConnection>
{
public:
	CiTunesIPCSvrConnect(IIpcHandle* pHandle, IServer*_server) :m_ipcHandle(pHandle)
	{
		m_iServer = _server;
	}
	~CiTunesIPCSvrConnect()
	{
		m_ipcHandle = NULL;
	}

	FUN_BEGIN
		if(m_iServer)
			bHandled=m_iServer->HandleFun(m_ipcHandle,uMsg, ps);
	FUN_END

public:
	// Í¨¹ý TObjRefImpl ¼Ì³Ð
	virtual IIpcHandle* GetIpcHandle() override
	{
		return m_ipcHandle;
	}

	virtual void BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, TCHAR szBuf[MAX_PATH]) const override
	{
		_stprintf(szBuf, iTunes_SHARE_BUF_NAME_FMT, (DWORD)(((LPARAM)idLocal) & 0xffffffff), (DWORD)(((LPARAM)idRemote) & 0xffffffff));
	}
private:
	CAutoRefPtr<IIpcHandle> m_ipcHandle;
	IServer *m_iServer = NULL;;
};

class CiTunesIPCSvr :public IIpcSvrCallback
{
	static void DoFun(IIpcConnection* pConn, ULONG_PTR data)
	{
		SASSERT(data != 0);
		pConn->GetIpcHandle()->CallFun((IFunParams*)data);
	}	
public:
	CiTunesIPCSvr()
	{
		CAutoRefPtr<IIpcFactory> ipcFac;
		m_comMgr.CreateIpcObject((IObjRef **)& ipcFac);
		ipcFac->CreateIpcServer(&m_ipcSvr);
	}
	~CiTunesIPCSvr()
	{
		m_ipcSvr = NULL;
	}
	void Close()
	{
		Param_CLOSE param;
		m_ipcSvr->EnumClient(&CiTunesIPCSvr::DoFun, (ULONG_PTR)&param);
	}
	void StartCheck()
	{
		Param_SATRTCHECK param;
		m_ipcSvr->EnumClient(&CiTunesIPCSvr::DoFun, (ULONG_PTR)& param);
	}
	void StartRepair()
	{
		Param_REPAIR param;
		m_ipcSvr->EnumClient(&CiTunesIPCSvr::DoFun, (ULONG_PTR)& param);
	}
protected:
	CAutoRefPtr<SOUI::IIpcServer> m_ipcSvr;
	SComMgr2 m_comMgr;
};

