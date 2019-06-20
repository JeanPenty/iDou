#pragma once

#include "CBaseInfoDlg.h"
#include <string.h>

class CLoadingDlg :public CBaseInfoDlg
{
public:
	CLoadingDlg(const std::string& udid) :CBaseInfoDlg(udid), SHostDialog(L"LAYOUT:XML_dlg_loading") {}
protected:

	void OnOK() {};
	void OnCancel() {};

	EVENT_MAP_BEGIN()
		EVENT_ID_COMMAND(IDOK, OnOK)
		EVENT_ID_COMMAND(IDCANCEL, OnCancel)
	EVENT_MAP_END()
public:
};