#include "stdafx.h"
#include "CDataCenter.h"
#include "iOsDeviceLib/iOSUtils.h"
#include "TV_iOSDeviceAdapter.h"

CDataCenter *CDataCenter::ms_Singleton = NULL;

bool CDataCenter::IsExistDev(LPCSTR udid)
{
	if (udid)
	{
		if (m_listDev.find(udid) != m_listDev.end())
			return true;
	}
	return false;
}

bool CDataCenter::RemoveDevGUID(LPCSTR udid, CiOSDeviceTreeViewAdapter *pAdapter)
{
	if (udid)
	{
		auto ite = m_listDev.find(udid);

		if (ite==m_listDev.end())
			return false;

		m_listDev.erase(ite);
		pAdapter->RemoveDev(udid);
		return true;
	}
	return false;
}

bool CDataCenter::PairDev(LPCSTR udid, CiOSDeviceTreeViewAdapter* pAdapter)
{
	if (udid)
	{
		if (!IsExistDev(udid))
			return false;
		bool bCan = false;
		if (CiOSDevice::IsPair(udid))
		{
			if (m_listDev[udid].OpenDevice(udid))
			{
				bCan = true;
			}
		}
		pAdapter->SetDevCan(udid, bCan);
		return true;
	}
	return false;
}


bool CDataCenter::GetDevName(LPCSTR udid,SStringT& outName)
{
	if (!IsExistDev(udid))
		return false;
	return m_listDev[udid].GetDevName(outName);
}

bool CDataCenter::AddDevGUID(LPCSTR udid, CiOSDeviceTreeViewAdapter* pAdapter)
{
	if (udid)
	{
		if (IsExistDev(udid))
			return false;
		m_listDev[udid];
		bool bCan = false;
		if (CiOSDevice::IsPair(udid))
		{
			if (m_listDev[udid].OpenDevice(udid))
			{
				bCan = true;
			}
		}

		pAdapter->AddDev(udid, bCan);
		return true;
	}
	return false;
}

const std::map<std::string, CiOSDevice>& CDataCenter::GetDevGUIDList() const
{
	return m_listDev;
}

CDataCenter::CDataCenter()
{
}

CDataCenter::~CDataCenter()
{
}
