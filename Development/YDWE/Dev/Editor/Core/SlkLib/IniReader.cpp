
#include "IniReader.hpp"
#include "IniBaseReader.hpp"

namespace slk
{
	void IniReader::Read(buffer_reader& reader, IniTable& table)
	{
		IniBaseReader::Read(reader, table);
	}
}
