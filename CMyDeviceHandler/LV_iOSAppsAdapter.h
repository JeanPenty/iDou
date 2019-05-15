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
	
	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}

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
			if ((pItem->GetState() & WndState_Check) || (pItem->GetState() & WndState_Hover))
			{

			}
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