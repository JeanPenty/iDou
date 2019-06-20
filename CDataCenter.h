#pragma once
#include <set>
#include <string>
#include <map>

#include "iOsDeviceLib/CiOSDevice.h"
class CiOSDeviceTreeViewAdapter;

struct iOS {
	SWindow* InfoWnd;
	CiOSDevice iOSDevObject;
};

class CDataCenter:public SSingleton<CDataCenter>
{
protected:
	std::map<std::string, iOS> m_listDev;
public:
	CDataCenter();
	~CDataCenter();
	bool IsExistDev(LPCSTR guid);
	bool GetDevName(LPCSTR udid, SStringT& outName);
	bool AddDevUDID(LPCSTR udid, bool& bCan);
	bool BeginUpdataInfoASync(LPCSTR udid);
	bool RemoveDevGUID(LPCSTR udid);
	const std::map<std::string, iOS>& GetDevGUIDList()const;
	bool PairDev(LPCSTR id, bool& bCan);
	bool BindInfoWindow(LPCSTR udid, SWindow* pInfoWnd);
	bool UpdataBaseInfo(LPCSTR udid);
	void ShutDown(SWindow* pWnd);
	void Reboot(SWindow* pWnd);
	void Sleep(SWindow* pWnd);
	std::string GetUDIDByWindow(SWindow* pWnd);
	CiOSDevice* GetDevByUDID(LPCSTR udid);
	CiOSDevice* GetDevByWindow(SWindow* pWnd);
	bool GetGasGauge(LPCSTR udid, BatteryBaseInfo& out);
	bool UpdataDiskInfo(LPCSTR udid);	
	void UninstallApp(LPCSTR udid,LPCSTR appid);
	int GetAppsCount(LPCSTR udid);
private:	
	void _docmd(SWindow* pWnd, diagnostics_cmd_mode cmd);

	bool _initdevbaseinfo(const iOSDevInfo&devInfo,SWindow* pInfoWnd);
	
};