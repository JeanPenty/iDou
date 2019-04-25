#pragma once
#include "STabCtrlHeaderBinder.h"
#include <map>

namespace SOUI
{

	class STabCtrlTemplate:public STabCtrl
	{
		 SOUI_CLASS_NAME(STabCtrlTemplate, L"tabctrltemplate")
	public:
		STabCtrlTemplate(void);
		~STabCtrlTemplate(void);
		int InsertItem();
		virtual void OnInitFinished(pugi::xml_node xmlNode);
		void bindTab(LPCWSTR name,STabCtrl *pTab)
		{
			std::map<SStringW,STabCtrlHeaderBinder>::iterator ite= m_binders.find(name);
			if(ite!=m_binders.end())
				ite->second.BindHost(pTab);
			else
			{
				m_binders[name]=STabCtrlHeaderBinder();
				m_binders[name].BindHost(pTab);
			}
		}
		void Bind(LPCWSTR name, SWindow *pWnd,int idx)
		{
			std::map<SStringW,STabCtrlHeaderBinder>::iterator ite= m_binders.find(name);
			if(ite!=m_binders.end())
				ite->second.Bind(pWnd,idx);
		}
	protected:
		std::map<SStringW,STabCtrlHeaderBinder> m_binders;
		pugi::xml_document templateNode;
	};

}
