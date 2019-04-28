#include "pch.h"
#include "iOSUtils.h"

#include <map>
#include <string>

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
			if(subnode)
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
			stream << u ;
			break;

		case PLIST_REAL:
			plist_get_real_val(node, &d);
			stream << d ;
			break;

		case PLIST_STRING:
			plist_get_string_val(node, &s);
			stream << s ;
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
				stream << s ;
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
		//Ä£ÄâÆ÷
		{L"i386", L"Simulator"},
		{L"x86_64", L"Simulator"}
	};

	void productType_to_phonename(SOUI::SStringT& productType)
	{
		std::wstring strProductType = productType.MakeLower();
		auto ite=g_mapDevproductType.find(strProductType);
		if (ite != g_mapDevproductType.end())
		{
			productType = ite->second.c_str();
		}
	}
}