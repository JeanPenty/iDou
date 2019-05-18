#pragma once

class CAppsListAdapter :public SMcAdapterBaseEx
{
	SArray<SStringW> m_colNames;
	std::string iosudid;
public:
	CAppsListAdapter(LPCSTR udid) :iosudid(udid)
	{		
	}

	virtual int getCount()
	{
		const std::vector<AppInfo>* pApps=CDataCenter::getSingleton().GetApps(iosudid.c_str());
		if (pApps)
			return pApps->size();
		return 0;
	}
		
	bool CheckClick(EventCmd *e)
	{
		SWindow* pWnd = sobj_cast<SWindow>(e->sender);
		SASSERT(pWnd);
		ControlSel(pWnd->GetUserData());
		return true;
	}

	bool OnItemClick(EventItemPanelClick* pEvt)
	{
		SItemPanel* pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
		SWND m_hHover = pItemPanel->SwndFromPoint(CPoint(GET_X_LPARAM(pEvt->lParam), GET_Y_LPARAM(pEvt->lParam)), FALSE);
		SWindow* pHover = SWindowMgr::GetWindow(m_hHover);
		if (pHover && !pHover->IsDisabled(TRUE)&& (pHover->GetID()== R.id.check))
		{
			//ControlSel(pItemPanel->GetItemIndex());
			pEvt->bubbleUp = false;
		}
		return true;
	}
	
	bool OnUninstallApp(EventCmd* e)
	{
		SWindow* pWnd = sobj_cast<SWindow>(e->sender);
		SASSERT(pWnd);
		
		const std::vector<AppInfo>* pApps = CDataCenter::getSingleton().GetApps(iosudid.c_str());
		SASSERT(pApps);
		SASSERT(pWnd->GetUserData() < pApps->size());
		if (pApps)
		{
			const AppInfo& appInfo = (*pApps)[pWnd->GetUserData()];
			if (SMessageBox(NULL, SStringT().Format(_T("确定要卸载 [%s] 吗?"), appInfo.DisplayName),
				_T("卸载应用"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			{
				CDataCenter::getSingleton().UninstallApp(iosudid.c_str(),appInfo.AppID.c_str());
			}
		}
		return true;
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
			pItem->FindChildByID(R.id.check)->GetEventSet()->subscribeEvent(&CAppsListAdapter::CheckClick, this);
			pItem->GetEventSet()->subscribeEvent(&CAppsListAdapter::OnItemClick,this);
			pItem->FindChildByID(R.id.btn_appUninstall)->GetEventSet()->subscribeEvent(&CAppsListAdapter::OnUninstallApp, this);
		}

		const TCHAR* color[] = {L"#ffffff",L"#f9f9f9"};
		pItem->SetAttribute(L"colorNormal", color[position % 2]);


		const std::vector<AppInfo>* pApps = CDataCenter::getSingleton().GetApps(iosudid.c_str());
		SASSERT(position < pApps->size());
		if (pApps)
		{
			const AppInfo& appInfo = (*pApps)[position];
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

	virtual bool OnSort(int iCol, SHDSORTFLAG * stFlags, int nCols)
	{
		return false;
	}

	virtual void InitByTemplate(pugi::xml_node xmlTemplate)
	{
		IniColNames(xmlTemplate);
	}
	
private:
};