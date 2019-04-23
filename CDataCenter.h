#pragma once
#include <set>
#include <string>
#include "TV_iOSDeviceAdapter.h"

class CDataCenter:public SSingleton<CDataCenter>
{
protected:
	std::set<std::string> m_listDevGUID;

public:
	bool IsExistDev(LPCSTR guid);
	bool AddDevGUID(LPCSTR guid, CiOSDeviceTreeViewAdapter *pAdapter);
	const std::set<std::string>& GetDevGUIDList()const;
	CDataCenter();
	~CDataCenter();
};