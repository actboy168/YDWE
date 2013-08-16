
#include <slk/reader/TxtReader.hpp>
#include <slk/table/VariableData.hpp>
#include <slk/reader/IniBaseReader.hpp>

namespace slk
{
	void TxtReader::Read(buffer_reader& reader, SlkTable& table)
	{
		IniBaseReader::Read(reader, table);
	}
}
