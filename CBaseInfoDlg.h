#pragma once
#include <core\SHostWnd.h>
#include <string>

class CBaseInfoDlg :public virtual SHostDialog
{
protected:
	std::string m_udid;
	CBaseInfoDlg(const std::string& udid):m_udid(udid),SHostDialog(NULL)	{}
public:	
	virtual bool IsEm(LPCSTR udid) { return m_udid == udid; };
};


class CBattryInfoDlg :
	public CBaseInfoDlg
{
	
public:
	CBattryInfoDlg(const std::string& udid);
	~CBattryInfoDlg();

protected:
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	BEGIN_MSG_MAP_EX(CBattryInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
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