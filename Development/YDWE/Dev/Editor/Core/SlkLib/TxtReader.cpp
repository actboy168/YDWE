
#include "TxtReader.hpp"
#include "VariableData.hpp"
#include "IniBaseReader.hpp"

namespace slk
{
	void TxtReader::Read(buffer&& buf, SlkTable& table)
	{
		IniBaseReader::Read(std::forward<buffer>(buf), table);
	}
}
