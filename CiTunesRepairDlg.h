#pragma once
#include <core\SHostWnd.h>
#include "iTunesRepair\CiTunesIPCSvr.h"
//两个远端库提交测试

class CiTunesRepairDlg :public SHostDialog,public CiTunesIPCSvr,public IServer
{
public:
	CiTunesRepairDlg():SHostDialog(_T("XML_dlg_itunesrepair")) {};
	~CiTunesRepairDlg() { };
	
	// 通过 IIpcSvrCallback 继承
	virtual void OnNewConnection(IIpcHandle* pIpcHandle, IIpcConnection** ppConn) override;
	virtual int GetBufSize() const override;
	virtual void* GetSecurityAttr() const override;
	virtual void ReleaseSecurityAttr(void* psa) const override;
	virtual void OnConnected(IIpcConnection* pConn) override;
	virtual void OnDisconnected(IIpcConnection* pConn) override;
	void OnCheckRet(Param_CHECKRET& ret);
	void OnRepairRet(Param_REPAIR& ret);
	void OnConnectedr(Param_CONNECTED& ret);
	void OnOpenItunesUrl();
	FUN_BEGINEX
		FUN_HANDLEREX(Param_CHECKRET, OnCheckRet)
		FUN_HANDLEREX(Param_REPAIR, OnRepairRet)
		FUN_HANDLEREX(Param_CONNECTED, OnConnectedr)
	FUN_END
protected:
	BOOL OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnDestroy();
	void OnCheck();
	void OnRepair();

	EVENT_MAP_BEGIN()
		//EVENT_NAME_COMMAND(L"btn_check", OnCheck)
		EVENT_NAME_COMMAND(L"btn_repair", OnRepair)
		EVENT_NAME_COMMAND(L"btn_openitunesurl",OnOpenItunesUrl)
	EVENT_MAP_END()

	BEGIN_MSG_MAP_EX(CiTunesRepairDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP_2_IPC(m_ipcSvr)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()
};
