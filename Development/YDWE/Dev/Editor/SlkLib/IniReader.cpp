
#include "IniReader.hpp"
#include "IniBaseReader.hpp"

namespace slk
{
	void IniReader::Read(buffer&& buf, IniTable& table)
	{
		IniBaseReader::Read(std::forward<buffer>(buf), table);
	}
}
