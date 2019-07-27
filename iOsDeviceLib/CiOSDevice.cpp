#include "CiOSDevice.h"
#include "CiOSDevice.h"
#include "CiOSDevice.h"
#include "pch.h"
#include "CiOSDevice.h"
#include <zip.h>
#include <libimobiledevice\include\asprintf.h>
#include <common/utils.c>
#include "iOSUtils.h"
#include <string/strcpcvt.h>
#include "libimobiledevice/diagnostics_relay.h"
#include <souistd.h>
#include <libimobiledevice/screenshotr.h>
#include <libimobiledevice/mobilebackup2.h>
#include <libimobiledevice/notification_proxy.h>
#include <libimobiledevice/afc.h>
#include <libimobiledevice/installation_proxy.h>
#include <libimobiledevice/sbservices.h>
#include <event/NotifyCenter.h>
#include <libimobiledevice/installation_proxy.h>
#include <libimobiledevice/src/mobilesync.h>

EXTERN_C{
#include "base64.h"
}

/////////////////////////////////////////////
static void notifier(const char* notification, void* unused)
{
	if (strlen(notification) == 0) {
		return;
	}
	CiOSDevice* ios = (CiOSDevice*)unused;

	ios->app_notifier_fun(notification);
}

void GetAppValue(plist_t appnode, LPCSTR nodename, std::string& outValue)
{
	SASSERT(appnode);
	SASSERT(nodename);
	plist_t DisplayNameNode = plist_dict_get_item(appnode, nodename);
	if (DisplayNameNode)
	{
		char* strDisplayName = NULL;
		plist_get_string_val(DisplayNameNode, &strDisplayName);
		if (strDisplayName)
		{
			outValue = strDisplayName;
			free(strDisplayName);
		}
	}
}

void GetAppValue(plist_t appnode, LPCSTR nodename, SOUI::SStringT& outValue)
{
	SASSERT(appnode);
	SASSERT(nodename);
	plist_t DisplayNameNode = plist_dict_get_item(appnode, nodename);
	if (DisplayNameNode)
	{
		char* strDisplayName = NULL;
		plist_get_string_val(DisplayNameNode, &strDisplayName);
		if (strDisplayName)
		{
			outValue = SOUI::S_CA2W(strDisplayName, CP_UTF8);
			free(strDisplayName);
		}
	}
}

void GetAppValue(plist_t appnode, LPCSTR nodename, uint64_t& outValue)
{
	SASSERT(appnode);
	SASSERT(nodename);
	plist_t DisplayNameNode = plist_dict_get_item(appnode, nodename);
	if (DisplayNameNode)
	{
		plist_get_uint_val(DisplayNameNode, &outValue);
	}
}

void GetAppValue(plist_t appnode, LPCSTR nodename, char* outValue, uint64_t& datalen)
{
	SASSERT(appnode);
	SASSERT(nodename);
	plist_t DisplayNameNode = plist_dict_get_item(appnode, nodename);
	if (DisplayNameNode)
	{
		plist_get_data_val(DisplayNameNode, &outValue, &datalen);
	}
}

void syslog_callback(char c, void* user_data)
{
	putchar(c);
	if (c == '\n') {
		fflush(stdout);
	}
}

bool _GetAddress(lockdownd_client_t client, SOUI::SStringA& outAddress, LPCSTR nodename)
{
	plist_t address_node = NULL;
	if (LOCKDOWN_E_SUCCESS == lockdownd_get_value(client, NULL, nodename, &address_node))
	{
		char* node_value = NULL;
		plist_type type = plist_get_node_type(address_node);
		switch (type)
		{
			case PLIST_STRING:
				plist_get_string_val(address_node, &node_value); break;
			case PLIST_BOOLEAN:
				{
					uint8_t boolvalue;
					plist_get_bool_val(address_node, &boolvalue);
					node_value = (char*)malloc(10);
					strcpy_s(node_value, 10, boolvalue ? "true" : "false");
				}break;
			case PLIST_ARRAY:
				{
					plist_array_iter iter = NULL;
					plist_array_new_iter(address_node, &iter);
					while (iter)
					{
						plist_t item = NULL;
						plist_array_next_item(address_node, iter, &item);
						plist_type itemtype = plist_get_node_type(item);
						if (PLIST_DICT == plist_get_node_type(item))
						{
							plist_dict_iter iter2 = NULL;
							plist_dict_new_iter(item, &iter2);
							plist_t val = NULL;
							do {
								char* key = NULL;
								//plist_dict_get_item_key(item, &key);
								plist_dict_next_item(item, iter2, &key, &val);
								if (PLIST_STRING == plist_get_node_type(val))
								{
									char* item_val = NULL;
									plist_get_string_val(val, &item_val);
									if (item_val)
										free(item_val);
								}
								if (key)
									free(key);

							} while (val);
						}
					}

				}break;
		}
		plist_free(address_node);
		address_node = NULL;
		if (node_value) {
			using namespace SOUI;
			outAddress = node_value;
			free(node_value);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void idevice_event_cb(const idevice_event_t* event, void* user_data)
{
	IDeviceEventCallBack* reCallBack = (IDeviceEventCallBack*)user_data;
	if (reCallBack)
		reCallBack->idevice_event_cb_t(event);
}

bool CheckError(lockdownd_error_t err)
{
	bool bRet = true;
	switch (err) {
		case LOCKDOWN_E_SUCCESS:
			break;
		case LOCKDOWN_E_PASSWORD_PROTECTED:
			//("ERROR: Could not validate with device %s because a passcode is set. Please enter the passcode on the device and retry.\n", udid);
			break;
		case LOCKDOWN_E_INVALID_CONF:
		case LOCKDOWN_E_INVALID_HOST_ID:
			//("ERROR: Device %s is not paired with this host\n", udid);
			break;
		case LOCKDOWN_E_PAIRING_DIALOG_RESPONSE_PENDING:
			//("ERROR: Please accept the trust dialog on the screen of device %s, then attempt to pair again.\n", udid);
			break;
		case LOCKDOWN_E_USER_DENIED_PAIRING:
			//("ERROR: Device %s said that the user denied the trust dialog.\n", udid);
			break;
		default:
			//("ERROR: Device %s returned unhandled error code %d\n", udid, err);
			break;
	}
	return bRet;
}

void notify_cb(const char* notification, void* userdata)
{
	if (strlen(notification) == 0) {
		return;
	}
	if (!strcmp(notification, NP_SYNC_CANCEL_REQUEST)) {
		//PRINT_VERBOSE(1, "User has cancelled the backup process on the device.\n");
		//quit_flag++;
	}
	else if (!strcmp(notification, NP_BACKUP_DOMAIN_CHANGED)) {
		//backup_domain_changed = 1;
	}
	else {
		//PRINT_VERBOSE(1, "Unhandled notification '%s' (TODO: implement)\n", notification);
	}
}

typedef struct __stat64 stat_generic_t;

static int stat_generic(const char* filename, stat_generic_t* stat_gen)
{
	return __stat64(filename, stat_gen);
}

static int stat_generic(const wchar_t* filename, stat_generic_t* stat_gen)
{
	return _wstat64(filename, stat_gen);
}

static void uninstallstatus_cb(plist_t command, plist_t status, void* unused)
{
	((CiOSDevice*)unused)->uninstallstatus_cb(command, status);
}

/////////////////////////////////////////////


CiOSDevice::CiOSDevice()
{
}

CiOSDevice::~CiOSDevice()
{
	CloseDevice();
}

bool CiOSDevice::OpenDevice(LPCSTR udid)
{
	if (udid)
	{
		if (idevice_new(&m_device, udid) == IDEVICE_E_SUCCESS)
		{
			m_iosInfo.m_strDevUDID = SOUI::S_CA2T(udid, CP_UTF8).MakeUpper();
			if (lockdownd_client_new_with_handshake(m_device, &m_client, "my_iOSDevice") == LOCKDOWN_E_SUCCESS)
			{
				_InitCallBack();
				_InitInstproxy();
				return true;
			}
		}
	}
	return false;
}

void CiOSDevice::CloseDevice()
{
	m_bCap = false;
	m_bUpdata = false;
	m_bUpdataBattreyInfo = false;
	m_bUpdataDiskInfo = false;

	StopLogging();

	for (auto& th : m_workThread)
	{
		if (th.joinable())
			th.join();
	}

	if (m_npClient)
		np_client_free(m_npClient);

	if (m_instproxy)
		instproxy_client_free(m_instproxy);

	if (m_client)
	{
		lockdownd_client_free(m_client);
		m_client = NULL;
	}

	if (m_device)
	{
		idevice_free(m_device);
		m_device = NULL;
	}
}

bool CiOSDevice::IsOpen()
{
	return m_device && m_client;
}

bool CiOSDevice::GetDevName(SStringT& outName)
{
	if (m_iosInfo.m_strDevName.IsEmpty())
	{
		char* name = NULL;
		lockdownd_get_device_name(m_client, &name);
		if (name) {
			using namespace SOUI;
			m_iosInfo.m_strDevName = S_CA2T(name, CP_UTF8);
			free(name);
		}
		else
			return false;
	}
	outName = m_iosInfo.m_strDevName;
	return true;
}

bool CiOSDevice::_GetAddress(SStringT& outAddress, LPCSTR nodename)
{
	plist_t address_node = NULL;
	if (LOCKDOWN_E_SUCCESS == lockdownd_get_value(m_client, NULL, nodename, &address_node))
	{
		char* node_value = NULL;

		plist_type type = plist_get_node_type(address_node);
		switch (type)
		{
			case PLIST_STRING:
				plist_get_string_val(address_node, &node_value); break;
			case PLIST_BOOLEAN:
				{
					uint8_t boolvalue;
					plist_get_bool_val(address_node, &boolvalue);
					node_value = (char*)malloc(10);
					strcpy_s(node_value, 10, boolvalue ? "true" : "false");
				}break;
			case PLIST_UINT:
				{
					uint64_t u64value;
					plist_get_uint_val(address_node, &u64value);
					node_value = (char*)malloc(66);
					sprintf_s(node_value, 66, "%llX", u64value);
				}break;
		}
		plist_free(address_node);
		address_node = NULL;
		if (node_value) {
			using namespace SOUI;
			outAddress = S_CA2T(node_value, CP_UTF8);
			free(node_value);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CiOSDevice::_GetDiskAddress(lockdownd_client_t client, DiskInfo& outAddress)
{
	plist_t address_node = NULL;
	//outAddress.bReady = false;
	if (LOCKDOWN_E_SUCCESS == lockdownd_get_value(client, domains[1], NULL, &address_node))
	{
		//outAddress.bReady = true;

		//_GetDiskAddress(m_iosInfo.m_diskInfo.TotalDiskCapacity, "TotalDiskCapacity");
		plist_t TotalDiskCapacityNode = plist_dict_get_item(address_node, "TotalDiskCapacity");
		if (TotalDiskCapacityNode)
			plist_get_uint_val(TotalDiskCapacityNode, &outAddress.TotalDiskCapacity);

		//_GetDiskAddress(m_iosInfo.m_diskInfo.TotalDataAvailable, "TotalDataAvailable");
		plist_t TotalDataAvailableNode = plist_dict_get_item(address_node, "TotalDataAvailable");
		if (TotalDataAvailableNode)
			plist_get_uint_val(TotalDataAvailableNode, &outAddress.TotalDataAvailable);
		//_GetDiskAddress(m_iosInfo.m_diskInfo.TotalDataCapacity, "TotalDataCapacity");
		plist_t TotalDataCapacityNode = plist_dict_get_item(address_node, "TotalDataCapacity");
		if (TotalDataCapacityNode)
			plist_get_uint_val(TotalDataCapacityNode, &outAddress.TotalDataCapacity);
		//_GetDiskAddress(m_iosInfo.m_diskInfo.TotalSystemCapacity, "TotalSystemCapacity");
		plist_t TotalSystemCapacityNode = plist_dict_get_item(address_node, "TotalSystemCapacity");
		if (TotalSystemCapacityNode)
			plist_get_uint_val(TotalSystemCapacityNode, &outAddress.TotalSystemCapacity);
		plist_t TotalSystemAvailableNode = plist_dict_get_item(address_node, "TotalSystemAvailable");
		if (TotalSystemAvailableNode)
			plist_get_uint_val(TotalSystemAvailableNode, &outAddress.TotalSystemAvailable);
		plist_free(address_node);
		address_node = NULL;
		return true;
	}
	return false;
}

bool CiOSDevice::GetWifiAddress(SStringT& outMac)
{
	if (m_iosInfo.m_strDevWiFiAddress.IsEmpty())
	{
		if (!_GetAddress(m_iosInfo.m_strDevWiFiAddress, NODE_WIFI_ADDRESS))
			return false;
	}
	outMac = m_iosInfo.m_strDevWiFiAddress;
	return true;
}

bool CiOSDevice::GetBluetoothAddress(SStringT& outMac)
{
	if (m_iosInfo.m_strDevBluetoothAddress.IsEmpty())
	{
		if (!_GetAddress(m_iosInfo.m_strDevBluetoothAddress, NODE_BLUETOOTH_ADDRESS))
			return false;
	}
	outMac = m_iosInfo.m_strDevBluetoothAddress;
	return true;
}

bool CiOSDevice::SetDevName(LPCTSTR newName)
{
	if (newName && (m_iosInfo.m_strDevName != newName))
	{
		using namespace SOUI;
		SStringA strU8Name = S_CT2A(newName, CP_UTF8);
		lockdownd_error_t ec = lockdownd_set_value(m_client, NULL, "DeviceName", plist_new_string(strU8Name));
		if (lockdownd_set_value(m_client, NULL, "DeviceName", plist_new_string(strU8Name)) == LOCKDOWN_E_SUCCESS) {
			m_iosInfo.m_strDevName = newName;
			return true;
		}
	}
	return false;
}

void CiOSDevice::_GetTypeFromProductName()
{
	m_iosInfo.m_type = Type_iPhone;

	SStringT dev = m_iosInfo.m_strDevProductName;
	dev.MakeLower();
	if (dev.Find(L"iphone") != -1)
	{
		m_iosInfo.m_type = Type_iPhone;
		return;
	}
	if (dev.Find(L"ipad") != -1)
	{
		m_iosInfo.m_type = Type_iPad;
		return;
	}
}

void CiOSDevice::_GetDiskInfo()
{
	/**/
	_GetDiskAddress(m_client, m_iosInfo.m_diskInfo);
}

bool CiOSDevice::_IsJailreak()
{
	lockdownd_service_descriptor_t service = NULL;
	lockdownd_error_t er = lockdownd_start_service_with_escrow_bag(m_client, "com.apple.afc2", &service);
	if (service)
	{
		lockdownd_service_descriptor_free(service);
	}
	return (er == LOCKDOWN_E_SUCCESS);
}

bool CiOSDevice::GetDeviceBaseInfo()
{
	if (!IsOpen())
		return false;

	//设备名
	_GetAddress(m_iosInfo.m_strDevName, NODE_DEVICENAME);

	_GetAddress(m_iosInfo.m_strFirmwareVersion, NODE_FIRMWAREVER);

	_GetAddress(m_iosInfo.m_strActivationState, NODE_ACTIVATIONSTATE);

	_GetAddress(m_iosInfo.m_strProductVersion, NODE_PRODUCTVER);
	_GetAddress(m_iosInfo.m_strBuildVersion, NODE_BUILDVERSION);
	_GetAddress(m_iosInfo.m_strICCD, NODE_ICCD);

	_GetAddress(m_iosInfo.m_strMLBSerialNumber, NODE_MLBSN);
	_GetAddress(m_iosInfo.m_strECID, NODE_ECID);
	_GetAddress(m_iosInfo.m_strHardwarePlatform, NODE_HardwarePlatform);
	_GetAddress(m_iosInfo.m_strEthernetAddress, NODE_EthernetAddress);
	_GetAddress(m_iosInfo.m_strDeviceColor, NODE_DeviceColor);
	_GetAddress(m_iosInfo.m_strDeviceEnclosureColor, NODE_DeviceEnclosureColor);
	_GetAddress(m_iosInfo.m_strRegionInfo, NODE_RegionInfo);
	_GetAddress(m_iosInfo.m_strPhoneNum, NODE_PhoneNumber);
	//wifi address
	_GetAddress(m_iosInfo.m_strDevWiFiAddress, NODE_WIFI_ADDRESS);
	m_iosInfo.m_strDevWiFiAddress = m_iosInfo.m_strDevWiFiAddress.MakeUpper();
	//蓝牙地址
	_GetAddress(m_iosInfo.m_strDevBluetoothAddress, NODE_BLUETOOTH_ADDRESS);
	m_iosInfo.m_strDevBluetoothAddress = m_iosInfo.m_strDevBluetoothAddress.MakeUpper();
	//序列号
	_GetAddress(m_iosInfo.m_strDevSerialNumber, NODE_SERIALNUMBER);
	//型号
	_GetAddress(m_iosInfo.m_strDevModelNumber, NODE_MODELNUMBER);
	//硬件模型
	_GetAddress(m_iosInfo.m_strDevHardwareModel, NODE_HARDWAREMODEL);
	//IMEI
	_GetAddress(m_iosInfo.m_strDevIMEI, NODE_IMEI);
	if (m_iosInfo.m_strDevIMEI.IsEmpty())
		m_iosInfo.m_strDevIMEI = GETSTRING(L"@string/cantread");
	//CPU类型
	_GetAddress(m_iosInfo.m_strDevCpuarc, NODE_CPUARC);
	//产品
	_GetAddress(m_iosInfo.m_strDevProductType, NODE_PRODUCTTYPE);
	//尝试转换成电话型号名称
	m_iosInfo.m_strDevProductName = m_iosInfo.m_strDevProductType;

	_GetTypeFromProductName();

	utils::productType_to_phonename(m_iosInfo.m_strDevProductName);
	//获取电池信息
	_GetBatteryBaseInfo(m_iosInfo.m_sGasGauge);
	//获取硬盘信息有一定机率会失败后面处理
	_GetDiskInfo();
	//_SyncContacts();

	m_iosInfo.m_bIsJailreak = _IsJailreak();

	return true;
}

void CiOSDevice::StartUpdata()
{
	if (!(m_workThread[Thread_Updata].joinable()))
		m_workThread[Thread_Updata] = std::thread(&CiOSDevice::_Updata, this);
}

int CiOSDevice::StartLogging()
{
	StopLogging();
	/* start syslog_relay service */
	lockdownd_service_descriptor_t svc = NULL;
	lockdownd_error_t lerr = lockdownd_start_service(m_client, SYSLOG_RELAY_SERVICE_NAME, &svc);
	if (lerr == LOCKDOWN_E_PASSWORD_PROTECTED) {
		return LOCKDOWN_E_PASSWORD_PROTECTED;
	}
	if (lerr != LOCKDOWN_E_SUCCESS) {
		return -1;
	}
	/* connect to syslog_relay service */
	syslog_relay_error_t serr = SYSLOG_RELAY_E_UNKNOWN_ERROR;
	serr = syslog_relay_client_new(m_device, svc, &syslog);
	lockdownd_service_descriptor_free(svc);
	if (serr != SYSLOG_RELAY_E_SUCCESS) {
		return -1;
	}

	/* start capturing syslog */
	serr = syslog_relay_start_capture(syslog, syslog_callback, NULL);
	if (serr != SYSLOG_RELAY_E_SUCCESS) {
		syslog_relay_client_free(syslog);
		syslog = NULL;
		return -1;
	}
	return 0;
}

void CiOSDevice::StopLogging()
{
	if (syslog) {
		syslog_relay_client_free(syslog);
		syslog = NULL;
	}
}

void CiOSDevice::StartUpdataDiskInfo()
{
	if (!(m_workThread[Thread_UpdataDiskInfo].joinable()))
		m_workThread[Thread_UpdataDiskInfo] = std::thread(&CiOSDevice::_UpdataDiskInfo, this);
}

void CiOSDevice::StartCapshot()
{
	if (!(m_workThread[Thread_Cap].joinable()))
		m_workThread[Thread_Cap] = std::thread(&CiOSDevice::_ScreenShot, this);
}

const iOSDevInfo& CiOSDevice::GetiOSBaseInfo()
{
	return m_iosInfo;
}

idevice_error_t CiOSDevice::SetCallBack(IDeviceEventCallBack* relCallBack)
{
	idevice_event_unsubscribe();
	return idevice_event_subscribe(idevice_event_cb, (void*)relCallBack);
}

bool CiOSDevice::IsPair(LPCSTR udid)
{
	if (udid)
	{
		lockdownd_client_t client = NULL;
		idevice_t device = NULL;
		idevice_error_t ret = IDEVICE_E_UNKNOWN_ERROR;
		lockdownd_error_t lerr;

		if (udid) {
			ret = idevice_new(&device, udid);
			if (ret != IDEVICE_E_SUCCESS) {
				return false;
			}
		}
		lerr = lockdownd_client_new_with_handshake(device, &client, "idevicepair");
		lockdownd_client_free(client);
		idevice_free(device);
		return (lerr == LOCKDOWN_E_SUCCESS);
	}
	return false;
}

bool CiOSDevice::GetiOSDeviceGUIDList(std::vector<std::string>& iosList)
{
	idevice_t device = NULL;
	char** dev_list = NULL;
	int i;
	const char* udid = NULL;

	if (idevice_get_device_list(&dev_list, &i) < 0) {
		return false;
	}
	for (int m = 0; m < i; i++) {
		iosList.push_back(dev_list[i]);
	}
	idevice_device_list_free(dev_list);
	return true;
}

bool CiOSDevice::DoCmd(diagnostics_cmd_mode cmd)
{
	lockdownd_service_descriptor_t service = NULL;
	diagnostics_relay_client_t diagnostics_client = NULL;

	plist_t node = NULL;

	lockdownd_error_t ret = lockdownd_start_service(m_client, "com.apple.mobile.diagnostics_relay", &service);
	if (ret != LOCKDOWN_E_SUCCESS) {
		/*  attempt to use older diagnostics service */
		ret = lockdownd_start_service(m_client, "com.apple.iosdiagnostics.relay", &service);
	}
	bool bRet = false;
	if ((ret == LOCKDOWN_E_SUCCESS) && service && (service->port > 0)) {
		if (diagnostics_relay_client_new(m_device, service, &diagnostics_client) == DIAGNOSTICS_RELAY_E_SUCCESS)
		{
			switch (cmd) {
				case CMD_SLEEP:
					if (diagnostics_relay_sleep(diagnostics_client) == DIAGNOSTICS_RELAY_E_SUCCESS) {
						bRet = true;
					}
					break;
				case CMD_RESTART:
					if (diagnostics_relay_restart(diagnostics_client, DIAGNOSTICS_RELAY_ACTION_FLAG_WAIT_FOR_DISCONNECT) == DIAGNOSTICS_RELAY_E_SUCCESS) {
						bRet = true;
					}
					break;
				case CMD_SHUTDOWN:
					if (diagnostics_relay_shutdown(diagnostics_client, DIAGNOSTICS_RELAY_ACTION_FLAG_WAIT_FOR_DISCONNECT) == DIAGNOSTICS_RELAY_E_SUCCESS) {
						bRet = true;
					}
					break;
			}
			diagnostics_relay_goodbye(diagnostics_client);
			diagnostics_relay_client_free(diagnostics_client);
		}
	}

	if (service) {
		lockdownd_service_descriptor_free(service);
		service = NULL;
	}
	return true;
}

void CiOSDevice::GetBatteryBaseInfo(BatteryBaseInfo& outasGauge)
{
	outasGauge = m_iosInfo.m_sGasGauge;
}

bool CiOSDevice::_GetBatteryBaseInfo(BatteryBaseInfo& outInfo)
{
	lockdownd_service_descriptor_t service = NULL;
	diagnostics_relay_client_t diagnostics_client = NULL;
	/*lockdownd_client_t DiagnosticsClient = NULL;
	if (lockdownd_client_new_with_handshake(m_device, &DiagnosticsClient, "tmpClient") != LOCKDOWN_E_SUCCESS)
	{
		return false;
	}*/
	plist_t node = NULL;
	lockdownd_error_t ret = lockdownd_start_service(m_client, "com.apple.mobile.diagnostics_relay", &service);
	if (ret != LOCKDOWN_E_SUCCESS) {
		/*  attempt to use older diagnostics service */
		ret = lockdownd_start_service(m_client, "com.apple.iosdiagnostics.relay", &service);
	}
	//lockdownd_client_free(DiagnosticsClient);

	if ((ret == LOCKDOWN_E_SUCCESS) && service && (service->port > 0)) {
		if (diagnostics_relay_client_new(m_device, service, &diagnostics_client) == DIAGNOSTICS_RELAY_E_SUCCESS)
		{
#ifdef _DEBUG
#define	TEST
#endif // _DEBUG Device Characteristics
			//Device Characteristics AppleH4CamIn  ASPStorage
			if (diagnostics_relay_query_ioregistry_entry(diagnostics_client, "ASPStorage", "", &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
				if (node) {
#ifdef TEST	
					{
						char* xml = NULL;
						uint32_t len;
						plist_to_xml(node, &xml, &len);
						if (xml)
							free(xml);
					}
#endif // TEST					
					plist_free(node);
					node = NULL;
				}
			}



			if (diagnostics_relay_request_diagnostics(diagnostics_client, "GasGauge", &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
				if (node) {

					plist_t nodeGasGauge = plist_dict_get_item(node, "GasGauge");

					plist_t value = plist_dict_get_item(nodeGasGauge, "CycleCount");
					if (PLIST_UINT == plist_get_node_type(value))
					{
						uint64_t item_val;
						plist_get_uint_val(value, &item_val);
						outInfo.CycleCount = (int)item_val;
					}
					value = plist_dict_get_item(nodeGasGauge, "DesignCapacity");
					if (PLIST_UINT == plist_get_node_type(value))
					{
						uint64_t item_val;
						plist_get_uint_val(value, &item_val);
						outInfo.DesignCapacity = (int)item_val;
					}
					/*
					value = plist_dict_get_item(nodeGasGauge, "FullChargeCapacity");
					if (PLIST_UINT == plist_get_node_type(value))
					{
						uint64_t item_val;
						plist_get_uint_val(value, &item_val);
						outInfo.FullChargeCapacity = (int)item_val;
					}*/
					plist_free(node);
					node = NULL;
				}
			}
			//IOPMPowerSource AppleSmartBattery
			if (diagnostics_relay_query_ioregistry_entry(diagnostics_client, "AppleARMPMUCharger", NULL, &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
				if (node == NULL)//新版本上类名不一样
					diagnostics_relay_query_ioregistry_entry(diagnostics_client, "AppleSmartBattery", NULL, &node);
				if (node) {
					using namespace SOUI;
					plist_t batterysnnode = plist_dict_get_item(plist_dict_get_item(node, "IORegistry"), "Serial");
					if (batterysnnode)
					{
						char* strBatterySerialNumber = NULL;
						plist_get_string_val(batterysnnode, &strBatterySerialNumber);
						if (strBatterySerialNumber)
						{
							outInfo.BatterySerialNumber = S_CA2T(strBatterySerialNumber, CP_UTF8);
							free(strBatterySerialNumber);
						}
					}
					//NominalChargeCapacity
					plist_t batterynccnode = plist_dict_get_item(plist_dict_get_item(node, "IORegistry"), "NominalChargeCapacity");
					if (batterynccnode)
					{
						plist_get_uint_val(batterynccnode, &outInfo.NominalChargeCapacity);
					}
					else
					{
						//BatteryData/MaxCapacity
						batterynccnode = plist_dict_get_item(plist_dict_get_item(plist_dict_get_item(node, "IORegistry"), "BatteryData"), "MaxCapacity");
						if (batterynccnode)
						{
							plist_get_uint_val(batterynccnode, &outInfo.NominalChargeCapacity);
						}
					}

					plist_t BootVoltageNode = plist_dict_get_item(plist_dict_get_item(node, "IORegistry"), "BootVoltage");
					if (BootVoltageNode)
					{
						plist_get_uint_val(BootVoltageNode, &outInfo.BootVoltage);
					}
					if (!outInfo.BatterySerialNumber.IsEmpty())
					{
						if (utils::isIp6OrLater(m_iosInfo.m_strProductVersion))
							utils::getbatteryManufactureDateFormSN(outInfo.BatterySerialNumber, outInfo.Origin, outInfo.ManufactureDate);
						else
							utils::getbatteryManufactureDateFormSNOld(outInfo.BatterySerialNumber, outInfo.Origin, outInfo.ManufactureDate);
					}
					plist_free(node);
					node = NULL;
				}
			}

			diagnostics_relay_goodbye(diagnostics_client);
			diagnostics_relay_client_free(diagnostics_client);
		}
	}

	if (service) {
		lockdownd_service_descriptor_free(service);
		service = NULL;
	}
	return true;
}

void CiOSDevice::_ScreenShot()
{
	EventScreenShot* pScreenshotEvt = new EventScreenShot(NULL);
	pScreenshotEvt->code = -1;
	pScreenshotEvt->udid = m_iosInfo.m_strDevUDID;
	pScreenshotEvt->udid.MakeLower();
	if (_GetScreenWallpaper(&pScreenshotEvt->imgbuf, pScreenshotEvt->bufsize))
	{
		pScreenshotEvt->code = 0;
	}
	SOUI::SNotifyCenter::getSingleton().FireEventAsync(pScreenshotEvt);
	pScreenshotEvt->Release();
}

void CiOSDevice::_Updata()
{
	auto_bool_value flag(m_bUpdata);

	lockdownd_client_t updataClient = NULL;
	if (lockdownd_client_new_with_handshake(m_device, &updataClient, "my_iOSDeviceUpdata") != LOCKDOWN_E_SUCCESS)
	{
		return;
	}

	SOUI::SStringT udid = m_iosInfo.m_strDevUDID;
	udid.MakeLower();
	while (m_bUpdata)
	{
		//"com.apple.mobile.battery"
		EventUpdataInfo* e = new EventUpdataInfo(NULL);
		e->udid = udid;
		plist_t address_node = NULL;
		if (LOCKDOWN_E_SUCCESS == lockdownd_get_value(updataClient, "com.apple.mobile.battery", "BatteryCurrentCapacity", &address_node))
		{
			plist_get_uint_val(address_node, &(e->BatteryCurrentCapacity));
		}
		if (LOCKDOWN_E_SUCCESS == lockdownd_get_value(updataClient, "com.apple.mobile.battery", "BatteryIsCharging", &address_node))
		{
			plist_get_bool_val(address_node, &(e->BatteryIsCharging));
		}
		SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
		e->Release();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void CiOSDevice::_Backup(LPCSTR pDir)
{
	lockdownd_client_t backupClient = NULL;
	uint8_t willEncrypt = 0;
	plist_t node_tmp = NULL;

	if (lockdownd_client_new_with_handshake(m_device, &backupClient, "my_iOSDeviceBackup2") != LOCKDOWN_E_SUCCESS)
	{
		return;
	}

	node_tmp = NULL;
	lockdownd_get_value(backupClient, "com.apple.mobile.backup", "WillEncrypt", &node_tmp);
	if (node_tmp) {
		if (plist_get_node_type(node_tmp) == PLIST_BOOLEAN) {
			plist_get_bool_val(node_tmp, &willEncrypt);
		}
		plist_free(node_tmp);
		node_tmp = NULL;
	}
	lockdownd_service_descriptor_t service = NULL;
	//* start notification_proxy 
	np_client_t np = NULL;
	lockdownd_start_service(backupClient, NP_SERVICE_NAME, &service);
	if (service && service->port) {
		np_client_new(m_device, service, &np);
		np_set_notify_callback(np, notify_cb, this);
		const char* noties[5] = {
			NP_SYNC_CANCEL_REQUEST,
			NP_SYNC_SUSPEND_REQUEST,
			NP_SYNC_RESUME_REQUEST,
			NP_BACKUP_DOMAIN_CHANGED,
			NULL
		};
		np_observe_notifications(np, noties);

	}
	else {
		return;
	}
	if (service) {
		lockdownd_service_descriptor_free(service);
		service = NULL;
	}
	afc_client_t afc = NULL;
	//* start AFC, we need this for the lock file
	lockdownd_start_service(backupClient, AFC_SERVICE_NAME, &service);
	if (service && service->port) {
		afc_client_new(m_device, service, &afc);
	}
	if (service) {
		lockdownd_service_descriptor_free(service);
		service = NULL;
	}
	//* start mobilebackup service and retrieve port
	mobilebackup2_client_t mobilebackup2 = NULL;
	lockdownd_start_service_with_escrow_bag(backupClient, MOBILEBACKUP2_SERVICE_NAME, &service);
	lockdownd_client_free(backupClient);
	backupClient = NULL;
	if (service && service->port) {
		mobilebackup2_client_new(m_device, service, &mobilebackup2);
		if (service) {
			lockdownd_service_descriptor_free(service);
			service = NULL;
		}

		//* send Hello message 
		double local_versions[2] = { 2.0, 2.1 };
		double remote_version = 0.0;
		mobilebackup2_error_t err = mobilebackup2_version_exchange(mobilebackup2, local_versions, 2, &remote_version);
		if (err != MOBILEBACKUP2_E_SUCCESS) {

			goto checkpoint;
		}

		uint64_t lockfile = 0;
		utils::do_post_notification(m_device, NP_SYNC_WILL_START);
		afc_file_open(afc, "/com.apple.itunes.lock_sync", AFC_FOPEN_RW, &lockfile);

		if (lockfile) {
			afc_error_t aerr;
			utils::do_post_notification(m_device, NP_SYNC_LOCK_REQUEST);
			int i = 0;
			for (; i < LOCK_ATTEMPTS; i++) {
				aerr = afc_file_lock(afc, lockfile, AFC_LOCK_EX);
				if (aerr == AFC_E_SUCCESS) {
					utils::do_post_notification(m_device, NP_SYNC_DID_START);
					break;
				}
				else if (aerr == AFC_E_OP_WOULD_BLOCK) {
					Sleep(LOCK_WAIT / 1000);
					continue;
				}
				else {
					afc_file_close(afc, lockfile);
					lockfile = 0;
				}
			}
			if (i == LOCK_ATTEMPTS) {
				afc_file_close(afc, lockfile);
				lockfile = 0;
			}
		}
		SOUI::SStringA udid = SOUI::S_CW2A(m_iosInfo.m_strDevUDID);

		if (lockfile)
		{
			//Starting backup..

			/* TODO: check domain com.apple.mobile.backup key RequiresEncrypt and WillEncrypt with lockdown */
			/* TODO: verify battery on AC enough battery remaining */

			/* re-create Info.plist (Device infos, IC-Info.sidb, photos, app_ids, iTunesPrefs) */

			plist_t info_plist = utils::mobilebackup_factory_info_plist_new(udid, m_device, afc);
			if (!info_plist) {
			}
			plist_write_to_filename(info_plist, "path", PLIST_FORMAT_XML);

			plist_free(info_plist);
			info_plist = NULL;
			plist_t opts = NULL;
			if (false) {

				opts = plist_new_dict();
				plist_dict_set_item(opts, "ForceFullBackup", plist_new_bool(1));
			}

			err = mobilebackup2_send_request(mobilebackup2, "Backup", udid, NULL, opts);
			if (opts)
				plist_free(opts);
			if (err == MOBILEBACKUP2_E_SUCCESS) {

			}
		}
		if (true) {
			/* reset operation success status */
			int operation_ok = 0;
			plist_t message = NULL;

			char* dlmsg = NULL;
			int file_count = 0;
			int errcode = 0;
			const char* errdesc = NULL;
			int progress_finished = 0;
			struct stat st;

			char* backup_directory = NULL;
			/* process series of DLMessage* operations */
			do {
				free(dlmsg);
				dlmsg = NULL;
				mobilebackup2_receive_message(mobilebackup2, &message, &dlmsg);
				if (!message || !dlmsg) {
					Sleep(2);
					goto files_out;
				}

				if (!strcmp(dlmsg, "DLMessageDownloadFiles")) {
					/* device wants to download files from the computer */
					utils::mb2_set_overall_progress_from_message(message, dlmsg);
					utils::mb2_handle_send_files(mobilebackup2, message, backup_directory);
				}
				else if (!strcmp(dlmsg, "DLMessageUploadFiles")) {
					/* device wants to send files to the computer */
					utils::mb2_set_overall_progress_from_message(message, dlmsg);
					file_count += utils::mb2_handle_receive_files(mobilebackup2, message, backup_directory);
				}
				else if (!strcmp(dlmsg, "DLMessageGetFreeDiskSpace")) {
					/* device wants to know how much disk space is available on the computer */
					uint64_t freespace = 0;
					int res = -1;
					if (GetDiskFreeSpaceExA(backup_directory, (PULARGE_INTEGER)& freespace, NULL, NULL)) {
						res = 0;
					}
					plist_t freespace_item = plist_new_uint(freespace);
					mobilebackup2_send_status_response(mobilebackup2, res, NULL, freespace_item);
					plist_free(freespace_item);
				}
				else if (!strcmp(dlmsg, "DLContentsOfDirectory")) {
					/* list directory contents */
					utils::mb2_handle_list_directory(mobilebackup2, message, backup_directory);
				}
				else if (!strcmp(dlmsg, "DLMessageCreateDirectory")) {
					/* make a directory */
					utils::mb2_handle_make_directory(mobilebackup2, message, backup_directory);
				}
				else if (!strcmp(dlmsg, "DLMessageMoveFiles") || !strcmp(dlmsg, "DLMessageMoveItems")) {
					/* perform a series of rename operations */
					utils::mb2_set_overall_progress_from_message(message, dlmsg);
					plist_t moves = plist_array_get_item(message, 1);
					uint32_t cnt = plist_dict_get_size(moves);
					//PRINT_VERBOSE(1, "Moving %d file%s\n", cnt, (cnt == 1) ? "" : "s");
					plist_dict_iter iter = NULL;
					plist_dict_new_iter(moves, &iter);
					errcode = 0;
					errdesc = NULL;
					if (iter) {
						char* key = NULL;
						plist_t val = NULL;
						do {
							plist_dict_next_item(moves, iter, &key, &val);
							if (key && (plist_get_node_type(val) == PLIST_STRING)) {
								char* str = NULL;
								plist_get_string_val(val, &str);
								if (str) {
									char* newpath = string_build_path(backup_directory, str, NULL);
									free(str);
									char* oldpath = string_build_path(backup_directory, key, NULL);

									if ((stat(newpath, &st) == 0) && S_ISDIR(st.st_mode))
										utils::rmdir_recursive(newpath);
									else
										utils::remove_file(newpath);
									if (rename(oldpath, newpath) < 0) {
										printf("Renameing '%s' to '%s' failed: %s (%d)\n", oldpath, newpath, strerror(errno), errno);
										errcode = utils::errno_to_device_error(errno);
										errdesc = strerror(errno);
										break;
									}
									free(oldpath);
									free(newpath);
								}
								free(key);
								key = NULL;
							}
						} while (val);
						free(iter);
					}
					else {
						errcode = -1;
						errdesc = "Could not create dict iterator";
						printf("Could not create dict iterator\n");
					}
					plist_t empty_dict = plist_new_dict();
					err = mobilebackup2_send_status_response(mobilebackup2, errcode, errdesc, empty_dict);
					plist_free(empty_dict);
					if (err != MOBILEBACKUP2_E_SUCCESS) {
						printf("Could not send status response, error %d\n", err);
					}
				}
				else if (!strcmp(dlmsg, "DLMessageRemoveFiles") || !strcmp(dlmsg, "DLMessageRemoveItems")) {
					utils::mb2_set_overall_progress_from_message(message, dlmsg);
					plist_t removes = plist_array_get_item(message, 1);
					uint32_t cnt = plist_array_get_size(removes);
					//PRINT_VERBOSE(1, "Removing %d file%s\n", cnt, (cnt == 1) ? "" : "s");
					uint32_t ii = 0;
					errcode = 0;
					errdesc = NULL;
					for (ii = 0; ii < cnt; ii++) {
						plist_t val = plist_array_get_item(removes, ii);
						if (plist_get_node_type(val) == PLIST_STRING) {
							char* str = NULL;
							plist_get_string_val(val, &str);
							if (str) {
								const char* checkfile = strchr(str, '/');
								int suppress_warning = 0;
								if (checkfile) {
									if (strcmp(checkfile + 1, "Manifest.mbdx") == 0) {
										suppress_warning = 1;
									}
								}
								char* newpath = string_build_path(backup_directory, str, NULL);
								free(str);
								int res = 0;
								if ((stat(newpath, &st) == 0) && S_ISDIR(st.st_mode)) {
									res = utils::rmdir_recursive(newpath);
								}
								else {
									res = utils::remove_file(newpath);
								}
								if (res != 0 && res != ENOENT) {
									if (!suppress_warning)
										printf("Could not remove '%s': %s (%d)\n", newpath, strerror(res), res);
									errcode = utils::errno_to_device_error(res);
									errdesc = strerror(res);
								}
								free(newpath);
							}
						}
					}
					plist_t empty_dict = plist_new_dict();
					err = mobilebackup2_send_status_response(mobilebackup2, errcode, errdesc, empty_dict);
					plist_free(empty_dict);
					if (err != MOBILEBACKUP2_E_SUCCESS) {
						printf("Could not send status response, error %d\n", err);
					}
				}
				else if (!strcmp(dlmsg, "DLMessageCopyItem")) {
					plist_t srcpath = plist_array_get_item(message, 1);
					plist_t dstpath = plist_array_get_item(message, 2);
					errcode = 0;
					errdesc = NULL;
					if ((plist_get_node_type(srcpath) == PLIST_STRING) && (plist_get_node_type(dstpath) == PLIST_STRING)) {
						char* src = NULL;
						char* dst = NULL;
						plist_get_string_val(srcpath, &src);
						plist_get_string_val(dstpath, &dst);
						if (src && dst) {
							char* oldpath = string_build_path(backup_directory, src, NULL);
							char* newpath = string_build_path(backup_directory, dst, NULL);

							//PRINT_VERBOSE(1, "Copying '%s' to '%s'\n", src, dst);

							/* check that src exists */
							if ((stat(oldpath, &st) == 0) && S_ISDIR(st.st_mode)) {
								utils::mb2_copy_directory_by_path(oldpath, newpath);
							}
							else if ((stat(oldpath, &st) == 0) && S_ISREG(st.st_mode)) {
								utils::mb2_copy_file_by_path(oldpath, newpath);
							}

							free(newpath);
							free(oldpath);
						}
						free(src);
						free(dst);
					}
					plist_t empty_dict = plist_new_dict();
					err = mobilebackup2_send_status_response(mobilebackup2, errcode, errdesc, empty_dict);
					plist_free(empty_dict);
					if (err != MOBILEBACKUP2_E_SUCCESS) {
						printf("Could not send status response, error %d\n", err);
					}
				}
				else if (!strcmp(dlmsg, "DLMessageDisconnect")) {
					break;
				}
				else if (!strcmp(dlmsg, "DLMessageProcessMessage")) {
					node_tmp = plist_array_get_item(message, 1);
					if (plist_get_node_type(node_tmp) != PLIST_DICT) {
						printf("Unknown message received!\n");
					}
					plist_t nn;
					int error_code = -1;
					nn = plist_dict_get_item(node_tmp, "ErrorCode");
					if (nn && (plist_get_node_type(nn) == PLIST_UINT)) {
						uint64_t ec = 0;
						plist_get_uint_val(nn, &ec);
						error_code = (uint32_t)ec;
						if (error_code == 0) {
							operation_ok = 1;
						}
					}
					nn = plist_dict_get_item(node_tmp, "ErrorDescription");
					char* str = NULL;
					if (nn && (plist_get_node_type(nn) == PLIST_STRING)) {
						plist_get_string_val(nn, &str);
					}
					if (error_code != 0) {
						if (str) {
							printf("ErrorCode %d: %s\n", error_code, str);
						}
						else {
							printf("ErrorCode %d: (Unknown)\n", error_code);
						}
					}
					if (str) {
						free(str);
					}
					nn = plist_dict_get_item(node_tmp, "Content");
					if (nn && (plist_get_node_type(nn) == PLIST_STRING)) {
						str = NULL;
						plist_get_string_val(nn, &str);
						//PRINT_VERBOSE(1, "Content:\n");
						printf("%s", str);
						free(str);
					}
					break;
				}

				/* print status */
				//if ((utils::overall_progress > 0) && !progress_finished) {
				//	if (utils::overall_progress >= 100.0f) {
				//		progress_finished = 1;
				//	}
				//	utils::print_progress_real(utils::overall_progress, 0);
					//PRINT_VERBOSE(1, " Finished\n");
				//}

			files_out:
				plist_free(message);
				message = NULL;
				free(dlmsg);
				dlmsg = NULL;

				//if (utils::quit_flag > 0) {
					/* need to cancel the backup here */
					//mobilebackup_send_error(mobilebackup, "Cancelling DLSendFile");

					/* remove any atomic Manifest.plist.tmp */

					/*manifest_path = mobilebackup_build_path(backup_directory, "Manifest", ".plist.tmp");
					if (stat(manifest_path, &st) == 0)
						remove(manifest_path);*/
						//	break;
						//}
			} while (1);

			plist_free(message);
			free(dlmsg);

			/* report operation status to user */

			if (operation_ok && utils::mb2_status_check_snapshot_state(backup_directory, udid, "finished")) {
				//PRINT_VERBOSE(1, "Backup Successful.\n");
			}
			else {
				//if (utils::quit_flag) {
					//PRINT_VERBOSE(1, "Backup Aborted.\n");
				//}
				//else {
					//PRINT_VERBOSE(1, "Backup Failed (Error Code %d).\n", -result_code);
				//}
			}
		}
		if (lockfile) {
			afc_file_lock(afc, lockfile, AFC_LOCK_UN);
			afc_file_close(afc, lockfile);
			lockfile = 0;
			utils::do_post_notification(m_device, NP_SYNC_DID_FINISH);
		}
	}
checkpoint:
	lockdownd_client_free(backupClient);
	if (service) {
		lockdownd_service_descriptor_free(service);
		service = NULL;
	}

	if (mobilebackup2) {
		mobilebackup2_client_free(mobilebackup2);
		mobilebackup2 = NULL;
	}

	if (afc) {
		afc_client_free(afc);
		afc = NULL;
	}

	if (np) {
		np_client_free(np);
		np = NULL;
	}
}

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

void CiOSDevice::StartUpdataBatteryInfo()
{
	if (!(m_workThread[Thread_UpdataBattreyInfo].joinable()))
		m_workThread[Thread_UpdataBattreyInfo] = std::thread(&CiOSDevice::_UpdataBatteryInfo, this);
}

void CiOSDevice::StopUpdataBatteryInfo()
{
	if (m_workThread[Thread_UpdataBattreyInfo].joinable())
	{
		m_bUpdataBattreyInfo = false;
		m_workThread[Thread_UpdataBattreyInfo].join();
	}
}

void CiOSDevice::_UpdataBatteryInfo()
{
	auto_bool_value flag(m_bUpdataBattreyInfo);

	lockdownd_service_descriptor_t service = NULL;
	diagnostics_relay_client_t diagnostics_client = NULL;
	lockdownd_client_t updataBatteryClient = NULL;
	if (lockdownd_client_new_with_handshake(m_device, &updataBatteryClient, "tmpClient") != LOCKDOWN_E_SUCCESS)
	{
		return;
	}
	plist_t node = NULL;
	lockdownd_error_t ret = lockdownd_start_service(updataBatteryClient, "com.apple.mobile.diagnostics_relay", &service);
	if (ret != LOCKDOWN_E_SUCCESS) {
		/*  attempt to use older diagnostics service */
		ret = lockdownd_start_service(updataBatteryClient, "com.apple.iosdiagnostics.relay", &service);
	}
	lockdownd_client_free(updataBatteryClient);
	if ((ret == LOCKDOWN_E_SUCCESS) && service && (service->port > 0)) {
		if (diagnostics_relay_client_new(m_device, service, &diagnostics_client) == DIAGNOSTICS_RELAY_E_SUCCESS)
		{
			SOUI::SStringT udid = m_iosInfo.m_strDevUDID;
			udid.MakeLower();
			while (m_bUpdataBattreyInfo)
			{
				if (diagnostics_relay_query_ioregistry_entry(diagnostics_client, "AppleARMPMUCharger", NULL, &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
					if (node == NULL)
						diagnostics_relay_query_ioregistry_entry(diagnostics_client, "AppleSmartBattery", NULL, &node);
					if (node) {
						using namespace SOUI;
						EventUpdataBattreyInfo* e = new EventUpdataBattreyInfo(NULL);
						e->udid = udid;
						//Temperature
						plist_t TemperatureNode = plist_dict_get_item(plist_dict_get_item(node, "IORegistry"), "Temperature");
						if (TemperatureNode)
						{
							plist_get_uint_val(TemperatureNode, &(e->Temperature));
						}
						//Voltage
						plist_t VoltageNode = plist_dict_get_item(plist_dict_get_item(node, "IORegistry"), "Voltage");
						if (VoltageNode)
						{
							plist_get_uint_val(VoltageNode, &(e->Voltage));
						}
						//Current
						plist_t CurrentCapacityNode = plist_dict_get_item(plist_dict_get_item(node, "IORegistry"), "InstantAmperage");
						if (CurrentCapacityNode)
						{
							plist_get_uint_val(CurrentCapacityNode, &(e->Current));
						}

						plist_free(node);
						node = NULL;
						SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
						e->Release();
					}
				}std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			diagnostics_relay_goodbye(diagnostics_client);
			diagnostics_relay_client_free(diagnostics_client);
		}
	}
	if (service) {
		lockdownd_service_descriptor_free(service);
		service = NULL;
	}
}

void CiOSDevice::FreeAFCClient(afc_client_t afc_client)
{
	afc_client_free(afc_client);
}

bool CiOSDevice::CreateAFCClient(afc_client_t& afc_client)
{
	//afc_client = NULL;
	lockdownd_service_descriptor_t service = NULL;
	lockdownd_client_t CreateAFCClient = NULL;
	bool bret = false;
	if (lockdownd_client_new_with_handshake(m_device, &CreateAFCClient, "afc_client") != LOCKDOWN_E_SUCCESS)
	{
		return false;
	}
	if ((lockdownd_start_service(CreateAFCClient, "com.apple.afc", &service) !=
		LOCKDOWN_E_SUCCESS) || !service) {
		goto leave_cleanup;
	}
	if (afc_client_new(m_device, service, &afc_client) == AFC_E_SUCCESS) {
		bret = true;
	}
leave_cleanup:
	lockdownd_service_descriptor_free(service);
	lockdownd_client_free(CreateAFCClient);
	return bret;
}

bool CiOSDevice::CreateAFC2Client(afc_client_t& afc_client)
{
	lockdownd_service_descriptor_t service = NULL;
	lockdownd_client_t CreateAFCClient = NULL;
	bool bret = false;
	if (lockdownd_client_new_with_handshake(m_device, &CreateAFCClient, "afc2_client") != LOCKDOWN_E_SUCCESS)
	{
		return false;
	}
	if ((lockdownd_start_service(CreateAFCClient, "com.apple.afc2", &service) !=
		LOCKDOWN_E_SUCCESS) || !service) {
		goto leave_cleanup;
	}
	if (afc_client_new(m_device, service, &afc_client) == AFC_E_SUCCESS) {
		bret = true;
	}
leave_cleanup:
	lockdownd_service_descriptor_free(service);
	lockdownd_client_free(CreateAFCClient);
	return bret;
}

void CiOSDevice::_UpdataDiskInfo()
{
	auto_bool_value flag(m_bUpdataDiskInfo);

	//m_bUpdataDiskInfo = true;
	m_iosInfo.m_diskInfo.bReady = false;

	SOUI::SStringA udid = SOUI::S_CW2A(m_iosInfo.m_strDevUDID);
	udid.MakeLower();
	/*while (m_bUpdataDiskInfo)
	{
		//"/iTunes_Control/iTunes/iTunesPrefs.plist"


		if (_GetDiskAddress(updataDiskClient, m_iosInfo.m_diskInfo))
		{

			break;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}*/
	afc_client_t afc = NULL;
	lockdownd_service_descriptor_t service = NULL;
	lockdownd_client_t updataDiskClient = NULL;
	if (lockdownd_client_new_with_handshake(m_device, &updataDiskClient, "updataDiskClient") != LOCKDOWN_E_SUCCESS)
	{
		return;
	}
	if ((lockdownd_start_service(updataDiskClient, "com.apple.afc", &service) !=
		LOCKDOWN_E_SUCCESS) || !service) {
		goto leave_cleanup;
	}
	if (afc_client_new(m_device, service, &afc) != AFC_E_SUCCESS) {
		goto leave_cleanup;
	}
	//  /var/mobile/Containers/Bundle/Application

#define AppDirPath "/User/Applications"
	size_t size = 0;
	if (utils::afc_get_file_size(afc, size, AppDirPath) == 0)
		m_iosInfo.m_diskInfo.AppUsage;
#define UDiskPath "/general_storage"
	if (utils::afc_get_file_size(afc, size, UDiskPath) == 0)
		m_iosInfo.m_diskInfo.UDisk = size;
#define PhotePath "/DCIM"
	if (utils::afc_get_file_size(afc, size, PhotePath) == 0)
		m_iosInfo.m_diskInfo.PhotoUsage = size;

	m_iosInfo.m_diskInfo.bReady = true;
	EventUpdataDiskInfo* e = new EventUpdataDiskInfo(NULL);
	e->udid = udid;
	SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
	e->Release();
leave_cleanup:
	afc_client_free(afc);
	lockdownd_service_descriptor_free(service);
	lockdownd_client_free(updataDiskClient);
}

void CiOSDevice::app_notifier_fun(const char* notification)
{
	if (m_instproxy)
	{
		if ((strcmp(notification, NP_APP_INSTALLED) == 0)
			|| (strcmp(notification, NP_APP_UNINSTALLED) == 0)) {
			//安装重新拉取列表。
			instproxy_error_t err;
			plist_t client_opts = instproxy_client_options_new();
			instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
			plist_t apps = NULL;
			instproxy_client_options_set_return_attributes(client_opts,
				"CFBundleIdentifier",
				NULL
			);

			if (m_instproxy == NULL)
				_InitInstproxy();
			if (m_instproxy == NULL)
				return;
			err = instproxy_browse(m_instproxy, client_opts, &apps);

			instproxy_client_options_free(client_opts);
			if (!apps || (plist_get_node_type(apps) != PLIST_ARRAY)) {
				return;
			}

			WLocker locker(m_appsLocker);
			std::set<std::string> oldlist = m_applist;
			m_applist.clear();
			uint32_t size = plist_array_get_size(apps);
			//新安装数
			int newinstallcount = 0;
			for (uint32_t i = 0; i < size; i++)
			{
				plist_t app = plist_array_get_item(apps, i);
				if (app)
				{
					std::string id;
					GetAppValue(app, "CFBundleIdentifier", id);
					//原列表没有的说明是新安装的；
					if (oldlist.count(id) == 0)
					{
						++newinstallcount;
						const char* ids[] = { id.c_str(),NULL };
						_GetInstallAppInfo(ids);
					}
					m_applist.insert(id);
				}
			}
			//如果新列表大于老列表加安装的应用数则说明有卸载的应用
			if (m_applist.size() < (oldlist.size() + newinstallcount))
			{
				//老列表有，新列表没有，则说明是卸载的应用
				for (auto& ite : oldlist)
				{
					if (m_applist.count(ite) == 0)
					{
						EventUninstallApp* e = new EventUninstallApp(NULL);
						e->udid = m_iosInfo.m_strDevUDID;
						e->udid.MakeLower();
						e->appid = ite;
						e->bSucessed = true;
						SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
						e->Release();
					}
				}
			}
			plist_free(apps);
		}
	}
}

void CiOSDevice::StartUpdataApps()
{
	if (m_bUpdataApps)
		return;
	if (m_workThread[Thread_UpdataAppsInfo].joinable())
		m_workThread[Thread_UpdataAppsInfo].join();
	m_workThread[Thread_UpdataAppsInfo] = std::thread(&CiOSDevice::_UpdataAppsInfo, this);
}

int CiOSDevice::GetAppsCount()
{
	return m_applist.size();
}

void CiOSDevice::IntallApp(LPCWSTR appfile)
{
	if (m_workThread[Thread_InstallApp].joinable())
		m_workThread[Thread_InstallApp].join();
	m_workThread[Thread_InstallApp] = std::thread(&CiOSDevice::_InstallApp, this, std::wstring(appfile), true);
}

bool CiOSDevice::_GetAppIcon(LPCSTR id, char** outBuf, uint64_t& len)
{
	if (id == NULL)
		return false;
	sbservices_client_t sbclient = NULL;
	if (sbservices_client_start_service(m_device, &sbclient, "geticon") != SBSERVICES_E_SUCCESS)
		return false;

	sbservices_error_t ret = sbservices_get_icon_pngdata(sbclient, id, outBuf, &len);
	sbservices_client_free(sbclient);

	return SBSERVICES_E_SUCCESS == ret;
}

bool CiOSDevice::_GetScreenWallpaper(char** outBuf, uint64_t& len)
{
	sbservices_client_t sbclient = NULL;
	if (sbservices_client_start_service(m_device, &sbclient, "geticon") != SBSERVICES_E_SUCCESS)
		return false;
	sbservices_error_t ret = sbservices_get_home_screen_wallpaper_pngdata(sbclient, outBuf, &len);
	sbservices_client_free(sbclient);
	return SBSERVICES_E_SUCCESS == ret;
}

void CiOSDevice::_UpdataApps(plist_t apps)
{
	if (apps)
	{
		WLocker locker(m_appsLocker);
		m_applist.clear();
		/*char* xml = NULL;
		uint32_t len = 0;
		plist_to_xml(apps, &xml, &len);
		if (xml) {
			SOUI::SStringW wxml = SOUI::S_CA2W(xml, CP_UTF8);
			free(xml);
		}*/
		uint32_t size = plist_array_get_size(apps);
		EventUpdataAppsInfo* e = new EventUpdataAppsInfo(NULL);
		for (uint32_t i = 0; i < size; i++)
		{
			plist_t app = plist_array_get_item(apps, i);
			if (app)
			{
				AppInfo appInfo;
				GetAppValue(app, "CFBundleIdentifier", appInfo.AppID);
				m_applist.insert(appInfo.AppID);
				char* pngdata = NULL; uint64_t len;
				if (_GetAppIcon(appInfo.AppID.c_str(), &pngdata, len))
				{
					appInfo.m_ico.Attach(SOUI::SResLoadFromMemory::LoadImage(pngdata, (uint32_t)len));
					free(pngdata);
				}
				GetAppValue(app, "CFBundleDisplayName", appInfo.DisplayName);
				GetAppValue(app, "CFBundleShortVersionString", appInfo.Version);
				GetAppValue(app, "DynamicDiskUsage", appInfo.DynamicDiskUsage);
				GetAppValue(app, "StaticDiskUsage", appInfo.StaticDiskUsage);
				e->apps.push_back(appInfo);
			}
		}
		e->udid = m_iosInfo.m_strDevUDID;
		e->udid.MakeLower();
		SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
		e->Release();
	}
}

void CiOSDevice::uninstallstatus_cb(plist_t command, plist_t status)
{
	if (command && status) {
		char* command_name = NULL;
		instproxy_command_get_name(command, &command_name);

		if (strcmp(command_name, "Uninstall") != 0) {
			return;
		}
		char* appid = NULL;
		char* error_name = NULL;
		char* error_description = NULL;
		plist_t node = plist_dict_get_item(command, "ApplicationIdentifier");
		if (node) {
			plist_get_string_val(node, &appid);
		}
		/* get status */
		char* status_name = NULL;
		instproxy_status_get_name(status, &status_name);
		bool exit = false;
		if (status_name) {
			//完成
			if (!strcmp(status_name, "Complete")) {
				{
					WLocker locker(m_appsLocker);

					auto iter = m_applist.find(appid);

					if (iter != m_applist.end())
					{
						m_applist.erase(iter);
						EventUninstallApp* e = new EventUninstallApp(NULL);
						e->udid = m_iosInfo.m_strDevUDID;
						e->udid.MakeLower();
						e->appid = appid;
						e->bSucessed = true;
						SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
						e->Release();
					}
				}
				exit = true;

				goto clearup;
			}
		}

		/* get error if any */

		uint64_t error_code = 0;
		instproxy_status_get_error(status, &error_name, &error_description, &error_code);

		/* output/handling */
		if (!error_name) {
			if (status_name) {
				/* get progress if any */
				int percent = -1;
				instproxy_status_get_percent_complete(status, &percent);

				if (percent >= 0) {
					printf("\r%s: %s (%d%%)", command_name, status_name, percent);
				}
				else {
					printf("\r%s: %s", command_name, status_name);
				}
			}
		}
		else {
			EventUninstallApp* e = new EventUninstallApp(NULL);
			e->udid = m_iosInfo.m_strDevUDID;
			e->udid.MakeLower();
			e->appid = appid;
			e->bSucessed = false;
			SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
			e->Release();
			exit = true;
		}
	clearup:
		/* clean up */
		free(appid);
		free(error_name);
		free(error_description);
		free(command_name);
		command_name = NULL;

		if (exit)
			appUnistallcv.notify_one();
	}
	else {
		fprintf(stderr, "ERROR: %s was called with invalid arguments!\n", __func__);
	}
}

static void installstatus_cb(plist_t command, plist_t status, void* unused)
{
	((CiOSDevice*)unused)->installstatus_cb(command, status);
}

void CiOSDevice::_GetInstallAppInfo(LPCSTR* appid)
{
	if (m_instproxy && appid)
	{
		plist_t client_opts = instproxy_client_options_new();
		plist_t apps = NULL;
		instproxy_client_options_set_return_attributes(client_opts,
			"CFBundleIdentifier",
			"CFBundleDisplayName",
			"CFBundleShortVersionString",
			"StaticDiskUsage",
			"DynamicDiskUsage",
			NULL
		);
		if (instproxy_lookup(m_instproxy, appid, client_opts, &apps) == INSTPROXY_E_SUCCESS)
		{
			plist_dict_iter ite = NULL;
			plist_dict_new_iter(apps, &ite);
			plist_t app = NULL;

			do {
				char* key = NULL;
				plist_dict_next_item(apps, ite, &key, &app);
				free(key);
				if (app)
				{
					EventInstallApp* e = new EventInstallApp(NULL);
					e->udid = m_iosInfo.m_strDevUDID;
					e->udid.MakeLower();
					e->bSucessed = true;
					GetAppValue(app, "CFBundleIdentifier", e->appinfo.AppID);
					m_applist.insert(e->appinfo.AppID);
					char* pngdata = NULL; uint64_t len;
					if (_GetAppIcon(e->appinfo.AppID.c_str(), &pngdata, len))
					{
						e->appinfo.m_ico.Attach(SOUI::SResLoadFromMemory::LoadImage(pngdata, (uint32_t)len));
						free(pngdata);
					}
					GetAppValue(app, "CFBundleDisplayName", e->appinfo.DisplayName);
					GetAppValue(app, "CFBundleShortVersionString", e->appinfo.Version);
					GetAppValue(app, "DynamicDiskUsage", e->appinfo.DynamicDiskUsage);
					GetAppValue(app, "StaticDiskUsage", e->appinfo.StaticDiskUsage);

					SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
					e->Release();
				}
			} while (app);
		}
		plist_free(apps);
		instproxy_client_options_free(client_opts);
	}
}

void CiOSDevice::installstatus_cb(plist_t command, plist_t status)
{
	if (command && status) {
		char* command_name = NULL;
		instproxy_command_get_name(command, &command_name);

		if (strcmp(command_name, "Install") != 0) {
			return;
		}
		//command_name==Uninstall ApplicationIdentifier
		char* appid = NULL;
		char* error_name = NULL;
		char* error_description = NULL;
		plist_t node = plist_dict_get_item(command, "ClientOptions");
		if (node) {
			//CFBundleIdentifier
			node = plist_dict_get_item(node, "CFBundleIdentifier");
			if (node) {
				plist_get_string_val(node, &appid);
			}
		}
		SASSERT(appid);
		/* get status */
		char* status_name = NULL;
		instproxy_status_get_name(status, &status_name);
		bool command_completed = false;
		if (status_name) {
			//完成
			if (!strcmp(status_name, "Complete")) {

				appIstallcv.notify_one();
				{
					WLocker locker(m_appsLocker);

					auto iter = m_applist.find(appid);
					if (iter == m_applist.end())
					{
						const char* ids[] = { appid,NULL };
						_GetInstallAppInfo(ids);
					}
					goto clearup;
				}
			}
		}

		/* get error if any */

		uint64_t error_code = 0;
		instproxy_status_get_error(status, &error_name, &error_description, &error_code);

		/* output/handling */
		if (!error_name) {
			if (status_name) {
				/* get progress if any */
				int percent = -1;
				instproxy_status_get_percent_complete(status, &percent);

				if (percent >= 0) {
					printf("\r%s: %s (%d%%)", command_name, status_name, percent);
				}
				else {
					printf("\r%s: %s", command_name, status_name);
				}
			}
		}
		else {
			appIstallcv.notify_one();
			EventInstallApp* e = new EventInstallApp(NULL);
			e->udid = m_iosInfo.m_strDevUDID;
			e->udid.MakeLower();
			e->appid = appid;
			e->bSucessed = false;
			SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
			e->Release();
		}
	clearup:
		/* clean up */
		free(appid);
		free(error_name);
		free(error_description);
		free(command_name);
		command_name = NULL;
	}
	else {
		fprintf(stderr, "ERROR: %s was called with invalid arguments!\n", __func__);
	}
}

void CiOSDevice::UninstallApp(LPCSTR appID)
{
	if (m_workThread[Thread_UninstallApp].joinable())
		m_workThread[Thread_UninstallApp].join();

	std::vector<std::string> apps;
	apps.push_back(appID);
	m_workThread[Thread_UninstallApp] = std::thread(&CiOSDevice::_UninstallApp, this, apps);
}

void CiOSDevice::UninstallApp(std::vector<std::string>& apps)
{
	if (m_workThread[Thread_UninstallApp].joinable())
		m_workThread[Thread_UninstallApp].join();
	m_workThread[Thread_UninstallApp] = std::thread(&CiOSDevice::_UninstallApp, this, apps);
}

void CiOSDevice::_InitCallBack()
{
	lockdownd_service_descriptor_t service = NULL;
	if ((lockdownd_start_service
	(m_client, "com.apple.mobile.notification_proxy",
		&service) != LOCKDOWN_E_SUCCESS) || !service) {
		//"Could not start com.apple.mobile.notification_proxy!\n");		
		return;
	}

	np_error_t nperr = np_client_new(m_device, service, &m_npClient);

	lockdownd_service_descriptor_free(service);

	if (nperr != NP_E_SUCCESS) {
		//fprintf(stderr, "Could not connect to notification_proxy!\n");		
		return;
	}
	np_set_notify_callback(m_npClient, notifier, this);

	const char* noties[3] = { NP_APP_INSTALLED, NP_APP_UNINSTALLED, NULL };

	np_observe_notifications(m_npClient, noties);

}

void CiOSDevice::_InitInstproxy()
{
	lockdownd_service_descriptor_t service = NULL;

	if ((lockdownd_start_service(m_client, "com.apple.mobile.installation_proxy",
		&service) != LOCKDOWN_E_SUCCESS) || !service) {
		//"Could not start com.apple.mobile.installation_proxy!\n");
		goto leave_cleanup;
	}
	instproxy_client_new(m_device, service, &m_instproxy);
leave_cleanup:
	lockdownd_service_descriptor_free(service);
}

void CiOSDevice::_UpdataAppsInfo()
{
	auto_bool_value flag(m_bUpdataApps);
	instproxy_error_t err;
	plist_t client_opts = instproxy_client_options_new();
	instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
	plist_t apps = NULL;
	//此处不请求占用大小可以加快请求速度
	instproxy_client_options_set_return_attributes(client_opts,
		"CFBundleIdentifier",
		"CFBundleDisplayName",
		"CFBundleShortVersionString",
		"StaticDiskUsage",
		"DynamicDiskUsage",
		NULL
	);

	if (m_instproxy == NULL)
		_InitInstproxy();
	if (m_instproxy == NULL)
		return;
	err = instproxy_browse(m_instproxy, client_opts, &apps);

	instproxy_client_options_free(client_opts);
	if (!apps || (plist_get_node_type(apps) != PLIST_ARRAY)) {
		return;
	}
	_UpdataApps(apps);
	plist_free(apps);
}

void CiOSDevice::_UninstallApp(const std::vector<std::string> appID)
{
	std::mutex _mx;
	std::unique_lock<std::mutex> lck(_mx);
	for (auto& iter : appID)
	{
		instproxy_uninstall(m_instproxy, iter.c_str(), NULL, ::uninstallstatus_cb, this);
		appUnistallcv.wait(lck);
	}
}

void CiOSDevice::_InstallApp(const std::wstring apppath, bool bInstall)
{
	lockdownd_service_descriptor_t service = NULL;
	instproxy_client_t ipc = NULL;
	lockdownd_client_t updataAppsClient = NULL;
	instproxy_error_t err;
	np_client_t np = NULL;

	afc_client_t afc = NULL;
	const char PKG_PATH[] = "PublicStaging";
	const char APPARCH_PATH[] = "ApplicationArchives";

#define ITUNES_METADATA_PLIST_FILENAME "iTunesMetadata.plist"

	plist_t sinf = NULL;
	plist_t meta = NULL;
	char* pkgname = NULL;
	stat_generic_t fst;
	uint64_t af = 0;
	//char buf[8192];
	char* bundleidentifier = NULL;
	std::string filename;

	std::mutex _mx;
	std::unique_lock<std::mutex> lck(_mx);

	if (lockdownd_client_new_with_handshake(m_device, &updataAppsClient, "updataAppsClient") != LOCKDOWN_E_SUCCESS)
	{
		return;
	}
	if ((lockdownd_start_service
	(updataAppsClient, "com.apple.mobile.notification_proxy",
		&service) != LOCKDOWN_E_SUCCESS) || !service) {
		//"Could not start com.apple.mobile.notification_proxy!\n");		
		goto leave_cleanup;
	}
	np_error_t nperr = np_client_new(m_device, service, &np);
	if (service) {
		lockdownd_service_descriptor_free(service);
		service = NULL;
	}
	if (nperr != NP_E_SUCCESS) {
		//fprintf(stderr, "Could not connect to notification_proxy!\n");		
		goto leave_cleanup;
	}

	np_set_notify_callback(np, notifier, this);

	const char* noties[3] = { NP_APP_INSTALLED, NP_APP_UNINSTALLED, NULL };

	np_observe_notifications(np, noties);


	if ((lockdownd_start_service(updataAppsClient, "com.apple.mobile.installation_proxy",
		&service) != LOCKDOWN_E_SUCCESS) || !service) {
		//"Could not start com.apple.mobile.installation_proxy!\n");		
		goto leave_cleanup;
	}
	err = instproxy_client_new(m_device, service, &ipc);
	if (service) {
		lockdownd_service_descriptor_free(service);
		service = NULL;
	}

	if (err != INSTPROXY_E_SUCCESS) {
		//"Could not connect to installation_proxy!\n");		
		goto leave_cleanup;
	}

	if ((lockdownd_start_service(updataAppsClient, "com.apple.afc", &service) !=
		LOCKDOWN_E_SUCCESS) || !service) {

		goto leave_cleanup;
	}

	if (afc_client_new(m_device, service, &afc) != AFC_E_SUCCESS) {

		goto leave_cleanup;
	}

	if (stat_generic(apppath.c_str(), &fst) != 0) {

		goto leave_cleanup;
	}

	char** strs = NULL;
	if (afc_get_file_info(afc, PKG_PATH, &strs) != AFC_E_SUCCESS) {
		if (afc_make_directory(afc, PKG_PATH) != AFC_E_SUCCESS) {
			fprintf(stderr, "WARNING: Could not create directory '%s' on device!\n", PKG_PATH);
		}
	}
	if (strs) {
		int i = 0;
		while (strs[i]) {
			free(strs[i]);
			i++;
		}
		free(strs);
	}

	plist_t client_opts = instproxy_client_options_new();
	/* open install package */
	int errp = 0;
	struct zip* zf = NULL;



	zf = utils::windows_open(apppath.c_str());
	if (!zf) {
		goto leave_cleanup;
	}

	/* extract iTunesMetadata.plist from package */
	char* zbuf = NULL;
	uint32_t len = 0;
	plist_t meta_dict = NULL;
	if (utils::zip_get_contents(zf, ITUNES_METADATA_PLIST_FILENAME, 0, &zbuf, &len) == 0) {
		meta = plist_new_data(zbuf, len);
		if (memcmp(zbuf, "bplist00", 8) == 0) {
			plist_from_bin(zbuf, len, &meta_dict);
		}
		else {
			plist_from_xml(zbuf, len, &meta_dict);
		}
	}
	else {
		fprintf(stderr, "WARNING: could not locate %s in archive!\n", ITUNES_METADATA_PLIST_FILENAME);
	}
	free(zbuf);

	/* determine .app directory in archive */
	zbuf = NULL;
	len = 0;
	plist_t info = NULL;
	char* app_directory_name = NULL;

	if (utils::zip_get_app_directory(zf, &app_directory_name)) {
		fprintf(stderr, "Unable to locate app directory in archive!\n");
		goto leave_cleanup;
	}

	/* construct full filename to Info.plist */
	filename = app_directory_name;

	free(app_directory_name);
	app_directory_name = NULL;
	filename += "Info.plist";

	if (utils::zip_get_contents(zf, filename.c_str(), 0, &zbuf, &len) < 0) {
		zip_unchange_all(zf);
		zip_close(zf);
		goto leave_cleanup;
	}

	if (memcmp(zbuf, "bplist00", 8) == 0) {
		plist_from_bin(zbuf, len, &info);
	}
	else {
		plist_from_xml(zbuf, len, &info);
	}
	free(zbuf);

	if (!info) {
		// "Could not parse Info.plist!
		zip_unchange_all(zf);
		zip_close(zf);
		goto leave_cleanup;
	}

	char* bundleexecutable = NULL;

	plist_t bname = plist_dict_get_item(info, "CFBundleExecutable");
	if (bname) {
		plist_get_string_val(bname, &bundleexecutable);
	}

	bname = plist_dict_get_item(info, "CFBundleIdentifier");
	if (bname) {
		plist_get_string_val(bname, &bundleidentifier);
	}
	plist_free(info);
	info = NULL;

	if (!bundleexecutable) {
		fprintf(stderr, "Could not determine value for CFBundleExecutable!\n");
		zip_unchange_all(zf);
		zip_close(zf);
		goto leave_cleanup;
	}

	char* sinfname = NULL;
	if (asprintf(&sinfname, "Payload/%s.app/SC_Info/%s.sinf", bundleexecutable, bundleexecutable) < 0) {
		fprintf(stderr, "Out of memory!?\n");
		goto leave_cleanup;
	}
	free(bundleexecutable);

	/* extract .sinf from package */
	zbuf = NULL;
	len = 0;
	if (utils::zip_get_contents(zf, sinfname, 0, &zbuf, &len) == 0) {
		sinf = plist_new_data(zbuf, len);
	}
	else {
		//fprintf(stderr, "WARNING: could not locate %s in archive!\n", sinfname);
	}
	free(sinfname);
	free(zbuf);

	/* copy archive to device */
	//pkgname = "PublicStaging/com.ss.iphone.ugc.Aweme";
	pkgname = NULL;
	if (asprintf(&pkgname, "%s/%s", PKG_PATH, bundleidentifier) < 0) {
		//fprintf(stderr, "Out of memory!?\n");
		goto leave_cleanup;
	}

	if (utils::afc_upload_file(afc, SOUI::S_CW2A(apppath.c_str()), pkgname) < 0) {
		free(pkgname);
		goto leave_cleanup;
	}

	//("DONE.\n");

	if (bundleidentifier) {
		instproxy_client_options_add(client_opts, "CFBundleIdentifier", bundleidentifier, NULL);

	}
	if (sinf) {
		instproxy_client_options_add(client_opts, "ApplicationSINF", sinf, NULL);
	}
	if (meta) {
		instproxy_client_options_add(client_opts, "iTunesMetadata", meta, NULL);
	}
	if (zf) {
		zip_unchange_all(zf);
		zip_close(zf);
	}
	if (bInstall)
		instproxy_install(ipc, pkgname, client_opts, ::installstatus_cb, NULL);
	else
		instproxy_upgrade(ipc, pkgname, client_opts, ::installstatus_cb, NULL);
	instproxy_client_options_free(client_opts);
	free(pkgname);

	appIstallcv.wait(lck);

leave_cleanup:
	free(bundleidentifier);
	lockdownd_service_descriptor_free(service);
	np_client_free(np);
	instproxy_client_free(ipc);
	lockdownd_client_free(updataAppsClient);
}

void CiOSDevice::StartGetContacts()
{
	if (m_bUpdataContacts)
		return;
	if (m_workThread[Thread_Contacts].joinable())
		m_workThread[Thread_Contacts].join();
	m_workThread[Thread_Contacts] = std::thread(&CiOSDevice::_SyncContacts, this);
}

bool GetContactId(char* strkey, int& id, int& idx)
{
	//<key>3/720/0</key>
	SASSERT(strkey);
	if (!strkey)
		return false;
	std::vector<std::string> out;
	std::string key = strkey;
	utils::SplitString(key, out, "/");
	SASSERT(out.size() == 3);
	if (out.size() != 3)
		return false;
	id = atoi(out[1].c_str());
	idx = atoi(out[2].c_str());
	return true;
}

void GetPhoneNumber(plist_t val, SOUI::SStringW& phoneNumber, PhoneType& type)
{
	/*
	<dict>
		<key>value</key>
		<string>62581</string>
		<key>com.apple.syncservices.RecordEntityName</key>
		<string>com.apple.contacts.Phone Number</string>
		<key>type</key>
		<string>mobile</string>
		<key>contact</key>
		<array>
			<string>720</string>
		</array>
	</dict>
	*/
	plist_t phoneNumberNode = plist_dict_get_item(val, "value");
	if (phoneNumberNode)
	{
		char* strPhoneNumber = NULL;
		plist_get_string_val(phoneNumberNode, &strPhoneNumber);
		if (strPhoneNumber)
		{
			phoneNumber = SOUI::S_CA2W(strPhoneNumber, CP_UTF8);
			free(strPhoneNumber);
		}
	}
	plist_t typeNode = plist_dict_get_item(val, "type");
	if (typeNode)
	{
		char* strPhoneType = NULL;
		plist_get_string_val(typeNode, &strPhoneType);
		if (strPhoneType)
		{
			type = utils::StringToPhoneType(strPhoneType);
			free(strPhoneType);
		}
	}
}

bool GetContactImg(plist_t val, SOUI::CAutoRefPtr<SOUI::IBitmap>& m_img)
{
	plist_t ContactImgNode = plist_dict_get_item(val, "image");
	if (ContactImgNode)
	{
		char* imgdata = NULL;
		uint64_t len;
		plist_get_data_val(ContactImgNode, &imgdata, &len);
		if (imgdata)
		{
			m_img.Attach(SOUI::SResLoadFromMemory::LoadImage(imgdata, (uint32_t)len));
			free(imgdata);
			return true;
		}
	}
	return false;
}

void GetName(plist_t val, SStringT& outLastName, SStringT& outFirstName)
{
	/*		<dict>
			<key>display as company</key>
			<string>person</string>
			<key>last name</key>
			<string>x</string>
			<key>com.apple.syncservices.RecordEntityName</key>
			<string>com.apple.contacts.Contact</string>
			<key>first name</key>
			<string>xx</string>
		</dict>
	*/
	plist_t lastNameNode = plist_dict_get_item(val, "last name");
	if (lastNameNode)
	{
		char* strLastName = NULL;
		plist_get_string_val(lastNameNode, &strLastName);
		if (strLastName)
		{
			outLastName = SOUI::S_CA2W(strLastName, CP_UTF8);
			free(strLastName);
		}
	}
	plist_t firstNameNode = plist_dict_get_item(val, "first name");
	if (firstNameNode)
	{
		char* strFirstName = NULL;
		plist_get_string_val(firstNameNode, &strFirstName);
		if (strFirstName)
		{
			outFirstName = SOUI::S_CA2W(strFirstName, CP_UTF8);
			free(strFirstName);
		}
	}
}

bool CiOSDevice::EnableSyncContacts()
{
	
	char** classes = NULL;
	int count = 0;
	lockdownd_get_sync_data_classes(m_client, &classes, &count);
	bool bRet = false;
	for (int i = 0; i < count; i++) {
		if (strcmp(classes[i], "com.apple.Contacts") == 0)
		{
			bRet = true;
			break;
		}
	}
	lockdownd_data_classes_free(classes);
	return bRet;
		
	//plist_t syncContactsNode = NULL;
	//bool bRet = false;
	//if (LOCKDOWN_E_SUCCESS == lockdownd_get_value(m_client, "com.apple.mobile.tethered_sync", "Contacts", &syncContactsNode))
	//{
	//	//DisableTethered
	//	plist_t disable = plist_dict_get_item(syncContactsNode, "DisableTethered");
	//	if (disable)
	//	{
	//		uint8_t value = 0;
	//		plist_get_bool_val(disable, &value);
	//		bRet = (1 == value);
	//	}
	//}
	//plist_free(syncContactsNode);
	return bRet;
}

void CiOSDevice::_SyncContacts()
{
	auto_bool_value flag(m_bUpdataContacts);
	mobilesync_client_t client = NULL;
	EventUpdataContacts* e = new EventUpdataContacts(NULL);
	e->udid = m_iosInfo.m_strDevUDID;
	e->udid.MakeLower();
	e->bRet = false;
	if (!EnableSyncContacts())
	{
		goto clear;
	}

	
	if (MOBILESYNC_E_SUCCESS != mobilesync_client_start_service(m_device, &client, "MobileSync"))
	{
		goto clear;
	}

	mobilesync_sync_type_t type;
	char* errstr = NULL;
	mobilesync_anchors_t anchors;

	mobilesync_anchors_new(NULL, "ContactsAnchors", &anchors);
	uint64_t devicever;
	mobilesync_error_t error = mobilesync_start(client, "com.apple.Contacts", anchors, 106, &type, &devicever, &errstr);
	mobilesync_anchors_free(anchors);
	free(errstr);
	if (error != MOBILESYNC_E_SUCCESS)
		goto clear;
	if (mobilesync_get_all_records_from_device(client) != MOBILESYNC_E_SUCCESS)
		goto clear;
	uint8_t is_last_record = 0;
	
	do {
		plist_t entities = NULL;
		error = mobilesync_receive_changes(client, &entities, &is_last_record, NULL);
		if (error != MOBILESYNC_E_SUCCESS)
			goto clear;

#if _DEBUG
		SOUI::SStringW wxml;
		uint32_t len;
		char* xml = NULL;
		plist_to_xml(entities, &xml, &len);
		wxml = SOUI::S_CA2W(xml, CP_UTF8);
		free(xml);
#endif

		
		plist_dict_iter iter = NULL;
		plist_dict_new_iter(entities, &iter);
		plist_t val = NULL;
		do {
			char* key = NULL;
			plist_dict_next_item(entities, iter, &key, &val);
			if (val && (PLIST_DICT == plist_get_node_type(val)))
			{
				//"com.apple.syncservices.RecordEntityName"
				plist_t RecordEntityName = plist_dict_get_item(val, "com.apple.syncservices.RecordEntityName");
				if (RecordEntityName)
				{
					char* strRecordEntityName = NULL;
					plist_get_string_val(RecordEntityName, &strRecordEntityName);
					if (strRecordEntityName)
					{
						//姓名等基本信息
						//com.apple.contacts.Contact
						//电话信息
						//com.apple.contacts.Phone Number
						//电子邮件信息
						//com.apple.contacts.Email Address
						//电子邮件信息
						//com.apple.contacts.URL
						//生日
						//com.apple.contacts.Date
						//即时消息
						//com.apple.contacts.IM
						//地址
						//com.apple.contacts.Street Address
						if (_stricmp("com.apple.contacts.Contact", strRecordEntityName) == 0)
						{
							e->contacts[atoi(key)];
							ContactInfo& contactInfo = e->contacts[atoi(key)];
							GetName(val, contactInfo.LastName, contactInfo.FirstName);
							GetContactImg(val, contactInfo.m_img);
						}
						if (_stricmp("com.apple.contacts.Phone Number", strRecordEntityName) == 0)
						{
							int id, idx;
							if (GetContactId(key, id, idx))
							{
								ContactInfo& contactInfo = e->contacts[id];
								SOUI::SStringW phoneNumber;
								PhoneType type;
								GetPhoneNumber(val, phoneNumber, type);
								contactInfo.PhoneNumber.push_back(phoneNumber);
							}
						}
						if (_stricmp("com.apple.contacts.Email Address", strRecordEntityName) == 0)
						{

						}
						if (_stricmp("com.apple.contacts.URL", strRecordEntityName) == 0)
						{

						}
						if (_stricmp("com.apple.contacts.Date", strRecordEntityName) == 0)
						{

						}
						if (_stricmp("com.apple.contacts.IM", strRecordEntityName) == 0)
						{

						}
						if (_stricmp("com.apple.contacts.Street Address", strRecordEntityName) == 0)
						{

						}
						free(strRecordEntityName);
					}
				}
			}
			if (key)
				free(key);
		} while (val);
		plist_free(entities);

		if (mobilesync_acknowledge_changes_from_device(client) != MOBILESYNC_E_SUCCESS)
			goto clear;
	} while (!is_last_record);
	mobilesync_finish(client);
	e->bRet = true;
clear:
	SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
	e->Release();
	mobilesync_client_free(client);
}

void ChangedSync()
{
	mobilesync_client_t client = NULL;
	//if (MOBILESYNC_E_SUCCESS != mobilesync_client_start_service(m_device, &client, "MobileSync"))
	{
		goto clear;
	}
	/*mobilesync_sync_type_t type;
	char* errstr = NULL;
	mobilesync_anchors_t anchors;
	mobilesync_anchors_new(NULL, "ContactsAnchors", &anchors);
	uint64_t devicever;
	mobilesync_error_t error = mobilesync_start(client, "com.apple.Contacts", anchors, 106, &type, &devicever, &errstr);
	mobilesync_anchors_free(anchors);
	free(errstr);
	if (error != MOBILESYNC_E_SUCCESS)
		goto clear;*/

	mobilesync_error_t error = mobilesync_ready_to_send_changes_from_computer(client);
		if (error != MOBILESYNC_E_SUCCESS) {
		goto clear;
	}
		int number_of_changed = 10;
		for (int i = 0; i < number_of_changed; i++) {
			plist_t actions = mobilesync_actions_new();
			mobilesync_actions_add(actions, "SyncDeviceLinkEntityNamesKey", "com.apple.Contacts", 1,"SyncDeviceLinkAllRecordsOfPulledEntityTypeSentKey", 1, NULL);
			error = mobilesync_send_changes(client, "com.apple.Contacts", i == (number_of_changed - 1) ? 0 : 1, actions);
			mobilesync_actions_free(actions);
			//error = mobilesync_remap_identifiers(client, &remapping);
	}
	//mobilesync_finish(client);
	clear:;
}