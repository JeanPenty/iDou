#include "stdafx.h"
#include "STabCtrlTemplate.h"
namespace SOUI
{
	const wchar_t * KTemplate_Page = L"pageTemplate";
	STabCtrlTemplate::STabCtrlTemplate(void)
	{
	}

	STabCtrlTemplate::~STabCtrlTemplate(void)
	{
	}

	int STabCtrlTemplate::InsertItem()
	{
		return STabCtrl::InsertItem(templateNode.first_child().child(L"page"),-1,TRUE);		
	}

	void STabCtrlTemplate::OnInitFinished(pugi::xml_node xmlNode)
	{
		__super::OnInitFinished(xmlNode);
		templateNode.append_copy(xmlNode.child(KTemplate_Page));
	}
}
