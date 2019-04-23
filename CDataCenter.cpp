#include "stdafx.h"
#include "CDataCenter.h"


CDataCenter *CDataCenter::ms_Singleton = NULL;

bool CDataCenter::IsExistDev(LPCSTR guid)
{
	if (guid)
	{
		if (m_listDevGUID.find(guid) != m_listDevGUID.end())
			return true;
	}
	return false;
}

bool CDataCenter::AddDevGUID(LPCSTR guid, CiOSDeviceTreeViewAdapter *pAdapter)
{
	if (guid)
	{
		if (IsExistDev(guid))
			return false;
		m_listDevGUID.insert(guid);
	
		pAdapter->AddDev(guid);
		return true;
	}
	return false;
}

const std::set<std::string>& CDataCenter::GetDevGUIDList() const
{
	return m_listDevGUID;
}

CDataCenter::CDataCenter()
{
}

CDataCenter::~CDataCenter()
{
}
