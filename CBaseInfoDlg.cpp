#include "stdafx.h"
#include "CBaseInfoDlg.h"
#include "CDataCenter.h"

CBattryInfoDlg::CBattryInfoDlg(const std::string& udid) :CBaseInfoDlg(udid), SHostDialog(L"LAYOUT:XML_dlg_battry")
{
	m_sudid = SOUI::S_CA2W(udid.c_str());
}


CBattryInfoDlg::~CBattryInfoDlg()
{
}

BOOL CBattryInfoDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	BatteryBaseInfo batteryBaseInfo;
	if (CDataCenter::getSingleton().GetGasGauge(m_udid.c_str(), batteryBaseInfo))
	{
		FindChildByID(R.id.lable_CycleCount)->SetWindowText(SStringT().Format(L"%d´Î", batteryBaseInfo.CycleCount));
		FindChildByID(R.id.lable_ActuallyCapacity)->SetWindowText(SStringT().Format(L"%dmAH", batteryBaseInfo.NominalChargeCapacity));
		FindChildByID(R.id.lable_DesignCapacity)->SetWindowText(SStringT().Format(L"%dmAH", batteryBaseInfo.DesignCapacity));
		FindChildByID(R.id.lable_BatterySerialNumber)->SetWindowText(batteryBaseInfo.BatterySerialNumber);
		FindChildByID(R.id.lable_BatteryBootVoltage)->SetWindowText(SStringT().Format(L"%0.2fV", (double)batteryBaseInfo.BootVoltage/1000));
		FindChildByID(R.id.lable_BatteryOrigin)->SetWindowText(batteryBaseInfo.Origin);
		FindChildByID(R.id.lable_BatteryDate)->SetWindowText(batteryBaseInfo.ManufactureDate);
		int iCycleLife = (int)(((float)batteryBaseInfo.NominalChargeCapacity * 100 / batteryBaseInfo.DesignCapacity) + 0.5);
		if (iCycleLife > 100)iCycleLife = 100;
		FindChildByID(R.id.lable_CycleLife)->SetWindowText(SStringT().Format(L"%d%%", iCycleLife));
	}	

	CiOSDevice* dev = CDataCenter::getSingleton().GetDevByUDID(m_udid.c_str());
	if (dev)
		dev->StartUpdataBatteryInfo();
	return TRUE;
}
void CBattryInfoDlg::OnUpdataInfo(EventArgs* pEArg)
{
	EventUpdataInfo* pUpdataInfo = sobj_cast<EventUpdataInfo>(pEArg);
	if (pUpdataInfo && pUpdataInfo->udid == m_sudid&& m_lastBatteryCurrentCapacity!= pUpdataInfo->BatteryCurrentCapacity)
	{
		m_lastBatteryCurrentCapacity = pUpdataInfo->BatteryCurrentCapacity;
		FindChildByID(R.id.img_batterycurrentcap)->GetLayoutParam()->SetAttribute(L"weight", SStringT().Format(L"%d", m_lastBatteryCurrentCapacity), FALSE);
		FindChildByID(R.id.img_batteryemtycap)->GetLayoutParam()->SetAttribute(L"weight", SStringT().Format(L"%d", 100- m_lastBatteryCurrentCapacity), FALSE);
		FindChildByID(R.id.batterycap)->RequestRelayout();
	}
}

void CBattryInfoDlg::OnUpdataBattaryInfo(EventArgs* pEArg)
{
	EventUpdataBattreyInfo* pUpdataInfo = sobj_cast<EventUpdataBattreyInfo>(pEArg);
	if (pUpdataInfo && pUpdataInfo->udid == m_sudid)
	{
		FindChildByID(R.id.lable_BatteryTemperature)->SetWindowText(SStringT().Format(L"%0.2f¡æ", (double)pUpdataInfo->Temperature / 100));
		FindChildByID(R.id.lable_BatteryVoltage)->SetWindowText(SStringT().Format(L"%0.2fV", (double)pUpdataInfo->Voltage / 1000));
		FindChildByID(R.id.lable_BatteryCurrentCapacity)->SetWindowText(SStringT().Format(L"%dmA", pUpdataInfo->Current));		
	}
}

void CBattryInfoDlg::OnDestroy()
{
	CiOSDevice* dev = CDataCenter::getSingleton().GetDevByUDID(m_udid.c_str());
	if (dev)
		dev->StopUpdataBatteryInfo();
	__super::OnDestroy();
}

CDevInfoDlg::CDevInfoDlg(const std::string& udid) :CBaseInfoDlg(udid), SHostDialog(L"LAYOUT:XML_dlg_all") 
{
}

CDevInfoDlg::~CDevInfoDlg()
{
}

BOOL CDevInfoDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{	
	return 0;
}

