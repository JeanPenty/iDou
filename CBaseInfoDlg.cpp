#include "stdafx.h"
#include "CBaseInfoDlg.h"
#include "CDataCenter.h"

CBattryInfoDlg::CBattryInfoDlg(const std::string& udid) :CBaseInfoDlg(udid), SHostDialog(L"LAYOUT:XML_dlg_battry")
{
}


CBattryInfoDlg::~CBattryInfoDlg()
{
}

BOOL CBattryInfoDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	GasGauge gasGauge;
	if (CDataCenter::getSingleton().GetGasGauge(m_udid.c_str(), gasGauge))
	{
		FindChildByID(R.id.lable_CycleCount)->SetWindowText(SStringT().Format(L"%d´Î", gasGauge.CycleCount));
		FindChildByID(R.id.lable_ActuallyCapacity)->SetWindowText(SStringT().Format(L"%dmAH", gasGauge.FullChargeCapacity));
		FindChildByID(R.id.lable_DesignCapacity)->SetWindowText(SStringT().Format(L"%dmAH", gasGauge.DesignCapacity));
	}
	return TRUE;
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
