#include "logging.h"

#include <boost/locale/generator.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/support/date_time.hpp>
#include "logging_backend.h"


namespace logging
{
	bool initiate(const boost::filesystem::path& root_path)
	{
		try
		{
			using namespace boost::log;

			typedef sinks::synchronous_sink<logging_backend> logging_sink;
			boost::shared_ptr<logging_sink> sink = boost::make_shared<logging_sink>(root_path);

			sink->set_formatter
				(
					expressions::format("%1% [%2%]-[%3%] %4%")
						% expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
						% expressions::attr<std::string>("Module")
						% trivial::severity
						% expressions::message
				);

			core::get()->add_sink(sink);
			core::get()->add_global_attribute("TimeStamp", attributes::local_clock());

			return true;
		}
		catch (std::exception& )
		{
			return false;
		}
	}

	logger get_logger(const char* name)
	{
		logger lg;
		lg.add_attribute("Module", ::boost::log::attributes::make_constant(name));
		return std::move(lg);
	}

	wlogger get_wlogger(const char* name)
	{
		wlogger lg;
		lg.add_attribute("Module", ::boost::log::attributes::make_constant(name));
		return std::move(lg);
	}
}
