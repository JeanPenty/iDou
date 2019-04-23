#include "pch.h"
#include "iOSUtils.h"

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
}