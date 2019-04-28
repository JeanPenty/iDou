#pragma once
#ifndef __IOS_UTILS_HH__
#define __IOS_UTILS_HH__

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

#define NODE_BLUETOOTH_ADDRESS "BluetoothAddress"
#define NODE_WIFI_ADDRESS "WiFiAddress"
#define NODE_SERIALNUMBER "SerialNumber"
#define NODE_MODELNUMBER "ModelNumber"
#define NODE_HARDWAREMODEL "HardwareModel"
#define NODE_DEVICENAME "DeviceName"
#define NODE_IMEI "InternationalMobileEquipmentIdentity"
#define NODE_CPUARC "CPUArchitecture"
#define NODE_PRODUCTTYPE "ProductType"


enum DomainsID
{
	BATTERY = 2,
};
#include <sstream>
#include <common/utils.h>
#include <time.h>
#include <WinSock2.h>
#include <string/tstring.h>

namespace utils
{
	void productType_to_phonename(SOUI::SStringT &productType);
	void plist_print_to_stringstream(plist_t plist, std::stringstream& stream);
}



#endif