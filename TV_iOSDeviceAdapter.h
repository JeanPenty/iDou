#pragma once

#include <helper/SAdapterBase.h>
#include <string>

struct DevInfo {
	int nCmd;
	std::string guid;
};

class CiOSDeviceTreeViewAdapter :public STreeAdapterBase<DevInfo>
{
	friend class CDataCenter;
public:
	struct IListener
	{
		virtual void ChildMenuItemClick(int nGID) = 0;
		virtual void ChildMenuItemDBClick(int nGID) = 0;
		virtual void ChildMenuItemRClick(int nGID) = 0;
	};
public:

	CiOSDeviceTreeViewAdapter(IListener* pListener)
	{
		m_pListener = pListener;
	}

	~CiOSDeviceTreeViewAdapter() {}

	virtual void getView(SOUI::HTREEITEM loc, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)loc);
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
					Subscriber(&CiOSDeviceTreeViewAdapter::OnGroupItemPanelClick, this));
			}
		}
		if (itemType != 0)
		{
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_CLICK,
				Subscriber(&CiOSDeviceTreeViewAdapter::OnItemPanelClick, this));
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_DBCLICK,
				Subscriber(&CiOSDeviceTreeViewAdapter::OnItemPanelDBClick, this));
			pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_RCLICK,
				Subscriber(&CiOSDeviceTreeViewAdapter::OnItemPanelRClick, this));
			//pItem->FindChildByName2<SImageWnd>(L"face")->SetAttribute(L"skin", ii.data.strImg, FALSE);
		}
		//pItem->FindChildByName(L"name")->SetWindowText(ii.data.strName);
	}

	bool OnGroupItemPanelClick(EventArgs *pEvt)
	{
		SItemPanel *pItemPanel = sobj_cast<SItemPanel>(pEvt->sender);
		SASSERT(pItemPanel);
		SOUI::HTREEITEM hItem = (SOUI::HTREEITEM)pItemPanel->GetItemIndex();
		BOOL bExpanded = IsItemExpanded(hItem);
		SetItemExpanded(hItem, !bExpanded);
		notifyBranchExpandChanged(hItem, bExpanded, !bExpanded);
		return true;
	}

	bool OnItemPanelClick(EventArgs *e)
	{
		EventItemPanelClick* pEvt = sobj_cast<EventItemPanelClick>(e);
		if (NULL == pEvt) return true;

		SItemPanel* pItem = sobj_cast<SItemPanel>(pEvt->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (m_pListener)
		{
			//m_pListener->ChildMenuItemClick(ii.data.gid, ii.data.strID);
		}

		return true;
	}

	bool OnItemPanelDBClick(EventArgs* e)
	{
		EventItemPanelDbclick* pEvt = sobj_cast<EventItemPanelDbclick>(e);
		if (NULL == pEvt) return true;

		SItemPanel* pItem = sobj_cast<SItemPanel>(pEvt->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (m_pListener)
		{
			//m_pListener->ChildMenuItemDBClick(ii.data.gid, ii.data.strID);
		}

		return true;
	}

	bool OnItemPanelRClick(EventArgs *e)
	{
		EventItemPanelRclick* pEvt = sobj_cast<EventItemPanelRclick>(e);
		if (NULL == pEvt) return true;

		SItemPanel* pItem = sobj_cast<SItemPanel>(pEvt->sender);
		if (NULL == pItem) return true;

		int hItem = static_cast<SOUI::HTREEITEM>(pItem->GetItemIndex());
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		if (m_pListener)
		{
			//m_pListener->ChildMenuItemRClick(ii.data.gid, ii.data.strID);
		}

		return true;
	}

	virtual int getViewType(SOUI::HTREEITEM hItem) const
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		return (ii.data.nCmd==0)?0:1;
	}

	virtual int getViewTypeCount() const
	{
		return 2;
	}

protected:
	void AddDev(LPCSTR id)
	{
		DevInfo item;
		item.nCmd = 0;
		item.guid = id;
		SOUI::HTREEITEM hRoot = InsertItem(item);
		SetItemExpanded(hRoot, TRUE);
				
		for(int i=1;i<6;i++)
		{
			item.nCmd = i;
			InsertItem(item, hRoot);
		}
		notifyBranchInvalidated(ITEM_ROOT);
	}
	void RemoveDev(LPCSTR id)
	{
		HSTREEITEM node = m_tree.GetChildItem(ITEM_ROOT);
		while (node)
		{
			if (m_tree.GetItem(node).data.guid == id)
			{
				m_tree.DeleteItem(node);
				break;
			}
			node = m_tree.GetNextSiblingItem(node);
		}

		notifyBranchInvalidated(ITEM_ROOT);
	}

private:
	IListener*		m_pListener;
};