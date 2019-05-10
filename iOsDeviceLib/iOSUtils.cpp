#include "pch.h"
#include "iOSUtils.h"

#include <map>
#include <string>
#define __mkdir2(path, mode) CreateDirectoryA(path, NULL)

namespace utils
{

	static void plist_node_print_to_stringstream(plist_t node, int* indent_level, std::stringstream& stream);

	static void plist_array_print_to_stringstream(plist_t node, int* indent_level, std::stringstream& stream)
	{
		/* iterate over items */
		int i, count;
		plist_t subnode = NULL;

		count = plist_array_get_size(node);

		for (i = 0; i < count; i++) {
			subnode = plist_array_get_item(node, i);
			stream << i << ": ";
			plist_node_print_to_stringstream(subnode, indent_level, stream);
			stream << std::endl;
		}
	}

	static void plist_dict_print_to_stringstream(plist_t node, int* indent_level, std::stringstream& stream)
	{
		/* iterate over key/value pairs */
		plist_dict_iter it = NULL;

		char* key = NULL;
		plist_t subnode = NULL;
		plist_dict_new_iter(node, &it);
		plist_dict_next_item(node, it, &key, &subnode);
		while (subnode)
		{
			stream << key;
			if (plist_get_node_type(subnode) == PLIST_ARRAY)
				stream << "[" << plist_array_get_size(subnode) << "]: ";
			else
				stream << ": ";
			free(key);
			key = NULL;
			plist_node_print_to_stringstream(subnode, indent_level, stream);
			plist_dict_next_item(node, it, &key, &subnode);
			if (subnode)
				stream << std::endl;
		}
		free(it);
	}

	static void plist_node_print_to_stringstream(plist_t node, int* indent_level, std::stringstream & stream)
	{
		char* s = NULL;
		char* data = NULL;
		double d;
		uint8_t b;
		uint64_t u = 0;
		struct timeval tv = { 0, 0 };

		plist_type t;

		if (!node)
			return;

		t = plist_get_node_type(node);

		switch (t) {
		case PLIST_BOOLEAN:
			plist_get_bool_val(node, &b);
			stream << (b ? "true" : "false");
			break;

		case PLIST_UINT:
			plist_get_uint_val(node, &u);
			stream << u;
			break;

		case PLIST_REAL:
			plist_get_real_val(node, &d);
			stream << d;
			break;

		case PLIST_STRING:
			plist_get_string_val(node, &s);
			stream << s;
			free(s);
			break;

		case PLIST_KEY:
			plist_get_key_val(node, &s);
			stream << s << ": ";
			free(s);
			break;

		case PLIST_DATA:
			plist_get_data_val(node, &data, &u);
			if (u > 0) {
				s = base64encode((unsigned char*)data, u);
				free(data);
				if (s) {
					stream << s << std::endl;
					free(s);
				}
			}
			break;

		case PLIST_DATE:
			plist_get_date_val(node, (int32_t*)& tv.tv_sec, (int32_t*)& tv.tv_usec);
			{
				time_t ti = (time_t)tv.tv_sec;
				struct tm* btime = localtime(&ti);
				if (btime) {
					s = (char*)malloc(24);
					memset(s, 0, 24);
					if (strftime(s, 24, "%Y-%m-%dT%H:%M:%SZ", btime) <= 0) {
						free(s);
						s = NULL;
					}
				}
			}
			if (s) {
				stream << s;
				free(s);
			}
			break;

		case PLIST_ARRAY:
			stream << std::endl;
			(*indent_level)++;
			plist_array_print_to_stringstream(node, indent_level, stream);
			(*indent_level)--;
			break;

		case PLIST_DICT:
			stream << std::endl;
			(*indent_level)++;
			plist_dict_print_to_stringstream(node, indent_level, stream);
			(*indent_level)--;
			break;

		default:
			break;
		}
	}

	void plist_print_to_stringstream(plist_t plist, std::stringstream & stream)
	{
		int indent = 0;

		if (!plist)
			return;

		stream.clear();

		switch (plist_get_node_type(plist)) {
		case PLIST_DICT:
			plist_dict_print_to_stringstream(plist, &indent, stream);
			break;
		case PLIST_ARRAY:
			plist_array_print_to_stringstream(plist, &indent, stream);
			break;
		default:
			plist_node_print_to_stringstream(plist, &indent, stream);
		}
	}

	std::map<std::wstring, std::wstring> g_mapDevproductType = {
		//iPhone
		{L"iphone1,1",L"iPhone"},
		{L"iphone1,2",L"iPhone3"},
		{L"iphone2,1",L"iPhone3"},
		{L"iphone3,1",L"iPhone 4"},
		{L"iphone3,2", L"iPhone 4"},
		{L"iphone3,3", L"iPhone 4"},
		{L"iphone4,1", L"iPhone 4s"},
		{L"iphone4,2", L"iPhone 4s"},
		{L"iphone4,3", L"iPhone 4s"},
		{L"iphone5,1", L"iPhone 5"},
		{L"iphone5,2", L"iPhone 5"},
		{L"iphone5,3", L"iPhone 5c"},
		{L"iphone5,4", L"iPhone 5c"},
		{L"iphone6,1", L"iPhone 5s"},
		{L"iphone6,2", L"iPhone 5s"},
		{L"iphone7,2", L"iPhone 6"},
		{L"iphone7,1", L"iPhone 6p"},
		{L"iphone8,1", L"iPhone 6s"},
		{L"iphone8,2", L"iPhone 6sp"},
		{L"iphone8,4", L"iPhone se"},
		{L"iphone9,1", L"iPhone 7"},
		{L"iphone9,2", L"iPhone 7p"},
		{L"iphone9,3", L"iPhone 7"},
		{L"iphone9,4", L"iPhone 7p"},
		{L"iphone10.1",L"iPhone 8"},
		{L"iphone10.2",L"iPhone 8 Plus"},
		{L"iphone10.3",L"iPhone X"},
		{L"iphone10.4",L"iPhone 8"},
		{L"iphone10.5",L"iPhone 8 Plus"},
		{L"iphone10.6",L"iPhone X"},
		//ipad
		{L"ipad1,1", L"iPad1"},
		{L"ipad2,1", L"iPad2"},
		{L"ipad2,2", L"iPad2"},
		{L"ipad2,3", L"iPad2"},
		{L"ipad2,4", L"iPad2"},
		{L"ipad3,1", L"iPad3"},
		{L"ipad3,2", L"iPad3"},
		{L"ipad3,3", L"iPad3"},
		{L"ipad3,4", L"iPad4"},
		{L"ipad3,5", L"iPad4"},
		{L"ipad3,6", L"iPad4"},
		{L"ipad4,1", L"iPadAir"},
		{L"ipad4,2", L"iPadAir"},
		{L"ipad4,3", L"iPadAir"},
		{L"ipad5,3", L"iPadAir2"},
		{L"ipad5,4", L"iPadAir2"},
		{L"ipad2,5", L"iPadMini"},
		{L"ipad2,6", L"iPadMini"},
		{L"ipad2,7", L"iPadMini"},
		{L"ipad4,4", L"iPadMini2"},
		{L"ipad4,5", L"iPadMini2"},
		{L"ipad4,6", L"iPadMini2"},
		{L"ipad4,7", L"iPadMini3"},
		{L"ipad4,8", L"iPadMini3"},
		{L"ipad4,9", L"iPadMini3"},
		{L"ipad5,1", L"iPadMini4"},
		{L"ipad5,2", L"iPadMini4"},
		{L"ipad6,3", L"iPadPro"},
		{L"ipad6,4", L"iPadPro"},
		{L"ipad6,7", L"iPadPro"},
		{L"ipad6,8", L"iPadPro"},
		{L"ipad6,11", L"iPad(9.7)"},
		{L"ipad6,12", L"iPad 9.7)"},
		//ipod
		{L"ipod1,1", L"iPodTouch1Gen"},
		{L"ipod2,1", L"iPodTouch2Gen"},
		{L"ipod3,1", L"iPodTouch3Gen"},
		{L"ipod4,1", L"iPodTouch4Gen"},
		{L"ipod5,1", L"iPodTouch5Gen"},
		{L"ipod7,1", L"iPodTouch6Gen"},
		//模拟器
		{L"i386", L"Simulator"},
		{L"x86_64", L"Simulator"}
	};

	void productType_to_phonename(SOUI::SStringT & productType)
	{
		std::wstring strProductType = productType.MakeLower();
		auto ite = g_mapDevproductType.find(strProductType);
		if (ite != g_mapDevproductType.end())
		{
			productType = ite->second.c_str();
		}
	}

	std::map<std::wstring, int> g_mapDevID = {
		//iPhone
		{L"iphone1,1",0},
		{L"iphone1,2",1},
		{L"iphone2,1",1},
		{L"iphone3,1",2},
		{L"iphone3,2",2},
		{L"iphone3,3",2},
		{L"iphone4,1",2},
		{L"iphone4,2",2},
		{L"iphone4,3",2},
		{L"iphone5,1",3},//ip5
		{L"iphone5,2",3},
		{L"iphone5,3",4},//ip5c
		{L"iphone5,4",4},
		{L"iphone6,1",5},//ip5s
		{L"iphone6,2",5},
		{L"iphone7,2",6},//ip6/6p
		{L"iphone7,1",6},
		{L"iphone8,1",7},//ip6s/6sp
		{L"iphone8,2",7},
		{L"iphone8,4",8},//se
		{L"iphone9,1",9},//ip7/7p
		{L"iphone9,2",9},
		{L"iphone9,3",9},
		{L"iphone9,4",9},
		{L"iphone10.1",10},//8 X
		{L"iphone10.2",10},
		{L"iphone10.3",11},
		{L"iphone10.4",10},
		{L"iphone10.5",10},
		{L"iphone10.6",11},
	};

	SOUI::SStringT getphonecolor(SOUI::SStringT DevProductType, const SOUI::SStringT & DeviceColor, const SOUI::SStringT & DeviceEnclosureColor,
		SOUI::SStringT & outDeviceColor, SOUI::SStringT & outDeviceEnclosureColor)
	{
		std::wstring strProductType = DevProductType.MakeLower();
		auto ite = g_mapDevID.find(strProductType);
		SOUI::SStringT ouColor = L"#ffffff";
		outDeviceColor = L"未知色";
		outDeviceEnclosureColor = L"未知色";
		if (ite != g_mapDevID.end())
		{
			switch (ite->second)
			{
			case 3://ip5
			{
				//#3b3b3c #99989b
				if (DeviceEnclosureColor == L"#99989b")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"黑色";
					ouColor = L"#000000";
				}
				//#e1e4e3 #d7d9d8
				else if (DeviceEnclosureColor == L"#d7d9d8")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"白色";
				}
			}break;
			case 4://ip5c
			{
				//#f5f4f7
				if (DeviceEnclosureColor == L"#f5f4f7")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"白色";
				}
				//#fe767a
				else if (DeviceEnclosureColor == L"#fe767a")
				{
					outDeviceColor = L"粉色";
					outDeviceEnclosureColor = L"粉色";
					ouColor = L"#fe767a";
				}
				//#faf189
				else if (DeviceEnclosureColor == L"#faf189")
				{
					outDeviceColor = L"黄色";
					outDeviceEnclosureColor = L"黄色";
					ouColor = L"#faf189";
				}
				//#46abe0
				else if (DeviceEnclosureColor == L"#46abe0")
				{
					outDeviceColor = L"蓝色";
					outDeviceEnclosureColor = L"蓝色";
					ouColor = L"#46abe0";

				}
				//#a1e877
				else if (DeviceEnclosureColor == L"#a1e877")
				{
					outDeviceColor = L"绿色";
					outDeviceEnclosureColor = L"绿色";
					ouColor = L"#a1e877";

				}
			}break;
			case 5://ip5s
			{
				//#99989b
				if (DeviceEnclosureColor == L"#99989b")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"太空灰";
					ouColor = L"#99989b";
				}

				//#d7d9d8
				else if (DeviceEnclosureColor == L"#d7d9d8")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"银白色";
					ouColor = L"#d7d9d8";
				}
				//#d4c5b3
				else if (DeviceEnclosureColor == L"#d4c5b3")
				{
					outDeviceColor = L"太空灰";
					outDeviceEnclosureColor = L"金色";
					ouColor = L"#d4c5b3";
				}
			}break;
			case 6://ip6/6p
			{
				//#b4b5b9
				if (DeviceEnclosureColor == L"#b4b5b9")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"太空灰";
					ouColor = L"#b4b5b9";
				}
				//#d7d9d8
				else if (DeviceEnclosureColor == L"#d7d9d8")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"银白色";
					ouColor = L"#99989b";
				}
				//#e1ccb5
				else if (DeviceEnclosureColor == L"#e1ccb5")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"金色";
					ouColor = L"#99989b";
				}
			}break;
			case 7://ip6s/6sp
			{
				//#b9b7ba
				if (DeviceEnclosureColor == L"#b9b7ba")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"太空灰";
					ouColor = L"#b9b7ba";
				}
				//#dadcdb
				else if (DeviceEnclosureColor == L"#dadcdb")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"银白色";
					ouColor = L"#dadcdb";
				}
				//#e1ccb7
				else if (DeviceEnclosureColor == L"#e1ccb7")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"金色";
					ouColor = L"#e1ccb7";
				}
				//#e4c1b9
				else if (DeviceEnclosureColor == L"#e4c1b9")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"玫瑰金色";
					ouColor = L"#e4c1b9";
				}
			}break;
			case 8://ip se
			{
				//#aeb1b8
				if (DeviceEnclosureColor == L"#aeb1b8")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"太空灰";
					ouColor = L"#aeb1b8";
				}
				//#dcdede
				else if (DeviceEnclosureColor == L"#dcdede")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"银白色";
					ouColor = L"#dcdede";
				}
				//#d6c8b9
				else if (DeviceEnclosureColor == L"#d6c8b9")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"金色";
					ouColor = L"#d6c8b9";
				}
				//#e5bdb5
				else if (DeviceEnclosureColor == L"#e5bdb5")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"玫瑰金色";
					ouColor = L"#e5bdb5";
				}
			}break;
			case 9://ip 7
			{
				if (DeviceEnclosureColor == L"2")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"太空灰";
					ouColor = L"#aeb1b8";
				}
				else if (DeviceEnclosureColor == L"3")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"银白色";
					ouColor = L"#d6c8b9";
				}
				else if (DeviceEnclosureColor == L"4")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"金色";
					ouColor = L"#e5bdb5";
				}
				else if (DeviceEnclosureColor == L"1")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"黑色";
					ouColor = L"#000000";
				}
				else if (DeviceEnclosureColor == L"5")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"乌黑色";
					ouColor = L"#000000";
				}
				else if (DeviceEnclosureColor == L"6")
				{
					outDeviceColor = L"银色";
					outDeviceEnclosureColor = L"红色";
					ouColor = L"#ff0000";
				}
			}break;
			case 10://ip 8
			{
				if (DeviceEnclosureColor == L"1")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"太空灰";
					ouColor = L"#aeb1b8";
				}
				else if (DeviceEnclosureColor == L"2")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"银白色";
					ouColor = L"#dcdede";
				}
				else if (DeviceEnclosureColor == L"3")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"金色";
					ouColor = L"#d6c8b9";
				}
			}break;
			case 11://ip x
			{
				if (DeviceEnclosureColor == L"1")
				{
					outDeviceColor = L"黑色";
					outDeviceEnclosureColor = L"太空灰";
					ouColor = L"#aeb1b8";
				}
				else if (DeviceEnclosureColor == L"2")
				{
					outDeviceColor = L"白色";
					outDeviceEnclosureColor = L"银白色";
					ouColor = L"#dcdede";
				}
			}break;
			}
		}
		return ouColor;
	}

	int remove_directory(const char* path)
	{
		int e = 0;
		if (!RemoveDirectoryA(path)) {
			e = win32err_to_errno(GetLastError());
		}
		return e;
	}

	time_t FormatTime2(LPCWSTR szTime)
	{
		struct tm tm1;
		time_t time1;
		swscanf(szTime, L"%4d%2d%2d%2d%2d%2d",
			&tm1.tm_year,
			&tm1.tm_mon,
			&tm1.tm_mday,
			&tm1.tm_hour,
			&tm1.tm_min,
			&tm1.tm_sec);

		tm1.tm_year -= 1900;
		tm1.tm_mon--;

		tm1.tm_isdst = -1;

		time1 = mktime(&tm1);
		return time1;
	}

	void FormatTime(time_t time1, SOUI::SStringT & szTime)
	{
		struct tm tm1;

#ifdef WIN32
		tm1 = *localtime(&time1);
#else
		localtime_r(&time1, &tm1);
#endif
		szTime.Format(L"%4.4d-%2.2d-%2.2d",
			tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday);
	}

	std::map<SOUI::SStringT, SOUI::SStringT> OriginID = {
		{L"F5D",L"惠州德赛"},
		{L"F9G",L"东莞华普"},
		{L"F8Y",L"欣旺达"},
		{L"ABY",L"欣旺达"},
		{L"C01",L"苏州顺达"},
	};

	int CaculateFirstDayWeekDay(int y)
	{
		int m = 13, d = 1;	
		y--;
		//周是从星期天开始算的。。。。
		return ((d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400)+1) % 7;
	}


	void getbatteryManufactureDateFormeSN(const SOUI::SStringT & SN, SOUI::SStringT & outOrigin, SOUI::SStringT & outDate)
	{		
		//XXX X XX X
		//厂家 年 周 日
		if (SN.GetLength() > 7)
		{
			SOUI::SStringT origin = SN.Mid(0, 3);
			SOUI::SStringT year = SN.Mid(3, 1);
			SOUI::SStringT week = SN.Mid(4, 2);
			SOUI::SStringT day = SN.Mid(6, 1);

			auto iter=OriginID.find(origin);
			if (iter != OriginID.end())
			{
				outOrigin = iter->second;
			}
			else
			{
				outOrigin= L"未知厂家";
			}

			if (year[0] > L'0' && year[0] < L'9')
			{				
				year.Format(L"201%c",year[0]);
			}
			else if (year[0] > L'A' && year[0] < L'Z')
			{
				year.Format(L"20%2.d", 10 + (year[0] - L'A'));
			}
			else
				return;
			int iyear=0;
			swscanf(year, L"%d", &iyear);
			year.Format(L"%s0101010101",(LPCTSTR) year);
			time_t firstdate = FormatTime2(year);
			int iweek = 0;
			swscanf(week, L"%d", &iweek);
			int iday = 0;
			swscanf(day, L"%d", &iday);
			
			time_t reldate= firstdate+ ((iweek-1)*7+ iday - CaculateFirstDayWeekDay(iyear))* 86400;
			FormatTime(reldate,outDate);
		}
	}

	void getbatteryManufactureDate(LPCSTR date, SOUI::SStringT & outOrigin, SOUI::SStringT & outDate)
	{
		//C（顺达电子）、D（德赛电子）、F（常熟电子）
		if (date && strlen(date) == 4)
		{
			switch (date[0])
			{
			case 'C':
				outOrigin = L"顺达电子";
				break;
			case 'D':
				outOrigin = L"惠州德赛";
				break;
			case 'F':
				outOrigin = L"常熟电子";
				break;
			default:
				outOrigin = L"未知地区";
			}

		}
	}

	//
	void mobilebackup_afc_get_file_contents(afc_client_t afc, const char* filename, char** data, uint64_t * size)
	{
		if (!afc || !data || !size) {
			return;
		}

		char** fileinfo = NULL;
		uint32_t fsize = 0;

		afc_get_file_info(afc, filename, &fileinfo);
		if (!fileinfo) {
			return;
		}
		int i;
		for (i = 0; fileinfo[i]; i += 2) {
			if (!strcmp(fileinfo[i], "st_size")) {
				fsize = atol(fileinfo[i + 1]);
				break;
			}
		}
		afc_dictionary_free(fileinfo);

		if (fsize == 0) {
			return;
		}

		uint64_t f = 0;
		afc_file_open(afc, filename, AFC_FOPEN_RDONLY, &f);
		if (!f) {
			return;
		}
		char* buf = (char*)malloc((uint32_t)fsize);
		uint32_t done = 0;
		while (done < fsize) {
			uint32_t bread = 0;
			afc_file_read(afc, f, buf + done, 65536, &bread);
			if (bread > 0) {
				done += bread;
			}
			else {
				break;
			}
		}
		if (done == fsize) {
			*size = fsize;
			*data = buf;
		}
		else {
			free(buf);
		}
		afc_file_close(afc, f);
	}
	int quit_flag = 0;
	double overall_progress = 0;

	void mb2_set_overall_progress(double progress)
	{
		if (progress > 0.0)
			overall_progress = progress;
	}

	void mb2_set_overall_progress_from_message(plist_t message, char* identifier)
	{
		plist_t node = NULL;
		double progress = 0.0;

		if (!strcmp(identifier, "DLMessageDownloadFiles")) {
			node = plist_array_get_item(message, 3);
		}
		else if (!strcmp(identifier, "DLMessageUploadFiles")) {
			node = plist_array_get_item(message, 2);
		}
		else if (!strcmp(identifier, "DLMessageMoveFiles") || !strcmp(identifier, "DLMessageMoveItems")) {
			node = plist_array_get_item(message, 3);
		}
		else if (!strcmp(identifier, "DLMessageRemoveFiles") || !strcmp(identifier, "DLMessageRemoveItems")) {
			node = plist_array_get_item(message, 3);
		}

		if (node != NULL) {
			plist_get_real_val(node, &progress);
			mb2_set_overall_progress(progress);
		}
	}

	void mb2_multi_status_add_file_error(plist_t status_dict, const char* path, int error_code, const char* error_message)
	{
		if (!status_dict) return;
		plist_t filedict = plist_new_dict();
		plist_dict_set_item(filedict, "DLFileErrorString", plist_new_string(error_message));
		plist_dict_set_item(filedict, "DLFileErrorCode", plist_new_uint(error_code));
		plist_dict_set_item(status_dict, path, filedict);
	}

	int errno_to_device_error(int errno_value)
	{
		switch (errno_value) {
		case ENOENT:
			return -6;
		case EEXIST:
			return -7;
		default:
			return -errno_value;
		}
	}

	int mb2_handle_send_file(mobilebackup2_client_t mobilebackup2, const char* backup_dir, const char* path, plist_t * errplist)
	{
		uint32_t nlen = 0;
		uint32_t pathlen = strlen(path);
		uint32_t bytes = 0;
		char* localfile = string_build_path(backup_dir, path, NULL);
		char buf[32768];
#ifdef WIN32
		struct _stati64 fst;
#else
		struct stat fst;
#endif

		FILE* f = NULL;
		uint32_t slen = 0;
		int errcode = -1;
		int result = -1;
		uint32_t length;
#ifdef WIN32
		uint64_t total;
		uint64_t sent;
#else
		off_t total;
		off_t sent;
#endif

		mobilebackup2_error_t err;

		/* send path length */
		nlen = htobe32(pathlen);
		err = mobilebackup2_send_raw(mobilebackup2, (const char*)& nlen, sizeof(nlen), &bytes);
		if (err != MOBILEBACKUP2_E_SUCCESS) {
			goto leave_proto_err;
		}
		if (bytes != (uint32_t)sizeof(nlen)) {
			err = MOBILEBACKUP2_E_MUX_ERROR;
			goto leave_proto_err;
		}

		/* send path */
		err = mobilebackup2_send_raw(mobilebackup2, path, pathlen, &bytes);
		if (err != MOBILEBACKUP2_E_SUCCESS) {
			goto leave_proto_err;
		}
		if (bytes != pathlen) {
			err = MOBILEBACKUP2_E_MUX_ERROR;
			goto leave_proto_err;
		}

#ifdef WIN32
		if (_stati64(localfile, &fst) < 0)
#else
		if (stat(localfile, &fst) < 0)
#endif
		{
			if (errno != ENOENT)
				printf("%s: stat failed on '%s': %d\n", __func__, localfile, errno);
			errcode = errno;
			goto leave;
		}

		total = fst.st_size;

		char* format_size = string_format_size(total);
		//PRINT_VERBOSE(1, "Sending '%s' (%s)\n", path, format_size);
		free(format_size);

		if (total == 0) {
			errcode = 0;
			goto leave;
		}

		f = fopen(localfile, "rb");
		if (!f) {
			printf("%s: Error opening local file '%s': %d\n", __func__, localfile, errno);
			errcode = errno;
			goto leave;
		}

		sent = 0;
		do {
			length = ((total - sent) < (long long)sizeof(buf)) ? (uint32_t)total - sent : (uint32_t)sizeof(buf);
			/* send data size (file size + 1) */
			nlen = htobe32(length + 1);
			memcpy(buf, &nlen, sizeof(nlen));
			buf[4] = CODE_FILE_DATA;
			err = mobilebackup2_send_raw(mobilebackup2, (const char*)buf, 5, &bytes);
			if (err != MOBILEBACKUP2_E_SUCCESS) {
				goto leave_proto_err;
			}
			if (bytes != 5) {
				goto leave_proto_err;
			}

			/* send file contents */
			size_t r = fread(buf, 1, sizeof(buf), f);
			if (r <= 0) {
				printf("%s: read error\n", __func__);
				errcode = errno;
				goto leave;
			}
			err = mobilebackup2_send_raw(mobilebackup2, buf, r, &bytes);
			if (err != MOBILEBACKUP2_E_SUCCESS) {
				goto leave_proto_err;
			}
			if (bytes != (uint32_t)r) {
				printf("Error: sent only %d of %d bytes\n", bytes, (int)r);
				goto leave_proto_err;
			}
			sent += r;
		} while (sent < total);
		fclose(f);
		f = NULL;
		errcode = 0;

	leave:
		if (errcode == 0) {
			result = 0;
			nlen = 1;
			nlen = htobe32(nlen);
			memcpy(buf, &nlen, 4);
			buf[4] = CODE_SUCCESS;
			mobilebackup2_send_raw(mobilebackup2, buf, 5, &bytes);
		}
		else {
			if (!*errplist) {
				*errplist = plist_new_dict();
			}
			char* errdesc = strerror(errcode);
			mb2_multi_status_add_file_error(*errplist, path, errno_to_device_error(errcode), errdesc);

			length = strlen(errdesc);
			nlen = htobe32(length + 1);
			memcpy(buf, &nlen, 4);
			buf[4] = CODE_ERROR_LOCAL;
			slen = 5;
			memcpy(buf + slen, errdesc, length);
			slen += length;
			err = mobilebackup2_send_raw(mobilebackup2, (const char*)buf, slen, &bytes);
			if (err != MOBILEBACKUP2_E_SUCCESS) {
				printf("could not send message\n");
			}
			if (bytes != slen) {
				printf("could only send %d from %d\n", bytes, slen);
			}
		}

	leave_proto_err:
		if (f)
			fclose(f);
		free(localfile);
		return result;
	}

	void mb2_handle_send_files(mobilebackup2_client_t mobilebackup2, plist_t message, const char* backup_dir)
	{
		uint32_t cnt;
		uint32_t i = 0;
		uint32_t sent;
		plist_t errplist = NULL;

		if (!message || (plist_get_node_type(message) != PLIST_ARRAY) || (plist_array_get_size(message) < 2) || !backup_dir) return;

		plist_t files = plist_array_get_item(message, 1);
		cnt = plist_array_get_size(files);

		for (i = 0; i < cnt; i++) {
			plist_t val = plist_array_get_item(files, i);
			if (plist_get_node_type(val) != PLIST_STRING) {
				continue;
			}
			char* str = NULL;
			plist_get_string_val(val, &str);
			if (!str)
				continue;

			if (mb2_handle_send_file(mobilebackup2, backup_dir, str, &errplist) < 0) {
				free(str);
				//printf("Error when sending file '%s' to device\n", str);
				// TODO: perhaps we can continue, we've got a multi status response?!
				break;
			}
			free(str);
		}

		/* send terminating 0 dword */
		uint32_t zero = 0;
		mobilebackup2_send_raw(mobilebackup2, (char*)& zero, 4, &sent);

		if (!errplist) {
			plist_t emptydict = plist_new_dict();
			mobilebackup2_send_status_response(mobilebackup2, 0, NULL, emptydict);
			plist_free(emptydict);
		}
		else {
			mobilebackup2_send_status_response(mobilebackup2, -13, "Multi status", errplist);
			plist_free(errplist);
		}
	}

	int mb2_receive_filename(mobilebackup2_client_t mobilebackup2, char** filename)
	{
		uint32_t nlen = 0;
		uint32_t rlen = 0;

		do {
			nlen = 0;
			rlen = 0;
			mobilebackup2_receive_raw(mobilebackup2, (char*)& nlen, 4, &rlen);
			nlen = be32toh(nlen);

			if ((nlen == 0) && (rlen == 4)) {
				// a zero length means no more files to receive
				return 0;
			}
			else if (rlen == 0) {
				// device needs more time, waiting...
				continue;
			}
			else if (nlen > 4096) {
				// filename length is too large
				printf("ERROR: %s: too large filename length (%d)!\n", __func__, nlen);
				return 0;
			}

			if (*filename != NULL) {
				free(*filename);
				*filename = NULL;
			}

			*filename = (char*)malloc(nlen + 1);

			rlen = 0;
			mobilebackup2_receive_raw(mobilebackup2, *filename, nlen, &rlen);
			if (rlen != nlen) {
				printf("ERROR: %s: could not read filename\n", __func__);
				return 0;
			}

			char* p = *filename;
			p[rlen] = 0;

			break;
		} while (1 && !quit_flag);

		return nlen;
	}

	int mb2_handle_receive_files(mobilebackup2_client_t mobilebackup2, plist_t message, const char* backup_dir)
	{
		uint64_t backup_real_size = 0;
		uint64_t backup_total_size = 0;
		uint32_t blocksize;
		uint32_t bdone;
		uint32_t rlen;
		uint32_t nlen = 0;
		uint32_t r;
		char buf[32768];
		char* fname = NULL;
		char* dname = NULL;
		char* bname = NULL;
		char code = 0;
		char last_code = 0;
		plist_t node = NULL;
		FILE* f = NULL;
		unsigned int file_count = 0;

		if (!message || (plist_get_node_type(message) != PLIST_ARRAY) || plist_array_get_size(message) < 4 || !backup_dir) return 0;

		node = plist_array_get_item(message, 3);
		if (plist_get_node_type(node) == PLIST_UINT) {
			plist_get_uint_val(node, &backup_total_size);
		}
		if (backup_total_size > 0) {
			//PRINT_VERBOSE(1, "Receiving files\n");
		}

		do {
			if (quit_flag)
				break;

			nlen = mb2_receive_filename(mobilebackup2, &dname);
			if (nlen == 0) {
				break;
			}

			nlen = mb2_receive_filename(mobilebackup2, &fname);
			if (!nlen) {
				break;
			}

			if (bname != NULL) {
				free(bname);
				bname = NULL;
			}

			bname = string_build_path(backup_dir, fname, NULL);

			if (fname != NULL) {
				free(fname);
				fname = NULL;
			}

			r = 0;
			nlen = 0;
			mobilebackup2_receive_raw(mobilebackup2, (char*)& nlen, 4, &r);
			if (r != 4) {
				printf("ERROR: %s: could not receive code length!\n", __func__);
				break;
			}
			nlen = be32toh(nlen);

			last_code = code;
			code = 0;

			mobilebackup2_receive_raw(mobilebackup2, &code, 1, &r);
			if (r != 1) {
				printf("ERROR: %s: could not receive code!\n", __func__);
				break;
			}

			/* TODO remove this */
			if ((code != CODE_SUCCESS) && (code != CODE_FILE_DATA) && (code != CODE_ERROR_REMOTE)) {
				//PRINT_VERBOSE(1, "Found new flag %02x\n", code);
			}

			remove_file(bname);
			f = fopen(bname, "wb");
			while (f && (code == CODE_FILE_DATA)) {
				blocksize = nlen - 1;
				bdone = 0;
				rlen = 0;
				while (bdone < blocksize) {
					if ((blocksize - bdone) < sizeof(buf)) {
						rlen = blocksize - bdone;
					}
					else {
						rlen = sizeof(buf);
					}
					mobilebackup2_receive_raw(mobilebackup2, buf, rlen, &r);
					if ((int)r <= 0) {
						break;
					}
					fwrite(buf, 1, r, f);
					bdone += r;
				}
				if (bdone == blocksize) {
					backup_real_size += blocksize;
				}
				if (backup_total_size > 0) {
					print_progress(backup_real_size, backup_total_size);
				}
				if (quit_flag)
					break;
				nlen = 0;
				mobilebackup2_receive_raw(mobilebackup2, (char*)& nlen, 4, &r);
				nlen = be32toh(nlen);
				if (nlen > 0) {
					last_code = code;
					mobilebackup2_receive_raw(mobilebackup2, &code, 1, &r);
				}
				else {
					break;
				}
			}
			if (f) {
				fclose(f);
				file_count++;
			}
			else {
				printf("Error opening '%s' for writing: %s\n", bname, strerror(errno));
			}
			if (nlen == 0) {
				break;
			}

			/* check if an error message was received */
			if (code == CODE_ERROR_REMOTE) {
				/* error message */
				char* msg = (char*)malloc(nlen);
				mobilebackup2_receive_raw(mobilebackup2, msg, nlen - 1, &r);
				msg[r] = 0;
				/* If sent using CODE_FILE_DATA, end marker will be CODE_ERROR_REMOTE which is not an error! */
				if (last_code != CODE_FILE_DATA) {
					fprintf(stdout, "\nReceived an error message from device: %s\n", msg);
				}
				free(msg);
			}
		} while (1);

		if (fname != NULL)
			free(fname);

		/* if there are leftovers to read, finish up cleanly */
		if ((int)nlen - 1 > 0) {
			//PRINT_VERBOSE(1, "\nDiscarding current data hunk.\n");
			fname = (char*)malloc(nlen - 1);
			mobilebackup2_receive_raw(mobilebackup2, fname, nlen - 1, &r);
			free(fname);
			remove_file(bname);
		}

		/* clean up */
		if (bname != NULL)
			free(bname);

		if (dname != NULL)
			free(dname);

		// TODO error handling?!
		plist_t empty_plist = plist_new_dict();
		mobilebackup2_send_status_response(mobilebackup2, 0, NULL, empty_plist);
		plist_free(empty_plist);

		return file_count;
	}

	void mb2_handle_list_directory(mobilebackup2_client_t mobilebackup2, plist_t message, const char* backup_dir)
	{
		if (!message || (plist_get_node_type(message) != PLIST_ARRAY) || plist_array_get_size(message) < 2 || !backup_dir) return;

		plist_t node = plist_array_get_item(message, 1);
		char* str = NULL;
		if (plist_get_node_type(node) == PLIST_STRING) {
			plist_get_string_val(node, &str);
		}
		if (!str) {
			printf("ERROR: Malformed DLContentsOfDirectory message\n");
			// TODO error handling
			return;
		}

		char* path = string_build_path(backup_dir, str, NULL);
		free(str);

		plist_t dirlist = plist_new_dict();

		DIR* cur_dir = opendir(path);
		if (cur_dir) {
			struct dirent* ep;
			while ((ep = readdir(cur_dir))) {
				if ((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..") == 0)) {
					continue;
				}
				char* fpath = string_build_path(path, ep->d_name, NULL);
				if (fpath) {
					plist_t fdict = plist_new_dict();
					struct stat st;
					stat(fpath, &st);
					const char* ftype = "DLFileTypeUnknown";
					if (S_ISDIR(st.st_mode)) {
						ftype = "DLFileTypeDirectory";
					}
					else if (S_ISREG(st.st_mode)) {
						ftype = "DLFileTypeRegular";
					}
					plist_dict_set_item(fdict, "DLFileType", plist_new_string(ftype));
					plist_dict_set_item(fdict, "DLFileSize", plist_new_uint(st.st_size));
					plist_dict_set_item(fdict, "DLFileModificationDate",
						plist_new_date(st.st_mtime - MAC_EPOCH, 0));

					plist_dict_set_item(dirlist, ep->d_name, fdict);
					free(fpath);
				}
			}
			closedir(cur_dir);
		}
		free(path);

		/* TODO error handling */
		mobilebackup2_error_t err = mobilebackup2_send_status_response(mobilebackup2, 0, NULL, dirlist);
		plist_free(dirlist);
		if (err != MOBILEBACKUP2_E_SUCCESS) {
			printf("Could not send status response, error %d\n", err);
		}
	}

	char* dirname(char* path)
	{
		static char buffer[260];
		size_t len;
		if (path == NULL) {
			strcpy_s(buffer, 1, ".");
			return buffer;
		}
		len = strlen(path);
		assert(len < sizeof(buffer));
		if (len != 0 && (path[len - 1] == '/' || path[len - 1] == '\\')) {
			--len;
		}
		while (len != 0 && path[len - 1] != '/' && path[len - 1] != '\\') {
			--len;
		}
		if (len == 0) {
			strcpy_s(buffer, 1, ".");
		}
		else if (len == 1) {
			if (path[0] == '/' || path[0] == '\\') {
				strcpy_s(buffer, 1, "/");
			}
			else {
				strcpy_s(buffer, 1, ".");
			}
		}
		else {
			memcpy(buffer, path, len - 1);
		}
		return buffer;
		}

	void scan_directory(const char* path, struct entry** files, struct entry** directories)
	{
		DIR* cur_dir = opendir(path);
		if (cur_dir) {
			struct dirent* ep;
			while ((ep = readdir(cur_dir))) {
				if ((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..") == 0)) {
					continue;
				}
				char* fpath = string_build_path(path, ep->d_name, NULL);
				if (fpath) {
#ifdef HAVE_DIRENT_D_TYPE
					if (ep->d_type & DT_DIR) {
#else
					struct stat st;
					if (stat(fpath, &st) != 0) return;
					if (S_ISDIR(st.st_mode)) {
#endif
						struct entry* ent = (entry*)malloc(sizeof(struct entry));
						if (!ent) return;
						ent->name = fpath;
						ent->next = *directories;
						*directories = ent;
						scan_directory(fpath, files, directories);
						fpath = NULL;
					}
					else {
						struct entry* ent = (entry*)malloc(sizeof(struct entry));
						if (!ent) return;
						ent->name = fpath;
						ent->next = *files;
						*files = ent;
						fpath = NULL;
					}
					}
				}
			closedir(cur_dir);
			}
		}

	int mkdir_with_parents(const char* dir, int mode)
	{
		if (!dir) return -1;
		if (__mkdir2(dir, mode) == 0) {
			return 0;
		}
		else {
			if (errno == EEXIST) {
				return 0;
			}
			else if (errno == ENOENT) {
				// ignore
			}
			else {
				return -1;
			}
		}
		int res;
		char* parent = _strdup(dir);
		char* parentdir = dirname(parent);
		if (parentdir && (strcmp(parentdir, ".") != 0) && (strcmp(parentdir, dir) != 0)) {
			res = mkdir_with_parents(parentdir, mode);
		}
		else {
			res = -1;
		}
		free(parent);
		if (res == 0) {
			mkdir_with_parents(dir, mode);
		}
		return res;
	}

	void mb2_handle_make_directory(mobilebackup2_client_t mobilebackup2, plist_t message, const char* backup_dir)
	{
		if (!message || (plist_get_node_type(message) != PLIST_ARRAY) || plist_array_get_size(message) < 2 || !backup_dir) return;

		plist_t dir = plist_array_get_item(message, 1);
		char* str = NULL;
		int errcode = 0;
		char* errdesc = NULL;
		plist_get_string_val(dir, &str);

		char* newpath = string_build_path(backup_dir, str, NULL);
		free(str);

		if (mkdir_with_parents(newpath, 0755) < 0) {
			errdesc = strerror(errno);
			if (errno != EEXIST) {
				printf("mkdir: %s (%d)\n", errdesc, errno);
			}
			errcode = errno_to_device_error(errno);
		}
		free(newpath);
		mobilebackup2_error_t err = mobilebackup2_send_status_response(mobilebackup2, errcode, errdesc, NULL);
		if (err != MOBILEBACKUP2_E_SUCCESS) {
			printf("Could not send status response, error %d\n", err);
		}
	}

	void mb2_copy_file_by_path(const char* src, const char* dst)
	{
		FILE* from, * to;
		char buf[BUFSIZ];
		size_t length;

		/* open source file */
		if ((from = fopen(src, "rb")) == NULL) {
			printf("Cannot open source path '%s'.\n", src);
			return;
		}

		/* open destination file */
		if ((to = fopen(dst, "wb")) == NULL) {
			printf("Cannot open destination file '%s'.\n", dst);
			fclose(from);
			return;
		}

		/* copy the file */
		while ((length = fread(buf, 1, BUFSIZ, from)) != 0) {
			fwrite(buf, 1, length, to);
		}

		if (fclose(from) == EOF) {
			printf("Error closing source file.\n");
		}

		if (fclose(to) == EOF) {
			printf("Error closing destination file.\n");
		}
	}

	void mb2_copy_directory_by_path(const char* src, const char* dst)
	{
		if (!src || !dst) {
			return;
		}

		struct stat st;

		/* if src does not exist */
		if ((stat(src, &st) < 0) || !S_ISDIR(st.st_mode)) {
			printf("ERROR: Source directory does not exist '%s': %s (%d)\n", src, strerror(errno), errno);
			return;
		}

		/* if dst directory does not exist */
		if ((stat(dst, &st) < 0) || !S_ISDIR(st.st_mode)) {
			/* create it */
			if (mkdir_with_parents(dst, 0755) < 0) {
				printf("ERROR: Unable to create destination directory '%s': %s (%d)\n", dst, strerror(errno), errno);
				return;
			}
		}

		/* loop over src directory contents */
		DIR* cur_dir = opendir(src);
		if (cur_dir) {
			struct dirent* ep;
			while ((ep = readdir(cur_dir))) {
				if ((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..") == 0)) {
					continue;
				}
				char* srcpath = string_build_path(src, ep->d_name, NULL);
				char* dstpath = string_build_path(dst, ep->d_name, NULL);
				if (srcpath && dstpath) {
					/* copy file */
					mb2_copy_file_by_path(srcpath, dstpath);
				}

				if (srcpath)
					free(srcpath);
				if (dstpath)
					free(dstpath);
			}
			closedir(cur_dir);
		}
	}

	void print_progress(uint64_t current, uint64_t total)
	{
		char* format_size = NULL;
		double progress = ((double)current / (double)total) * 100;
		if (progress < 0)
			return;

		if (progress > 100)
			progress = 100;

		print_progress_real((double)progress, 0);

		format_size = string_format_size(current);
		//PRINT_VERBOSE(1, " (%s", format_size);
		free(format_size);
		format_size = string_format_size(total);
		//PRINT_VERBOSE(1, "/%s)     ", format_size);
		free(format_size);

		fflush(stdout);
		if (progress == 100);
		//PRINT_VERBOSE(1, "\n");
	}

	void print_progress_real(double progress, int flush)
	{
		int i = 0;
		//PRINT_VERBOSE(1, "\r[");
		for (i = 0; i < 50; i++) {
			if (i < progress / 2) {
				//PRINT_VERBOSE(1, "=");
			}
			else {
				//PRINT_VERBOSE(1, " ");
			}
		}
		//PRINT_VERBOSE(1, "] %3.0f%%", progress);

		if (flush > 0) {
			fflush(stdout);
			if (progress == 100);
			//PRINT_VERBOSE(1, "\n");
		}
	}

	void do_post_notification(idevice_t device, const char* notification)
	{
		lockdownd_service_descriptor_t service = NULL;
		np_client_t np;

		lockdownd_client_t lockdown = NULL;

		if (lockdownd_client_new_with_handshake(device, &lockdown, "idevicebackup2") != LOCKDOWN_E_SUCCESS) {
			return;
		}

		lockdownd_start_service(lockdown, NP_SERVICE_NAME, &service);
		if (service && service->port) {
			np_client_new(device, service, &np);
			if (np) {
				np_post_notification(np, notification);
				np_client_free(np);
			}
		}
		else {
			printf("Could not start %s\n", NP_SERVICE_NAME);
		}

		if (service) {
			lockdownd_service_descriptor_free(service);
			service = NULL;
		}
		lockdownd_client_free(lockdown);
	}

	int mb2_status_check_snapshot_state(const char* path, const char* udid, const char* matches)
	{
		int ret = 0;
		plist_t status_plist = NULL;
		char* file_path = string_build_path(path, udid, "Status.plist", NULL);

		plist_read_from_filename(&status_plist, file_path);
		free(file_path);
		if (!status_plist) {
			printf("Could not read Status.plist!\n");
			return ret;
		}
		plist_t node = plist_dict_get_item(status_plist, "SnapshotState");
		if (node && (plist_get_node_type(node) == PLIST_STRING)) {
			char* sval = NULL;
			plist_get_string_val(node, &sval);
			if (sval) {
				ret = (strcmp(sval, matches) == 0) ? 1 : 0;
				free(sval);
			}
		}
		else {
			printf("%s: ERROR could not get SnapshotState key from Status.plist!\n", __func__);
		}
		plist_free(status_plist);
		return ret;
	}

	int write_restore_applications(plist_t info_plist, afc_client_t afc)
	{
		int res = -1;
		uint64_t restore_applications_file = 0;
		char* applications_plist_xml = NULL;
		uint32_t applications_plist_xml_length = 0;

		plist_t applications_plist = plist_dict_get_item(info_plist, "Applications");
		if (applications_plist) {
			plist_to_xml(applications_plist, &applications_plist_xml, &applications_plist_xml_length);
		}
		if (!applications_plist_xml) {
			printf("Error preparing RestoreApplications.plist\n");
			goto leave;
		}

		afc_error_t afc_err = AFC_E_SUCCESS;
		afc_err = afc_make_directory(afc, "/iTunesRestore");
		if (afc_err != AFC_E_SUCCESS) {
			printf("Error creating directory /iTunesRestore, error code %d\n", afc_err);
			goto leave;
		}

		afc_err = afc_file_open(afc, "/iTunesRestore/RestoreApplications.plist", AFC_FOPEN_WR, &restore_applications_file);
		if (afc_err != AFC_E_SUCCESS || !restore_applications_file) {
			printf("Error creating /iTunesRestore/RestoreApplications.plist, error code %d\n", afc_err);
			goto leave;
		}

		uint32_t bytes_written = 0;
		afc_err = afc_file_write(afc, restore_applications_file, applications_plist_xml, applications_plist_xml_length, &bytes_written);
		if (afc_err != AFC_E_SUCCESS || bytes_written != applications_plist_xml_length) {
			printf("Error writing /iTunesRestore/RestoreApplications.plist, error code %d, wrote %u of %u bytes\n", afc_err, bytes_written, applications_plist_xml_length);
			goto leave;
		}

		afc_err = afc_file_close(afc, restore_applications_file);
		restore_applications_file = 0;
		if (afc_err != AFC_E_SUCCESS) {
			goto leave;
		}
		/* success */
		res = 0;

	leave:
		free(applications_plist_xml);

		if (restore_applications_file) {
			afc_file_close(afc, restore_applications_file);
			restore_applications_file = 0;
		}

		return res;
	}

	plist_t mobilebackup_factory_info_plist_new(const char* udid, idevice_t device, afc_client_t afc)
	{
		/* gather data from lockdown */
		plist_t value_node = NULL;
		plist_t root_node = NULL;
		plist_t itunes_settings = NULL;
		plist_t min_itunes_version = NULL;
		char* udid_uppercase = NULL;

		lockdownd_client_t lockdown = NULL;
		if (lockdownd_client_new_with_handshake(device, &lockdown, "idevicebackup2") != LOCKDOWN_E_SUCCESS) {
			return NULL;
		}

		plist_t ret = plist_new_dict();

		/* get basic device information in one go */
		lockdownd_get_value(lockdown, NULL, NULL, &root_node);

		/* get iTunes settings */
		lockdownd_get_value(lockdown, "com.apple.iTunes", NULL, &itunes_settings);

		/* get minimum iTunes version */
		lockdownd_get_value(lockdown, "com.apple.mobile.iTunes", "MinITunesVersion", &min_itunes_version);

		lockdownd_client_free(lockdown);

		/* get a list of installed user applications */
		plist_t app_dict = plist_new_dict();
		plist_t installed_apps = plist_new_array();
		instproxy_client_t ip = NULL;
		if (instproxy_client_start_service(device, &ip, "idevicebackup2") == INSTPROXY_E_SUCCESS) {
			plist_t client_opts = instproxy_client_options_new();
			instproxy_client_options_add(client_opts, "ApplicationType", "User", NULL);
			instproxy_client_options_set_return_attributes(client_opts, "CFBundleIdentifier", "ApplicationSINF", "iTunesMetadata", NULL);

			plist_t apps = NULL;
			instproxy_browse(ip, client_opts, &apps);

			sbservices_client_t sbs = NULL;
			if (sbservices_client_start_service(device, &sbs, "idevicebackup2") != SBSERVICES_E_SUCCESS) {
				printf("Couldn't establish sbservices connection. Continuing anyway.\n");
			}

			if (apps && (plist_get_node_type(apps) == PLIST_ARRAY)) {
				uint32_t app_count = plist_array_get_size(apps);
				uint32_t i;
				for (i = 0; i < app_count; i++) {
					plist_t app_entry = plist_array_get_item(apps, i);
					plist_t bundle_id = plist_dict_get_item(app_entry, "CFBundleIdentifier");
					if (bundle_id) {
						char* bundle_id_str = NULL;
						plist_array_append_item(installed_apps, plist_copy(bundle_id));

						plist_get_string_val(bundle_id, &bundle_id_str);
						plist_t sinf = plist_dict_get_item(app_entry, "ApplicationSINF");
						plist_t meta = plist_dict_get_item(app_entry, "iTunesMetadata");
						if (sinf && meta) {
							plist_t adict = plist_new_dict();
							plist_dict_set_item(adict, "ApplicationSINF", plist_copy(sinf));
							if (sbs) {
								char* pngdata = NULL;
								uint64_t pngsize = 0;
								sbservices_get_icon_pngdata(sbs, bundle_id_str, &pngdata, &pngsize);
								if (pngdata) {
									plist_dict_set_item(adict, "PlaceholderIcon", plist_new_data(pngdata, pngsize));
									free(pngdata);
								}
							}
							plist_dict_set_item(adict, "iTunesMetadata", plist_copy(meta));
							plist_dict_set_item(app_dict, bundle_id_str, adict);
						}
						free(bundle_id_str);
					}
				}
			}
			plist_free(apps);

			if (sbs) {
				sbservices_client_free(sbs);
			}

			instproxy_client_options_free(client_opts);

			instproxy_client_free(ip);
		}

		/* Applications */
		plist_dict_set_item(ret, "Applications", app_dict);

		/* set fields we understand */
		value_node = plist_dict_get_item(root_node, "BuildVersion");
		plist_dict_set_item(ret, "Build Version", plist_copy(value_node));

		value_node = plist_dict_get_item(root_node, "DeviceName");
		plist_dict_set_item(ret, "Device Name", plist_copy(value_node));
		plist_dict_set_item(ret, "Display Name", plist_copy(value_node));

		char* uuid = get_uuid();
		plist_dict_set_item(ret, "GUID", plist_new_string(uuid));
		free(uuid);

		value_node = plist_dict_get_item(root_node, "IntegratedCircuitCardIdentity");
		if (value_node)
			plist_dict_set_item(ret, "ICCID", plist_copy(value_node));

		value_node = plist_dict_get_item(root_node, "InternationalMobileEquipmentIdentity");
		if (value_node)
			plist_dict_set_item(ret, "IMEI", plist_copy(value_node));

		/* Installed Applications */
		plist_dict_set_item(ret, "Installed Applications", installed_apps);

		plist_dict_set_item(ret, "Last Backup Date", plist_new_date(time(NULL) - MAC_EPOCH, 0));

		value_node = plist_dict_get_item(root_node, "MobileEquipmentIdentifier");
		if (value_node)
			plist_dict_set_item(ret, "MEID", plist_copy(value_node));

		value_node = plist_dict_get_item(root_node, "PhoneNumber");
		if (value_node && (plist_get_node_type(value_node) == PLIST_STRING)) {
			plist_dict_set_item(ret, "Phone Number", plist_copy(value_node));
		}

		/* FIXME Product Name */

		value_node = plist_dict_get_item(root_node, "ProductType");
		plist_dict_set_item(ret, "Product Type", plist_copy(value_node));

		value_node = plist_dict_get_item(root_node, "ProductVersion");
		plist_dict_set_item(ret, "Product Version", plist_copy(value_node));

		value_node = plist_dict_get_item(root_node, "SerialNumber");
		plist_dict_set_item(ret, "Serial Number", plist_copy(value_node));

		/* FIXME Sync Settings? */

		value_node = plist_dict_get_item(root_node, "UniqueDeviceID");
		plist_dict_set_item(ret, "Target Identifier", plist_new_string(udid));

		plist_dict_set_item(ret, "Target Type", plist_new_string("Device"));

		/* uppercase */
		udid_uppercase = string_toupper((char*)udid);
		plist_dict_set_item(ret, "Unique Identifier", plist_new_string(udid_uppercase));
		free(udid_uppercase);

		char* data_buf = NULL;
		uint64_t data_size = 0;
		mobilebackup_afc_get_file_contents(afc, "/Books/iBooksData2.plist", &data_buf, &data_size);
		if (data_buf) {
			plist_dict_set_item(ret, "iBooks Data 2", plist_new_data(data_buf, data_size));
			free(data_buf);
		}

		plist_t files = plist_new_dict();
		const char* itunesfiles[] = {
			"ApertureAlbumPrefs",
			"IC-Info.sidb",
			"IC-Info.sidv",
			"PhotosFolderAlbums",
			"PhotosFolderName",
			"PhotosFolderPrefs",
			"VoiceMemos.plist",
			"iPhotoAlbumPrefs",
			"iTunesApplicationIDs",
			"iTunesPrefs",
			"iTunesPrefs.plist",
			NULL
		};
		int i = 0;
		for (i = 0; itunesfiles[i]; i++) {
			data_buf = NULL;
			data_size = 0;
			char* fname = (char*)malloc(strlen("/iTunes_Control/iTunes/") + strlen(itunesfiles[i]) + 1);
			strcpy(fname, "/iTunes_Control/iTunes/");
			strcat(fname, itunesfiles[i]);
			mobilebackup_afc_get_file_contents(afc, fname, &data_buf, &data_size);
			free(fname);
			if (data_buf) {
				plist_dict_set_item(files, itunesfiles[i], plist_new_data(data_buf, data_size));
				free(data_buf);
			}
		}
		plist_dict_set_item(ret, "iTunes Files", files);

		plist_dict_set_item(ret, "iTunes Settings", itunes_settings ? plist_copy(itunes_settings) : plist_new_dict());

		/* since we usually don't have iTunes, let's get the minimum required iTunes version from the device */
		if (min_itunes_version) {
			plist_dict_set_item(ret, "iTunes Version", plist_copy(min_itunes_version));
		}
		else {
			plist_dict_set_item(ret, "iTunes Version", plist_new_string("10.0.1"));
		}

		plist_free(itunes_settings);
		plist_free(min_itunes_version);
		plist_free(root_node);

		return ret;
	}

	char* get_uuid()
	{
		const char* chars = "ABCDEF0123456789";
		int i = 0;
		char* uuid = (char*)malloc(sizeof(char) * 33);

		srand(time(NULL));

		for (i = 0; i < 32; i++) {
			uuid[i] = chars[rand() % 16];
		}

		uuid[32] = '\0';

		return uuid;
	}

	int remove_file(const char* path)
	{
		int e = 0;
#ifdef WIN32
		if (!DeleteFileA(path)) {
			e = win32err_to_errno(GetLastError());
		}
#else
		if (remove(path) < 0) {
			e = errno;
		}
#endif
		return e;
	}

	int win32err_to_errno(int err_value)
	{
		switch (err_value) {
		case ERROR_FILE_NOT_FOUND:
			return ENOENT;
		case ERROR_ALREADY_EXISTS:
			return EEXIST;
		default:
			return EFAULT;
		}
	}

	int rmdir_recursive(const char* path)
	{
		int res = 0;
		struct entry* files = NULL;
		struct entry* directories = NULL;
		struct entry* ent;

		ent = (entry*)malloc(sizeof(struct entry));
		if (!ent) return ENOMEM;
		ent->name = _strdup(path);
		ent->next = NULL;
		directories = ent;

		scan_directory(path, &files, &directories);

		ent = files;
		while (ent) {
			struct entry* del = ent;
			res = remove_file(ent->name);
			free(ent->name);
			ent = ent->next;
			free(del);
		}
		ent = directories;
		while (ent) {
			struct entry* del = ent;
			res = remove_directory(ent->name);
			free(ent->name);
			ent = ent->next;
			free(del);
		}

		return res;
	}
	}