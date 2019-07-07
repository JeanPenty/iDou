#include "stdafx.h"
#include "CDataCenter.h"
#include "iOsDeviceLib/iOSUtils.h"
#include "CMyDeviceHandler/TV_iOSDeviceAdapter.h"

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

bool CDataCenter::RemoveDevGUID(LPCSTR udid)
{
	if (udid)
	{
		auto ite = m_listDev.find(udid);

		if (ite == m_listDev.end())
			return false;

		m_listDev.erase(ite);
		return true;
	}
	return false;
}

bool CDataCenter::PairDev(LPCSTR udid, bool& bCan)
{
	if (udid)
	{
		if (!IsExistDev(udid))
			return false;
		bCan = false;
		if (CiOSDevice::IsPair(udid))
		{
			if (m_listDev[udid].iOSDevObject.OpenDevice(udid))
			{
				m_listDev[udid].iOSDevObject.GetDeviceBaseInfo();
				bCan = true;
			}
		}
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

bool CDataCenter::AddDevUDID(LPCSTR udid, bool& bCan)
{
	if (udid)
	{
		if (IsExistDev(udid))
			return false;
		m_listDev[udid];
		bCan = false;
		if (CiOSDevice::IsPair(udid))
		{
			if (m_listDev[udid].iOSDevObject.OpenDevice(udid))
			{
				m_listDev[udid].iOSDevObject.GetDeviceBaseInfo();
				bCan = true;
			}
		}
		return true;
	}
	return false;
}

bool CDataCenter::BeginUpdataInfoASync(LPCSTR udid)
{
	SASSERT(udid);
	if (udid)
	{
		auto ite = m_listDev.find(udid);

		if (ite == m_listDev.end())
			return false;

		ite->second.iOSDevObject.StartUpdata();
		ite->second.iOSDevObject.StartUpdataApps();
		ite->second.iOSDevObject.StartCapshot();
		ite->second.iOSDevObject.StartUpdataDiskInfo();
		ite->second.iOSDevObject.StartGetContacts();
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
		bool bRet = _initdevbaseinfo(ite->second.iOSDevObject.GetiOSBaseInfo(), ite->second.InfoWnd);
		/*if (!(ite->second.iOSDevObject.GetiOSBaseInfo().m_diskInfo.bReady))
		{
			ite->second.iOSDevObject.StartUpdataDiskInfo();
		}*/
		return bRet;
	}
	return false;
}

CDataCenter::CDataCenter()
{
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventScreenShot));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventUpdataInfo));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventUpdataBattreyInfo));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventUpdataDiskInfo));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventUpdataAppsInfo));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventUninstallApp));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventInstallApp));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventAFCInit));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventUpdataFile));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventOpenFileRet));
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventUpdataContacts));
}

CDataCenter::~CDataCenter()
{
	SNotifyCenter::getSingleton().removeAllEvents();
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

bool CDataCenter::GetGasGauge(LPCSTR udid, BatteryBaseInfo & out)
{
	bool bRet = false;
	auto iter = m_listDev.find(udid);
	if (iter == m_listDev.end())
		return false;
	iter->second.iOSDevObject.GetBatteryBaseInfo(out);
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

CiOSDevice* CDataCenter::GetDevByUDID(LPCSTR udid)
{
	auto iter = m_listDev.find(udid);
	if (iter == m_listDev.end())
		return NULL;
	return &iter->second.iOSDevObject;
}

CiOSDevice* CDataCenter::GetDevByWindow(SWindow* pWnd)
{
	for (auto& iter : m_listDev)
	{
		if (iter.second.InfoWnd == pWnd)
		{
			return GetDevByUDID(iter.first.c_str());
		}
	}
	return NULL;
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

void _initdiskspase(SWindow * pWnd, const uint64_t & size, const uint64_t & tsize)
{
	if (pWnd)
	{
		int percentage = (int)(size * 1000 / tsize);
		double gb = (double)size / 1024 / 1024 / 1024 + 0.005;
		pWnd->GetLayoutParam()->SetAttribute(L"weight", SStringT().Format(L"%d", percentage), FALSE);
		pWnd->SetAttribute(L"tip", SStringT().Format(L"%0.2fGB", gb), FALSE);
	}
}

void _initipaddiskinfo(SWindow * pInfoWnd, const DiskInfo & diskInfo)
{
	if (pInfoWnd)
	{
		SWindow* lable1 = pInfoWnd->FindChildByID(R.id.lable_DiskSize);
		SASSERT(lable1);
		pInfoWnd->FindChildByID(R.id.wnd_iphonedsk)->SetVisible(FALSE, TRUE);
		SWindow* pIpadWnd = pInfoWnd->FindChildByID(R.id.wnd_ipaddsk);
		pIpadWnd->SetVisible(TRUE, TRUE);
		{
			SWindow* lable2 = pIpadWnd->FindChildByID(R.id.lable_fudisk);
			SASSERT(lable2);
			double gb = (double)diskInfo.TotalDiskCapacity / 1024 / 1024 / 1024;
			SStringT totalGB;
			if (gb < 8)
			{
				totalGB = (L"8GB");
			}
			else if (gb < 16)
			{
				totalGB = (L"16GB");
			}
			else if (gb < 32)
			{
				totalGB = (L"32GB");
			}
			else if (gb < 64)
			{
				totalGB = (L"64GB");
			}
			else if (gb < 128)
			{
				totalGB = (L"128GB");
			}
			else if (gb < 256)
			{
				totalGB = (L"256GB");
			}
			else if (gb < 512)
			{
				totalGB = (L"512GB");
			}
			else if (gb < 1024)
			{
				totalGB = (L"1024GB");
			}
			lable1->SetWindowText(totalGB);
			uint64_t use = diskInfo.TotalDiskCapacity - diskInfo.TotalDataAvailable - diskInfo.TotalSystemAvailable;
			gb = (double)use / 1024 / 1024 / 1024 + 0.005;
			lable2->SetWindowText(SStringT().Format(L"%0.2fGB", gb) + L"/" + totalGB);

			_initdiskspase(pIpadWnd->FindChildByID(R.id.disk_sys), diskInfo.TotalSystemCapacity, diskInfo.TotalDiskCapacity);
			_initdiskspase(pIpadWnd->FindChildByID(R.id.disk_app), diskInfo.TotalSystemCapacity, diskInfo.TotalDiskCapacity);
			//_initdiskspase(pInfoWnd->FindChildByID(R.id.disk_sys), devInfo.m_diskInfo.TotalSystemCapacity, devInfo.m_diskInfo.TotalDiskCapacity);
			//_initdiskspase(pInfoWnd->FindChildByID(R.id.disk_free), diskInfo.TotalSystemCapacity, diskInfo.TotalDiskCapacity);
			_initdiskspase(pIpadWnd->FindChildByID(R.id.disk_free), diskInfo.TotalDataAvailable + diskInfo.TotalSystemAvailable, diskInfo.TotalDiskCapacity);
		}
	}
}

void _initidiskinfo(SWindow * pInfoWnd, const DiskInfo & diskInfo)
{
	if (pInfoWnd)
	{
		SWindow* lable = pInfoWnd->FindChildByID(R.id.lable_DiskSize);
		SASSERT(lable);
		pInfoWnd->FindChildByID(R.id.wnd_ipaddsk)->SetVisible(FALSE, TRUE);
		SWindow* pIphoneWnd = pInfoWnd->FindChildByID(R.id.wnd_iphonedsk);
		pIphoneWnd->SetVisible(TRUE, TRUE);		
		{
			double gb = (double)diskInfo.TotalDiskCapacity / 1024 / 1024 / 1024;
			SStringT totalGB;
			if (gb < 8)
			{
				totalGB = (L"8GB");
			}
			else if (gb < 16)
			{
				totalGB = (L"16GB");
			}
			else if (gb < 32)
			{
				totalGB = (L"32GB");
			}
			else if (gb < 64)
			{
				totalGB = (L"64GB");
			}
			else if (gb < 128)
			{
				totalGB = (L"128GB");
			}
			else if (gb < 256)
			{
				totalGB = (L"256GB");
			}
			else if (gb < 512)
			{
				totalGB = (L"512GB");
			}
			else if (gb < 1024)
			{
				totalGB = (L"1024GB");
			}
			lable->SetWindowText(totalGB);


			lable = pIphoneWnd->FindChildByID(R.id.lable_sysdisk);
			SASSERT(lable);


			gb = (double)diskInfo.TotalSystemCapacity / 1024 / 1024 / 1024 + 0.005;
			double gbu = (double)(diskInfo.TotalSystemCapacity - diskInfo.TotalSystemAvailable) / 1024 / 1024 / 1024 + 0.005;
			lable->SetWindowText(SStringT().Format(L"%0.2fGB", gbu) + L"/" + SStringT().Format(L"%0.2fGB", gb));

			lable = pIphoneWnd->FindChildByID(R.id.lable_datadisk);
			SASSERT(lable);

			gb = (double)diskInfo.TotalDataCapacity / 1024 / 1024 / 1024 + 0.005;
			gbu = (double)(diskInfo.TotalDataCapacity - diskInfo.TotalDataAvailable) / 1024 / 1024 / 1024 + 0.005;
			lable->SetWindowText(SStringT().Format(L"%0.2fGB", gbu) + L"/" + SStringT().Format(L"%0.2fGB", gb));
			//系统区
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_sys), diskInfo.TotalSystemCapacity - diskInfo.TotalSystemAvailable, diskInfo.TotalSystemCapacity);
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_free1), diskInfo.TotalSystemAvailable, diskInfo.TotalSystemCapacity);
			//数据区			
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_other), diskInfo.TotalDataCapacity- diskInfo.TotalDataAvailable, diskInfo.TotalDataCapacity);
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_free2), diskInfo.TotalDataAvailable, diskInfo.TotalDataCapacity);
		}
	}
}

void _initidiskinfo2(SWindow* pInfoWnd, const DiskInfo& diskInfo)
{
	if (pInfoWnd)
	{
		if (diskInfo.bReady)
		{			
			SWindow* pIpadWnd = pInfoWnd->FindChildByID(R.id.wnd_ipaddsk);
			_initdiskspase(pIpadWnd->FindChildByID(R.id.disk_sys), diskInfo.TotalSystemCapacity, diskInfo.TotalDiskCapacity);
			_initdiskspase(pIpadWnd->FindChildByID(R.id.disk_app), diskInfo.TotalSystemCapacity, diskInfo.TotalDiskCapacity);
			//_initdiskspase(pInfoWnd->FindChildByID(R.id.disk_sys), devInfo.m_diskInfo.TotalSystemCapacity, devInfo.m_diskInfo.TotalDiskCapacity);
			//_initdiskspase(pInfoWnd->FindChildByID(R.id.disk_free), diskInfo.TotalSystemCapacity, diskInfo.TotalDiskCapacity);
			_initdiskspase(pIpadWnd->FindChildByID(R.id.disk_free), diskInfo.TotalDataAvailable + diskInfo.TotalSystemAvailable, diskInfo.TotalDiskCapacity);
		}
	}
}

void _initiphonediskinfo2(SWindow * pInfoWnd, const DiskInfo & diskInfo)
{
	if (pInfoWnd)
	{		
		if (diskInfo.bReady)
		{
			SWindow* pIphoneWnd = pInfoWnd->FindChildByID(R.id.wnd_iphonedsk);			
			//系统区
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_sys), diskInfo.TotalSystemCapacity - diskInfo.TotalSystemAvailable, diskInfo.TotalSystemCapacity);
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_free1), diskInfo.TotalSystemAvailable, diskInfo.TotalSystemCapacity);
			//数据区
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_app), diskInfo.AppUsage, diskInfo.TotalDataCapacity);
			uint64_t other = diskInfo.TotalDataCapacity - diskInfo.TotalDataAvailable - diskInfo.AppUsage;
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_photo), diskInfo.PhotoUsage, diskInfo.TotalDataCapacity);
			other -= diskInfo.PhotoUsage;
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_udisk), 0, diskInfo.TotalDataCapacity);
			other -= 0;
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_other), other, diskInfo.TotalDataCapacity);
			_initdiskspase(pIphoneWnd->FindChildByID(R.id.disk_free2), diskInfo.TotalDataAvailable, diskInfo.TotalDataCapacity);
		}
	}
}

bool CDataCenter::_initdevbaseinfo(const iOSDevInfo & devInfo, SWindow * pInfoWnd)
{
	SASSERT(pInfoWnd);
	SWindow* pWnd = pInfoWnd->FindChildByID(R.id.BaseInfoWnd);
	SASSERT(pWnd);
	if (pWnd)
	{
		pWnd->FindChildByID(R.id.lable_ProductName)->SetWindowText(devInfo.m_strDevProductName);
		pWnd->FindChildByID(R.id.lable_FirmwareVersion)->SetWindowText(devInfo.m_strProductVersion + L"(" + devInfo.m_strBuildVersion + L")");
		pWnd->FindChildByID(R.id.lable_SerialNumber)->SetWindowText(devInfo.m_strDevSerialNumber);
		pWnd->FindChildByID(R.id.lable_ModelNumber)->SetWindowText(devInfo.m_strDevModelNumber + L" " + devInfo.m_strRegionInfo);

		pWnd->FindChildByID(R.id.lable_HardwareModel)->SetWindowText(devInfo.m_strDevHardwareModel);
		pWnd->FindChildByID(R.id.lable_IMEI)->SetWindowText(devInfo.m_strDevIMEI);
		pWnd->FindChildByID(R.id.lable_ProductType)->SetWindowText(devInfo.m_strDevProductType);
		pWnd->FindChildByID(R.id.lable_UDID)->SetWindowText(devInfo.m_strDevUDID);
		pWnd->FindChildByID(R.id.lable_CycleCount)->SetWindowText(SStringT().Format(L"%d次", devInfo.m_sGasGauge.CycleCount));

		pWnd->FindChildByID(R.id.lable_ECID)->SetWindowText(devInfo.m_strECID);
		pWnd->FindChildByID(R.id.lable_IsJailreak)->SetWindowText(devInfo.m_bIsJailreak ? L"已越狱" : L"未越狱");
		pWnd->FindChildByID(R.id.lable_ActivationState)->SetWindowText(devInfo.m_strActivationState == L"Activated" ? L"已激活" : L"未激活");

		pWnd->FindChildByID(R.id.lable_LOC)->SetWindowText(utils::getLoc(devInfo.m_strRegionInfo));

		SStringT productDate;
		if(utils::getDevManufactureDateFormSN(devInfo.m_strDevSerialNumber, productDate))
			pWnd->FindChildByID(R.id.lable_ProductDate)->SetWindowText(productDate);

		int iCycleLife = (int)(((float)devInfo.m_sGasGauge.NominalChargeCapacity * 100 / devInfo.m_sGasGauge.DesignCapacity) + 0.5);
		if (iCycleLife > 100)iCycleLife = 100;
		pWnd->FindChildByID(R.id.lable_CycleLife)->SetWindowText(SStringT().Format(L"%d%%", iCycleLife));

		pInfoWnd->FindChildByID(R.id.txt_devname)->SetWindowText(devInfo.m_strDevName);

		pInfoWnd->FindChildByID(R.id.et_devname)->SetUserData((ULONG_PTR)pInfoWnd);

		pWnd->FindChildByID(R.id.btn_WARRANTYEXPIRATIONDATE)->SetUserData((ULONG_PTR)pInfoWnd);
		pWnd->FindChildByID(R.id.btn_batteryInfo)->SetUserData((ULONG_PTR)pInfoWnd);
		pWnd->FindChildByID(R.id.btn_showDevInfo)->SetUserData((ULONG_PTR)pInfoWnd);

		pInfoWnd->FindChildByID(R.id.btn_installapp)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_reboot)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_shutdown)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.btn_sleep)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.apps_ctrl_wnd)->SetUserData((ULONG_PTR)pInfoWnd);
		pInfoWnd->FindChildByID(R.id.wnd_file_uri)->SetUserData((ULONG_PTR)pInfoWnd);
		
		const WCHAR * screenskin[] = { L"skin_iphonescreen",L"skin_ipadscreen", };
		if (devInfo.m_bIsJailreak)
		{
			SStringT color1, color2;
			SStringT color = utils::getphonecolor(devInfo.m_strDevProductType, devInfo.m_strDeviceColor, devInfo.m_strDeviceEnclosureColor, color1, color2);
			SWindow* pSrceenShotBk = pInfoWnd->FindChildByID(R.id.img_srceenshotbk);
			pSrceenShotBk->SetAttribute(L"colorBkgnd", color);
			pSrceenShotBk->SetAttribute(L"skin", screenskin[0]);
			pSrceenShotBk->SetAttribute(L"inset", L"15,47,14,46");
			pInfoWnd->FindChildByID(R.id.lable_DevColor)->SetWindowText(color2);
			_initidiskinfo(pInfoWnd, devInfo.m_diskInfo);
		}
		else
		{
			_initidiskinfo2(pInfoWnd, devInfo.m_diskInfo);
			SWindow* pSrceenShotBk = pInfoWnd->FindChildByID(R.id.img_srceenshotbk);
			//pSrceenShotBk->SetAttribute(L"colorBkgnd", color);
			pSrceenShotBk->SetAttribute(L"skin", screenskin[1]);
			pSrceenShotBk->SetAttribute(L"inset", L"13,28,14,29");
		}

		return true;
	}
	return false;
}

bool CDataCenter::UpdataDiskInfo(LPCSTR udid)
{
	SASSERT(udid);

	if (udid)
	{
		auto ite = m_listDev.find(udid);
		if (ite == m_listDev.end())
			return false;
		const iOSDevInfo & devInfo = ite->second.iOSDevObject.GetiOSBaseInfo();
		if (devInfo.m_diskInfo.bReady)
		{
			if (devInfo.m_bIsJailreak)
			{
				_initiphonediskinfo2(ite->second.InfoWnd, devInfo.m_diskInfo);
			}
			else
			{
				_initidiskinfo2(ite->second.InfoWnd, devInfo.m_diskInfo);
			}
			return true;
		}		
	}
	return false;
}

void CDataCenter::UninstallApp(LPCSTR udid,LPCSTR appid)
{
	SASSERT(udid);
	if (udid)
	{
		auto ite = m_listDev.find(udid);
		if (ite == m_listDev.end())
			return;
		ite->second.iOSDevObject.UninstallApp(appid);
	}
}

int CDataCenter::GetAppsCount(LPCSTR udid)
{
	SASSERT(udid);
	if (udid)
	{
		auto ite = m_listDev.find(udid);
		if (ite == m_listDev.end())
			return 0;
		return ite->second.iOSDevObject.GetAppsCount();
	}
	return 0;
}
