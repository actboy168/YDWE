#include <base/i18n/mofile.h>
#include <base/file/stream.h>
#include <base/util/dynarray.h>

namespace base { 
namespace i18n {

	namespace detail
	{
		bool mofile_read_strings_array(buffer_reader& reader, uint32_t number_of_strings, uint32_t offset, std::string_view* result)
		{
			reader.seek(offset, std::ios::beg);

			std::dynarray<uint32_t> strings_lengths(number_of_strings);

			uint32_t first_offset = 0;
			uint32_t last_offset  = 0;
			for (uint32_t i = 0; i < number_of_strings; i++)
			{
				uint32_t current_length = reader.read<uint32_t>();
				uint32_t current_offset = reader.read<uint32_t>();

				strings_lengths[i] = current_length;

				if (i == 0)
				{
					first_offset = current_offset;
				}

				if (i == (number_of_strings - 1))
				{
					last_offset = current_offset;
				}
			}

			uint32_t string_array_size = last_offset + strings_lengths.back() + 1 - first_offset;
			if (string_array_size == 0)
			{
				return false;
			}

			reader.seek(first_offset, std::ios::beg);

			std::string_view string_array(reader.reads_ptr(string_array_size), string_array_size);

			std::string_view::iterator iter = string_array.begin();
			for (uint32_t i = 0; i < number_of_strings; i++)
			{
				size_t len = strings_lengths[i] + 1;
				result[i] = std::string_view(iter, len);
				iter += len;
			}

			return true;
		}
	}

	void mofile::initialize(uint32_t number_of_strings)
	{
		number_of_strings_ = number_of_strings;
		sorted_orig_strings_.reset(new std::string_view[number_of_strings_]);
		translated_strings_.reset(new std::string_view[number_of_strings_]);
	}

	bool mofile::read()
	{
		buffer_reader reader(this->buffer_);

		uint32_t magic_number = reader.read<uint32_t>();
		if ((magic_number != 0x950412DE) && (magic_number != 0xDE120495))
			return false;

		uint32_t file_format_revision = reader.read<uint32_t>();
		if (file_format_revision != 0)
			return false;

		initialize(reader.read<uint32_t>());

		if (this->number_of_strings_ == 0)
		{
			return false;
		}

		uint32_t offset_orig_table  = reader.read<uint32_t>();
		uint32_t offset_trans_table = reader.read<uint32_t>();

		if (!detail::mofile_read_strings_array(reader, this->number_of_strings_, offset_orig_table, this->sorted_orig_strings_.get()))
		{
			return false;
		}

		if (!detail::mofile_read_strings_array(reader, this->number_of_strings_, offset_trans_table, this->translated_strings_.get()))
		{
			return false;
		}

		return true;
	}

	mofile* mofile::read(fs::path const& filename)
	{
		try {
			std::unique_ptr<mofile> mf(new mofile(file::read_stream(filename).read<buffer>()));
			if (mf->read())
			{
				return mf.release();
			}
		}
		catch (...) {
		}

		return nullptr;
	}

	mofile* mofile::read(const char* filename)
	{
		try {
			std::unique_ptr<mofile> mf(new mofile(file::read_stream(filename).read<buffer>()));
			if (mf->read())
			{
				return mf.release();
			}
		}
		catch (...) {
		}

		return nullptr;
	}

	const std::string_view* mofile::get_translated_string(const std::string_view& orig) const
	{
		const std::string_view* end_iter = sorted_orig_strings_.get() + number_of_strings_;
		const std::string_view* orig_str_ptr = std::lower_bound((const std::string_view*)sorted_orig_strings_.get(), end_iter, orig);
		if (!orig_str_ptr || (orig_str_ptr == end_iter))
		{
			return nullptr;
		}
		else
		{
			return &translated_strings_[orig_str_ptr - sorted_orig_strings_.get()];
		}
	}
}

}
