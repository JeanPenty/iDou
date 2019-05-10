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
	bool AddDevGUID(LPCSTR guid, CiOSDeviceTreeViewAdapter *pAdapter);
	bool RemoveDevGUID(LPCSTR udid, CiOSDeviceTreeViewAdapter* pAdapter);
	const std::map<std::string, iOS>& GetDevGUIDList()const;
	bool PairDev(LPCSTR id, CiOSDeviceTreeViewAdapter* pAdapter);
	bool BindInfoWindow(LPCSTR udid, SWindow* pInfoWnd);
	bool UpdataBaseInfo(LPCSTR udid);
	void ShutDown(SWindow* pWnd);
	void Reboot(SWindow* pWnd);
	void Sleep(SWindow* pWnd);
	std::string GetUDIDByWindow(SWindow* pWnd);
	CiOSDevice* GetDevByUDID(LPCSTR udid);
	bool GetGasGauge(LPCSTR udid, BatteryBaseInfo& out);
private:	
	void _docmd(SWindow* pWnd, diagnostics_cmd_mode cmd);

	bool _initdevbaseinfo(const iOSDevInfo&devInfo,SWindow* pInfoWnd);
};