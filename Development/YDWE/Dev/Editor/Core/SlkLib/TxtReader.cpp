
#include "TxtReader.hpp"
#include "VariableData.hpp"
#include "IniBaseReader.hpp"

namespace slk
{
	void TxtReader::Read(buffer_reader& reader, SlkTable& table)
	{
		IniBaseReader::Read(reader, table);
	}
}
