#include <ydwe/i18n/mofile.h>
#include <ydwe/file/steam.h>

namespace ydwe { namespace i18n {

	namespace detail
	{
		bool mofile_read_strings_array(util::buffer& buf, uint32_t number_of_strings, uint32_t offset, std::string* result)
		{
			buf.seek(offset, std::ios::beg);

			std::unique_ptr<uint32_t []> strings_lengths(new uint32_t[number_of_strings]);

			uint32_t first_offset = 0;
			uint32_t last_offset  = 0;
			for (uint32_t i = 0; i < number_of_strings; i++)
			{
				uint32_t current_length = buf.read<uint32_t>();
				uint32_t current_offset = buf.read<uint32_t>();

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

			uint32_t string_array_size = last_offset + strings_lengths[number_of_strings - 1] + 1 - first_offset;
			if (string_array_size == 0)
			{
				return false;
			}

			buf.seek(first_offset, std::ios::beg);

			std::unique_ptr<char []> string_array(new char[string_array_size]);
			memcpy(string_array.get(), buf.reads_ptr(string_array_size), string_array_size);

			const char* iter = string_array.get();
			for (uint32_t i = 0; i < number_of_strings; i++)
			{
				const char* end_iter = iter + strings_lengths[i] + 1;
				result[i] = std::string(iter, end_iter);
				iter = end_iter;
			}

			return true;
		}
	}

	mofile* mofile::read(util::buffer& buf)
	{
		uint32_t magic_number = buf.read<uint32_t>();
		if ((magic_number != 0x950412DE) && (magic_number != 0xDE120495))
			return nullptr;

		uint32_t file_format_revision = buf.read<uint32_t>();
		if (file_format_revision != 0)
			return nullptr;

		std::unique_ptr<mofile> mf(new mofile(buf.read<uint32_t>()));

		if (mf->number_of_strings_ == 0)
		{
			return nullptr;
		}

		uint32_t offset_orig_table  = buf.read<uint32_t>();
		uint32_t offset_trans_table = buf.read<uint32_t>();

		if (!detail::mofile_read_strings_array(buf, mf->number_of_strings_, offset_orig_table, mf->sorted_orig_strings_.get()))
		{
			return nullptr;
		}

		if (!detail::mofile_read_strings_array(buf, mf->number_of_strings_, offset_trans_table, mf->translated_strings_.get()))
		{
			return nullptr;
		}

		return mf.release();
	}

	mofile* mofile::read(boost::filesystem::path const& filename)
	{
		try {
			util::buffer buf = file::read_steam(filename).read<util::buffer>();
			return read(buf);
		}
		catch (...) {
		}

		return nullptr;
	}

	mofile* mofile::read(const char* filename)
	{
		try {
			util::buffer buf = file::read_steam(filename).read<util::buffer>();
			return read(buf);
		}
		catch (...) {
		}

		return nullptr;
	}

	const std::string* mofile::get_translated_string(const std::string& orig) const
	{
		const std::string* end_iter = sorted_orig_strings_.get() + number_of_strings_;
		const std::string* orig_str_ptr = std::lower_bound((const std::string*)sorted_orig_strings_.get(), end_iter, orig);
		if (!orig_str_ptr || (orig_str_ptr == end_iter))
		{
			return nullptr;
		}
		else
		{
			return &translated_strings_[orig_str_ptr - sorted_orig_strings_.get()];
		}
	}
}}
