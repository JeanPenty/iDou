
#pragma once
#ifndef __IOS_UTILS_HH__
#define __IOS_UTILS_HH__

#include <souistd.h>
#include <event/Events.h>
#include <dirent.h>
#include <tools/config_msvc.h>
#include <libimobiledevice/mobilebackup2.h>
#include <libimobiledevice/notification_proxy.h>
#include <libimobiledevice/afc.h>
#include <libimobiledevice/installation_proxy.h>
#include <libimobiledevice/sbservices.h>
#include <include\endianness.h>
#include <string>
#include <sstream>
#include <common/utils.h>
#include <time.h>
#include <WinSock2.h>
#include <string/tstring.h>
#include <zip.h>
#include <list>
#include <vector>
struct AppInfo
{
	std::string AppID;
	SOUI::SStringT DisplayName;
	SOUI::SStringT Version;
	uint64_t StaticDiskUsage;
	uint64_t DynamicDiskUsage;
	SOUI::CAutoRefPtr<SOUI::IBitmap> m_ico;
};

static const char* domains[] = {
	"com.apple.disk_usage",
	"com.apple.disk_usage.factory",
	"com.apple.mobile.battery",
	/* FIXME: For some reason lockdownd segfaults on this, works sometimes though
		"com.apple.mobile.debug",. */
		"com.apple.iqagent",
		"com.apple.purplebuddy",
		"com.apple.PurpleBuddy",
		"com.apple.mobile.chaperone",
		"com.apple.mobile.third_party_termination",
		"com.apple.mobile.lockdownd",
		"com.apple.mobile.lockdown_cache",
		"com.apple.xcode.developerdomain",
		"com.apple.international",
		"com.apple.mobile.data_sync",
		"com.apple.mobile.tethered_sync",
		"com.apple.mobile.mobile_application_usage",
		"com.apple.mobile.backup",
		"com.apple.mobile.nikita",
		"com.apple.mobile.restriction",
		"com.apple.mobile.user_preferences",
		"com.apple.mobile.sync_data_class",
		"com.apple.mobile.software_behavior",
		"com.apple.mobile.iTunes.SQLMusicLibraryPostProcessCommands",
		"com.apple.mobile.iTunes.accessories",
		"com.apple.mobile.internal", /**< iOS 4.0+ */
		"com.apple.mobile.wireless_lockdown", /**< iOS 4.0+ */
		"com.apple.fairplay",
		"com.apple.iTunes",
		"com.apple.mobile.iTunes.store",
		"com.apple.mobile.iTunes",
		NULL
};

const char* const batteryKey[] = {
		"BatteryCurrentCapacity",//电量
		"BatteryIsCharging",//是否在充电
		"ExternalChargeCapable",//
		"ExternalConnected",
		"FullyCharged",
		"GasGaugeCapability",
		"HasBattery"
};

enum diagnostics_cmd_mode {
	CMD_NONE = 0,
	CMD_SLEEP,
	CMD_RESTART,
	CMD_SHUTDOWN,
	CMD_DIAGNOSTICS,
	CMD_MOBILEGESTALT,
	CMD_IOREGISTRY,
	CMD_IOREGISTRY_ENTRY
};

//const char* const AddressName[] = { "BluetoothAddress","WiFiAddress" };

#define LOCK_ATTEMPTS 50
#define LOCK_WAIT 200000

#define CODE_SUCCESS 0x00
#define CODE_ERROR_LOCAL 0x06
#define CODE_ERROR_REMOTE 0x0b
#define CODE_FILE_DATA 0x0c

#define NODE_BLUETOOTH_ADDRESS "BluetoothAddress"
#define NODE_WIFI_ADDRESS "WiFiAddress"
#define NODE_SERIALNUMBER "SerialNumber"
#define NODE_MODELNUMBER "ModelNumber"
#define NODE_HARDWAREMODEL "HardwareModel"
#define NODE_DEVICENAME "DeviceName"
#define NODE_IMEI "InternationalMobileEquipmentIdentity"
#define NODE_CPUARC "CPUArchitecture"
#define NODE_PRODUCTTYPE "ProductType"
#define NODE_PRODUCTVER "ProductVersion"
#define NODE_FIRMWAREVER "FirmwareVersion"
#define NODE_ACTIVATIONSTATE "ActivationState"
#define NODE_ACTIVATIONSTATEACK "ActivationStateAcknowledged"
#define NODE_BASEBANDVER "BasebandVersion"
#define NODE_BUILDVERSION "BuildVersion"
#define NODE_ICCD "IntegratedCircuitCardIdentity"
#define NODE_MLBSN "MLBSerialNumber"
#define NODE_ECID "UniqueChipID"
#define NODE_HardwarePlatform "HardwarePlatform"
#define NODE_EthernetAddress "EthernetAddress"
#define NODE_DeviceColor "DeviceColor"
#define NODE_RegionInfo "RegionInfo"
#define NODE_DeviceEnclosureColor "DeviceEnclosureColor"
#define NODE_PhoneNumber "PhoneNumber"
//---------------------AsyncEventStart-----------------------

enum {
	EVT_SCREEN_SHOT = SOUI::EVT_EXTERNAL_BEGIN,
	EVT_UPDATA_INFO,
	EVT_UPDATABATTREY_INFO,
	EVT_UPDATADISK_INFO,
	EVT_UPDATAAPPS_INFO,
	EVT_UNINSTALL_APP,
	EVT_INSTALL_APP,
	EVT_AFC_INIT,
	EVT_UPDATA_FILES,
	EVT_OPEN_FILE_END,
	EVT_UPDATA_CONTACTS,
	EVT_END
};

struct FILEINFO
{
	std::string m_path;
	std::string m_fullpath;
	struct stat m_stat;


	FILEINFO(LPCSTR path,LPCSTR fullpath, struct stat _stat):m_path(path), m_fullpath(fullpath), m_stat(_stat){}
};

SEVENT_BEGIN(EventUpdataContacts, EVT_UPDATA_CONTACTS)
SOUI::SStringT udid;
bool bRet;
std::vector<SOUI::SStringW> contacts;
SEVENT_END()

SEVENT_BEGIN(EventOpenFileRet, EVT_OPEN_FILE_END)
std::string udid;
bool bRet;
SEVENT_END()


SEVENT_BEGIN(EventUpdataFile, EVT_UPDATA_FILES)
std::string udid;
UINT taskId;
std::list<FILEINFO> filelist;
SEVENT_END()


SEVENT_BEGIN(EventAFCInit, EVT_AFC_INIT)
std::string udid;
bool afc_sucssed;
bool afc2_sucssed;
SEVENT_END()

SEVENT_BEGIN(EventScreenShot, EVT_SCREEN_SHOT)
SOUI::SStringT udid;
char* imgbuf;
uint64_t bufsize;
int code;
SEVENT_END()

SEVENT_BEGIN(EventUpdataInfo, EVT_UPDATA_INFO)
SOUI::SStringT udid;
uint64_t BatteryCurrentCapacity;
uint8_t BatteryIsCharging;
SEVENT_END()

SEVENT_BEGIN(EventUpdataBattreyInfo, EVT_UPDATABATTREY_INFO)
SOUI::SStringT udid;
uint64_t Temperature;
uint64_t Voltage;
uint64_t Current;
SEVENT_END()

SEVENT_BEGIN(EventUpdataDiskInfo, EVT_UPDATADISK_INFO)
std::string udid;
SEVENT_END()

SEVENT_BEGIN(EventUpdataAppsInfo, EVT_UPDATAAPPS_INFO)
SOUI::SStringT udid;
std::vector<AppInfo> apps;
bool bSucessed;
SEVENT_END()

SEVENT_BEGIN(EventUninstallApp, EVT_UNINSTALL_APP)
SOUI::SStringT udid;
std::string appid;
bool bSucessed;
SEVENT_END()

SEVENT_BEGIN(EventInstallApp, EVT_INSTALL_APP)
SOUI::SStringT udid;
std::string appid;
AppInfo appinfo;
bool bSucessed;
SEVENT_END()
//---------------------AsyncEventEnd-----------------------


enum DomainsID
{
	BATTERY = 2,
};

namespace utils
{
	void productType_to_phonename(SOUI::SStringT& productType);
	void plist_print_to_stringstream(plist_t plist, std::stringstream& stream);
	SOUI::SStringT getphonecolor(SOUI::SStringT DevProductType, const SOUI::SStringT& DeviceColor,
		const SOUI::SStringT& DeviceEnclosureColor, SOUI::SStringT& outDeviceColor, SOUI::SStringT& outDeviceEnclosureColor);
	//创建一个合适的大小
	template<class T>
	SOUI::SStringT MakePrintSize(T& size)
	{
		const TCHAR* strUnit[] = {_T("B"),_T("KB"),_T("MB"),_T("GB"),_T("TB")};
		static int max =ARRAYSIZE(strUnit);
		int i = 0;		
		uint64_t tsize=1;
		while (i< max)
		{
			tsize *=1024;
			if ((size/ tsize)==0)
				break;
			++i;
		}
		tsize /= 1024;
		return SOUI::SStringT().Format(L"%0.2f%s", (float)size/ tsize, strUnit[i]);
	}
	
	SOUI::SStringT getLoc(const SOUI::SStringT& loccode);

	void getbatteryManufactureDateFormSNOld(const SOUI::SStringT& SN, SOUI::SStringT& outOrigin, SOUI::SStringT& outDate);

	void getbatteryManufactureDateFormSN(const SOUI::SStringT& SN, SOUI::SStringT& outOrigin, SOUI::SStringT& outDate);

	void getbatteryManufactureDate(LPCSTR date, SOUI::SStringT& outOrigin, SOUI::SStringT& outDate);
	bool isIp6OrLater(SOUI::SStringT DevProductType);
	bool getDevManufactureDateFormSN(const SOUI::SStringT& SN, SOUI::SStringT& outDate);


	////////////////////////////////////////////////////////////////////////////////////////////////
	char* basename(const char* path);

	zip_t* windows_open(const wchar_t* name, int flags = 0);

	int afc_upload_file(afc_client_t afc, const char* filename, const char* dstfn);

	int afc_download_file(afc_client_t afc, char** outbuf, const char* remotefile);

	int afc_download_file(afc_client_t afc, HANDLE hfile, const char* remotefile);

	int afc_get_file_size(afc_client_t afc, size_t& outsize, const char* remotefile);
	
	void afc_upload_dir(afc_client_t afc, const char* path, const char* afcpath);

	int zip_get_contents(zip* zf, const char* filename, int locate_flags, char** buffer, uint32_t* len);

	int zip_get_app_directory(zip* zf, char** path);

	int remove_directory(const char* path);

	SOUI::SStringT& FormatTime(time_t time1, SOUI::SStringT& szTime);

	struct entry {
		char* name;
		struct entry* next;
	};

	
	//
	void mobilebackup_afc_get_file_contents(afc_client_t afc, const char* filename, char** data, uint64_t* size);

	void scan_directory(const char* path, struct entry** files, struct entry** directories);

	int remove_file(const char* path);

	int win32err_to_errno(int err_value);

	int rmdir_recursive(const char* path);

	char* get_uuid();

	plist_t mobilebackup_factory_info_plist_new(const char* udid, idevice_t device, afc_client_t afc);

	int write_restore_applications(plist_t info_plist, afc_client_t afc);

	int mb2_status_check_snapshot_state(const char* path, const char* udid, const char* matches);

	void do_post_notification(idevice_t device, const char* notification);

	void print_progress_real(double progress, int flush);

	void print_progress(uint64_t current, uint64_t total);

	void mb2_set_overall_progress(double progress);

	void mb2_set_overall_progress_from_message(plist_t message, char* identifier);

	void mb2_multi_status_add_file_error(plist_t status_dict, const char* path, int error_code, const char* error_message);

	int errno_to_device_error(int errno_value);

	int mb2_handle_send_file(mobilebackup2_client_t mobilebackup2, const char* backup_dir, const char* path, plist_t* errplist);

	void mb2_handle_send_files(mobilebackup2_client_t mobilebackup2, plist_t message, const char* backup_dir);

	inline int mb2_receive_filename(mobilebackup2_client_t mobilebackup2, char** filename);

	int mb2_handle_receive_files(mobilebackup2_client_t mobilebackup2, plist_t message, const char* backup_dir);

	void mb2_handle_list_directory(mobilebackup2_client_t mobilebackup2, plist_t message, const char* backup_dir);

	void mb2_handle_make_directory(mobilebackup2_client_t mobilebackup2, plist_t message, const char* backup_dir);

	void mb2_copy_file_by_path(const char* src, const char* dst);

	void mb2_copy_directory_by_path(const char* src, const char* dst);
}

#endif