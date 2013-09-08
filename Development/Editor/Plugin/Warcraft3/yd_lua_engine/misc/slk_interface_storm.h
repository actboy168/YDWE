#include "storm.h"
#include <slk/InterfaceStorm.hpp>

_BASE_BEGIN
namespace warcraft3 {

	class slk_interface_storm
		: public slk::InterfaceStorm
	{
	public:
		slk_interface_storm()
			: s_()
		{ }

		bool has(std::string const& path)
		{
			return s_.has_file(path.c_str());
		}

		std::string load(std::string const& path, error_code& ec)
		{
			const void* buf_data = nullptr;
			size_t      buf_size = 0;

			if (!s_.load_file(path.c_str(), &buf_data, &buf_size))
			{
				ec = ERROR_FILE_NOT_FOUND;
				return std::move(std::string());
			}
			std::string result((const char*)buf_data, ((const char*)buf_data) + buf_size);
			s_.unload_file(buf_data);
			return std::move(result);
		}

	private:
		storm s_;
	};
}

_BASE_END
