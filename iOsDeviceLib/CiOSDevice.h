#pragma once

#include <string>
#include <vector>
#include <set>
#include <string/tstring.h>
#include "iOSUtils.h"
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

struct iOSDevInfo
{
	SStringT m_strDevUDID;
	SStringT m_strDevName;//设备名
	SStringT m_strDevWiFiAddress;//wifi地址
	SStringT m_strDevBluetoothAddress;//蓝牙地址
	SStringT m_strDevSerialNumber;//序列号
	SStringT m_strDevModelNumber;//型号
	SStringT m_strDevHardwareModel;//硬件模型
	SStringT m_strDevIMEI;
	SStringT m_strDevCpuarc;
	SStringT m_strDevProductType;
	SStringT m_strDevProductName;
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
	bool GetWifiAddress(SStringT& outMac);
	bool GetBluetoothAddress(SStringT& outMac);
	bool SetDevName(LPCTSTR newName);
	template<class T>
	bool GetBattery(LPCSTR key, T& outValue);
	//获取不耗时的信息
	bool GetDeviceBaseInfo();
	const iOSDevInfo& GetiOSBaseInfo();
	bool diagnostics(diagnostics_cmd_mode cmd);
protected:
	bool _GetAddress(SStringT& outAddress, LPCSTR nodename);
	
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
	iOSDevInfo m_iosInfo;
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
