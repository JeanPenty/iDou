#include "stdafx.h"
#include "CiTunesRepairDlg.h"
#include <versionhelpers.h>

//检查并根据系统版本选择打开程序方式
BOOL ShellExecuteExOpen(SStringT appName, SStringT appPath, SStringT par,SStringT &ec)
{

	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = IsWindowsVistaOrGreater() ? TEXT("runas") : NULL;
	sei.lpFile = appName;
	sei.lpDirectory = appPath;
	sei.nShow = SW_SHOWNORMAL;
	sei.lpParameters = par;
	if (!ShellExecuteEx(&sei))
	{
		DWORD dwStatus = GetLastError();
		if (dwStatus == ERROR_CANCELLED)
		{
			ec= L"提升权限被用户拒绝！";
		}
		else if (dwStatus == ERROR_FILE_NOT_FOUND)
		{
			ec = L"所要执行的文件没有找到！";
		}
		else
		{
			ec.Format(L"无法启动修复程序,错误：%d", dwStatus);
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CiTunesRepairDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_ipcSvr->Init((ULONG_PTR)m_hWnd, this);
	wchar_t wszHWnd[64] = {};
	::wnsprintfW(wszHWnd, _countof(wszHWnd), L"0x%p", m_hWnd);
	SStringT outMsg;
	BOOL bRet = ShellExecuteExOpen(L"iTunesRepair.exe", SApplication::getSingleton().GetAppDir(), wszHWnd, outMsg);
	if (!bRet)
	{
		//lable_error_msg
		STabCtrl* tab = FindChildByID2<STabCtrl>(R.id.tab_repair);
		SASSERT(tab);
		tab->SetCurSel(6);
		tab->FindChildByID(R.id.lable_error_msg)->SetWindowText(outMsg);
	}
	return bRet;
}


// 通过 IIpcSvrCallback 继承

void CiTunesRepairDlg::OnNewConnection(IIpcHandle* pIpcHandle, IIpcConnection** ppConn)
{
	*ppConn = new CiTunesIPCSvrConnect(pIpcHandle, this);
}

int CiTunesRepairDlg::GetBufSize() const
{
	return 1024;
}

void* CiTunesRepairDlg::GetSecurityAttr() const
{
	return NULL;
}

void CiTunesRepairDlg::ReleaseSecurityAttr(void* psa) const
{

}

void CiTunesRepairDlg::OnConnected(IIpcConnection* pConn)
{
	//此处连接未完全准备好，不能操作！！！！！！！！！！！！！！！！！！！！！！！
	/*Param_SATRTCHECK startChk;
	bool bRet= pConn->GetIpcHandle()->CallFun(&startChk);*/
	SWindow* pBtn = FindChildByID(R.id.btn_check);
	SASSERT(pBtn);
	pBtn->EnableWindow(TRUE, TRUE);
}

void CiTunesRepairDlg::OnDisconnected(IIpcConnection* pConn)
{
	//SMessageBox(m_hWnd, SStringT().Format(L"连接:Ox%p断开", pConn->GetIpcHandle()->GetRemoteId()), L"", MB_OK);
}

void CiTunesRepairDlg::OnCheckRet(Param_CHECKRET& ret)
{
	STabCtrl* tab = FindChildByID2<STabCtrl>(R.id.tab_repair);
	SASSERT(tab);

	switch (ret.checkRet)
	{
	case CHK_NO:
		tab->SetCurSel(1);
		break;
	case CHK_OK:
		tab->FindChildByID(R.id.lable_amds_ver)->SetWindowTextW(ret.ver.c_str());
		tab->SetCurSel(2);
		break;
	case CHK_FAILE:
		tab->SetCurSel(3);
		break;
	}
}

void CiTunesRepairDlg::OnRepairRet(Param_REPAIR& ret)
{
	STabCtrl* tab = FindChildByID2<STabCtrl>(R.id.tab_repair);
	SASSERT(tab);

	switch (ret.repairRet)
	{
	case REPAIR_OK:
	{
		tab->SetCurSel(5);
	}
	break;
	case REPAIR_FAILE:
		tab->SetCurSel(4);
		break;
	}
}

void CiTunesRepairDlg::OnConnectedr(Param_CONNECTED& ret)
{
	StartCheck();
}

void CiTunesRepairDlg::OnOpenItunesUrl()
{
	const TCHAR* Url = _T("https://support.apple.com/zh_CN/downloads/itunes");
	ShellExecute(NULL, L"open", Url, NULL, NULL, SW_SHOWNORMAL);
	OnOK();
}

void CiTunesRepairDlg::OnDestroy()
{
	Close();
	SetMsgHandled(FALSE);
}

void CiTunesRepairDlg::OnCheck() {
	StartCheck();
	SWindow* pBtn = FindChildByID(R.id.btn_check);
	SASSERT(pBtn);
	pBtn->EnableWindow(FALSE, TRUE);
}

void CiTunesRepairDlg::OnRepair() {
	StartRepair();
	SWindow* pBtn = FindChildByID(R.id.btn_repair);
	SASSERT(pBtn);
	pBtn->EnableWindow(FALSE, TRUE);
}
