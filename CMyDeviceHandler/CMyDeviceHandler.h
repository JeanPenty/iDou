#pragma once
#include "..\PageHandlerBase.h"
#include "TV_iOSDeviceAdapter.h"
#include "..\iOsDeviceLib\CiOSDevice.h"
#include "LV_iOSAppsAdapter.h"
#include <deque>

class CBaseInfoDlg;

class CMyDeviceHandler :public CPageHandlerBase
{
public:
	CMyDeviceHandler();
	~CMyDeviceHandler();
	virtual void OnInit(SWindow* pRoot);
	void AddDev(const idevice_event_t* event, bool bCan);
	void RemoveDev(const idevice_event_t* event);
	void PairDev(const idevice_event_t* event, bool bCan);
	//当前显示信息是否是被移除的设备。如果是就关掉窗口
	void TryEndInfoDlg(LPCSTR udid);
protected:
	template<class T>
	void ShowInfoDlg(const std::string&);
	SWindow* GetDevCmdWindow(LPCWSTR udid, int cmd);
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
	void OnUnistallApp(EventArgs* pEArg);
	void OnTVSelChanged(EventArgs* pEArg);
	void OnTVFilesMgrSelChanged(EventArgs* pEArg);
	void OnInstallApp(EventArgs* pEArg);
	void OnCheckWarrantyexpirationDate(EventArgs* pEArg);
	void OnInitAfc(EventArgs* pEArg);
	void OnFilesNavTvEventOfPanel(EventArgs* pEvt);
	void OnFileslvEventOfPanel(EventArgs* pEvt);
	void OnUpdataFileList(EventArgs* e);	
	void OnOpenFileRet(EventArgs* e);
	void OnBatteryInfo(EventArgs* pEArg);
	void OnDevInfo(EventArgs* pEArg);
	void OnUninstallapps(EventArgs* pEArg);
	void OnRefershapp(EventArgs* pEArg);
	void OnInstallAppCb(EventArgs* pEArg);
	void OnGoBack(EventArgs* pEArg);
	void OnGoForward(EventArgs* pEArg);
	void OnUpdataContacts(EventArgs* pEArg);
	void OnContactSelChanged(EventArgs* pEArg);
//文件浏览记录
	struct FileRecord {
		bool isAfc2;
		std::string path;
	};
	struct FileRecords {
		std::deque<FileRecord> record1;
		std::deque<FileRecord> record2;
	};
	void PushFileRecord(const std::string& udid,bool isafc2,const std::string& path);
	bool PopFileRecord(const std::string& udid, FileRecord &filerecord);
	void ResetRecord(const std::string& udid);
	std::map<std::string, FileRecords> m_filerecords;
	FileRecord m_lastRecord;
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(GetRoot())
		EVENT_ID_COMMAND(R.id.btn_etdevname, OnEditDevName)
		EVENT_ID_HANDLER(R.id.btn_batteryInfo, EVT_CMD, OnBatteryInfo)
		EVENT_ID_HANDLER(R.id.btn_showDevInfo, EVT_CMD, OnDevInfo)
		EVENT_ID_HANDLER(R.id.tv_ios, EventTVSelChanged::EventID, OnTVSelChanged)
		EVENT_ID_HANDLER(R.id.btn_refershapp, EVT_CMD, OnRefershapp)
		EVENT_ID_HANDLER(R.id.btn_reboot, EVT_CMD, OnReboot)
		EVENT_ID_HANDLER(R.id.btn_shutdown, EVT_CMD, OnShutdown)
		EVENT_ID_HANDLER(R.id.btn_sleep, EVT_CMD, OnSleep)
		EVENT_ID_HANDLER(R.id.btn_uninstallapp, EVT_CMD, OnUninstallapps)
		EVENT_ID_HANDLER(0, EventScreenShot::EventID, OnScreenShot)
		EVENT_ID_HANDLER(0, EventUpdataInfo::EventID, OnUpdataInfo)
		EVENT_ID_HANDLER(0, EventUpdataDiskInfo::EventID, OnUpdataDiskInfo)
		EVENT_ID_HANDLER(0, EventUpdataAppsInfo::EventID, OnUpdataAppsInfo)
		EVENT_ID_HANDLER(0, EventUninstallApp::EventID, OnUnistallApp)
		EVENT_ID_HANDLER(0, EventAFCInit::EventID, OnInitAfc)
		EVENT_ID_HANDLER(0, EventUpdataFile::EventID, OnUpdataFileList)
		EVENT_ID_HANDLER(0, EventOpenFileRet::EventID,OnOpenFileRet)
		EVENT_ID_HANDLER(0, EventInstallApp::EventID, OnInstallAppCb)
		EVENT_ID_HANDLER(0, EventUpdataContacts::EventID,OnUpdataContacts)

		EVENT_ID_HANDLER(R.id.tv_files_mgr, EventOfPanel::EventID, OnFilesNavTvEventOfPanel)
		EVENT_ID_HANDLER(R.id.lv_filesList, EventOfPanel::EventID, OnFileslvEventOfPanel)
		EVENT_ID_HANDLER(R.id.et_devname, EventKeyDown::EventID, OnKeyDownByDevName)
		EVENT_ID_HANDLER(R.id.et_devname, EventKillFocus::EventID, OnKillFoucusByDevName)
		EVENT_ID_HANDLER(R.id.btn_WARRANTYEXPIRATIONDATE, EVT_CMD, OnCheckWarrantyexpirationDate)
		EVENT_ID_HANDLER(R.id.btn_installapp, EVT_CMD,OnInstallApp)
		EVENT_ID_HANDLER(R.id.btn_goback, EVT_CMD, OnGoBack)
		EVENT_ID_HANDLER(R.id.btn_goforward, EVT_CMD, OnGoForward)
		EVENT_ID_HANDLER(R.id.lv_contactsList, EventLVSelChanged::EventID,OnContactSelChanged)
	EVENT_MAP_BREAK()
public:
	CAutoRefPtr<CiOSDeviceTreeViewAdapter> m_pTreeViewAdapter;
	CBaseInfoDlg* m_pHostDlg = NULL;
	
};

