#pragma once

class CPageHandlerBase
{
public:
	CPageHandlerBase();
	~CPageHandlerBase();

	virtual void OnInit(SWindow *pRoot)
	{
	}

	virtual void OnUninit() {
		m_pPageRoot = NULL;
	}

	SWindow * GetRoot() {
		return m_pPageRoot;
	}
protected:
	SWindow * m_pPageRoot;
};

