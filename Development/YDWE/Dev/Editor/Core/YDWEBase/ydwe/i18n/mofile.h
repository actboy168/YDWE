#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <algorithm>
#include <ydwe/util/buffer.h>
#include <ydwe/util/noncopyable.h>
#include <ydwe/util/string_ref.h>
#include <boost/filesystem.hpp>

_BASE_BEGIN 
namespace i18n {

	class mofile : public util::noncopyable
	{
	public:
		static mofile* read(boost::filesystem::path const& filename);
		static mofile* read(const char* filename);

	private:
		uint32_t                        number_of_strings_;
		util::buffer                    buffer_;
		std::unique_ptr<boost::string_ref []> sorted_orig_strings_;
		std::unique_ptr<boost::string_ref []> translated_strings_;

	private:
		mofile()
		{ }

		util::buffer& buffer()
		{
			return buffer_;
		}

		util::buffer const& buffer() const
		{
			return buffer_;
		}

		void reset(uint32_t number_of_strings);
		bool read();

	public:
		const boost::string_ref* get_translated_string(const boost::string_ref& orig) const;
	};
}

_BASE_END
