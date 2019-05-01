//stamp:1251d5a7537a6889
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#pragma once
#include <res.mgr/snamedvalue.h>
#define ROBJ_IN_CPP \
namespace SOUI \
{\
    const _R R;\
    const _UIRES UIRES;\
}
namespace SOUI
{
	class _UIRES{
		public:
		class _UIDEF{
			public:
			_UIDEF(){
				XML_INIT = _T("UIDEF:XML_INIT");
			}
			const TCHAR * XML_INIT;
		}UIDEF;
		class _LAYOUT{
			public:
			_LAYOUT(){
				XML_MAINWND = _T("LAYOUT:XML_MAINWND");
				XML_page = _T("LAYOUT:XML_page");
				XML_MyDevice = _T("LAYOUT:XML_MyDevice");
				XML_MyDevice_Info = _T("LAYOUT:XML_MyDevice_Info");
				XML_dlg_battry = _T("LAYOUT:XML_dlg_battry");
			}
			const TCHAR * XML_MAINWND;
			const TCHAR * XML_page;
			const TCHAR * XML_MyDevice;
			const TCHAR * XML_MyDevice_Info;
			const TCHAR * XML_dlg_battry;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
				template_cpp = _T("values:template");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
			const TCHAR * template_cpp;
		}values;
		class _IMG{
			public:
			_IMG(){
				traymenu_icons = _T("IMG:traymenu_icons");
				png_tab_main = _T("IMG:png_tab_main");
				png_page_icons = _T("IMG:png_page_icons");
				png_logo = _T("IMG:png_logo");
				png_mydevice = _T("IMG:png_mydevice");
				png_bell = _T("IMG:png_bell");
				png_skinbtn = _T("IMG:png_skinbtn");
				png_connect_dev = _T("IMG:png_connect_dev");
				png_pro_pc_apps = _T("IMG:png_pro_pc_apps");
				png_menu_bk = _T("IMG:png_menu_bk");
				png_icon_list = _T("IMG:png_icon_list");
				png_icon_article = _T("IMG:png_icon_article");
				png_iphonescreen = _T("IMG:png_iphonescreen");
				png_ipadscreen = _T("IMG:png_ipadscreen");
			}
			const TCHAR * traymenu_icons;
			const TCHAR * png_tab_main;
			const TCHAR * png_page_icons;
			const TCHAR * png_logo;
			const TCHAR * png_mydevice;
			const TCHAR * png_bell;
			const TCHAR * png_skinbtn;
			const TCHAR * png_connect_dev;
			const TCHAR * png_pro_pc_apps;
			const TCHAR * png_menu_bk;
			const TCHAR * png_icon_list;
			const TCHAR * png_icon_article;
			const TCHAR * png_iphonescreen;
			const TCHAR * png_ipadscreen;
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
		class _SMENU{
			public:
			_SMENU(){
				menu_skin = _T("SMENU:menu_skin");
				menu_tray = _T("SMENU:menu_tray");
			}
			const TCHAR * menu_skin;
			const TCHAR * menu_tray;
		}SMENU;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"BaseInfoWnd",65558},
		{L"Name",65548},
		{L"Txt_Tip",65552},
		{L"_name_start",65535},
		{L"btn_",65573},
		{L"btn_batteryInfo",65571},
		{L"btn_close",65544},
		{L"btn_max",65542},
		{L"btn_min",65541},
		{L"btn_mydevice",65536},
		{L"btn_reboot",65555},
		{L"btn_restore",65543},
		{L"btn_shutdown",65556},
		{L"btn_skin",65540},
		{L"btn_sleep",65557},
		{L"devname",65554},
		{L"disk_app",65575},
		{L"disk_av",65577},
		{L"disk_free",65580},
		{L"disk_other",65579},
		{L"disk_photo",65576},
		{L"disk_sys",65574},
		{L"disk_u",65578},
		{L"face",65549},
		{L"img_srceenshot",65553},
		{L"lable_ActivationState",65561},
		{L"lable_ActuallyCapacity",65581},
		{L"lable_BluetoothAddress",65566},
		{L"lable_CycleCount",65569},
		{L"lable_CycleLife",65570},
		{L"lable_DesignCapacity",65582},
		{L"lable_FirmwareVersion",65560},
		{L"lable_HardwareModel",65567},
		{L"lable_IMEI",65568},
		{L"lable_ModelNumber",65564},
		{L"lable_ProductName",65559},
		{L"lable_ProductType",65562},
		{L"lable_SerialNumber",65563},
		{L"lable_UDID",65572},
		{L"lable_WiFiAddress",65565},
		{L"nav_dev_cmd",65550},
		{L"tab2",65537},
		{L"tab3",65538},
		{L"tab4",65539},
		{L"tab_dev_cmd",65551},
		{L"tab_dev_info",65546},
		{L"tab_main",65545},
		{L"tv_ios",65547}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			BaseInfoWnd = namedXmlID[0].strName;
			Name = namedXmlID[1].strName;
			Txt_Tip = namedXmlID[2].strName;
			_name_start = namedXmlID[3].strName;
			btn_ = namedXmlID[4].strName;
			btn_batteryInfo = namedXmlID[5].strName;
			btn_close = namedXmlID[6].strName;
			btn_max = namedXmlID[7].strName;
			btn_min = namedXmlID[8].strName;
			btn_mydevice = namedXmlID[9].strName;
			btn_reboot = namedXmlID[10].strName;
			btn_restore = namedXmlID[11].strName;
			btn_shutdown = namedXmlID[12].strName;
			btn_skin = namedXmlID[13].strName;
			btn_sleep = namedXmlID[14].strName;
			devname = namedXmlID[15].strName;
			disk_app = namedXmlID[16].strName;
			disk_av = namedXmlID[17].strName;
			disk_free = namedXmlID[18].strName;
			disk_other = namedXmlID[19].strName;
			disk_photo = namedXmlID[20].strName;
			disk_sys = namedXmlID[21].strName;
			disk_u = namedXmlID[22].strName;
			face = namedXmlID[23].strName;
			img_srceenshot = namedXmlID[24].strName;
			lable_ActivationState = namedXmlID[25].strName;
			lable_ActuallyCapacity = namedXmlID[26].strName;
			lable_BluetoothAddress = namedXmlID[27].strName;
			lable_CycleCount = namedXmlID[28].strName;
			lable_CycleLife = namedXmlID[29].strName;
			lable_DesignCapacity = namedXmlID[30].strName;
			lable_FirmwareVersion = namedXmlID[31].strName;
			lable_HardwareModel = namedXmlID[32].strName;
			lable_IMEI = namedXmlID[33].strName;
			lable_ModelNumber = namedXmlID[34].strName;
			lable_ProductName = namedXmlID[35].strName;
			lable_ProductType = namedXmlID[36].strName;
			lable_SerialNumber = namedXmlID[37].strName;
			lable_UDID = namedXmlID[38].strName;
			lable_WiFiAddress = namedXmlID[39].strName;
			nav_dev_cmd = namedXmlID[40].strName;
			tab2 = namedXmlID[41].strName;
			tab3 = namedXmlID[42].strName;
			tab4 = namedXmlID[43].strName;
			tab_dev_cmd = namedXmlID[44].strName;
			tab_dev_info = namedXmlID[45].strName;
			tab_main = namedXmlID[46].strName;
			tv_ios = namedXmlID[47].strName;
		}
		 const wchar_t * BaseInfoWnd;
		 const wchar_t * Name;
		 const wchar_t * Txt_Tip;
		 const wchar_t * _name_start;
		 const wchar_t * btn_;
		 const wchar_t * btn_batteryInfo;
		 const wchar_t * btn_close;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_mydevice;
		 const wchar_t * btn_reboot;
		 const wchar_t * btn_restore;
		 const wchar_t * btn_shutdown;
		 const wchar_t * btn_skin;
		 const wchar_t * btn_sleep;
		 const wchar_t * devname;
		 const wchar_t * disk_app;
		 const wchar_t * disk_av;
		 const wchar_t * disk_free;
		 const wchar_t * disk_other;
		 const wchar_t * disk_photo;
		 const wchar_t * disk_sys;
		 const wchar_t * disk_u;
		 const wchar_t * face;
		 const wchar_t * img_srceenshot;
		 const wchar_t * lable_ActivationState;
		 const wchar_t * lable_ActuallyCapacity;
		 const wchar_t * lable_BluetoothAddress;
		 const wchar_t * lable_CycleCount;
		 const wchar_t * lable_CycleLife;
		 const wchar_t * lable_DesignCapacity;
		 const wchar_t * lable_FirmwareVersion;
		 const wchar_t * lable_HardwareModel;
		 const wchar_t * lable_IMEI;
		 const wchar_t * lable_ModelNumber;
		 const wchar_t * lable_ProductName;
		 const wchar_t * lable_ProductType;
		 const wchar_t * lable_SerialNumber;
		 const wchar_t * lable_UDID;
		 const wchar_t * lable_WiFiAddress;
		 const wchar_t * nav_dev_cmd;
		 const wchar_t * tab2;
		 const wchar_t * tab3;
		 const wchar_t * tab4;
		 const wchar_t * tab_dev_cmd;
		 const wchar_t * tab_dev_info;
		 const wchar_t * tab_main;
		 const wchar_t * tv_ios;
		}name;

		class _id{
		public:
		const static int BaseInfoWnd	=	65558;
		const static int Name	=	65548;
		const static int Txt_Tip	=	65552;
		const static int _name_start	=	65535;
		const static int btn_	=	65573;
		const static int btn_batteryInfo	=	65571;
		const static int btn_close	=	65544;
		const static int btn_max	=	65542;
		const static int btn_min	=	65541;
		const static int btn_mydevice	=	65536;
		const static int btn_reboot	=	65555;
		const static int btn_restore	=	65543;
		const static int btn_shutdown	=	65556;
		const static int btn_skin	=	65540;
		const static int btn_sleep	=	65557;
		const static int devname	=	65554;
		const static int disk_app	=	65575;
		const static int disk_av	=	65577;
		const static int disk_free	=	65580;
		const static int disk_other	=	65579;
		const static int disk_photo	=	65576;
		const static int disk_sys	=	65574;
		const static int disk_u	=	65578;
		const static int face	=	65549;
		const static int img_srceenshot	=	65553;
		const static int lable_ActivationState	=	65561;
		const static int lable_ActuallyCapacity	=	65581;
		const static int lable_BluetoothAddress	=	65566;
		const static int lable_CycleCount	=	65569;
		const static int lable_CycleLife	=	65570;
		const static int lable_DesignCapacity	=	65582;
		const static int lable_FirmwareVersion	=	65560;
		const static int lable_HardwareModel	=	65567;
		const static int lable_IMEI	=	65568;
		const static int lable_ModelNumber	=	65564;
		const static int lable_ProductName	=	65559;
		const static int lable_ProductType	=	65562;
		const static int lable_SerialNumber	=	65563;
		const static int lable_UDID	=	65572;
		const static int lable_WiFiAddress	=	65565;
		const static int nav_dev_cmd	=	65550;
		const static int tab2	=	65537;
		const static int tab3	=	65538;
		const static int tab4	=	65539;
		const static int tab_dev_cmd	=	65551;
		const static int tab_dev_info	=	65546;
		const static int tab_main	=	65545;
		const static int tv_ios	=	65547;
		}id;

		class _string{
		public:
		const static int ActivationState	=	0;
		const static int ActuallyCapacity	=	1;
		const static int BattaryInfo	=	2;
		const static int BluetoothAddress	=	3;
		const static int CycleCount	=	4;
		const static int CycleLife	=	5;
		const static int DesignCapacity	=	6;
		const static int FirmwareVersion	=	7;
		const static int HardwareModel	=	8;
		const static int IMEI	=	9;
		const static int ModelNumber	=	10;
		const static int ProductType	=	11;
		const static int SerialNumber	=	12;
		const static int ShowDevCheck	=	13;
		const static int ShowDevInfo	=	14;
		const static int WiFiAddress	=	15;
		const static int app	=	16;
		const static int av	=	17;
		const static int cantread	=	18;
		const static int cmd1	=	19;
		const static int cmd2	=	20;
		const static int cmd3	=	21;
		const static int cmd4	=	22;
		const static int cmd5	=	23;
		const static int free	=	24;
		const static int noname	=	25;
		const static int other	=	26;
		const static int photo	=	27;
		const static int system	=	28;
		const static int title	=	29;
		const static int udisk	=	30;
		const static int ver	=	31;
		const static int waitpair	=	32;
		}string;

		class _color{
		public:
		const static int black	=	0;
		const static int blue	=	1;
		const static int disk_app	=	2;
		const static int disk_av	=	3;
		const static int disk_free	=	4;
		const static int disk_other	=	5;
		const static int disk_photo	=	6;
		const static int disk_sys	=	7;
		const static int disk_u	=	8;
		const static int gray	=	9;
		const static int green	=	10;
		const static int infoborder	=	11;
		const static int infotabbknor	=	12;
		const static int infotabbkpush	=	13;
		const static int mainbk	=	14;
		const static int red	=	15;
		const static int white	=	16;
		}color;

	};

#ifdef R_IN_CPP
	 extern const _R R;
	 extern const _UIRES UIRES;
#else
	 extern const __declspec(selectany) _R & R = _R();
	 extern const __declspec(selectany) _UIRES & UIRES = _UIRES();
#endif//R_IN_CPP
}
