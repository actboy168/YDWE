
#include <slk/reader/IniReader.hpp>
#include <slk/reader/IniBaseReader.hpp>

namespace slk
{
	void IniReader::Read(buffer_reader& reader, IniTable& table)
	{
		IniBaseReader::Read(reader, table);
	}
}
