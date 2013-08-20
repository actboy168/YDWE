#include <slk/utility/sequence.h>
#include <cctype>

namespace slk
{
	namespace ctype
	{
		template<> SLKLIB_API
		bool is_space::operator()<char>(char Ch) const
		{
			return !!isspace(static_cast<unsigned char>(Ch));
		}
	}
}
