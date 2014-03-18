#pragma once

#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/core/record_view.hpp>
#include <boost/filesystem.hpp>

namespace logging
{
	using namespace boost::log;

	class logging_backend
		: public sinks::basic_formatted_sink_backend<char, sinks::combine_requirements<sinks::synchronized_feeding, sinks::flushing>::type>
	{
		typedef sinks::basic_formatted_sink_backend<char, sinks::combine_requirements<sinks::synchronized_feeding, sinks::flushing>::type> base_type;

	public:
		typedef base_type::char_type          char_type;
		typedef base_type::string_type        string_type;
		typedef std::basic_ostream<char_type> stream_type;

	private:
		struct implementation;
		implementation* impl_;

	public:
		logging_backend(const boost::filesystem::path& root_path);
		~logging_backend();

		void consume(record_view const& rec, string_type const& formatted_message);
		void flush();
		void rotate_file();
	};
}
