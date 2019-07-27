#include "stdafx.h"
#include "CMyDeviceHandler.h"
#include "..\CDataCenter.h"
#include "..\iOsDeviceLib/iOSUtils.h"
#include <..\controls.extend\dlghelp\souidlgs.h>
#include "LV_iOSFilesAdapter.h"
#include "TV_iOSFilesNavAdapter.h"
#include "..\Cafctask.h"
#include "CBaseInfoDlg.h"
#include "CLoadingDlg.h"
#include "LV_contactsAdapter.h"


using namespace SOUI;

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
		CAppsListAdapter* iosAppsAdapter = new CAppsListAdapter(event->udid, m_pPageRoot->FindChildByID(R.id.apps_ctrl_wnd));
		appslist->SetAdapter(iosAppsAdapter);
		iosAppsAdapter->Release();
	}

	STreeView* PTvFilesMgr = pTabPage->FindChildByID2<STreeView>(R.id.tv_files_mgr);
	if (PTvFilesMgr)
	{
		CiOSFilesTreeViewAdapter* iosFilesMgr = new CiOSFilesTreeViewAdapter(event->udid);
		PTvFilesMgr->SetAdapter(iosFilesMgr);
		iosFilesMgr->Release();
	}

	SMCListViewEx* filesslist = pTabPage->FindChildByID2<SMCListViewEx>(R.id.lv_filesList);
	if (filesslist)
	{
		CFilesListAdapter* iosFilessAdapter = new CFilesListAdapter(event->udid);
		filesslist->SetAdapter(iosFilessAdapter);
		iosFilessAdapter->Release();
	}

	SMCListViewEx* plv_contactsList = pTabPage->FindChildByID2<SMCListViewEx>(R.id.lv_contactsList);
	if (plv_contactsList)
	{
		CContactsListAdapter* contactsAdapter = new CContactsListAdapter(event->udid);
		plv_contactsList->SetAdapter(contactsAdapter);
		contactsAdapter->Release();
	}

	CDataCenter::getSingleton().BindInfoWindow(event->udid, pTabPage);
	if (bCan)
	{
		m_filerecords[event->udid];
		Cafctask::getSingleton().AddDev(event->udid);
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
	Cafctask::getSingleton().RemoveDev(event->udid);

	m_filerecords.erase(event->udid);
	 
	CDataCenter::getSingleton().RemoveDevGUID(event->udid);
	pTab->RemoveItem(pTab->GetPageIndex(S_CA2T(event->udid), TRUE), 0);

}

void CMyDeviceHandler::PairDev(const idevice_event_t* event, bool bCan)
{
	m_pTreeViewAdapter->SetDevCan(event->udid, bCan);
	if (bCan)
	{
		m_filerecords[event->udid];
		Cafctask::getSingleton().AddDev(event->udid);
		CDataCenter::getSingleton().UpdataBaseInfo(event->udid);
		CDataCenter::getSingleton().BeginUpdataInfoASync(event->udid);
		m_pTreeViewAdapter->ExpandItem(event->udid);
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

void CMyDeviceHandler::TryEndInfoDlg(LPCSTR udid)
{
	if (m_pHostDlg && m_pHostDlg->IsEm(udid))
	{
		m_pHostDlg->EndDialog(IDCANCEL);
		m_pHostDlg = NULL;
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
			SWindow* pWnd = GetDevCmdWindow(pSS->udid, 0);
			pWnd->FindChildByID2<SImageWnd>(R.id.img_srceenshot)->SetImage(bitmap);
			bitmap->Release();
		}
	}
}

void CMyDeviceHandler::OnUpdataInfo(EventArgs* pEArg)
{
	EventUpdataInfo* pUpdataInfo = sobj_cast<EventUpdataInfo>(pEArg);
	if (pUpdataInfo)
	{
		SWindow* pWnd = GetDevCmdWindow(pUpdataInfo->udid, 0);
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

void CMyDeviceHandler::OnKillFoucusByDevName(EventArgs* pEArg)
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

void CMyDeviceHandler::OnKeyDownByDevName(EventArgs* pEArg)
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

void CMyDeviceHandler::OnUpdataDiskInfo(EventArgs* pEArg)
{
	EventUpdataDiskInfo* evt = sobj_cast<EventUpdataDiskInfo>(pEArg);
	if (evt)
	{
		CDataCenter::getSingleton().UpdataDiskInfo(evt->udid.c_str());
	}
}

void CMyDeviceHandler::OnUpdataAppsInfo(EventArgs* pEArg)
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
			iosAppsAdapter->InitApps(std::move(evt->apps));
			m_pTreeViewAdapter->UpDataDevAppInfo(S_CW2A(evt->udid), iosAppsAdapter->getCount());
		}
	}
}

void CMyDeviceHandler::OnUnistallApp(EventArgs* pEArg)
{
	EventUninstallApp* evt = sobj_cast<EventUninstallApp>(pEArg);
	if (evt && evt->bSucessed)
	{
		STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
		SASSERT(pTab);
		SWindow* pWnd = pTab->GetPage(evt->udid);
		SASSERT(pWnd);
		SMCListViewEx* appslist = pWnd->FindChildByID2<SMCListViewEx>(R.id.lv_appsList);
		if (appslist)
		{
			CAutoRefPtr<CAppsListAdapter> iosAppsAdapter = (CAppsListAdapter*)appslist->GetAdapter();
			iosAppsAdapter->RemoveApp(evt->appid.c_str());
			m_pTreeViewAdapter->UpDataDevAppInfo(S_CW2A(evt->udid), iosAppsAdapter->getCount());
		}
		
	}
}

void CMyDeviceHandler::OnTVSelChanged(EventArgs* pEArg)
{
	EventTVSelChanged* pE2 = sobj_cast<EventTVSelChanged>(pEArg);
	SASSERT(pE2);

	const CiOSDeviceTreeViewAdapter::ItemInfo& data = m_pTreeViewAdapter->GetData(pE2->hNewSel);

	ChildMenuItemClick(data.data.udid.c_str(), data.data.nCmd);
	m_pTreeViewAdapter->notifyBranchInvalidated(pE2->hNewSel);
}

void CMyDeviceHandler::OnTVFilesMgrSelChanged(EventArgs* pEArg)
{
	EventTVSelChanged* pE2 = sobj_cast<EventTVSelChanged>(pEArg);
	SASSERT(pE2);

	STreeView* pTv = sobj_cast<STreeView>(pEArg->sender);
	SASSERT(pTv);
	CiOSFilesTreeViewAdapter* iosFilessAdapter = (CiOSFilesTreeViewAdapter*)pTv->GetAdapter();
	int cmd = iosFilessAdapter->GetCmd(pE2->hNewSel);

	SEdit* pETPATH = pTv->GetParent()->GetParent()->FindChildByID2<SEdit>(R.id.et_path);
	SASSERT(pETPATH);
	//重置历史记录列表
	ResetRecord(iosFilessAdapter->m_udid);
	pTv->GetParent()->GetParent()->FindChildByID(R.id.btn_goback)->EnableWindow(FALSE,TRUE);
	pTv->GetParent()->GetParent()->FindChildByID(R.id.btn_goforward)->EnableWindow(FALSE, TRUE);

	UINT taskid = -1;

	const char* path[] = { "/","/","/MyDocuments","/general_storage" ,"/Recordings" ,"/var/mobile/Containers/Bundle/Application" };
	const wchar_t* wpath[] = { L"/",L"/",L"/MyDocuments",L"/general_storage" ,L"/Recordings" ,L"/var/mobile/Containers/Bundle/Application" };
	switch (cmd)
	{
		//用户系统
		case 1:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), path[0], false);
			PushFileRecord(iosFilessAdapter->m_udid, false, path[0]);
			pETPATH->SetWindowText(L"/");
			break;
			//越狱系统
		case 2:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/", true);
			PushFileRecord(iosFilessAdapter->m_udid, true, path[1]);
			pETPATH->SetWindowText(L"/");
			break;
			//我的文档
		case 11:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/MyDocuments", false);
			PushFileRecord(iosFilessAdapter->m_udid, false, path[2]);
			pETPATH->SetWindowText(L"/MyDocuments");
			break;
			//手机U盘
		case 12:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/general_storage", false);
			PushFileRecord(iosFilessAdapter->m_udid, false, path[3]);
			pETPATH->SetWindowText(L"/general_storage");
			break;
			//语音备忘录
		case 13:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/Recordings", false);
			PushFileRecord(iosFilessAdapter->m_udid, false, path[4]);
			pETPATH->SetWindowText(L"/Recordings");
			break;
			//应用程序
		case 14:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/var/mobile/Containers/Bundle/Application", true);
			PushFileRecord(iosFilessAdapter->m_udid, true, path[5]);
			pETPATH->SetWindowText(L"/var/mobile/Containers/Bundle/Application");
			break;
	}
	/*
	switch (cmd)
	{
		//用户系统
		case 1:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/", false);
			pETPATH->SetWindowText(L"/");
			break;
			//越狱系统
		case 2:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/", true);
			pETPATH->SetWindowText(L"/");
			break;
			//我的文档
		case 11:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/MyDocuments", false);
			pETPATH->SetWindowText(L"/MyDocuments");
			break;
			//手机U盘
		case 12:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/general_storage", false);
			pETPATH->SetWindowText(L"/general_storage");
			break;
			//语音备忘录
		case 13:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/Recordings", false);
			pETPATH->SetWindowText(L"/Recordings");
			break;
			//应用程序
		case 14:
			taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/var/mobile/Containers/Bundle/Application", true);
			pETPATH->SetWindowText(L"/var/mobile/Containers/Bundle/Application");
			break;
	}*/
	if (taskid != -1)
	{
		SMCListViewEx* pFilesList = pTv->GetParent()->FindChildByID2<SMCListViewEx>(R.id.lv_filesList);
		SASSERT(pFilesList);
		CFilesListAdapter* iosFilesListAdapter = (CFilesListAdapter*)pFilesList->GetAdapter();
		iosFilesListAdapter->SetTaskID(taskid, (cmd == 2) || (cmd == 14));
	}
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
			CFileDialog appfiledlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"iOS应用程序\0*.ipa\0\0");//;*.ipcc
			if (IDOK == appfiledlg.DoModal(m_pPageRoot->GetContainer()->GetHostHwnd()))
			{
				dev->IntallApp(appfiledlg.m_szFileName);
			}
		}
	}
}

void CMyDeviceHandler::OnCheckWarrantyexpirationDate(EventArgs* pEArg)
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
			ShellExecute(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

void CMyDeviceHandler::OnInitAfc(EventArgs* pEArg)
{
	EventAFCInit* e = sobj_cast<EventAFCInit>(pEArg);
	SASSERT(e);
	SWindow* pWnd = GetDevCmdWindow(S_CA2W(e->udid.c_str()), 2);
	SASSERT(pWnd);
	STreeView* pTvFilesNav = pWnd->FindChildByID2<STreeView>(R.id.tv_files_mgr);
	SASSERT(pTvFilesNav);
	CiOSFilesTreeViewAdapter* pAd = (CiOSFilesTreeViewAdapter*)pTvFilesNav->GetAdapter();
	pAd->InitAfc(e->afc_sucssed, e->afc2_sucssed);
}

void CMyDeviceHandler::OnFilesNavTvEventOfPanel(EventArgs* pEvt)
{
	EventOfPanel* e2 = sobj_cast<EventOfPanel>(pEvt);
	SASSERT(e2);
	if ((e2->pOrgEvt->GetID() == EventItemPanelDbclick::EventID) || (e2->pOrgEvt->GetID() == EventItemPanelClick::EventID))
	{
		STreeView* pTv = sobj_cast<STreeView>(e2->sender);
		SASSERT(pTv);
		SItemPanel* pSender = sobj_cast<SItemPanel>(e2->pOrgEvt->sender);
		SASSERT(pSender);

		//if (pTv->GetSel() == pSender->GetItemIndex())
		{
			CiOSFilesTreeViewAdapter* iosFilessAdapter = (CiOSFilesTreeViewAdapter*)pTv->GetAdapter();
			int cmd = iosFilessAdapter->GetCmd(pSender->GetItemIndex());

			SEdit* pETPATH = pTv->GetParent()->GetParent()->FindChildByID2<SEdit>(R.id.et_path);
			SASSERT(pETPATH);
			UINT taskid = -1;
			//重置历史记录
			ResetRecord(iosFilessAdapter->m_udid);
			pTv->GetParent()->GetParent()->FindChildByID(R.id.btn_goback)->EnableWindow(FALSE, TRUE);
			pTv->GetParent()->GetParent()->FindChildByID(R.id.btn_goforward)->EnableWindow(FALSE, TRUE);
			const char* path[] = {"/","/","/MyDocuments","/general_storage" ,"/Recordings" ,"/var/mobile/Containers/Bundle/Application" };
			const wchar_t* wpath[] = { L"/",L"/",L"/MyDocuments",L"/general_storage" ,L"/Recordings" ,L"/var/mobile/Containers/Bundle/Application" };
			switch (cmd)
			{
				//用户系统
				case 1:
					taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), path[0], false);
					PushFileRecord(iosFilessAdapter->m_udid,false, path[0]);
					pETPATH->SetWindowText(L"/");
					break;
					//越狱系统
				case 2:
					taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/", true);
					PushFileRecord(iosFilessAdapter->m_udid, true, path[1]);
					pETPATH->SetWindowText(L"/");
					break;
					//我的文档
				case 11:
					taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/MyDocuments", false);
					PushFileRecord(iosFilessAdapter->m_udid, false, path[2]);
					pETPATH->SetWindowText(L"/MyDocuments");
					break;
					//手机U盘
				case 12:
					taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/general_storage", false);
					PushFileRecord(iosFilessAdapter->m_udid, false, path[3]);
					pETPATH->SetWindowText(L"/general_storage");
					break;
					//语音备忘录
				case 13:
					taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/Recordings", false);
					PushFileRecord(iosFilessAdapter->m_udid, false, path[4]);
					pETPATH->SetWindowText(L"/Recordings");
					break;
					//应用程序
				case 14:
					taskid = Cafctask::getSingleton().Updata(iosFilessAdapter->m_udid.c_str(), "/var/mobile/Containers/Bundle/Application", true);
					PushFileRecord(iosFilessAdapter->m_udid, true, path[5]);
					pETPATH->SetWindowText(L"/var/mobile/Containers/Bundle/Application");
					break;
			}
			if (taskid != -1)
			{
				SMCListViewEx* pFilesList = pTv->GetParent()->FindChildByID2<SMCListViewEx>(R.id.lv_filesList);
				SASSERT(pFilesList);
				CFilesListAdapter* iosFilesListAdapter = (CFilesListAdapter*)pFilesList->GetAdapter();
				iosFilesListAdapter->SetTaskID(taskid, (cmd == 2) || (cmd == 14));
			}
		}
	}

}

void CMyDeviceHandler::OnFileslvEventOfPanel(EventArgs* pEvt)
{
	EventOfPanel* e2 = sobj_cast<EventOfPanel>(pEvt);
	SASSERT(e2);
	if (e2->pOrgEvt->GetID() == EventItemPanelDbclick::EventID)
	{
		SMCListViewEx* pFilesList = sobj_cast<SMCListViewEx>(e2->sender);
		SASSERT(pFilesList);
		CFilesListAdapter* pFilesListAdapter = (CFilesListAdapter*)pFilesList->GetAdapter();
		EventItemPanelDbclick* e3 = sobj_cast<EventItemPanelDbclick>(e2->pOrgEvt);
		SItemPanel* pSender = sobj_cast<SItemPanel>(e3->sender);
		SASSERT(pSender);
		int iItem = pSender->GetItemIndex();
		bool isdir, isfile;
		std::string path = pFilesListAdapter->GetItemFullPath(iItem, isdir, isfile);
		if (!path.empty())
		{
			if (isdir)
			{
				PushFileRecord(pFilesListAdapter->m_udid,pFilesListAdapter->CurIsJailreak(), path);
				pFilesList->GetParent()->GetParent()->GetParent()->FindChildByID(R.id.btn_goback)->EnableWindow(TRUE, TRUE);
				pFilesList->GetParent()->GetParent()->GetParent()->FindChildByID(R.id.btn_goforward)->EnableWindow(FALSE, TRUE);

				pFilesListAdapter->UpdataTaskID(
					Cafctask::getSingleton().Updata(pFilesListAdapter->m_udid.c_str(), path.c_str(), pFilesListAdapter->CurIsJailreak())
				);
				SWindow* petpath = pFilesList->GetParent()->GetParent()->GetParent()->FindChildByID(R.id.et_path);
				SASSERT(petpath);
				petpath->SetWindowText(S_CA2W(path.c_str(), CP_UTF8));
			}
			else if (isfile)
			{
				Cafctask::getSingleton().OpenFile(pFilesListAdapter->m_udid.c_str(), path.c_str(), pFilesListAdapter->CurIsJailreak());
				ShowInfoDlg<CLoadingDlg>(pFilesListAdapter->m_udid);
			}
		}
	}
}

void CMyDeviceHandler::OnUpdataFileList(EventArgs* e)
{
	EventUpdataFile* updatafiles = sobj_cast<EventUpdataFile>(e);

	SWindow* pPage = GetDevCmdWindow(S_CA2W(updatafiles->udid.c_str()), 2);
	SASSERT(pPage);
	SMCListViewEx* pFilesList = pPage->FindChildByID2<SMCListViewEx>(R.id.lv_filesList);
	SASSERT(pFilesList);

	CFilesListAdapter* filesListAdapter = (CFilesListAdapter*)pFilesList->GetAdapter();

	if (filesListAdapter->IsCurTask(updatafiles->taskId))
	{		
		filesListAdapter->Updata(updatafiles->filelist);
	}
}

void CMyDeviceHandler::OnOpenFileRet(EventArgs* e)
{
	if (m_pHostDlg)
	{
		m_pHostDlg->EndDialog(IDCANCEL);
		m_pHostDlg = NULL;
	}
}

SWindow* CMyDeviceHandler::GetDevCmdWindow(LPCWSTR udid, int cmd)
{
	STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
	SASSERT(pTab);
	SWindow* devpage = pTab->GetPage(udid);
	SASSERT(devpage);
	STabCtrl* devtab = devpage->FindChildByID2<STabCtrl>(R.id.tab_dev_cmd);
	SASSERT(devtab);
	return devtab->GetPage(cmd);
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

template<class T>
void CMyDeviceHandler::ShowInfoDlg(const std::string& udid)
{
	T battryInfo(udid);
	m_pHostDlg = &battryInfo;
	m_pHostDlg->DoModal(m_pPageRoot->GetContainer()->GetHostHwnd());
	m_pHostDlg = NULL;
}

void CMyDeviceHandler::OnBatteryInfo(EventArgs* pEArg)
{
	SWindow* pWnd = sobj_cast<SWindow>(pEArg->sender);
	if (pWnd)
	{
		std::string udid = CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)pWnd->GetUserData());
		if (!udid.empty())
		{
			ShowInfoDlg<CBattryInfoDlg>(udid);
		}
	}
}

void CMyDeviceHandler::OnDevInfo(EventArgs* pEArg)
{
	SWindow* pWnd = sobj_cast<SWindow>(pEArg->sender);
	if (pWnd)
	{
		std::string udid = CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)pWnd->GetUserData());
		if (!udid.empty())
		{
			ShowInfoDlg<CDevInfoDlg>(udid);
		}
	}
}

void CMyDeviceHandler::OnUninstallapps(EventArgs* pEArg)
{
	SWindow* pWnd = sobj_cast<SWindow>(pEArg->sender);
	SASSERT(pWnd);
	CiOSDevice* iosdev = CDataCenter::getSingleton().GetDevByWindow((SWindow*)pWnd->GetParent()->GetUserData());
	
	if (iosdev)
	{
		SMCListViewEx* plist = pWnd->GetParent()->GetParent()->FindChildByID2 < SMCListViewEx >(R.id.lv_appsList);
		SASSERT(plist);
		CAppsListAdapter* pAppsListAd = (CAppsListAdapter*)plist->GetAdapter();
		std::vector<std::string>& selApps =pAppsListAd->GetAllSelApps();
		if ((!selApps.empty())&&
			(SMessageBox(m_pPageRoot->GetContainer()->GetHostHwnd(),
				SStringT().Format(L"你确信要卸载选中的%d个应用吗？",selApps.size()),L"卸载APP",MB_OKCANCEL)== IDOK))
		{
			iosdev->UninstallApp(selApps);
		}
	}
}

void CMyDeviceHandler::OnRefershapp(EventArgs* pEArg)
{
	SWindow* pWnd = sobj_cast<SWindow>(pEArg->sender);
	SASSERT(pWnd);
	CiOSDevice* iosdev = CDataCenter::getSingleton().GetDevByWindow((SWindow*)pWnd->GetParent()->GetUserData());
	if (iosdev)
	{
		iosdev->StartUpdataApps();
	}
}

void CMyDeviceHandler::OnInstallAppCb(EventArgs* pEArg)
{
	EventInstallApp* evt = sobj_cast<EventInstallApp>(pEArg);
	if (evt && evt->bSucessed)
	{
		STabCtrlTemplate* pTab = m_pPageRoot->FindChildByID2<STabCtrlTemplate>(R.id.nav_dev_cmd);
		SASSERT(pTab);
		SWindow* pWnd = pTab->GetPage(evt->udid);
		SASSERT(pWnd);
		SMCListViewEx* appslist = pWnd->FindChildByID2<SMCListViewEx>(R.id.lv_appsList);
		if (appslist)
		{
			CAutoRefPtr<CAppsListAdapter> iosAppsAdapter = (CAppsListAdapter*)appslist->GetAdapter();
			iosAppsAdapter->AddApp(evt->appinfo);
			m_pTreeViewAdapter->UpDataDevAppInfo(S_CW2A(evt->udid), iosAppsAdapter->getCount());
		}		
	}
}

void CMyDeviceHandler::OnGoBack(EventArgs* pEArg)
{
	SWindow* btn = sobj_cast<SWindow>(pEArg->sender);
	SASSERT(btn);
	SWindow* hostwnd = btn->GetParent();
	std::string udid= CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)hostwnd->GetUserData());
	SASSERT(!udid.empty());
	FileRecord record;
	if (PopFileRecord(udid, record))
	{
		SMCListViewEx* pFilesList = hostwnd->GetParent()->FindChildByID2<SMCListViewEx>(R.id.lv_filesList);
		SASSERT(pFilesList);
		CFilesListAdapter* iosFilesListAdapter = (CFilesListAdapter*)pFilesList->GetAdapter();		

		iosFilesListAdapter->UpdataTaskID(
			Cafctask::getSingleton().Updata(udid.c_str(), record.path.c_str(), record.isAfc2)
		);
		SWindow* petpath = hostwnd->FindChildByID(R.id.et_path);
		SASSERT(petpath);
		petpath->SetWindowText(S_CA2W(record.path.c_str(), CP_UTF8));
		hostwnd->FindChildByID(R.id.btn_goforward)->EnableWindow(TRUE, TRUE);
		btn->EnableWindow(m_filerecords[udid].record1.empty()?FALSE:TRUE);
	}
}

void CMyDeviceHandler::OnGoForward(EventArgs* pEArg)
{
	SWindow* btn = sobj_cast<SWindow>(pEArg->sender);
	SASSERT(btn);
	SWindow* hostwnd = btn->GetParent();
	std::string udid = CDataCenter::getSingleton().GetUDIDByWindow((SWindow*)hostwnd->GetUserData());
	SASSERT(!udid.empty());

}

void CMyDeviceHandler::OnUpdataContacts(EventArgs* pEArg)
{
	EventUpdataContacts* e = sobj_cast<EventUpdataContacts>(pEArg);
	SWindow* pContactsWindow = GetDevCmdWindow(e->udid,3);
	if (pContactsWindow)
	{
		if (e->bRet)
		{
			SMCListViewEx* plv_contactsList = pContactsWindow->FindChildByID2<SMCListViewEx>(R.id.lv_contactsList);
			if (plv_contactsList)
			{
				CContactsListAdapter* pContactsAdpter = (CContactsListAdapter*)plv_contactsList->GetAdapter();
				SASSERT(pContactsAdpter);
				pContactsAdpter->CopyForm(std::move(e->contacts));
			}
		}
	}
}

void CMyDeviceHandler::OnContactSelChanged(EventArgs* pEArg)
{
	EventLVSelChanged* e = sobj_cast<EventLVSelChanged>(pEArg);

	if (e->iNewSel == -1)
		return;

	SMCListViewEx* plv_contactsList = sobj_cast<SMCListViewEx>(pEArg->sender);
	if (plv_contactsList)
	{
		SWindow *pContactInfoWnd=plv_contactsList->GetParent()->FindChildByID(R.id.contactInfo);
		SASSERT(pContactInfoWnd);
		CContactsListAdapter* pContactsAdpter = (CContactsListAdapter*)plv_contactsList->GetAdapter();
		SASSERT(pContactsAdpter);
		ContactInfo contactInfo = pContactsAdpter->GetContactInfo(e->iNewSel);
		pContactInfoWnd->FindChildByID2<SImageWnd>(R.id.img_avatar)->SetImage(contactInfo.m_img, kHigh_FilterLevel);
		pContactInfoWnd->FindChildByID(R.id.lable_name)->SetWindowText(utils::MakeName(contactInfo.FirstName, contactInfo.LastName));
		SListBox *pPhoneNumbers= pContactInfoWnd->FindChildByID2<SListBox>(R.id.lb_phonenumbers);
		SASSERT(pPhoneNumbers);
		if (pPhoneNumbers)
		{
			pPhoneNumbers->DeleteAll();
			for (const auto&iter: contactInfo.PhoneNumber)
			{
				pPhoneNumbers->AddString(L"手机");
				pPhoneNumbers->AddString(iter);
			}
		}
	}
}

void CMyDeviceHandler::PushFileRecord(const std::string& udid, bool isafc2, const std::string& path)
{
	std::deque<FileRecord>& record = m_filerecords[udid].record1;
	while (record.size()>28)
		record.pop_front();
	record.push_back({ isafc2, path });
	m_filerecords[udid].record2.clear();
}

bool CMyDeviceHandler::PopFileRecord(const std::string& udid, FileRecord& filerecord)
{
	std::deque<FileRecord>& record = m_filerecords[udid].record1;	
	if(record.empty())
		return false;
	filerecord = record.back();
	m_filerecords[udid].record2.push_back(filerecord);
	record.pop_back();
	return true;
}

void CMyDeviceHandler::ResetRecord(const std::string &udid)
{
	m_filerecords[udid].record1.clear();
	m_filerecords[udid].record2.clear();
}
