#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <algorithm>
#include <ydwe/util/buffer.h>
#include <ydwe/util/noncopyable.h>
#include <boost/filesystem.hpp>

namespace ydwe { namespace i18n {

	class mofile : public util::noncopyable
	{
	public:
		static mofile* read(util::buffer& buf);
		static mofile* read(boost::filesystem::path const& filename);
		static mofile* read(const char* filename);

	private:
		uint32_t number_of_strings_;
		std::unique_ptr<std::string []> sorted_orig_strings_;
		std::unique_ptr<std::string []> translated_strings_;

	private:
		mofile(uint32_t number_of_strings)
			: number_of_strings_(number_of_strings)
			, sorted_orig_strings_(new std::string[number_of_strings_])
			, translated_strings_(new std::string[number_of_strings_])
		{ }

	public:
		const std::string* get_translated_string(const std::string& orig) const;
	};
}}
