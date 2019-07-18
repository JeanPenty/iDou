#pragma once

class CAppsListAdapter :public SMcAdapterBaseEx
{
	std::vector<AppInfo> m_apps;

	SArray<SStringW> m_colNames;
	std::string iosudid;
public:
	CAppsListAdapter(LPCSTR udid, SWindow* pCheckAll) :iosudid(udid)
	{
		m_apps_ctrl_wnd = pCheckAll;
		m_apps_ctrl_wnd->FindChildByID(R.id.app_check_all)->GetEventSet()->subscribeEvent(&CAppsListAdapter::OnCheckAllChanged, this);
	}

	bool OnCheckAllChanged(EventCmd* e)
	{
		SToggle2* pCheck = sobj_cast<SToggle2>(e->sender);
		bool bCheck = (pCheck->GetToggle() == TRUE);
		if (bCheck)
		{
			SelAll();
			updatactrl();
		}
		else
		{
			ClearSel();
			updatactrl();
		}
		notifyDataSetChanged();

		return true;
	}

	void InitApps(std::vector<AppInfo>&& apps)
	{
		m_apps = apps;
		notifyDataSetChanged();
	}

	void AddApp(AppInfo &app)
	{
		m_apps.push_back(app);
		notifyDataSetChanged();
	}

	void RemoveApp(LPCSTR id)
	{
		if (id)
		{
			auto iter = m_apps.begin();
			while (iter != m_apps.end())
			{
				if (iter->AppID == id)
				{
					m_apps.erase(iter);
					notifyDataSetChanged();
					break;
				}
				iter++;
			}
		}
	}

	virtual void SelAll()override
	{
		int max_id = getCount();
		m_selList.RemoveAll();
		for (int i = 0; i < max_id; i++)
			m_selList.AddTail(i);
	}

	virtual void SelectionEnd()override
	{
		updatactrl();
	}

	void updatactrl()
	{
		m_apps_ctrl_wnd->FindChildByID(R.id.btn_uninstallapp)->EnableWindow(m_selList.GetCount() > 1, TRUE);
		m_apps_ctrl_wnd->FindChildByID2<SToggle>(R.id.app_check_all)->SetToggle(getCount() == m_selList.GetCount(), TRUE);
	}

	virtual int getCount()
	{
		return m_apps.size();
	}

	bool CheckClick(EventCmd* e)
	{
		SToggle2* pWnd = sobj_cast<SToggle2>(e->sender);
		SASSERT(pWnd);
		ControlSel(pWnd->GetUserData());
		updatactrl();
		e->bubbleUp = false;
		return true;
	}

	bool OnItemClick(EventItemPanelClick* pEvt)
	{
		SItemPanel* pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
		SWND m_hHover = pItemPanel->SwndFromPoint(CPoint(GET_X_LPARAM(pEvt->lParam), GET_Y_LPARAM(pEvt->lParam)), FALSE);
		SWindow* pHover = SWindowMgr::GetWindow(m_hHover);
		if (pHover && !pHover->IsDisabled(TRUE) && (pHover->GetID() == R.id.check))
		{
			//ControlSel(pItemPanel->GetItemIndex());
			pEvt->bubbleUp = false;
		}
		else//单选。如果不是只有一项一定是没全选
		{
			updatactrl();
			//m_apps_ctrl_wnd->FindChildByID2<SToggle>(R.id.app_check_all)->SetToggle(getCount() == 1, TRUE);
		}
		return true;
	}

	bool OnUninstallApp(EventCmd* e)
	{
		SWindow* pWnd = sobj_cast<SWindow>(e->sender);
		SASSERT(pWnd);
		SASSERT(pWnd->GetUserData() < m_apps.size());

		{

			CRect rc = pWnd->GetWindowRect();
			SItemPanel* pItemPanel = (SItemPanel*)pWnd->GetTopLevelParent();
			CRect rc2 = pItemPanel->GetItemRect();
			rc += rc2.TopLeft();
			POINT pt = { rc.left,rc.bottom };
			ClientToScreen(pItemPanel->GetContainer()->GetHostHwnd(),& pt);
			SMenuEx menu;
			menu.LoadMenu(_T("SMENU:menu_main"));
			menu.TrackPopupMenu(0, pt.x, pt.y + 2, pItemPanel->GetContainer()->GetHostHwnd());

			/*
			const AppInfo& appInfo = m_apps[pWnd->GetUserData()];
			if (SMessageBox(NULL, SStringT().Format(_T("确定要卸载 [%s] 吗?"), appInfo.DisplayName),
				_T("卸载应用"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			{
				CDataCenter::getSingleton().UninstallApp(iosudid.c_str(), appInfo.AppID.c_str());
			}*/
		}
		return true;
	}

	virtual void getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
			pItem->FindChildByID(R.id.check)->GetEventSet()->subscribeEvent(&CAppsListAdapter::CheckClick, this);
			pItem->GetEventSet()->subscribeEvent(&CAppsListAdapter::OnItemClick, this);
			pItem->FindChildByID(R.id.btn_appUninstall)->GetEventSet()->subscribeEvent(&CAppsListAdapter::OnUninstallApp, this);
		}

		const TCHAR* color[] = { L"#ffffff",L"#f9f9f9" };
		pItem->SetAttribute(L"colorNormal", color[position % 2]);

		{
			const AppInfo& appInfo = m_apps[position];
			pItem->FindChildByID(R.id.lable_appName)->SetWindowText(appInfo.DisplayName);
			pItem->FindChildByID(R.id.lable_appVer)->SetWindowText(appInfo.Version);
			pItem->FindChildByID(R.id.lable_appSize)->SetWindowText(utils::MakePrintSize(appInfo.StaticDiskUsage));
			pItem->FindChildByID(R.id.lable_appCacheSize)->SetWindowText(utils::MakePrintSize(appInfo.DynamicDiskUsage));
			pItem->FindChildByID2<SImageWnd>(R.id.img_appIcon)->SetImage(appInfo.m_ico, FilterLevel::kHigh_FilterLevel);
			SToggle* check = pItem->FindChildByID2<SToggle>(R.id.check);
			SASSERT(check);
			check->SetToggle(pItem->GetState() & WndState_Check);
			check->SetUserData(position);
			pItem->FindChildByID(R.id.btn_appUninstall)->SetUserData(position);;
		}
	}

	int GetColCount()
	{
		return m_colNames.GetCount();
	}

	SStringW GetColumnName(int iCol) const {
		return m_colNames[iCol];
	}

	void IniColNames(pugi::xml_node xmlTemplate)
	{
		for (xmlTemplate = xmlTemplate.first_child(); xmlTemplate; xmlTemplate = xmlTemplate.next_sibling())
		{
			m_colNames.Add(xmlTemplate.attribute(L"name").value());
		}
	}

	virtual bool OnSort(int iCol, SHDSORTFLAG* stFlags, int nCols)
	{
		return false;
	}

	virtual void InitByTemplate(pugi::xml_node xmlTemplate)
	{
		IniColNames(xmlTemplate);
	}

	std::vector<std::string> GetAllSelApps()
	{
		std::vector<std::string> apps;
		{
			SPOSITION pos = m_selList.GetHeadPosition();
			while (pos)
			{
				long position = m_selList.GetNext(pos);
				SASSERT(position < m_apps.size());
				const AppInfo& appInfo = m_apps[position];
				apps.push_back(appInfo.AppID);
			}
		}
		return apps;
	}

private:
	CAutoRefPtr<SWindow> m_apps_ctrl_wnd;
};