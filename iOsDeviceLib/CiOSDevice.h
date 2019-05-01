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

//设备回调接口
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
	SStringT m_strFirmwareVersion;//固件版本
	SStringT m_strActivationState;//激活状态
	SStringT m_strDevUDID;//UDID
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
	
	//获取不耗时的信息
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
	//设备设备改变回调
	static idevice_error_t SetCallBack(IDeviceEventCallBack* relCallBack);
	//设备是否配对
	static bool IsPair(LPCSTR udid);
	//获取设备列表。不需要。直接处理callback就OK了。即使先插入的设备。在程序启动后仍然会有通知
	static bool GetiOSDeviceGUIDList(std::vector<std::string>& iosList);
private:
	static void idevice_event_cb_t(const idevice_event_t* event, void* user_data);
	std::thread m_capThread;
	std::atomic_bool m_bCap=false;
private:
	iOSDevInfo m_iosInfo;
};
