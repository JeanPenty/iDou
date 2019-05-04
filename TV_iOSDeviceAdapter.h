#pragma once

#include <helper/SAdapterBase.h>
#include <string>
#include "CDataCenter.h"


struct DevInfo {
	int nCmd;
	bool bCan;
	std::string guid;
};

class CiOSDeviceTreeViewAdapter :public STreeAdapterBase<DevInfo>
{
	friend class CDataCenter;
public:

	CiOSDeviceTreeViewAdapter()
	{		
	}

	int GetItemCount()const
	{
		return m_tree.GetChildrenCount(ITEM_ROOT);
	}

	const ItemInfo& GetData(HSTREEITEM item)
	{
		return m_tree.GetItemRef(item);
	}

	HSTREEITEM GetFirstTreeItem()
	{
		HSTREEITEM hFistItem= m_tree.GetChildItem(ITEM_ROOT);
		if(hFistItem)
			return m_tree.GetChildItem(hFistItem);
		return NULL;
	}
	~CiOSDeviceTreeViewAdapter() {}

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
			case 1:xmlTemplate = xmlTemplate.child(L"item_firstdata");
				break;
			case 2:xmlTemplate = xmlTemplate.child(L"item_data");
				break;
			}
			pItem->InitFromXml(xmlTemplate);
			if (itemType == 0)
			{
				pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_CLICK,
					Subscriber(&CiOSDeviceTreeViewAdapter::OnGroupItemPanelClick, this));
			}
		}
		/*
		if (itemType != 0)
		{
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_CLICK,
				Subscriber(&CiOSDeviceTreeViewAdapter::OnItemPanelClick, this));
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_DBCLICK,
				Subscriber(&CiOSDeviceTreeViewAdapter::OnItemPanelDBClick, this));
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_RCLICK,
				Subscriber(&CiOSDeviceTreeViewAdapter::OnItemPanelRClick, this));
			//pItem->FindChildByName2<SImageWnd>(L"face")->SetAttribute(L"skin", ii.data.strImg, FALSE);
		}*/
		SWindow* pName = pItem->FindChildByName(L"Name");
		if (itemType == 0)
		{
			if (ii.data.bCan)
			{
				pItem->EnableWindow(TRUE);
				SStringT devName;
				if (CDataCenter::getSingleton().GetDevName(ii.data.guid.c_str(), devName))
					pName->SetWindowText(devName);
				else
					pName->SetWindowText(GETSTRING(L"@string/noname"));
			}
			else
			{
				pItem->EnableWindow(FALSE);
				pName->SetWindowText(GETSTRING(L"@string/waitpair"));
			}
		}
		else
		{
			switch (ii.data.nCmd)
			{
			case 1:
			{
				pName->SetWindowText(GETSTRING(L"@string/cmd1"));
			}break;
			case 2:
			{
				pName->SetWindowText(GETSTRING(L"@string/cmd2"));
			}break;
			case 3:
			{
				pName->SetWindowText(GETSTRING(L"@string/cmd3"));
			}break;
			case 4:
			{
				pName->SetWindowText(GETSTRING(L"@string/cmd4"));
			}break;
			case 5:
			{
				pName->SetWindowText(GETSTRING(L"@string/cmd5"));
			}break;
			default:
				break;
			}
			if (pItem->GetState() & WndState_Check)
			{
				//pItem->FindChildByID(R.id.bkwnd)->SetAttribute(L"skin", L"skin_bk");
				((SItemPanel*)pItem)->SetSkin(GETSKIN(L"skin_bk",100));
			}
			else
			{
				//pItem->FindChildByID(R.id.bkwnd)->SetAttribute(L"skin", L"");
				((SItemPanel*)pItem)->SetSkin(NULL);
			}			
		}
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

/*

	bool OnItemPanelClick(EventArgs* e)
	{
		SItemPanel* pItem = sobj_cast<SItemPanel>(e->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (m_pListener)
		{
			m_pListener->ChildMenuItemClick(ii.data.guid.c_str(), ii.data.nCmd);
		}

		return true;
	}

	bool OnItemPanelDBClick(EventArgs* e)
	{
		SItemPanel* pItem = sobj_cast<SItemPanel>(e->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (m_pListener)
		{
			m_pListener->ChildMenuItemDBClick(ii.data.guid.c_str(), ii.data.nCmd);
		}

		return true;
	}

	bool OnItemPanelRClick(EventArgs* e)
	{
		SItemPanel* pItem = sobj_cast<SItemPanel>(e->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (m_pListener)
		{
			m_pListener->ChildMenuItemRClick(ii.data.guid.c_str(), ii.data.nCmd);
		}

		return true;
	}
	//*/
	virtual int getViewType(SOUI::HTREEITEM hItem) const
	{
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		return (ii.data.nCmd == 0) ? 0 : (ii.data.nCmd == 1)?1:2;
	}

	virtual int getViewTypeCount() const
	{
		return 3;
	}

protected:
	//添加一个设备id:udid can:是否可操作
	void AddDev(LPCSTR id, bool can)
	{
		DevInfo item;
		item.nCmd = 0;
		item.guid = id;
		item.bCan = can;
		SOUI::HTREEITEM hRoot = InsertItem(item);
		SetItemExpanded(hRoot, can ? TRUE : FALSE);

		for (int i = 1; i < 6; i++)
		{
			item.nCmd = i;
			InsertItem(item, hRoot);
		}
		notifyBranchInvalidated(ITEM_ROOT);
	}
	void SetDevCan(LPCSTR id, bool can = true)
	{
		HSTREEITEM node = m_tree.GetChildItem(ITEM_ROOT);
		while (node)
		{
			ItemInfo& ii = m_tree.GetItemRef(node);
			if (ii.data.guid == id)
			{
				if (ii.data.bCan != can)
				{
					ii.data.bCan = can;
					notifyBranchInvalidated(node);
				}
				break;
			}
			node = m_tree.GetNextSiblingItem(node);
		}
	}
	void RemoveDev(LPCSTR id)
	{
		HSTREEITEM node = m_tree.GetChildItem(ITEM_ROOT);
		while (node)
		{
			if (m_tree.GetItem(node).data.guid == id)
			{
				m_tree.DeleteItem(node);
				notifyBranchInvalidated(ITEM_ROOT);
				break;
			}
			node = m_tree.GetNextSiblingItem(node);
		}
	}

private:
};