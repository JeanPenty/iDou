#pragma once

#include <helper/SAdapterBase.h>
#include <string>
#include "../CDataCenter.h"
/*
<item text="文件系统">
		  <item text="用户系统"/>
		  <item text="越狱系统"/>
		</item>
		<item text="常用目录">
		  <item text="应用程序"/>
		  <item text="我的文档"/>
		  <item text="手机U盘"/>
		  <item text="语音备忘录"/>
		</item>
*/

struct NavInfo {
	int nCmd;
	bool bCan;
	SOUI::SStringW m_title;
};

class CiOSFilesTreeViewAdapter :public STreeAdapterBase<NavInfo>
{
public:
	std::string m_udid;
public:
	explicit CiOSFilesTreeViewAdapter(LPCSTR udid) :m_udid(udid)
	{
		NavInfo item;
		item.nCmd = 0;
		item.m_title = L"文件系统";
		item.bCan = false;
		SOUI::HTREEITEM hRoot = InsertItem(item);
		
		item.nCmd = 10;
		item.m_title = L"常用目录";
		item.bCan = false;
		hRoot = InsertItem(item);
		item.bCan = true;
		LPCWSTR title[] = { L"我的文档",L"手机U盘",L"语音备忘录" };
		for (int i = 1; i <4; i++)
		{
			item.nCmd = 10 + i;
			item.m_title = title[i - 1];
			InsertItem(item, hRoot);
		}
		notifyBranchInvalidated(ITEM_ROOT);
	}

	int GetCmd(SOUI::HTREEITEM item)
	{
		ItemInfo* ii = m_tree.GetItemPt(item);
		if (ii)
		{
			return ii->data.nCmd;
		}
		return -1;
	}

	void InitAfc(bool afc, bool afc2)
	{
		if (afc)
		{
			HSTREEITEM item1=GetFirstChildItem(ITEM_ROOT);
			ItemInfo& ii = m_tree.GetItemRef(item1);
			ii.data.bCan = true;

			NavInfo item;
			item.nCmd = 1;
			item.m_title = L"用户系统";
			item.bCan = true;
			SOUI::HTREEITEM hItem = InsertItem(item, item1);
			if (afc2)
			{				
				item.nCmd = 2;
				item.m_title = L"越狱系统";
				item.bCan = true;
				InsertItem(item, item1);
			}
			SetItemExpanded(item1, TRUE);
			HSTREEITEM item2 = GetNextSiblingItem(item1);
			if (afc2)
			{				
				item.nCmd = 14;
				item.m_title = L"应用程序";
				item.bCan = true;
				InsertItem(item, item2, STVI_FIRST);
			}

			ItemInfo& ii2 = m_tree.GetItemRef(item2);
			ii2.data.bCan = true;
			SetItemExpanded(item2, TRUE);

			notifyBranchInvalidated(ITEM_ROOT);
		}
	}

	int GetItemCount()const
	{
		return m_tree.GetChildrenCount(ITEM_ROOT);
	}

	~CiOSFilesTreeViewAdapter() {}

	virtual void getView(SOUI::HTREEITEM loc, SWindow* pItem, pugi::xml_node xmlTemplate)
	{
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)loc);
		int itemType = getViewType(loc);
		if (pItem->GetChildrenCount() == 0)
		{
			switch (itemType)
			{
			case 0:xmlTemplate = xmlTemplate.child(L"item_group");
				break;
			case 1:xmlTemplate = xmlTemplate.child(L"item_data");
				break;			
			}

			pItem->InitFromXml(xmlTemplate);
			if (itemType == 0)
			{
				pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_CLICK,
					Subscriber(&CiOSFilesTreeViewAdapter::OnGroupItemPanelClick, this));
			}
		}
		SWindow* pName = pItem->FindChildByID(R.id.Name);
		pName->SetWindowText(ii.data.m_title);
		pItem->EnableWindow(ii.data.bCan ? TRUE : FALSE);
	}

	bool OnGroupItemPanelClick(EventArgs* pEvt)
	{
		SItemPanel* pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
		SASSERT(pItemPanel);
		SOUI::HTREEITEM hItem = (SOUI::HTREEITEM)pItemPanel->GetItemIndex();
		BOOL bExpanded = IsItemExpanded(hItem);
		SetItemExpanded(hItem, !bExpanded);
		notifyBranchExpandChanged(hItem, bExpanded, !bExpanded);
		pEvt->bubbleUp = false;
		return true;
	}

	virtual int getViewType(SOUI::HTREEITEM hItem) const
	{
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		int ret = 0;
		return (ii.data.nCmd % 10 == 0) ? 0 : 1;
	}

	virtual int getViewTypeCount() const
	{
		return 2;
	}

protected:

private:
};