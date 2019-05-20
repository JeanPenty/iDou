#include "stdafx.h"
#include "CMyDeviceHandler.h"
#include "..\CDataCenter.h"
#include "..\CBaseInfoDlg.h"
#include "..\iOsDeviceLib/iOSUtils.h"
#include <..\controls.extend\dlghelp\souidlgs.h>

CMyDeviceHandler::CMyDeviceHandler()
{
}

CMyDeviceHandler::~CMyDeviceHandler()
{
}

void CMyDeviceHandler::OnInit(SWindow* pRoot)
{

	m_pPageRoot = pRoot->FindChildByID(R.id.tab_dev_info);
	SASSERT(m_pPageRoot);

	STreeView* pTreeView = m_pPageRoot->FindChildByID2<STreeView>(R.id.tv_ios);
	if (pTreeView)
	{
		m_pTreeViewAdapter.Attach(new CiOSDeviceTreeViewAdapter());
		pTreeView->SetAdapter(m_pTreeViewAdapter);
	}
}

void CMyDeviceHandler::AddDev(const idevice_event_t* event, bool bCan)
{
	m_pTreeViewAdapter->AddDev(event->udid, bCan);
	if (m_pTreeViewAdapter->GetItemCount() > 0)
	{
		((STabCtrl*)m_pPageRoot)->SetCurSel(0);
	}
	STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
	SASSERT(pTab);
	int idx = pTab->InsertItem();
	pTab->SetItemTitle(idx, S_CA2W(event->udid));

	SWindow* pTabPage = pTab->GetPage(idx);
	SMCListViewEx* appslist = pTabPage->FindChildByID2<SMCListViewEx>(R.id.lv_appsList);
	if (appslist)
	{
		CAppsListAdapter* iosAppsAdapter = new CAppsListAdapter(event->udid, m_pPageRoot->FindChildByID2<SToggle2>(R.id.app_check_all));
		appslist->SetAdapter(iosAppsAdapter);
		iosAppsAdapter->Release();
	}
	CDataCenter::getSingleton().BindInfoWindow(event->udid, pTabPage);
	if (bCan)
	{
		CDataCenter::getSingleton().UpdataBaseInfo(event->udid);
		CDataCenter::getSingleton().BeginUpdataInfoASync(event->udid);
		if (m_pTreeViewAdapter->GetItemCount() == 1)
		{
			STreeView* pTreeView = m_pPageRoot->FindChildByID2<STreeView>(R.id.tv_ios);
			if (pTreeView)
			{
				pTreeView->SetSel(m_pTreeViewAdapter->GetFirstTreeItem(), TRUE);
			}
		}
	}
}

void CMyDeviceHandler::RemoveDev(const idevice_event_t* event)
{
	m_pTreeViewAdapter->RemoveDev(event->udid);
	if (m_pTreeViewAdapter->GetItemCount() == 0)
	{
		((STabCtrl*)m_pPageRoot)->SetCurSel(1);
	}
	STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
	SASSERT(pTab);
	pTab->RemoveItem(pTab->GetPageIndex(S_CA2T(event->udid), TRUE), 0);

}

void CMyDeviceHandler::PairDev(const idevice_event_t* event, bool bCan)
{
	m_pTreeViewAdapter->SetDevCan(event->udid, bCan);
	if (bCan)
	{
		CDataCenter::getSingleton().UpdataBaseInfo(event->udid);
		CDataCenter::getSingleton().BeginUpdataInfoASync(event->udid);
		if (m_pTreeViewAdapter->GetItemCount() == 1)
		{
			STreeView* pTreeView = m_pPageRoot->FindChildByID2<STreeView>(R.id.tv_ios);
			if (pTreeView)
			{
				pTreeView->SetSel(m_pTreeViewAdapter->GetFirstTreeItem(), TRUE);
			}
		}
	}
}

void CMyDeviceHandler::OnReboot(EventArgs* pEArg)
{
	SWindow* pWnd = sobj_cast<SWindow>(pEArg->sender);
	if (pWnd)
	{
		CDataCenter::getSingleton().Reboot((SWindow*)pWnd->GetUserData());
	}
}

void CMyDeviceHandler::OnSleep(EventArgs* pEArg)
{
	SWindow* pWnd = sobj_cast<SWindow>(pEArg->sender);
	if (pWnd)
	{
		CDataCenter::getSingleton().Sleep((SWindow*)pWnd->GetUserData());
	}
}

void CMyDeviceHandler::OnShutdown(EventArgs* pEArg)
{
	SWindow* pWnd = sobj_cast<SWindow>(pEArg->sender);
	if (pWnd)
	{
		CDataCenter::getSingleton().ShutDown((SWindow*)pWnd->GetUserData());
	}
}

void CMyDeviceHandler::OnScreenShot(EventArgs* pEArg)
{
	EventScreenShot* pSS = sobj_cast<EventScreenShot>(pEArg);
	if (pSS && pSS->code == 0)
	{
		IBitmap* bitmap = SResLoadFromMemory::LoadImageW(pSS->imgbuf, pSS->bufsize);
		free(pSS->imgbuf);
		if (bitmap)
		{
			m_pPageRoot->FindChildByID2<SImageWnd>(R.id.img_srceenshot)->SetImage(bitmap);
			bitmap->Release();
		}
	}
}

void CMyDeviceHandler::OnUpdataInfo(EventArgs* pEArg)
{
	EventUpdataInfo* pUpdataInfo = sobj_cast<EventUpdataInfo>(pEArg);
	if (pUpdataInfo)
	{
		STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
		SASSERT(pTab);
		SWindow* pWnd = pTab->GetPage(pUpdataInfo->udid);
		pWnd->FindChildByID2<SImageWnd>(R.id.batterybk)->SetIcon(pUpdataInfo->BatteryCurrentCapacity < 20 ? 1 : 0);
		pWnd->FindChildByID(R.id.lable_BatteryCurrentCapacity)->SetWindowText(SStringW().Format(L"%d%%", pUpdataInfo->BatteryCurrentCapacity));
		pWnd->FindChildByID(R.id.lable_BatteryIsCapacity)->SetWindowText(pUpdataInfo->BatteryIsCharging ? GETSTRING(L"@string/charging") : GETSTRING(L"@string/nocharging"));
	}
}

void CMyDeviceHandler::OnEditDevName()
{
	SWindow* pTEXT = m_pPageRoot->FindChildByID(R.id.txt_devname);
	SASSERT(pTEXT);
	SWindow* pET = m_pPageRoot->FindChildByID(R.id.et_devname);
	SASSERT(pET);
	if (pET->IsVisible())
	{
		std::string udid = CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)pET->GetUserData());
		CiOSDevice* dev = CDataCenter::getSingleton().GetDevByUDID(udid.c_str());
		SWindow* pTEXT = pET->GetParent()->FindChildByID(R.id.txt_devname);
		if (dev && dev->SetDevName(pET->GetWindowText()))
		{
			m_pTreeViewAdapter->UpDataDev(udid.c_str());
			SASSERT(pTEXT);
			pTEXT->SetWindowText(pET->GetWindowText());
		}
		pET->SetVisible(FALSE, TRUE);
		pTEXT->SetVisible(TRUE, TRUE);
	}
	else
	{
		pET->SetWindowText(pTEXT->GetWindowText());
		pET->SetVisible(TRUE, TRUE);
		pET->SetFocus();
		pTEXT->SetVisible(FALSE, TRUE);
	}
}

void CMyDeviceHandler::OnKillFoucusByDevName(EventArgs * pEArg)
{
	SWindow* pET = sobj_cast<SWindow>(pEArg->sender);
	if (pET)
	{
		std::string udid = CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)pET->GetUserData());
		CiOSDevice* dev = CDataCenter::getSingleton().GetDevByUDID(udid.c_str());
		SWindow* pTEXT = pET->GetParent()->FindChildByID(R.id.txt_devname);
		if (dev && dev->SetDevName(pET->GetWindowText()))
		{
			m_pTreeViewAdapter->UpDataDev(udid.c_str());
			SASSERT(pTEXT);
			pTEXT->SetWindowText(pET->GetWindowText());
		}
		pET->SetVisible(FALSE, TRUE);
		pTEXT->SetVisible(TRUE, TRUE);
	}
}

void CMyDeviceHandler::OnKeyDownByDevName(EventArgs * pEArg)
{
	EventKeyDown* evt = sobj_cast<EventKeyDown>(pEArg);
	SWindow* pET = sobj_cast<SWindow>(pEArg->sender);
	switch (evt->nChar)
	{
	case VK_RETURN:
	{
		std::string udid = CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)pET->GetUserData());
		CiOSDevice* dev = CDataCenter::getSingleton().GetDevByUDID(udid.c_str());
		SWindow* pTEXT = pET->GetParent()->FindChildByID(R.id.txt_devname);
		if (dev && dev->SetDevName(pET->GetWindowText()))
		{
			m_pTreeViewAdapter->UpDataDev(udid.c_str());
			SASSERT(pTEXT);
			pTEXT->SetWindowText(pET->GetWindowText());
		}
		pET->SetVisible(FALSE, TRUE);
		pTEXT->SetVisible(TRUE, TRUE);
		evt->bCancel = true;

	}break;
	case VK_ESCAPE:
	{
		SWindow* pTEXT = pET->GetParent()->FindChildByID(R.id.txt_devname);
		pET->SetVisible(FALSE, TRUE);
		pTEXT->SetVisible(TRUE, TRUE);
		evt->bCancel = true;
	}break;
	default:
		break;
	}
}

void CMyDeviceHandler::OnUpdataDiskInfo(EventArgs * pEArg)
{
	EventUpdataDiskInfo* evt = sobj_cast<EventUpdataDiskInfo>(pEArg);
	if (evt)
	{
		CDataCenter::getSingleton().UpdataDiskInfo(evt->udid.c_str());
	}
}

void CMyDeviceHandler::OnUpdataAppsInfo(EventArgs * pEArg)
{
	EventUpdataAppsInfo* evt = sobj_cast<EventUpdataAppsInfo>(pEArg);
	if (evt)
	{
		STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
		SASSERT(pTab);
		SWindow* pWnd = pTab->GetPage(evt->udid);
		SASSERT(pWnd);
		SMCListViewEx* appslist = pWnd->FindChildByID2<SMCListViewEx>(R.id.lv_appsList);
		if (appslist)
		{
			CAutoRefPtr<CAppsListAdapter> iosAppsAdapter = (CAppsListAdapter*)appslist->GetAdapter();
			iosAppsAdapter->notifyDataSetChanged();
		}
		m_pTreeViewAdapter->UpDataDevAppInfo(S_CW2A(evt->udid));
	}
}

void CMyDeviceHandler::OnUnistallApp(EventArgs* pEArg)
{
	EventUninstallApp* evt = sobj_cast<EventUninstallApp>(pEArg);
	if (evt&&evt->bSucessed)
	{
		STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
		SASSERT(pTab);
		SWindow* pWnd = pTab->GetPage(evt->udid);
		SASSERT(pWnd);
		SMCListViewEx* appslist = pWnd->FindChildByID2<SMCListViewEx>(R.id.lv_appsList);
		if (appslist)
		{
			CAutoRefPtr<CAppsListAdapter> iosAppsAdapter = (CAppsListAdapter*)appslist->GetAdapter();
			iosAppsAdapter->notifyDataSetChanged();
		}
		m_pTreeViewAdapter->UpDataDevAppInfo(S_CW2A(evt->udid));
	}
}

void CMyDeviceHandler::OnTVSelChanged(EventArgs * pEArg)
{
	EventTVSelChanged* pE2 = sobj_cast<EventTVSelChanged>(pEArg);
	SASSERT(pE2);

	const CiOSDeviceTreeViewAdapter::ItemInfo& data = m_pTreeViewAdapter->GetData(pE2->hNewSel);

	ChildMenuItemClick(data.data.guid.c_str(), data.data.nCmd);
	m_pTreeViewAdapter->notifyBranchInvalidated(pE2->hNewSel);
}

void CMyDeviceHandler::OnInstallApp(EventArgs* pEArg)
{
	SWindow* pET = sobj_cast<SWindow>(pEArg->sender);
	if (pET)
	{
		std::string udid = CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)pET->GetUserData());
		CiOSDevice* dev = CDataCenter::getSingleton().GetDevByUDID(udid.c_str());
		if (dev)
		{
			CFileDialog appfiledlg(TRUE, NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"iOSÓ¦ÓÃ³ÌÐò\0*.ipa\0\0");//;*.ipcc
			if (IDOK == appfiledlg.DoModal(m_pPageRoot->GetContainer()->GetHostHwnd()))
			{				
				dev->IntallApp(appfiledlg.m_szFileName);
			}
		}
	}
}

void CMyDeviceHandler::OnCheckWarrantyexpirationDate(EventArgs * pEArg)
{
	SWindow* pET = sobj_cast<SWindow>(pEArg->sender);
	if (pET)
	{
		std::string udid = CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)pET->GetUserData());
		CiOSDevice* dev = CDataCenter::getSingleton().GetDevByUDID(udid.c_str());
		if (dev)
		{
			const TCHAR* strUrl = _T("https://checkcoverage.apple.com/cn/zh/?sn=%s");

			SStringT url;
			url.Format(strUrl, dev->GetiOSBaseInfo().m_strDevSerialNumber);
			ShellExecute(NULL, L"open",url, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

void CMyDeviceHandler::ChildMenuItemClick(LPCSTR udid, int nGID)
{
	STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
	SASSERT(pTab);
	if (pTab)
	{
		SStringT strTitle = S_CA2W(udid, CP_UTF8);
		pTab->SetCurSel(strTitle);
		SWindow* pcmdwnd = pTab->GetPage(strTitle);
		SASSERT(pcmdwnd);
		if (pcmdwnd)
		{
			STabCtrl* pTabCmd = pcmdwnd->FindChildByID2<STabCtrl>(R.id.tab_dev_cmd);
			SASSERT(pTabCmd);
			pTabCmd->SetCurSel(nGID - 1);
		}
	}
}
