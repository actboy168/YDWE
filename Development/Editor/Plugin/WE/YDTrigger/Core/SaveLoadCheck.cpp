#include <string>
#include <map>

class CSaveLoadCheck {
public:
	typedef std::map<std::string, std::string> TSaveLoadMap;

	void Reset() {
		m_map.clear();
	}

	bool Set(const char *key, const char *value) {
		if (key && value && m_map.find(key) == m_map.end()) {
			m_map[key] = this->Convert(value);
			return true;
		}
		else {
			return (m_map[key].compare(this->Convert(value)) == 0);
		}
	}

	const char *Get(const char *key) {
		TSaveLoadMap::iterator itr;

		if ((itr = m_map.find(key)) == m_map.end()) {
			return nullptr;
		}
		else {
			return this->Convert(itr->second.c_str());
		}
	}

private:
	const char* Convert(const char* src) const {
		if (0 == strcmp(src, "StringExt")) {
			return "string";
		}
		if (0 == strcmp(src, "imagefile")) {
			return "string";
		}
		if (0 == strcmp(src, "modelfile")) {
			return "string";
		}
		if (0 == strcmp(src, "radian")) {
			return "real";
		}
		if (0 == strcmp(src, "degree")) {
			return "real";
		}
		if (0 == strcmp(src, "degree")) {
			return "real";
		}
		if (0 == strcmp(src, "unitcode")) {
			return "integer";
		}
		if (0 == strcmp(src, "abilcode")) {
			return "integer";
		}
		if (0 == strcmp(src, "itemcode")) {
			return "integer";
		}
		
		return src;
	}
	TSaveLoadMap m_map;
};

CSaveLoadCheck g_SaveLoadCheck;

#include <windows.h>

void SaveLoadCheck_Reset() {
	g_SaveLoadCheck.Reset();
}

BOOL SaveLoadCheck_Set(LPCSTR lpszKey, LPCSTR lpszName) {
	if (g_SaveLoadCheck.Set(lpszKey, lpszName)) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

LPCSTR SaveLoadCheck_Get(LPCSTR lpszKey) {
	LPCSTR result = g_SaveLoadCheck.Get(lpszKey);
	if (result == nullptr)
		result = "";

	return result;
}
