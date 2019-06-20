#pragma once



struct AFC_FILEINFO
{
	enum IOSFILETYPE
	{
		LINK, DIR, FILE, Other
	};

	SStringW path;
	std::string fullpath;
	bool isDir()
	{
		return (S_ISDIR(m_stat.st_mode) || (S_ISLNK(m_stat.st_mode)));
	}
	bool isFile()
	{
		return S_ISREG(m_stat.st_mode);
	}
	IOSFILETYPE GetType()
	{
		//判断是否为符号连接
		if (S_ISLNK(m_stat.st_mode))
			return LINK;
		//是否为一般文件
		if (S_ISREG(m_stat.st_mode))
			return FILE;
		//是否为目录
		if (S_ISDIR(m_stat.st_mode))
			return DIR;
		return Other;
	}
	SStringT GetFileSize()
	{
		if (S_ISREG(m_stat.st_mode))
			return utils::MakePrintSize(m_stat.st_size);
		return L"--";
	}
	SStringT GetTypeString()
	{
		switch (GetType())
		{
			case LINK:
				return L"符号链接";
				break;
			case DIR:
				return L"目录";
				break;
			case FILE:
				return L"文件";
				break;
			default:
				return L"其它";
		}
	}
	struct stat m_stat;
};

class CFilesListAdapter :public SMcAdapterBaseEx
{
	SArray<SStringW> m_colNames;
	SArray<AFC_FILEINFO> m_files;
	UINT m_nCurTaskID = -1;
	bool m_bIsJailreak = false;
public:
	std::string m_udid;

	CFilesListAdapter(LPCSTR udid) :m_udid(udid)
	{
		SASSERT(udid);
	}

	std::string GetItemFullPath(int pos, bool& isdir, bool& isfile)
	{
		if (pos < 0 || pos >= m_files.GetCount())
		{
			return "";
		}
		AFC_FILEINFO& fileinfo = m_files[pos];
		isdir = fileinfo.isDir();
		isfile = fileinfo.isFile();
		return fileinfo.fullpath;
	}

	bool IsCurTask(UINT ID)const
	{
		return ID == m_nCurTaskID;
	}

	void Updata(std::list<FILEINFO>& filelist)
	{
		m_files.RemoveAll();
		for (auto& iter : filelist)
		{
			AFC_FILEINFO fileinfo;
			fileinfo.path = S_CA2W(iter.m_path.c_str(), CP_UTF8);
			fileinfo.m_stat = iter.m_stat;
			fileinfo.fullpath = iter.m_fullpath;
			m_files.Add(fileinfo);
		}
		notifyDataSetChanged();
	}

	bool CurIsJailreak()
	{
		return m_bIsJailreak;
	}

	void SetTaskID(UINT ID, bool isJailreak)
	{
		m_nCurTaskID = ID;
		m_bIsJailreak = isJailreak;
	}

	void UpdataTaskID(UINT ID)
	{
		m_nCurTaskID = ID;
	}

	virtual int getCount()
	{
		return m_files.GetCount();
	}

	bool OnItemClick(EventItemPanelClick* pEvt)
	{
		SItemPanel* pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
		//SWND m_hHover = pItemPanel->SwndFromPoint(CPoint(GET_X_LPARAM(pEvt->lParam), GET_Y_LPARAM(pEvt->lParam)), FALSE);
		//SWindow* pHover = SWindowMgr::GetWindow(m_hHover);
		//if (pHover && !pHover->IsDisabled(TRUE) && (pHover->GetID() == R.id.check))
		//{
		//	//ControlSel(pItemPanel->GetItemIndex());
		//	pEvt->bubbleUp = false;
		//}
		//else//单选。如果不是只有一项一定是没全选
		//{
		//	m_pCheckBox->SetToggle(getCount() == 1, TRUE);
		//}
		return true;
	}

	virtual void getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
		const TCHAR* color[] = { L"#ffffff",L"#f9f9f9" };
		pItem->SetAttribute(L"colorNormal", color[position % 2]);

		AFC_FILEINFO& fileinfo = m_files[position];
		
		SHFILEINFO FILEINFO = { 0 };

		//clock_t s = clock();
		if (fileinfo.isDir())
		{
			SHGetFileInfo(L"folder", FILE_ATTRIBUTE_DIRECTORY, &FILEINFO, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
		}
		else
		{
			SHGetFileInfo(fileinfo.path, NULL, &FILEINFO, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
		}
		/*clock_t e= clock();
		clock_t dif = e - s;*/
		pItem->FindChildByID2<SIconWnd>(R.id.img_fileIcon)->SetIcon(FILEINFO.hIcon);

		pItem->FindChildByID(R.id.lable_path)->SetWindowText(fileinfo.path);
		pItem->FindChildByID(R.id.lable_size)->SetWindowText(fileinfo.GetFileSize());
		pItem->FindChildByID(R.id.lable_type)->SetWindowText(fileinfo.GetTypeString());
		SStringT time;
		pItem->FindChildByID(R.id.lable_time)->SetWindowText(utils::FormatTime(fileinfo.m_stat.st_mtime, time));
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