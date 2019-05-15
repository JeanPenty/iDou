#pragma once
#include "..\PageHandlerBase.h"
#include "TV_iOSDeviceAdapter.h"
#include "..\iOsDeviceLib\CiOSDevice.h"
#include "LV_iOSAppsAdapter.h"

class CMyDeviceHandler :public CPageHandlerBase
{
public:
	CMyDeviceHandler();
	~CMyDeviceHandler();
	virtual void OnInit(SWindow* pRoot);
	void AddDev(const idevice_event_t* event, bool bCan);
	void RemoveDev(const idevice_event_t* event);
	void PairDev(const idevice_event_t* event, bool bCan);
protected:

	void ChildMenuItemClick(LPCSTR udid, int nGID);

	void OnReboot(EventArgs* pEArg);
	void OnSleep(EventArgs* pEArg);
	void OnShutdown(EventArgs* pEArg);
	void OnScreenShot(EventArgs* pEArg);
	void OnUpdataInfo(EventArgs* pEArg);
	void OnEditDevName();
	void OnKillFoucusByDevName(EventArgs* pEArg);
	void OnKeyDownByDevName(EventArgs* pEArg);
	void OnUpdataDiskInfo(EventArgs* pEArg);
	void OnUpdataAppsInfo(EventArgs* pEArg);
	void OnTVSelChanged(EventArgs* pEArg);

	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(GetRoot())
		EVENT_ID_COMMAND(R.id.btn_etdevname, OnEditDevName)
		EVENT_ID_HANDLER(R.id.tv_ios, EventTVSelChanged::EventID, OnTVSelChanged)
		EVENT_ID_HANDLER(R.id.btn_reboot, EVT_CMD, OnReboot)
		EVENT_ID_HANDLER(R.id.btn_shutdown, EVT_CMD, OnShutdown)
		EVENT_ID_HANDLER(R.id.btn_sleep, EVT_CMD, OnSleep)
		EVENT_ID_HANDLER(0, EventScreenShot::EventID, OnScreenShot)
		EVENT_ID_HANDLER(0, EventUpdataInfo::EventID, OnUpdataInfo)
		EVENT_ID_HANDLER(0, EventUpdataDiskInfo::EventID, OnUpdataDiskInfo)
		EVENT_ID_HANDLER(0, EventUpdataAppsInfo::EventID, OnUpdataAppsInfo)
		EVENT_ID_HANDLER(R.id.et_devname, EventKeyDown::EventID, OnKeyDownByDevName)
		EVENT_ID_HANDLER(R.id.et_devname, EventKillFocus::EventID, OnKillFoucusByDevName)
		EVENT_MAP_BREAK()
public:
	//CAutoRefPtr<CAppsListAdapter> m_pListViewAdapter;
	CAutoRefPtr<CiOSDeviceTreeViewAdapter> m_pTreeViewAdapter;
};

