#pragma once
#include <string>
#include <sstream>
#include <interface/SIpcObj-i.h>
#include <helper/sipcparamhelper.hpp>

#define iTunes_SHARE_BUF_NAME_FMT _T("{38886186-A4A2-46F0-81B3-26B1F5FCBD55}_%08x_2_%08x")

#define FUN_BEGINEX \
virtual bool HandleFun(IIpcHandle * IpcHandle,UINT uMsg, SOUI::SParamStream &ps)override{ \
	bool bHandled = false; \

#define FUN_HANDLEREX(x,fun) \
	if(!bHandled && uMsg == x::FUN_ID) \
	{\
		x param; \
		IpcHandle->FromStream4Input(&param,ps.GetBuffer());\
		ps.GetBuffer()->Seek(SOUI::IShareBuffer::seek_cur,sizeof(int));\
		fun(param); \
		IpcHandle->ToStream4Output(&param,ps.GetBuffer());\
		bHandled = true;\
	}

//////////////////////////////////////////////////////////////////

namespace SOUI {
	template<>
	inline SParamStream& SParamStream::operator<<(const std::string& str)
	{
		int nSize = (int)str.size();
		GetBuffer()->Write(&nSize, sizeof(int));
		GetBuffer()->Write(str.c_str(), nSize);
		return *this;
	}
	template<>
	inline SParamStream& SParamStream::operator >> (std::string& str)
	{
		int nSize = 0;
		GetBuffer()->Read(&nSize, sizeof(int));
		char* pBuf = new char[nSize];
		GetBuffer()->Read(pBuf, nSize);
		str = std::string(pBuf, nSize);
		delete[]pBuf;
		return *this;
	}

	////////////////////////////////////////////////////////////////////////
	template<>
	inline SParamStream& SParamStream::operator<<(const std::wstring& str)
	{
		int nSize = (int)str.size();
		GetBuffer()->Write(&nSize, sizeof(int));
		GetBuffer()->Write(str.c_str(), nSize * sizeof(wchar_t));
		return *this;
	}
	template<>
	inline SParamStream& SParamStream::operator >> (std::wstring& str)
	{
		int nSize = 0;
		GetBuffer()->Read(&nSize, sizeof(int));
		wchar_t* pBuf = new wchar_t[nSize];
		GetBuffer()->Read(pBuf, nSize * sizeof(wchar_t));
		str = std::wstring(pBuf, nSize);
		delete[]pBuf;
		return *this;
	}
}

struct FunParams_Base : SOUI::IFunParams
{
	virtual void ToStream4Input(SOUI::SParamStream& ps) {}
	virtual void ToStream4Output(SOUI::SParamStream& ps) {}
	virtual void FromStream4Input(SOUI::SParamStream& ps) {}
	virtual void FromStream4Output(SOUI::SParamStream& ps) {}
};

struct IClient
{
	virtual int Exit() = 0;
};

struct IServer
{
	virtual bool HandleFun(IIpcHandle * IpcHandle,UINT uMsg, SOUI::SParamStream& ps) = 0;
};

enum {
	IID_CLOSE = SOUI::FUN_ID_START,
	IID_SATRTCHECK,
	IID_CHECKRET,
	IID_REPAIR,
};

struct Param_CLOSE : FunParams_Base
{
	FUNID(IID_CLOSE)
};
struct Param_SATRTCHECK : FunParams_Base
{
	FUNID(IID_SATRTCHECK)
};
enum REPAIRRET
{	
	REPAIR_OK,
	REPAIR_FAILE
};
struct Param_REPAIR : FunParams_Base
{
	REPAIRRET repairRet;
	FUNID(IID_REPAIR)
		PARAMS1(Input, repairRet)
};

enum CHECKRET
{
	CHK_NO,
	CHK_OK,
	CHK_FAILE
};

struct Param_CHECKRET : FunParams_Base
{
	CHECKRET checkRet;
	FUNID(IID_CHECKRET)
		PARAMS1(Input, checkRet)
};
