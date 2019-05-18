#include "StdAfx.h"
#include "SToggle2.h"

namespace SOUI
{
	SToggle2::SToggle2(void)
	{
	}

	SToggle2::~SToggle2(void)
	{
	}

	void SToggle2::OnPaint(IRenderTarget *pRT)
	{
		CRect rcWnd = GetClientRect();
		m_pSkin->Draw(pRT, GetWindowRect(),  (m_bToggled)?1:IIF_STATE3(GetState(),0,0,1));
	}

	//CSize SToggle2::GetDesiredSize(LPCRECT pRcContainer)
	//{
	//	return m_pSkin1->GetSkinSize();
	//}

}
