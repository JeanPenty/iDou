#include "stdafx.h"
#include "CDataCenter.h"
#include "iOsDeviceLib/iOSUtils.h"
#include "TV_iOSDeviceAdapter.h"

CDataCenter* CDataCenter::ms_Singleton = NULL;

bool CDataCenter::IsExistDev(LPCSTR udid)
{
	if (udid)
	{
		if (m_listDev.find(udid) != m_listDev.end())
			return true;
	}
	return false;
}

bool CDataCenter::RemoveDevGUID(LPCSTR udid, CiOSDeviceTreeViewAdapter * pAdapter)
{
	if (udid)
	{
		auto ite = m_listDev.find(udid);

		if (ite == m_listDev.end())
			return false;

		m_listDev.erase(ite);
		pAdapter->RemoveDev(udid);
		return true;
	}
	return false;
}

bool CDataCenter::PairDev(LPCSTR udid, CiOSDeviceTreeViewAdapter * pAdapter)
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


bool CDataCenter::GetDevName(LPCSTR udid, SStringT & outName)
{
	if (!IsExistDev(udid))
		return false;
	return m_listDev[udid].iOSDevObject.GetDevName(outName);
}

bool CDataCenter::AddDevGUID(LPCSTR udid, CiOSDeviceTreeViewAdapter * pAdapter)
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

bool CDataCenter::BindInfoWindow(LPCSTR udid, SWindow * pInfoWnd)
{
	SASSERT(udid);
	SASSERT(pInfoWnd);
	if (udid && pInfoWnd)
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
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventScreenShot));
}

CDataCenter::~CDataCenter()
{
}

void CDataCenter::_docmd(SWindow * pWnd, diagnostics_cmd_mode cmd)
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

bool CDataCenter::GetGasGauge(LPCSTR udid, GasGauge & out)
{
	bool bRet = false;
	auto iter = m_listDev.find(udid);
	if (iter == m_listDev.end())
		return false;
	iter->second.iOSDevObject.GetGasGauge(out);
	return true;
}

std::string CDataCenter::GetUDIDByWindow(SWindow * pWnd)
{
	for (auto& iter : m_listDev)
	{
		if (iter.second.InfoWnd == pWnd)
		{
			return iter.first;
		}
	}
	return "";
}

const CiOSDevice* CDataCenter::GetDevByUDID(LPCSTR udid)
{
	auto iter = m_listDev.find(udid);
	if (iter == m_listDev.end())
		return NULL;
	return &iter->second.iOSDevObject;
}

void CDataCenter::ShutDown(SWindow * pWnd)
{
	_docmd(pWnd, CMD_SHUTDOWN);
}
void CDataCenter::Reboot(SWindow * pWnd)
{
	_docmd(pWnd, CMD_RESTART);
}
void CDataCenter::Sleep(SWindow * pWnd)
{
	_docmd(pWnd, CMD_SLEEP);
}

bool CDataCenter::_initdevbaseinfo(const iOSDevInfo & devInfo, SWindow * pInfoWnd)
{
	SASSERT(pInfoWnd);
	SWindow* pWnd = pInfoWnd->FindChildByID(R.id.BaseInfoWnd);
	SASSERT(pWnd);
	if (pWnd)
	{
		pInfoWnd->FindChildByID(R.id.lable_ProductName)->SetWindowText(devInfo.m_strDevProductName);
		pInfoWnd->FindChildByID(R.id.lable_FirmwareVersion)->SetWindowText(devInfo.m_strProductVersion+L"("+ devInfo.m_strBuildVersion+L")");
		pInfoWnd->FindChildByID(R.id.lable_SerialNumber)->SetWindowText(devInfo.m_strDevSerialNumber);
		pInfoWnd->FindChildByID(R.id.lable_ModelNumber)->SetWindowText(devInfo.m_strDevModelNumber+L" "+devInfo.m_strRegionInfo);
		
		pInfoWnd->FindChildByID(R.id.lable_HardwareModel)->SetWindowText(devInfo.m_strDevHardwareModel);
		pInfoWnd->FindChildByID(R.id.lable_IMEI)->SetWindowText(devInfo.m_strDevIMEI);
		pInfoWnd->FindChildByID(R.id.lable_ProductType)->SetWindowText(devInfo.m_strDevProductType);
		pInfoWnd->FindChildByID(R.id.lable_UDID)->SetWindowText(devInfo.m_strDevUDID);
		pInfoWnd->FindChildByID(R.id.lable_CycleCount)->SetWindowText(SStringT().Format(L"%d次", devInfo.m_sGasGauge.CycleCount));

		pInfoWnd->FindChildByID(R.id.lable_ActivationState)->SetWindowText(devInfo.m_strActivationState==L"Activated"?L"已激活":L"未激活");

		pInfoWnd->FindChildByID(R.id.btn_reboot)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_shutdown)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_sleep)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_batteryInfo)->SetUserData((ULONG_PTR)pInfoWnd);

		const WCHAR* screenskin[] = {  L"skin_iphonescreen",L"skin_ipadscreen", };
		pInfoWnd->FindChildByID(R.id.img_srceenshot)->SetAttribute(L"skin", screenskin[devInfo.m_type]);

		//pInfoWnd->FindChildByID(R.id.BaseInfoWnd)->SetAttribute(L"colorBorder", devInfo.m_strDeviceColor);
		//换算成百分比
		int sys = devInfo.m_diskInfo.TotalSystemAvailable * 100 / devInfo.m_diskInfo.TotalDiskCapacity;

		pInfoWnd->FindChildByID(R.id.disk_sys)->GetLayoutParam()->SetAttribute(L"weight", L"",FALSE);
		pInfoWnd->FindChildByID(R.id.disk_app)->GetLayoutParam()->SetAttribute(L"weight", L"", FALSE);
		pInfoWnd->FindChildByID(R.id.disk_photo)->GetLayoutParam()->SetAttribute(L"weight", L"", FALSE);
		pInfoWnd->FindChildByID(R.id.disk_av)->GetLayoutParam()->SetAttribute(L"weight", L"", FALSE);
		pInfoWnd->FindChildByID(R.id.disk_u)->GetLayoutParam()->SetAttribute(L"weight", L"", FALSE);
		pInfoWnd->FindChildByID(R.id.disk_other)->GetLayoutParam()->SetAttribute(L"weight", L"", FALSE);
		pInfoWnd->FindChildByID(R.id.disk_free)->GetLayoutParam()->SetAttribute(L"weight", L"", FALSE);

		return true;
	}
	return false;
}


