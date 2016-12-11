#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <algorithm>
#include <base/util/buffer.h>
#include <base/util/noncopyable.h>
#include <base/util/string_view.h> 		
#include <base/filesystem.h>

namespace base { 
namespace i18n {

	class mofile : public noncopyable
	{
	public:
		static mofile* read(fs::path const& filename);
		static mofile* read(const char* filename);

	private:
		uint32_t                        number_of_strings_;
		buffer                    buffer_;
		std::unique_ptr<std::string_view []> sorted_orig_strings_;
		std::unique_ptr<std::string_view []> translated_strings_;

	private:
		mofile(buffer&& b)
			: buffer_(std::move(b))
		{ }

		void initialize(uint32_t number_of_strings);
		bool read();

	public:
		const std::string_view* get_translated_string(const std::string_view& orig) const;
	};
}

}
