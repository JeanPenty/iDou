#pragma once

#include <string>
#include <vector>
#include <set>
#include <string/tstring.h>
#include "iOSUtils.h"
#include <thread>
#include <atomic>

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

struct GasGauge
{
	int CycleCount;
	int DesignCapacity;
	int FullChargeCapacity;
};

struct DiskInfo
{
	//com.apple.disk_usage
	uint64_t TotalDataAvailable;
	uint64_t TotalDataCapacity;
	uint64_t TotalDiskCapacity;
	uint64_t TotalSystemAvailable;
	uint64_t TotalSystemCapacity;
};

enum DevType
{
	Type_iPhone=0,
	Type_iPad=1,
};

struct iOSDevInfo
{
	DevType	 m_type;
	SStringT m_strFirmwareVersion;//�̼��汾
	SStringT m_strActivationState;//����״̬
	SStringT m_strDevUDID;//UDID
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
	SStringT m_strProductVersion;
	SStringT m_strBuildVersion;
	SStringT m_strICCD;
	SStringT m_strMLBSerialNumber;
	SStringT m_strUniqueChipID;
	SStringT m_strHardwarePlatform;
	SStringT m_strEthernetAddress;
	SStringT m_strDeviceColor;
	SStringT m_strRegionInfo;
	GasGauge m_sGasGauge;
	DiskInfo m_diskInfo;
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
	bool DoCmd(diagnostics_cmd_mode cmd);
	void GetGasGauge(GasGauge& outasGauge);
protected:
	bool _GetAddress(SStringT& outAddress, LPCSTR nodename);
	bool _GetDiskAddress(uint64_t& outAddress, LPCSTR nodename);
	bool _GetGasGauge(GasGauge & outasGauge);
	void _GetTypeFromProductName();
	void _GetDiskInfo();
private:
	void ScreenShot();
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
	std::thread m_capThread;
	std::atomic_bool m_bCap=false;
private:
	iOSDevInfo m_iosInfo;
};
