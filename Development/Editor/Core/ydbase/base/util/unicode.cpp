#include <base/util/unicode.h>
#include <base/util/detail/ansi_codecvt.h>
#include <base/util/detail/utf8_codecvt.h>
#include <base/util/detail/codecvt.h>

namespace base { namespace util {
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

	std::wstring u2w(boost::string_ref const& from, conv_method how)
	{
		return std::move(detail::convert<char, wchar_t>(from, utf8_codecvt(), how));
	}

	std::string w2u(boost::wstring_ref const& from, conv_method how)
	{
		return std::move(detail::convert<wchar_t, char>(from, utf8_codecvt(), how));
	}

	std::wstring a2w(boost::string_ref const& from, conv_method how)
	{
		return std::move(detail::convert<char, wchar_t>(from, ansi_codecvt(), how));
	}

	std::string w2a(boost::wstring_ref const& from, conv_method how)
	{
		return std::move(detail::convert<wchar_t, char>(from, ansi_codecvt(), how));
	}

	std::string u2a(boost::string_ref  const& from, conv_method how) 
	{
		return std::move(w2a(u2w(from, how), how));
	}

	std::string a2u(boost::string_ref const& from, conv_method how) 
	{
		return std::move(w2u(a2w(from, how), how));
	}

}}
