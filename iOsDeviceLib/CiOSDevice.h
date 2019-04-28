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

//�豸�ص��ӿ�
struct IDeviceEventCallBack
{
public:
	virtual void idevice_event_cb_t(const idevice_event_t* event) = NULL;
};

struct iOSDevInfo
{
	SStringT m_strDevUDID;
	SStringT m_strDevName;//�豸��
	SStringT m_strDevWiFiAddress;//wifi��ַ
	SStringT m_strDevBluetoothAddress;//������ַ
	SStringT m_strDevSerialNumber;//���к�
	SStringT m_strDevModelNumber;//�ͺ�
	SStringT m_strDevHardwareModel;//Ӳ��ģ��
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
	//��ȡ����ʱ����Ϣ
	bool GetDeviceBaseInfo();
	const iOSDevInfo& GetiOSBaseInfo();
	bool diagnostics(diagnostics_cmd_mode cmd);
protected:
	bool _GetAddress(SStringT& outAddress, LPCSTR nodename);
	
private:
	lockdownd_client_t m_client = NULL;
	idevice_t m_device = NULL;
public:
	//�豸�豸�ı�ص�
	static idevice_error_t SetCallBack(IDeviceEventCallBack* relCallBack);
	//�豸�Ƿ����
	static bool IsPair(LPCSTR udid);
	//��ȡ�豸�б�����Ҫ��ֱ�Ӵ���callback��OK�ˡ���ʹ�Ȳ�����豸���ڳ�����������Ȼ����֪ͨ
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
