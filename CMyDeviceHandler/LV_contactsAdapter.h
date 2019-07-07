#pragma once



class CContactsListAdapter :public SMcAdapterBaseEx
{
	SArray<SStringW> m_colNames;
	
	std::vector<SOUI::SStringW> m_contacts;
	
public:
	std::string m_udid;
	CContactsListAdapter(LPCSTR udid) :m_udid(udid)
	{
		SASSERT(udid);
	}

	void CopyForm(std::vector<SOUI::SStringW>&& contacts)
	{
		m_contacts = contacts;
		notifyDataSetChanged();
	}

	virtual int getCount()
	{
		return m_contacts.size();
	}	

	virtual void getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
		const TCHAR* color[] = { L"#ffffff",L"#f9f9f9" };
		pItem->SetAttribute(L"colorNormal", color[position % 2]);			

		pItem->FindChildByID(R.id.lable_path)->SetWindowText(m_contacts[position]);
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
private:
};