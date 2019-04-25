#pragma once

#include <string>
#include <vector>
#include <set>
#include <string/tstring.h>
using SOUI::SStringT;
EXTERN_C{
#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <common/userpref.h>
#pragma comment(lib,"imobiledevice.lib")
#pragma comment(lib,"plist.lib")
#pragma comment(lib,"usbmuxd.lib")
}

//设备回调接口
struct IDeviceEventCallBack
{
public:
	virtual void idevice_event_cb_t(const idevice_event_t* event) = NULL;
};

class CiOSDevice
{
public:
	CiOSDevice();
	~CiOSDevice();
	bool OpenDevice(LPCSTR udid);
	void CloseDevice();
	bool IsOpen();
	bool GetDevName(SStringT& outName);
	bool SetDevName(LPCTSTR newName);
	template<class T>
	bool GetBattery(LPCSTR key, T& outValue);	
	bool GetDeviceInfo();

private:
	lockdownd_client_t m_client = NULL;
	idevice_t m_device = NULL;
public:	
	//设备设备改变回调
	static idevice_error_t SetCallBack(IDeviceEventCallBack* relCallBack);
	//设备是否配对
	static bool IsPair(LPCSTR udid);
	//获取设备列表。不需要。直接处理callback就OK了。即使先插入的设备。在程序启动后仍然会有通知
	static bool GetiOSDeviceGUIDList(std::vector<std::string>& iosList);
private:
	static void idevice_event_cb_t(const idevice_event_t* event, void* user_data);

private:
	SStringT m_strDevName;
};

template<class T>
bool CiOSDevice::GetBattery(LPCSTR key, T& outValue)
{
	if (!IsOpen())
		return false;
	plist_t node = NULL;
	std::stringstream out;
	if (lockdownd_get_value(m_client, domains[BATTERY], key, &node) == LOCKDOWN_E_SUCCESS) {
		if (node) {
			utils::plist_print_to_stringstream(node, out);
			plist_free(node);
			node = NULL;
		}
	}
	else
	{
		return false;
	}
	out >> outValue;
	return true;
}
