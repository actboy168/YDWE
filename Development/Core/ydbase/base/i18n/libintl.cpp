#include <base/i18n/libintl.h>
#include <base/i18n/mofile.h>
#include <base/util/singleton.h>
#include <map>
#include <string>
#include <cstdint>
#include <Windows.h>

namespace base { namespace i18n {

	class manager
	{
	public:
		const char* get_default() const
		{
			if (default_.empty())
			{
				return nullptr;
			}
			return default_.c_str();
		}

		const char* set_default(const char* str)
		{
			if (str)
			{
				default_ = str;
			}
			else
			{
				default_.clear();
			}

			return get_default();
		}

		const mofile* find(const char* str) const
		{
			auto it = mapping_.find(str);
			if (it == mapping_.end())
			{
				return nullptr;
			}

			return it->second.get();
		}

		bool set(const char* str, mofile* mf)
		{
			if (str && mf)
			{
				mapping_[str].reset(mf);
				return true;
			}
			return false;
		}

	private:
		std::string                                    default_;
		std::map<std::string, std::unique_ptr<mofile>> mapping_;
	};

	typedef singleton_nonthreadsafe<manager> manager_s;

	std::wstring language_name(LCID locale_id)
	{
		wchar_t buffer[9];
		int length = ::GetLocaleInfoW(locale_id, LOCALE_SISO639LANGNAME,  &buffer[0], _countof(buffer));
		if (length <= 1)
			return std::move(std::wstring());

		std::wstring language(buffer);
		if (SUBLANGID(locale_id) == SUBLANG_NEUTRAL)
			return std::move(language);

		length = ::GetLocaleInfoW(locale_id, LOCALE_SISO3166CTRYNAME, &buffer[0], _countof(buffer));
		if (length <= 1)
			return std::move(language);

		return std::move(language.append(1, '_').append(buffer));
	}

	std::wstring user_ui_language_name()
	{
		LANGID lang_id = ::GetUserDefaultUILanguage();
		if (lang_id == LOCALE_CUSTOM_UI_DEFAULT)
		{
			lang_id = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
		}
		return std::move(language_name(MAKELCID(lang_id, SORT_DEFAULT)));
	}

	bool bindtextdomain(const char* domain, fs::path const& mofile_path)
	{
		if (!domain)
		{
			return false;
		}

		return manager_s::instance().set(domain, mofile::read(mofile_path / user_ui_language_name() / L"LC_MESSAGES" / (std::string(domain) + ".mo")));	
	}

	const char* textdomain(const char* domain)
	{
		return manager_s::instance().set_default(domain);
	}

	std::string_view gettext(const char* orig_str)
	{
		return dgettext(nullptr, orig_str);
	}

	std::string_view dgettext(const char* domain, const char* orig_str)
	{
		if (!orig_str)
		{
			return nullptr;
		}

		if (!domain)
		{
			domain = manager_s::instance().get_default();

			if (!domain)
			{
				return orig_str;
			}
		}

		const mofile* mf = manager_s::instance().find(domain);

		if (!mf)
		{
			return orig_str;
		}

		const std::string_view* result = mf->get_translated_string(orig_str);
		if (!result)
		{
			return orig_str;
		}

		return *result;
	}
}}
