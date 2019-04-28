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
	bool IsExistDev(LPCSTR guid);
	bool GetDevName(LPCSTR udid, SStringT& outName);
	bool AddDevGUID(LPCSTR guid, CiOSDeviceTreeViewAdapter *pAdapter);
	bool RemoveDevGUID(LPCSTR udid, CiOSDeviceTreeViewAdapter* pAdapter);
	const std::map<std::string, iOS>& GetDevGUIDList()const;
	bool PairDev(LPCSTR id, CiOSDeviceTreeViewAdapter* pAdapter);
	bool BindInfoWindow(LPCSTR udid, SWindow* pInfoWnd);
	bool UpdataBaseInfo(LPCSTR udid);
	CDataCenter();
	~CDataCenter();
private:
	bool _initdevbaseinfo(const iOSDevInfo&devInfo,SWindow* pInfoWnd);
};