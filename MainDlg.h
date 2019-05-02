// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "controls/STabCtrlHeaderBinder.h"
#include "TV_iOSDeviceAdapter.h"
#include "iOsDeviceLib/CiOSDevice.h"

class CBaseInfoDlg;

class CMainDlg : public SHostWnd
	, public ISetOrLoadSkinHandler
	, public IDeviceEventCallBack
	, public TAutoEventMapReg<CMainDlg>
{
public:
	CMainDlg();
	~CMainDlg();
protected:
	template<class T>
	void ShowInfoDlg(const std::string&);
	bool LoadSkin();
	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);
	//托盘通知消息处理函数
	//LRESULT OnIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL/* bHandled*/);
	//演示如何响应菜单事件
	void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	virtual bool SaveSkin(SkinType skinType, SkinSaveInf& skinSaveInf);
	void ShowToolTip(LPCTSTR tipMsg);
	void OnSkin();

	void OnSetSkin(int id);

	void ChildMenuItemClick(LPCSTR udid, int nGID);
	void OnTVSelChanged(EventArgs* pEArg);

	void OnReboot(EventArgs* pEArg);
	void OnSleep(EventArgs* pEArg);
	void OnShutdown(EventArgs* pEArg);
	void OnBatteryInfo(EventArgs* pEArg);
	void OnDevInfo(EventArgs* pEArg);
	void OnScreenShot(EventArgs* pEArg);
protected:
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"btn_skin", OnSkin)
		EVENT_ID_HANDLER(R.id.tv_ios, EventTVSelChanged::EventID, OnTVSelChanged)
		EVENT_ID_COMMAND_RANGE(600, 608, OnSetSkin)
		EVENT_ID_HANDLER(R.id.btn_reboot, EVT_CMD, OnReboot)
		EVENT_ID_HANDLER(R.id.btn_shutdown, EVT_CMD, OnShutdown)
		EVENT_ID_HANDLER(R.id.btn_sleep, EVT_CMD, OnSleep)
		EVENT_ID_HANDLER(R.id.btn_batteryInfo, EVT_CMD, OnBatteryInfo)
		EVENT_ID_HANDLER(R.id.btn_showDevInfo, EVT_CMD, OnDevInfo)
		EVENT_ID_HANDLER(0,EventScreenShot::EventID,OnScreenShot)
	EVENT_MAP_END()

		//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		//托盘消息处理
		//MESSAGE_HANDLER(WM_ICONNOTIFY, OnIconNotify)
		MSG_WM_COMMAND(OnCommand)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	BOOL			m_bLayoutInited;
	STabCtrlHeaderBinder m_mainTabBinder;
	CAutoRefPtr<CiOSDeviceTreeViewAdapter> m_pTreeViewAdapter;

	// 通过 IDeviceEventCallBack 继承
	virtual void idevice_event_cb_t(const idevice_event_t* event) override;

	CBaseInfoDlg* m_pHostDlg = NULL;
};
