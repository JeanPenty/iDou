#pragma once
#include <core\SHostWnd.h>
#include <string>

#include "iOsDeviceLib/CiOSDevice.h"

class CBaseInfoDlg :public virtual SHostDialog
{
protected:
	std::string m_udid;
	SOUI::SStringW m_sudid;
	CBaseInfoDlg(const std::string& udid) :m_udid(udid), SHostDialog(NULL) {}
public:
	virtual bool IsEm(LPCSTR udid) { return m_udid == udid; };
};


class CBattryInfoDlg :
	public CBaseInfoDlg,
	public TAutoEventMapReg<CBattryInfoDlg>
{

public:
	CBattryInfoDlg(const std::string& udid);
	~CBattryInfoDlg();

protected:
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnUpdataInfo(EventArgs* pEArg);
	void OnUpdataBattaryInfo(EventArgs* pEArg);
	void OnDestroy();
	EVENT_MAP_BEGIN()
		EVENT_ID_HANDLER(0, EventUpdataInfo::EventID, OnUpdataInfo)
		EVENT_ID_HANDLER(0, EventUpdataBattreyInfo::EventID, OnUpdataBattaryInfo)
	EVENT_MAP_END()

	BEGIN_MSG_MAP_EX(CBattryInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()
};

class CDevInfoDlg :
	public CBaseInfoDlg
{

public:
	CDevInfoDlg(const std::string& udid);
	~CDevInfoDlg();

protected:
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	BEGIN_MSG_MAP_EX(CBattryInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()
};