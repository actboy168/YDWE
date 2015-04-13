#include <slk/reader/TxtReader.hpp>
#include <slk/table/VariableData.hpp>
#include <slk/reader/CommonReader.hpp>

namespace slk
{
	void TxtReader::Read(base::buffer_reader& reader, SlkTable& table, bool create_if_not_exists)
	{
		reader::utility::ini_read(reader, table, create_if_not_exists);
	}
}
