#pragma once



class CContactsListAdapter :public SMcAdapterBaseEx
{
	SArray<SStringW> m_colNames;
	
	std::map<int, ContactInfo> m_contacts;
	std::map<int, int> m_idx2id;

public:
	std::string m_udid;
	CContactsListAdapter(LPCSTR udid) :m_udid(udid)
	{
		SASSERT(udid);
	}

	void CopyForm(std::map<int, ContactInfo>&& contacts)
	{
		m_contacts = std::move(contacts);

		m_idx2id.clear();
		int i = 0;
		for (const auto& ite : m_contacts)
		{
			m_idx2id[i++] = ite.first;
		}


		notifyDataSetChanged();
	}

	ContactInfo GetContactInfo(int idx)
	{
		SASSERT(idx>=0&&idx<m_idx2id.size());
		return m_contacts[m_idx2id[idx]];
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
		ContactInfo & contactInfo=m_contacts[m_idx2id[position]];
		pItem->FindChildByID(R.id.lable_name)->SetWindowText(utils::MakeName(contactInfo.FirstName, contactInfo.LastName));
		pItem->FindChildByID(R.id.lable_phonenumber)->SetWindowText(utils::MakePhoneNumber(contactInfo.PhoneNumber));
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