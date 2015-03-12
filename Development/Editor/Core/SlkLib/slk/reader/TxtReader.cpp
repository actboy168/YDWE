#include <slk/reader/TxtReader.hpp>
#include <slk/table/VariableData.hpp>
#include <slk/reader/CommonReader.hpp>

namespace slk
{
	void TxtReader::Read(base::buffer_reader& reader, SlkTable& table)
	{
		reader::utility::ini_read(reader, table);
	}
}
