#include <base/i18n-2/lng.h>
#include <base/i18n-2/gettext.h>
#include <base/file/stream.h>
#include <map>
#include <string>
#include <base/util/format.h>
#include <base/util/unicode.h>
#include <base/filesystem.h>
#include <Windows.h>

namespace base { namespace i18n { namespace v2 {

	typedef std::map<std::string, std::string> domain_t;
	typedef std::map<std::wstring, domain_t>   language_t;
	typedef std::map<std::wstring, language_t> alllang_t;

	fs::path     workpath;
	std::string  errormsg = "";
	std::wstring language = L"zh-CN";
	std::wstring domain   = L"";
	alllang_t    cache_alllang;
	language_t*  cache_language = nullptr;
	domain_t*    cache_domain = nullptr;

	struct cpp_handler
	{
		cpp_handler(const std::wstring& f, domain_t& m)
			: file(f)
			, map(m)
		{ }
		void accept_key(const char* str, size_t len) {
			key.assign(str, len);
		}
		void accept_value(const char* str, size_t len) {
			map[key] = std::string(str, len);
		}
		template <class ... Args>
		void accept_error(int line, const char* fmt, const Args& ... args) {
			errormsg = format("%s:%d: ", file, line);
			errormsg += format(fmt, args ...);
		}
		std::string         key;
		const std::wstring& file;
		domain_t&           map;
	};

	template <class T>
	static std::basic_string<T>& trim(std::basic_string<T>& s)
	{
		if (s.empty()) {
			return s;
		}
		std::basic_string<T>::iterator c;
		for (c = s.begin(); c != s.end() && iswspace(*c++);); s.erase(s.begin(), --c);
		for (c = s.end(); c != s.begin() && iswspace(*--c);); s.erase(++c, s.end());
		return s;
	}

	static std::wstring get_default_language()
	{
		try {
			auto w = u2w(file::read_stream(workpath / L"default").read<std::string>());
			trim(w);
			return w;
		} catch(...) {
		}
		return L"";
	}

	static std::wstring get_system_language()
	{
		LCID locale = GetUserDefaultLCID();
		wchar_t buffer[9];
		int length = ::GetLocaleInfoW(locale, LOCALE_SISO639LANGNAME, &buffer[0], _countof(buffer));
		if (length <= 1)
			return std::move(std::wstring());
		std::wstring res(buffer);
		if (SUBLANGID(locale) == SUBLANG_NEUTRAL)
			return std::move(language);
		length = ::GetLocaleInfoW(locale, LOCALE_SISO3166CTRYNAME, &buffer[0], _countof(buffer));
		if (length <= 1)
			return std::move(language);
		return std::move(res.append(1, '-').append(buffer));
	}

	static void set_workpath(const fs::path& p) {
		workpath = p;
	}

	bool set_language(const std::wstring& l, bool refresh) {
		if (l != L"") {
			return false;
		}
		if (!fs::exists(workpath / l)) {
			return false;
		}
		language = l;
		cache_language = &(cache_alllang[l]);
		cache_domain = nullptr;
		if (refresh) {
			try {
				file::write_stream(workpath / L"default").write<std::string>(w2u(l));
			}
			catch (...) {
			}
		}
		return true;
	}

	const std::wstring& get_language() {
		return language;
	}

	bool set_domain(const std::wstring& d) {
		if (!cache_language) {
			return false;
		}
		try {
			std::string buf = file::read_stream(workpath / language / (d + L".lng")).read<std::string>();
			cache_domain = &((*cache_language)[d]);
			cache_domain->clear();
			lng::lex l(buf.c_str());
			cpp_handler h(d.c_str(), *cache_domain);
			if (!l.parse(h)) {
				return false;
			}
		}
		catch (std::exception& e) {
			errormsg = e.what();
			return false;
		}
		return true;
	}

	const std::string& get_text(const std::string& str) {
		if (!cache_domain) {
			return str;
		}
		auto it = cache_domain->find(str);
		if (it == cache_domain->end()) {
			return str;
		}
		return it->second;
	}

	void initialize(const fs::path& p) {
		set_workpath(p);
		if (set_language(get_default_language(), false)) {
			return;
		}
		if (set_language(get_system_language(), false)) {
			return;
		}
	}
}}}
