#include "stdafx.h"
#include "Cafctask.h"
#include "iOsDeviceLib/CiOSDevice.h"
#include "CDataCenter.h"
#include <shlwapi.h>


Cafctask* Cafctask::ms_Singleton = nullptr;

CAFCThread::CAFCThread(SComMgr& comMgr, LPCSTR udid) :m_udid(udid)
{
	SASSERT(udid);
	comMgr.CreateTaskLoop((IObjRef * *)& m_pTaskLoop);
	m_pTaskLoop->start("afc_taskloop", ITaskLoop::Low);
	STaskHelper::post(m_pTaskLoop, this, &CAFCThread::_InitAFC, false);
}

CAFCThread::~CAFCThread()
{
	m_pTaskLoop->stop();
	if (m_afc)
		CiOSDevice::FreeAFCClient(m_afc);
	if (m_afc2)
		CiOSDevice::FreeAFCClient(m_afc2);
}

Cafctask::Cafctask()
{
}

Cafctask::~Cafctask()
{
	for (auto& ite : m_threadPool)
	{
		delete ite.second;
	}
	m_threadPool.clear();
}

void Cafctask::AddDev(LPCSTR udid)
{
	auto iter = m_threadPool.find(udid);
	if (iter == m_threadPool.end())
	{
		m_threadPool[udid] = new CAFCThread(m_comMgr, udid);
	}
}

void Cafctask::RemoveDev(LPCSTR udid)
{
	auto ite = m_threadPool.find(udid);
	if (ite != m_threadPool.end())
	{
		delete ite->second;
		m_threadPool.erase(ite);
	}
}

UINT CreateID()
{
	static UINT id = 0;
	return id++;
}

UINT Cafctask::Updata(LPCSTR udid, LPCSTR path, bool bAfc2)
{
	auto iter = m_threadPool.find(udid);
	SASSERT(iter != m_threadPool.end());
	int taskid = CreateID();
	iter->second->Updata(taskid, path, bAfc2);
	return taskid;
}

void Cafctask::OpenFile(LPCSTR udid, LPCSTR path, bool bAfc2)
{
	auto iter = m_threadPool.find(udid);
	SASSERT(iter != m_threadPool.end());
	
	iter->second->OpenFile(path, bAfc2);
}

void CAFCThread::Updata(int id, LPCSTR path, bool bAfc2)
{
	SASSERT(path);
	STaskHelper::post(m_pTaskLoop, this, &CAFCThread::_Updata, id, std::string(path), bAfc2, false);
}

void CAFCThread::_Updata(int id, std::string path,bool bAfc2)
{
	afc_client_t afc = bAfc2 ? m_afc2 : m_afc;

	SASSERT(afc);

	EventUpdataFile* e = new EventUpdataFile(NULL);
	e->udid = m_udid;
	e->taskId = id;

	char source_filename[512];
	strcpy(source_filename, path.c_str());
	if (source_filename[strlen(source_filename) - 1] != '/') {
		strcat(source_filename, "/");
	}
	int device_directory_length = strlen(source_filename);
	char** dirinfo = NULL;

	if (afc_read_directory(afc, path.c_str(), &dirinfo) == AFC_E_SUCCESS)
	{
		for (int k = 0; dirinfo[k]; k++) {
			if (!strcmp(dirinfo[k], ".") || !strcmp(dirinfo[k], "..")) {
				continue;
			}

			char** fileinfo = NULL;
			strcpy(((char*)source_filename) + device_directory_length, dirinfo[k]);
			afc_get_file_info(afc, source_filename, &fileinfo);
			if (!fileinfo) {				
				continue;
			}
			struct stat stbuf;
			stbuf.st_size = 0;
			for (int i = 0; fileinfo[i]; i += 2) {
				if (!strcmp(fileinfo[i], "st_size")) {
					stbuf.st_size = atoll(fileinfo[i + 1]);
				}
				else if (!strcmp(fileinfo[i], "st_ifmt")) {
					if (!strcmp(fileinfo[i + 1], "S_IFREG")) {
						stbuf.st_mode = S_IFREG;
					}
					else if (!strcmp(fileinfo[i + 1], "S_IFDIR")) {
						stbuf.st_mode = S_IFDIR;
					}
					else if (!strcmp(fileinfo[i + 1], "S_IFLNK")) {
						stbuf.st_mode = S_IFLNK;
					}
					else if (!strcmp(fileinfo[i + 1], "S_IFBLK")) {
						stbuf.st_mode = S_IFBLK;
					}
					else if (!strcmp(fileinfo[i + 1], "S_IFCHR")) {
						stbuf.st_mode = S_IFCHR;
					}
					else if (!strcmp(fileinfo[i + 1], "S_IFIFO")) {
						stbuf.st_mode = S_IFIFO;
					}
					else if (!strcmp(fileinfo[i + 1], "S_IFSOCK")) {
						stbuf.st_mode = S_IFSOCK;
					}
				}
				else if (!strcmp(fileinfo[i], "st_nlink")) {
					stbuf.st_nlink = atoi(fileinfo[i + 1]);
				}
				else if (!strcmp(fileinfo[i], "st_mtime")) {
					stbuf.st_mtime = (time_t)(atoll(fileinfo[i + 1]) / 1000000000);
				}
				else if (!strcmp(fileinfo[i], "LinkTarget")) {
				}
			}
			e->filelist.push_back(FILEINFO(dirinfo[k], source_filename,stbuf));
			afc_dictionary_free(fileinfo);
		}
	}
	afc_dictionary_free(dirinfo);
	SNotifyCenter::getSingleton().FireEventAsync(e);
	e->Release();
}

void CAFCThread::OpenFile(LPCSTR path, bool bAfc2)
{
	SASSERT(path);
	STaskHelper::post(m_pTaskLoop, this, &CAFCThread::_OpenFile, std::string(path), bAfc2, false);
}

//void CAFCThread::_Updata2(int id, std::string path)
//{
//	SASSERT(m_afc2);
//
//	EventUpdataFile* e = new EventUpdataFile(NULL);
//	e->udid = m_udid;
//	e->taskId = id;
//
//	char source_filename[512];
//	strcpy(source_filename, path.c_str());
//	if (source_filename[strlen(source_filename) - 1] != '/') {
//		strcat(source_filename, "/");
//	}
//	int device_directory_length = strlen(source_filename);
//	char** dirinfo = NULL;
//
//	if (afc_read_directory(m_afc2, path.c_str(), &dirinfo) == AFC_E_SUCCESS)
//	{
//		for (int k = 0; dirinfo[k]; k++) {
//			if (!strcmp(dirinfo[k], ".") || !strcmp(dirinfo[k], "..")) {
//				continue;
//			}
//
//			char** fileinfo = NULL;
//			strcpy(((char*)source_filename) + device_directory_length, dirinfo[k]);
//			afc_get_file_info(m_afc2, source_filename, &fileinfo);
//			if (!fileinfo) {
//				continue;
//			}
//			struct stat stbuf;
//			stbuf.st_size = 0;
//			bool isLink=false;
//			std::string linkfullpath;
//			for (int i = 0; fileinfo[i]; i += 2) {
//				if (!strcmp(fileinfo[i], "st_size")) {
//					stbuf.st_size = atoll(fileinfo[i + 1]);
//				}
//				else if (!strcmp(fileinfo[i], "st_ifmt")) {
//					if (!strcmp(fileinfo[i + 1], "S_IFREG")) {
//						stbuf.st_mode = S_IFREG;
//					}
//					else if (!strcmp(fileinfo[i + 1], "S_IFDIR")) {
//						stbuf.st_mode = S_IFDIR;
//					}
//					else if (!strcmp(fileinfo[i + 1], "S_IFLNK")) {
//						stbuf.st_mode = S_IFLNK;
//					}
//					else if (!strcmp(fileinfo[i + 1], "S_IFBLK")) {
//						stbuf.st_mode = S_IFBLK;
//					}
//					else if (!strcmp(fileinfo[i + 1], "S_IFCHR")) {
//						stbuf.st_mode = S_IFCHR;
//					}
//					else if (!strcmp(fileinfo[i + 1], "S_IFIFO")) {
//						stbuf.st_mode = S_IFIFO;
//					}
//					else if (!strcmp(fileinfo[i + 1], "S_IFSOCK")) {
//						stbuf.st_mode = S_IFSOCK;
//					}
//				}
//				else if (!strcmp(fileinfo[i], "st_nlink")) {
//					stbuf.st_nlink = atoi(fileinfo[i + 1]);
//				}
//				else if (!strcmp(fileinfo[i], "st_mtime")) {
//					stbuf.st_mtime = (time_t)(atoll(fileinfo[i + 1]) / 1000000000);
//				}
//				else if (!strcmp(fileinfo[i], "LinkTarget")) {
//					isLink = true;
//					linkfullpath = fileinfo[i + 1];					
//				}
//			}
//			if(isLink)
//				e->filelist.push_back(FILEINFO(dirinfo[k], linkfullpath.c_str(), stbuf));
//			else
//				e->filelist.push_back(FILEINFO(dirinfo[k], source_filename, stbuf));
//			afc_dictionary_free(fileinfo);
//		}
//	}
//	afc_dictionary_free(dirinfo);
//	SNotifyCenter::getSingleton().FireEventAsync(e);
//	e->Release();
//}

void CAFCThread::_OpenFile(std::string path, bool bAfc2)
{
	afc_client_t afc = bAfc2 ? m_afc2 : m_afc;
	SASSERT(afc);
	HANDLE hFile;

	TCHAR szPathName[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	wcscpy_s(szFileName,S_CA2W(path.c_str(), CP_UTF8));
	::PathStripPath(szFileName);
	//获得临时文件目录
	if (!::GetTempPath(MAX_PATH, szPathName))
	{
		return;
	}
	SStringT filepath= szPathName;
	filepath += szFileName;
	hFile = CreateFile(filepath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;
	if (utils::afc_download_file(afc, hFile, path.c_str()) == 0)
	{
		CloseHandle(hFile);
		ShellExecute(NULL, L"open", filepath, NULL, NULL, SW_SHOWNORMAL);
	}
	else
		CloseHandle(hFile);
	EventOpenFileRet* e = new EventOpenFileRet(NULL);
	SNotifyCenter::getSingleton().FireEventAsync(e);
	e->Release();
}

void CAFCThread::_InitAFC()
{
	CiOSDevice* dev = CDataCenter::getSingleton().GetDevByUDID(m_udid.c_str());
	if (dev)
	{
		EventAFCInit* e = new EventAFCInit(NULL);
		e->udid = m_udid;
		e->afc_sucssed = dev->CreateAFCClient(m_afc);
		e->afc2_sucssed = false;
		if (dev->GetiOSBaseInfo().m_bIsJailreak)
		{
			e->afc2_sucssed = dev->CreateAFC2Client(m_afc2);
		}
		SNotifyCenter::getSingleton().FireEventAsync(e);
		e->Release();
	}
}
