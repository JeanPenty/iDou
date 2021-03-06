#pragma once

#include <helper/SAdapterBase.h>
#include <string>
#include "../CDataCenter.h"


struct DevInfo {
	int nCmd;
	bool bCan;
	bool bUpdataApp;
	int appsCount=-1;
	std::string udid;
	SStringT MakeAppTitle()
	{
		if (appsCount!=-1)
		{
			return GETSTRING(L"@string/cmd2") + SStringT().Format(L"(%d)", appsCount);
		}
		return GETSTRING(L"@string/cmd2");
	}
};

class CiOSDeviceTreeViewAdapter :public STreeAdapterBase<DevInfo>
{
	friend class CDataCenter;
	friend class CMyDeviceHandler;
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

	void ExpandItem(LPCSTR id)
	{
		HSTREEITEM node = m_tree.GetChildItem(ITEM_ROOT);
		while (node)
		{
			ItemInfo& ii = m_tree.GetItemRef(node);
			if (ii.data.udid == id)
			{
				SetItemExpanded(node, TRUE);
				notifyBranchExpandChanged(node, FALSE, TRUE);							
				break;
			}
			node = m_tree.GetNextSiblingItem(node);
		}
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
			case 2:xmlTemplate = xmlTemplate.child(L"item_appdata");
				break;
			case 3:xmlTemplate = xmlTemplate.child(L"item_data");
				break;
			}
			pItem->InitFromXml(xmlTemplate);
			if (itemType == 0)
			{
				pItem->GetEventSet()->subscribeEvent(EVT_ITEMPANEL_CLICK,
					Subscriber(&CiOSDeviceTreeViewAdapter::OnGroupItemPanelClick, this));
			}
		}
		SWindow* pName = pItem->FindChildByID(R.id.Name);
		if (itemType == 0)
		{
			pItem->FindChildByID(R.id.img_loading)->SetVisible(!ii.data.bCan);
			if (ii.data.bCan)
			{
				pItem->EnableWindow(TRUE);
				SStringT devName;
				if (CDataCenter::getSingleton().GetDevName(ii.data.udid.c_str(), devName))
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
			SImageWnd* pIcon = pItem->FindChildByID2<SImageWnd>(R.id.face);
			SASSERT(pIcon);
			switch (ii.data.nCmd)
			{
			case 1:
			{
				pIcon->SetIcon(0);
				pName->SetWindowText(GETSTRING(L"@string/cmd1"));
			}break;
			case 2:
			{
				pIcon->SetIcon(1);
				pItem->FindChildByID(R.id.img_loading)->SetVisible(ii.data.bUpdataApp?TRUE:FALSE);
				pName->SetWindowText(ii.data.MakeAppTitle());
			}break;
			case 3:
			{
					pIcon->SetIcon(2);
				pName->SetWindowText(GETSTRING(L"@string/cmd3"));
			}break;
			case 4:
			{
					pIcon->SetIcon(3);
				pName->SetWindowText(GETSTRING(L"@string/cmd4"));
			}break;
			case 5:
			{
					pIcon->SetIcon(4);
				pName->SetWindowText(GETSTRING(L"@string/cmd5"));
			}break;
			default:
				break;
			}
			if (pItem->GetState() & WndState_Check)
			{
				((SItemPanel*)pItem)->SetSkin(GETSKIN(L"skin_bk",100));
			}
			else
			{
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
	
	virtual int getViewType(SOUI::HTREEITEM hItem) const
	{
		ItemInfo& ii = m_tree.GetItemRef((HSTREEITEM)hItem);
		int ret = 0;
		switch (ii.data.nCmd)
		{
			case 0:ret = 0; break;
			case 1:ret = 1; break;
			case 2:ret = 2; break;
			default:ret = 3;
		}
		return ret;
	}

	virtual int getViewTypeCount() const
	{
		return 4;
	}
	void UpDataDev(LPCSTR id)
	{
		HSTREEITEM node = m_tree.GetChildItem(ITEM_ROOT);
		while (node)
		{
			if (m_tree.GetItem(node).data.udid == id)
			{
				notifyBranchInvalidated(node);
				break;
			}
			node = m_tree.GetNextSiblingItem(node);
		}
	}
	void UpDataDevAppInfo(LPCSTR id,int appscount)
	{
		HSTREEITEM node = m_tree.GetChildItem(ITEM_ROOT);
		while (node)
		{
			if (m_tree.GetItem(node).data.udid == id)
			{
				_UpdataChildItem(node,2, appscount);
				break;
			}
			node = m_tree.GetNextSiblingItem(node);
		}
	}
	private:
		void _UpdataChildItem(HSTREEITEM node,int id,int appcount)
		{
			HSTREEITEM cnode = m_tree.GetChildItem(node);
			while (cnode)
			{
				if (m_tree.GetItem(cnode).data.nCmd == id)
				{
					m_tree.GetItemRef(cnode).data.bUpdataApp = false;
					m_tree.GetItemRef(cnode).data.appsCount = appcount;
					notifyBranchInvalidated(cnode);
					break;
				}
				cnode = m_tree.GetNextSiblingItem(cnode);
			}
		}
		
protected:
	//添加一个设备id:udid can:是否可操作
	void AddDev(LPCSTR id, bool can)
	{
		DevInfo item;
		item.nCmd = 0;
		item.udid = id;
		item.bCan = can;
		SOUI::HTREEITEM hRoot = InsertItem(item);
		SetItemExpanded(hRoot, can ? TRUE : FALSE);

		for (int i = 1; i < 6; i++)
		{
			item.nCmd = i;
			if (can && i == 2)
				item.bUpdataApp = true;
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
			if (ii.data.udid == id)
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
			if (m_tree.GetItem(node).data.udid == id)
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