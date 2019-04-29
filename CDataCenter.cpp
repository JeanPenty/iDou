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
			if (m_listDev[udid].iOSDevObject.OpenDevice(udid))
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
	return m_listDev[udid].iOSDevObject.GetDevName(outName);
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
			if (m_listDev[udid].iOSDevObject.OpenDevice(udid))
			{
				m_listDev[udid].iOSDevObject.GetDeviceBaseInfo();
				bCan = true;
			}
		}
		pAdapter->AddDev(udid, bCan);
		return true;
	}
	return false;
}

const std::map<std::string, iOS>& CDataCenter::GetDevGUIDList() const
{
	return m_listDev;
}

bool CDataCenter::BindInfoWindow(LPCSTR udid, SWindow* pInfoWnd)
{
	SASSERT(udid);
	SASSERT(pInfoWnd);
	if (udid&& pInfoWnd)
	{
		auto ite = m_listDev.find(udid);

		if (ite == m_listDev.end())
			return false;
		ite->second.InfoWnd = pInfoWnd;
		return true;
	}
	return false;
}

bool CDataCenter::UpdataBaseInfo(LPCSTR udid)
{
	SASSERT(udid);
	if (udid)
	{
		auto ite = m_listDev.find(udid);
		if (ite == m_listDev.end())
			return false;
		return _initdevbaseinfo(ite->second.iOSDevObject.GetiOSBaseInfo(), ite->second.InfoWnd);
		 
	}
	return false;
}

CDataCenter::CDataCenter()
{
}

CDataCenter::~CDataCenter()
{
}

void CDataCenter::_docmd(SWindow* pWnd, diagnostics_cmd_mode cmd)
{
	for (auto& iter : m_listDev)
	{
		if (iter.second.InfoWnd == pWnd)
		{
			iter.second.iOSDevObject.DoCmd(cmd);
			break;
		}
	}
}

void CDataCenter::ShutDown(SWindow* pWnd)
{
	_docmd(pWnd, CMD_SHUTDOWN);
}
void CDataCenter::Reboot(SWindow* pWnd)
{
	_docmd(pWnd, CMD_RESTART);
}
void CDataCenter::Sleep(SWindow* pWnd)
{
	_docmd(pWnd, CMD_SLEEP);
}

bool CDataCenter::_initdevbaseinfo(const iOSDevInfo& devInfo, SWindow* pInfoWnd)
{
	SASSERT(pInfoWnd);
	SWindow* pWnd = pInfoWnd->FindChildByID(R.id.BaseInfoWnd);
	SASSERT(pWnd);
	if (pWnd)
	{
		pInfoWnd->FindChildByID(R.id.lable_ProductName)->SetWindowText(devInfo.m_strDevProductName);

		pInfoWnd->FindChildByID(R.id.lable_SerialNumber)->SetWindowText(devInfo.m_strDevSerialNumber);
		pInfoWnd->FindChildByID(R.id.lable_ModelNumber)->SetWindowText(devInfo.m_strDevModelNumber);
		pInfoWnd->FindChildByID(R.id.lable_WiFiAddress)->SetWindowText(devInfo.m_strDevWiFiAddress);
		pInfoWnd->FindChildByID(R.id.lable_BluetoothAddress)->SetWindowText(devInfo.m_strDevBluetoothAddress);
		pInfoWnd->FindChildByID(R.id.lable_HardwareModel)->SetWindowText(devInfo.m_strDevHardwareModel);
		pInfoWnd->FindChildByID(R.id.lable_IMEI)->SetWindowText(devInfo.m_strDevIMEI);
		pInfoWnd->FindChildByID(R.id.lable_ProductType)->SetWindowText(devInfo.m_strDevProductType);
		pInfoWnd->FindChildByID(R.id.lable_UDID)->SetWindowText(devInfo.m_strDevUDID);
		pInfoWnd->FindChildByID(R.id.lable_CycleCount)->SetWindowText(SStringT().Format(L"%d´Î", devInfo.m_sGasGauge.CycleCount));


		pInfoWnd->FindChildByID(R.id.btn_reboot)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_shutdown)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_sleep)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_batteryInfo)->SetUserData((ULONG_PTR)pInfoWnd);

		return true;
	}
	return false;
}
