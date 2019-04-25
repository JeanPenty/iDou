#pragma once
#include <set>
#include <string>
#include <map>

#include "iOsDeviceLib/CiOSDevice.h"
class CiOSDeviceTreeViewAdapter;

class CDataCenter:public SSingleton<CDataCenter>
{
protected:
	std::map<std::string, CiOSDevice> m_listDev;
public:
	bool IsExistDev(LPCSTR guid);
	bool GetDevName(LPCSTR udid, SStringT& outName);
	bool AddDevGUID(LPCSTR guid, CiOSDeviceTreeViewAdapter *pAdapter);
	bool RemoveDevGUID(LPCSTR udid, CiOSDeviceTreeViewAdapter* pAdapter);
	const std::map<std::string, CiOSDevice>& GetDevGUIDList()const;
	bool PairDev(LPCSTR id, CiOSDeviceTreeViewAdapter* pAdapter);
	CDataCenter();
	~CDataCenter();
};