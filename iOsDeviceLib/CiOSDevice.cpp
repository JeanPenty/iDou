#include "CiOSDevice.h"
#include "CiOSDevice.h"
#include "CiOSDevice.h"
#include "CiOSDevice.h"
#include "CiOSDevice.h"
#include "CiOSDevice.h"
#include "pch.h"
#include "CiOSDevice.h"
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

EXTERN_C{
#include "base64.h"
}

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
			return lockdownd_client_new_with_handshake(m_device, &m_client, "my_iOSDevice") == LOCKDOWN_E_SUCCESS;
		}
	}
	return false;
}

void CiOSDevice::CloseDevice()
{
	if (m_capThread[Thread_Cap].joinable())
	{
		m_bCap = false;
		m_capThread[Thread_Cap].join();
	}
	if (m_capThread[Thread_Updata].joinable())
	{
		m_bUpdata = false;
		m_capThread[Thread_Updata].join();
	}
	if (m_capThread[Thread_UpdataBattreyInfo].joinable())
	{
		m_bUpdataBattreyInfo = false;
		m_capThread[Thread_UpdataBattreyInfo].join();
	}
	if (m_capThread[Thread_UpdataDiskInfo].joinable())
	{
		m_bUpdataDiskInfo = false;
		m_capThread[Thread_UpdataDiskInfo].join();
	}
	if (m_capThread[Thread_UpdataAppsInfo].joinable())
		m_capThread[Thread_UpdataAppsInfo].join();

	if (m_device)
	{
		idevice_free(m_device);
		m_device = NULL;
	}
	if (m_client)
	{
		lockdownd_client_free(m_client);
		m_client = NULL;
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

bool CiOSDevice::_GetDiskAddress(lockdownd_client_t client, DiskInfo & outAddress)
{
	plist_t address_node = NULL;
	outAddress.bReady = false;
	if (LOCKDOWN_E_SUCCESS == lockdownd_get_value(client, domains[1], NULL, &address_node))
	{
		outAddress.bReady = true;

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

bool CiOSDevice::GetWifiAddress(SStringT & outMac)
{
	if (m_iosInfo.m_strDevWiFiAddress.IsEmpty())
	{
		if (!_GetAddress(m_iosInfo.m_strDevWiFiAddress, NODE_WIFI_ADDRESS))
			return false;
	}
	outMac = m_iosInfo.m_strDevWiFiAddress;
	return true;
}

bool CiOSDevice::GetBluetoothAddress(SStringT & outMac)
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
		if (lockdownd_set_value(m_client, NULL, "DeviceName", plist_new_string(strU8Name)) == LOCKDOWN_E_SUCCESS) {
			m_iosInfo.m_strDevName = newName;
			return true;
		}
	}
	return false;
}

bool _GetAddress(lockdownd_client_t client, SOUI::SStringA & outAddress, LPCSTR nodename)
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
					while (iter2)
					{
						plist_t val = NULL;
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
						else
							break;
					}
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

	m_iosInfo.m_bIsJailreak = _IsJailreak();
	//
#ifdef _DEBUG
	/*int i = 0;
	FILE* out;
	out = fopen("e:\\abc.txt", "w+");
	while (domains[i] != NULL) {
		plist_t node = NULL;
		if (lockdownd_get_value(m_client, domains[i], "NANDInfo", &node) == LOCKDOWN_E_SUCCESS) {
			if (node) {
				fprintf(out, "-----------%s-------------\n", domains[i]);
				plist_print_to_stream(node, out);
				plist_free(node);
				node = NULL;
			}
		}
		++i;
	}
	fclose(out);*/

	/*
	{
		FILE* outbin;
		outbin = fopen("e:\\bcd.bin", "w+");
		plist_t node = NULL;
		if (lockdownd_get_value(m_client, domains[1], "NANDInfo", &node) == LOCKDOWN_E_SUCCESS) {
			if (node) {
				char* test = NULL;
				uint64_t len;
				plist_get_data_val(node, &test, &len);
				fwrite(test, 1, len, outbin);
				//size_t len2=0;
				//char* test2=(char*)base64decode(test,&len2);
				free(test);
				//free(test2);
				//char* xml;
				//uint32_t len3;
				//plist_to_xml(node, &xml, &len3);
				//free(xml);
				plist_free(node);
				node = NULL;
			}
		}
		fclose(outbin);
	}*/

#endif // DEBUG

	return true;
}

void CiOSDevice::StartUpdata()
{
	if (!(m_capThread[Thread_Updata].joinable()))
		m_capThread[Thread_Updata] = std::thread(&CiOSDevice::_Updata, this);
}

void CiOSDevice::StartUpdataDiskInfo()
{
	if (!(m_capThread[Thread_UpdataDiskInfo].joinable()))
		m_capThread[Thread_UpdataDiskInfo] = std::thread(&CiOSDevice::_UpdataDiskInfo, this);
}

void CiOSDevice::StartCapshot()
{
	if (!(m_capThread[Thread_Cap].joinable()))
		m_capThread[Thread_Cap] = std::thread(&CiOSDevice::_ScreenShot, this);
}

const iOSDevInfo& CiOSDevice::GetiOSBaseInfo()
{
	return m_iosInfo;
}

void idevice_event_cb(const idevice_event_t * event, void* user_data)
{
	IDeviceEventCallBack* reCallBack = (IDeviceEventCallBack*)user_data;
	if (reCallBack)
		reCallBack->idevice_event_cb_t(event);
}

idevice_error_t CiOSDevice::SetCallBack(IDeviceEventCallBack * relCallBack)
{
	idevice_event_unsubscribe();
	return idevice_event_subscribe(idevice_event_cb, (void*)relCallBack);
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

bool CiOSDevice::GetiOSDeviceGUIDList(std::vector<std::string> & iosList)
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
				/*case CMD_MOBILEGESTALT:
					if (diagnostics_relay_query_mobilegestalt(diagnostics_client, keys, &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
						bRet = true;
						if (node) {
							//print_xml(node);
							//result = EXIT_SUCCESS;
						}
					}
					break;
			case CMD_IOREGISTRY_ENTRY:
				if (diagnostics_relay_query_ioregistry_entry(diagnostics_client, "", "", &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
					if (node) {
						char* xml = NULL;
						uint32_t len = 0;
						plist_to_xml(node, &xml, &len);
						if (xml) {
							puts(xml);
						}
						free(xml);
						plist_free(node);
						//print_xml(node);
						//result = EXIT_SUCCESS;
					}
				}
				break;
			case CMD_IOREGISTRY:
				if (diagnostics_relay_query_ioregistry_plane(diagnostics_client, "IOPower", &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
					if (node) {
						char* xml = NULL;
						uint32_t len = 0;
						plist_to_xml(node, &xml, &len);
						if (xml) {
							puts(xml);
						}
						free(xml);
						plist_free(node);
					}
				}
				break;
			case CMD_DIAGNOSTICS:
			default:
				if (diagnostics_relay_request_diagnostics(diagnostics_client, "All", &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
					if (node) {
						char* xml = NULL;
						uint32_t len = 0;
						plist_to_xml(node, &xml, &len);
						if (xml) {
							puts(xml);
						}
						free(xml);
						plist_free(node);
					}
				}/**/
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

void CiOSDevice::GetBatteryBaseInfo(BatteryBaseInfo & outasGauge)
{
	outasGauge = m_iosInfo.m_sGasGauge;
}

bool CiOSDevice::_GetBatteryBaseInfo(BatteryBaseInfo & outInfo)
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

			if (diagnostics_relay_query_ioregistry_entry(diagnostics_client, "AppleARMPMUCharger", NULL, &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
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
							utils::getbatteryManufactureDateFormeSN(outInfo.BatterySerialNumber, outInfo.Origin, outInfo.ManufactureDate);
						else
							utils::getbatteryManufactureDateFormeSNOld(outInfo.BatterySerialNumber, outInfo.Origin, outInfo.ManufactureDate);
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
	lockdownd_client_t syncClient = NULL;
	if (lockdownd_client_new_with_handshake(m_device, &syncClient, "my_iOSDeviceScreenShot") != LOCKDOWN_E_SUCCESS)
	{
		return;
	}
	m_bCap = true;
	lockdownd_service_descriptor_t service = NULL;;
	lockdownd_start_service(m_client, "com.apple.mobile.screenshotr", &service);
	lockdownd_client_free(syncClient);

	if (service && service->port > 0) {
		screenshotr_client_t shotr = NULL;
		if (screenshotr_client_new(m_device, service, &shotr) != SCREENSHOTR_E_SUCCESS) {
			goto clear;
		}
		else {
			char* imgdata = NULL;
			uint64_t imgsize = 0;

			while (m_bCap)
			{
				if (screenshotr_take_screenshot(shotr, &imgdata, &imgsize) == SCREENSHOTR_E_SUCCESS) {
					EventScreenShot* pScreenshotEvt = new EventScreenShot(NULL);
					pScreenshotEvt->code = 0;
					pScreenshotEvt->bufsize = imgsize;
					pScreenshotEvt->imgbuf = imgdata;
					SOUI::SNotifyCenter::getSingleton().FireEventAsync(pScreenshotEvt);
					pScreenshotEvt->Release();
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			screenshotr_client_free(shotr);
		}
	}
clear:
	if (service)
		lockdownd_service_descriptor_free(service);
}

void CiOSDevice::_Updata()
{
	lockdownd_client_t updataClient = NULL;
	if (lockdownd_client_new_with_handshake(m_device, &updataClient, "my_iOSDeviceUpdata") != LOCKDOWN_E_SUCCESS)
	{
		return;
	}
	m_bUpdata = true;

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

//Library/AddressBook/AddressBook.sqlitedb
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
bool CiOSDevice::GetBattery(LPCSTR key, T & outValue)
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
	if (!(m_capThread[Thread_UpdataBattreyInfo].joinable()))
		m_capThread[Thread_UpdataBattreyInfo] = std::thread(&CiOSDevice::_UpdataBatteryInfo, this);
}

void CiOSDevice::StopUpdataBatteryInfo()
{
	if (m_capThread[Thread_UpdataBattreyInfo].joinable())
	{
		m_bUpdataBattreyInfo = false;
		m_capThread[Thread_UpdataBattreyInfo].join();
	}
}

void CiOSDevice::_UpdataBatteryInfo()
{
	m_bUpdataBattreyInfo = true;

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
				if (diagnostics_relay_query_ioregistry_entry(diagnostics_client, "AppleARMPMUCharger", "", &node) == DIAGNOSTICS_RELAY_E_SUCCESS) {
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

void CiOSDevice::_UpdataDiskInfo()
{
	m_bUpdataDiskInfo = true;

	lockdownd_client_t updataDiskClient = NULL;
	if (lockdownd_client_new_with_handshake(m_device, &updataDiskClient, "tmpClient") != LOCKDOWN_E_SUCCESS)
	{
		return;
	}
	SOUI::SStringA udid = SOUI::S_CW2A(m_iosInfo.m_strDevUDID);
	udid.MakeLower();
	while (m_bUpdataDiskInfo)
	{
		if (_GetDiskAddress(updataDiskClient, m_iosInfo.m_diskInfo))
		{
			EventUpdataDiskInfo* e = new EventUpdataDiskInfo(NULL);
			e->udid = udid;
			SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
			e->Release();
			break;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	lockdownd_client_free(updataDiskClient);
}

static void notifier(const char* notification, void* unused)
{

}

void CiOSDevice::StartUpdataApps()
{
	if (!(m_capThread[Thread_UpdataAppsInfo].joinable()))
		m_capThread[Thread_UpdataAppsInfo] = std::thread(&CiOSDevice::_UpdataAppsInfo, this);
}

const std::vector<AppInfo>& CiOSDevice::GetApps()
{
	RLocker locker(m_appsLocker);
	return m_apps;
}

bool CiOSDevice::_GetAppIcon(LPCSTR id, char** outBuf, uint64_t & len)
{
	if (id == NULL)
		return false;
	lockdownd_service_descriptor_t service = NULL;
	sbservices_client_t sbclient = NULL;
	if (sbservices_client_start_service(m_device, &sbclient, "geticon") != SBSERVICES_E_SUCCESS)
		return false;

	sbservices_error_t ret = sbservices_get_icon_pngdata(sbclient, id, outBuf, &len);
	sbservices_client_free(sbclient);

	return SBSERVICES_E_SUCCESS == ret;
}

bool CiOSDevice::_GetScreenWallpaper(char** outBuf, uint64_t & len)
{
	lockdownd_service_descriptor_t service = NULL;
	sbservices_client_t sbclient = NULL;
	if (sbservices_client_start_service(m_device, &sbclient, "geticon") != SBSERVICES_E_SUCCESS)
		return false;
	sbservices_error_t ret = sbservices_get_home_screen_wallpaper_pngdata(sbclient, outBuf, &len);
	sbservices_client_free(sbclient);
	return SBSERVICES_E_SUCCESS == ret;
}

void GetAppValue(plist_t appnode, LPCSTR nodename, std::string & outValue)
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

void GetAppValue(plist_t appnode, LPCSTR nodename, SOUI::SStringT & outValue)
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

void GetAppValue(plist_t appnode, LPCSTR nodename, uint64_t & outValue)
{
	SASSERT(appnode);
	SASSERT(nodename);
	plist_t DisplayNameNode = plist_dict_get_item(appnode, nodename);
	if (DisplayNameNode)
	{
		plist_get_uint_val(DisplayNameNode, &outValue);
	}
}

void GetAppValue(plist_t appnode, LPCSTR nodename, char* outValue, uint64_t & datalen)
{
	SASSERT(appnode);
	SASSERT(nodename);
	plist_t DisplayNameNode = plist_dict_get_item(appnode, nodename);
	if (DisplayNameNode)
	{
		plist_get_data_val(DisplayNameNode, &outValue, &datalen);
	}
}

void CiOSDevice::_UpdataApps(plist_t apps)
{
	if (apps)
	{
		{
			WLocker locker(m_appsLocker);
			m_apps.clear();
			/**/char* xml = NULL;
			uint32_t len = 0;
			plist_to_xml(apps, &xml, &len);
			if (xml) {
				SOUI::SStringW wxml = SOUI::S_CA2W(xml, CP_UTF8);
				free(xml);
			}
			uint32_t size = plist_array_get_size(apps);
			for (uint32_t i = 0; i < size; i++)
			{
				plist_t app = plist_array_get_item(apps, i);
				if (app)
				{
					AppInfo appInfo;
					GetAppValue(app, "CFBundleIdentifier", appInfo.AppID);

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

					m_apps.push_back(appInfo);
				}
			}
		}
		EventUpdataAppsInfo* e = new EventUpdataAppsInfo(NULL);
		e->udid = m_iosInfo.m_strDevUDID;
		e->udid.MakeLower();
		SOUI::SNotifyCenter::getSingleton().FireEventAsync(e);
		e->Release();
	}
}

static void status_cb(plist_t command, plist_t status, void* unused)
{
	((CiOSDevice*)unused)->uninstallstatus_cb(command, status);
}

void CiOSDevice::uninstallstatus_cb(plist_t command, plist_t status)
{
	if (command && status) {
		char* command_name = NULL;
		instproxy_command_get_name(command, &command_name);

		/* get status */
		char* status_name = NULL;
		instproxy_status_get_name(status, &status_name);
		bool command_completed = false;
		if (status_name) {
			if (!strcmp(status_name, "Complete")) {
				command_completed = true;
				appUnistallcv.notify_one();
			}
		}

		/* get error if any */
		char* error_name = NULL;
		char* error_description = NULL;
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
			/* report error to the user */
			//if (error_description)
				//fprintf(stderr, "ERROR: %s failed. Got error \"%s\" with code 0x%08"PRIx64": %s\n", command_name, error_name, error_code, error_description ? error_description : "N/A");
			//else
				//fprintf(stderr, "ERROR: %s failed. Got error \"%s\".\n", command_name, error_name);			
		}

		/* clean up */
		free(error_name);
		free(error_description);
		free(command_name);
		command_name = NULL;
	}
	else {
		fprintf(stderr, "ERROR: %s was called with invalid arguments!\n", __func__);
	}
}

void CiOSDevice::_UpdataAppsInfo()
{
	lockdownd_service_descriptor_t service = NULL;
	instproxy_client_t ipc = NULL;
	lockdownd_client_t updataAppsClient = NULL;
	instproxy_error_t err;
	np_client_t np = NULL;

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

	plist_t client_opts = instproxy_client_options_new();
	instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
	plist_t apps = NULL;
	instproxy_client_options_set_return_attributes(client_opts,
		"CFBundleIdentifier",
		"CFBundleDisplayName",
		"CFBundleShortVersionString",
		"StaticDiskUsage",
		"DynamicDiskUsage",
		NULL
	);


	err = instproxy_browse(ipc, client_opts, &apps);
	instproxy_client_options_free(client_opts);
	if (!apps || (plist_get_node_type(apps) != PLIST_ARRAY)) {
		//"ERROR: instproxy_browse returnd an invalid plist!\n");				
		goto leave_cleanup;
	}
	_UpdataApps(apps);
	plist_free(apps);
	/*
	err = instproxy_browse_with_callback(ipc, client_opts, status_cb, NULL);
	if (err == INSTPROXY_E_RECEIVE_TIMEOUT) {
		fprintf(stderr, "NOTE: timeout waiting for device to browse apps, trying again...\n");
	}
	*/

	if (err != INSTPROXY_E_SUCCESS) {
		fprintf(stderr, "ERROR: instproxy_browse returned %d\n", err);
		goto leave_cleanup;
	}
leave_cleanup:
	lockdownd_service_descriptor_free(service);
	np_client_free(np);
	instproxy_client_free(ipc);
	lockdownd_client_free(updataAppsClient);
}

void CiOSDevice::_UninstallAppsInfo(std::string appID)
{
	lockdownd_service_descriptor_t service = NULL;
	instproxy_client_t ipc = NULL;
	lockdownd_client_t updataAppsClient = NULL;
	instproxy_error_t err;
	np_client_t np = NULL;

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

	instproxy_uninstall(ipc, appID.c_str(), NULL, status_cb, this);

	appUnistallcv.wait(lck);

	if (err != INSTPROXY_E_SUCCESS) {
		fprintf(stderr, "ERROR: instproxy_browse returned %d\n", err);
		goto leave_cleanup;
	}
leave_cleanup:
	lockdownd_service_descriptor_free(service);
	np_client_free(np);
	instproxy_client_free(ipc);
	lockdownd_client_free(updataAppsClient);
}