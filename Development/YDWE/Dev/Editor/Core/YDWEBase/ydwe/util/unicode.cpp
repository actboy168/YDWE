#include <ydwe/util/unicode.h>
#include <ydwe/util/detail/ansi_codecvt.h>
#include <ydwe/util/detail/utf8_codecvt.h>
#include <ydwe/util/detail/codecvt.h>

_BASE_BEGIN namespace util {
	std::locale utf8_locale(std::locale(), new detail::utf8_codecvt_facet);
	const detail::codecvt_type* utf8_codecvt_facet_ptr(&std::use_facet<detail::codecvt_type>(utf8_locale));

	std::locale ansi_locale(std::locale(), new detail::ansi_codecvt_facet);
	const detail::codecvt_type* ansi_codecvt_facet_ptr(&std::use_facet<detail::codecvt_type>(ansi_locale));

	namespace
	{
		template <class CodecvtT>
		struct codecvt
		{
			codecvt()
				: loc(std::locale(), new CodecvtT)
				, codecvt_facet_ptr(&std::use_facet<detail::codecvt_type>(loc))
			{ }

			const detail::codecvt_type& operator () ()
			{
				return *codecvt_facet_ptr;
			}

			std::locale loc;
			const detail::codecvt_type* codecvt_facet_ptr;	
		};


		codecvt<detail::utf8_codecvt_facet> utf8_codecvt;
		codecvt<detail::ansi_codecvt_facet> ansi_codecvt;
	}

	std::wstring u2w(std::string const& from, conv_method how)
	{
		return std::move(detail::convert<char, wchar_t>(from, utf8_codecvt(), how));
	}

	std::string w2u(std::wstring const& from, conv_method how)
	{
		return std::move(detail::convert<wchar_t, char>(from, utf8_codecvt(), how));
	}

	std::wstring a2w(std::string const& from, conv_method how)
	{
		return std::move(detail::convert<char, wchar_t>(from, ansi_codecvt(), how));
	}

	std::string w2a(std::wstring const& from, conv_method how)
	{
		return std::move(detail::convert<wchar_t, char>(from, ansi_codecvt(), how));
	}

	std::string u2a(std::string  const& from, conv_method how) 
	{
		return std::move(w2a(u2w(from, how), how));
	}

	std::string a2u(std::string  const& from, conv_method how) 
	{
		return std::move(w2u(a2w(from, how), how));
	}

	std::wstring u2w_ref(boost::string_ref const& from, conv_method how)
	{
		return std::move(detail::convert<char, wchar_t>(from, utf8_codecvt(), how));
	}

	std::string w2u_ref(boost::wstring_ref const& from, conv_method how)
	{
		return std::move(detail::convert<wchar_t, char>(from, utf8_codecvt(), how));
	}

	std::wstring a2w_ref(boost::string_ref const& from, conv_method how)
	{
		return std::move(detail::convert<char, wchar_t>(from, ansi_codecvt(), how));
	}

	std::string w2a_ref(boost::wstring_ref const& from, conv_method how)
	{
		return std::move(detail::convert<wchar_t, char>(from, ansi_codecvt(), how));
	}

	std::string u2a_ref(boost::string_ref  const& from, conv_method how) 
	{
		return std::move(w2a(u2w_ref(from, how), how));
	}

	std::string a2u_ref(boost::string_ref const& from, conv_method how) 
	{
		return std::move(w2u(a2w_ref(from, how), how));
	}

}}
