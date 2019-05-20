#pragma once

#include <string>
#include <vector>
#include <set>
#include <string/tstring.h>
#include "iOSUtils.h"
#include <thread>
#include <atomic>
#include <condition_variable> 

#include "CRWLocker.h"
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

struct AppInfo
{
	std::string AppID;
	SStringT DisplayName;
	SStringT Version;
	uint64_t StaticDiskUsage;
	uint64_t DynamicDiskUsage;
	SOUI::CAutoRefPtr<SOUI::IBitmap> m_ico;
};

struct BatteryBaseInfo
{
	int CycleCount;
	int DesignCapacity;
	//int FullChargeCapacity;
	SStringT BatterySerialNumber;
	uint64_t NominalChargeCapacity;
	uint64_t BootVoltage;
	SStringT ManufactureDate;
	SStringT Origin;
};

struct DiskInfo
{
	std::atomic_bool bReady = false;
	//com.apple.disk_usage
	uint64_t TotalDataAvailable;
	uint64_t TotalDataCapacity;
	uint64_t TotalDiskCapacity;
	uint64_t TotalSystemAvailable;
	uint64_t TotalSystemCapacity;
	uint64_t PhotoUsage;
	uint64_t AppUsage;
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
	SStringT m_strECID;
	SStringT m_strHardwarePlatform;
	SStringT m_strEthernetAddress;
	SStringT m_strDeviceColor;
	SStringT m_strDeviceEnclosureColor;
	SStringT m_strRegionInfo;
	SStringT m_strPhoneNum;
	BatteryBaseInfo m_sGasGauge;
	DiskInfo m_diskInfo;
	bool	m_bIsJailreak;
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
	void StartUpdata();
	void StartUpdataDiskInfo();
	void StartCapshot();
	void StartUpdataBatteryInfo();
	void StopUpdataBatteryInfo();
	const iOSDevInfo& GetiOSBaseInfo();
	bool DoCmd(diagnostics_cmd_mode cmd);
	void GetBatteryBaseInfo(BatteryBaseInfo& outasGauge);
	void StartUpdataApps();
	const std::vector<AppInfo>* GetApps();
	void IntallApp(LPCWSTR appfile);
	void uninstallstatus_cb(plist_t command, plist_t status);
	void installstatus_cb(plist_t command, plist_t status);
	void UninstallApp(LPCSTR appID);
protected:
	bool _GetAppIcon(LPCSTR id, char** outBuf, uint64_t& len);
	bool _GetScreenWallpaper(char** outBuf, uint64_t& len);
	void _UpdataApps(plist_t apps);
	bool _GetAddress(SStringT& outAddress, LPCSTR nodename);
	bool _GetDiskAddress(lockdownd_client_t client, DiskInfo& outAddress);
	bool _GetBatteryBaseInfo(BatteryBaseInfo & outasGauge);
	void _GetTypeFromProductName();
	void _GetDiskInfo();
	bool _IsJailreak();
	void _ScreenShot();
	void _Updata();
	void _Backup(LPCSTR pDir);
	void _UpdataBatteryInfo();
	void _UpdataDiskInfo();
	void _UpdataAppsInfo();
	void _UninstallApp(std::string appID);
	void _InstallApp(const std::wstring apppath, bool bInstall);	
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
	enum {
		Thread_Cap = 0,
		Thread_Updata = 1,
		Thread_UpdataBattreyInfo = 2,
		Thread_UpdataDiskInfo=3,
		Thread_UpdataAppsInfo = 4,
		Thread_UninstallApp =5,
		Thread_InstallApp = 6,
		Thread_end
	};
	std::vector<AppInfo> m_apps;

	//static void idevice_event_cb_t(const idevice_event_t* event, void* user_data);
	std::thread m_workThread[Thread_end];
	std::atomic_bool m_bCap=false;
	std::atomic_bool m_bUpdata = false;
	std::atomic_bool m_bUpdataBattreyInfo = false;
	std::atomic_bool m_bUpdataDiskInfo = false;
	//app�б��д����
	WfirstRWLock m_appsLocker;
	std::condition_variable appUnistallcv;
	std::condition_variable appIstallcv;
private:
	iOSDevInfo m_iosInfo;
};
