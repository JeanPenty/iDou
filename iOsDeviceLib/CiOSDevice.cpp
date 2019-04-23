#include "pch.h"
#include "CiOSDevice.h"
#include "../../libimobiledevice-vs/libimobiledevice/common/utils.c"
#include "iOSUtils.h"


CiOSDevice::CiOSDevice()
{
}


CiOSDevice::~CiOSDevice()
{
}

bool CiOSDevice::OpenDevice(LPCSTR udid)
{
	if (udid)
	{
		if (idevice_new(&m_device, udid) == IDEVICE_E_SUCCESS)
		{
			return lockdownd_client_new_with_handshake(m_device, &m_client, "my_iOSDevice") == LOCKDOWN_E_SUCCESS;
		}		
	}
	return false;
}

void CiOSDevice::CloseDevice()
{
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

bool CiOSDevice::GetDeviceInfo()
{
	if (!IsOpen())
		return false;
	
	plist_t node = NULL; char* key = NULL;
	char* xml_doc = NULL;
	int i = 0;
	FILE *out;
	out=fopen("e:\\abc.txt", "w+");
	while (domains[i] != NULL) {
		if (lockdownd_get_value(m_client, domains[i++], NULL, &node) == LOCKDOWN_E_SUCCESS) {
			if (node) {
				plist_print_to_stream(node, out);
				//plist_to_xml(node, &xml_doc, &xml_length);
				//printf("%s", xml_doc);
				//free(xml_doc);
				plist_free(node);
				node = NULL;
			}
		}
	}
	fclose(out);
	return true;
}

void CiOSDevice::idevice_event_cb_t(const idevice_event_t* event, void* user_data)
{
	IDeviceEventCallBack* reCallBack = (IDeviceEventCallBack*)user_data;
	if (reCallBack)
		reCallBack->idevice_event_cb_t(event);
}

idevice_error_t CiOSDevice::SetCallBack(IDeviceEventCallBack* relCallBack)
{
	idevice_event_unsubscribe();
	return idevice_event_subscribe(idevice_event_cb_t, (void*)relCallBack);
}

//static std::set<std::string> g_pairDeviceList;
//

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
