#pragma once

#include <iostream>
#include <memory>
#include <sstream>			
#include "logging_datetime.h"

namespace logging {
	enum level {
		trace = 0,
		debug,
		info,
		warning,
		error,
		fatal
	};

	inline char const* time_now_string()
	{
		static char str[64] = { 0 };
		long usec = 0;
		struct tm t = datetime::now(&usec);
#if defined _MSC_VER
		sprintf_s(str, 63,
#else
		sprintf(str,
#endif
			"%04d-%02d-%02d %02d:%02d:%02d.%3ld", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, usec / 1000);

		return str;
	}
	
	struct stdio_backend
	{
		void consume(const std::string& message)
		{
			std::cout << message << std::endl;
		}
		void flush()
		{ 
			std::cout.flush();
		}
	};

	template <class BackendT>
	struct sync_frontend
	{
		typedef BackendT backend_type;

		sync_frontend(backend_type& backend)
			: backend_(backend)
		{ }

		void set_backend(backend_type& backend)
		{
			backend_ = backend;
		}

		void consume(const std::string& message)
		{
			backend_.consume(message);
		}

		void flush(const std::string& message)
		{
			backend_.flush();
		}

	protected:
		backend_type& backend_;
	};

	template <class FrontendT>
	struct logger_t
	{
		typedef FrontendT frontend_type;
		typedef typename frontend_type::backend_type backend_type;
		
		logger_t(const std::string& name)
			: backend_()
			, frontend_(backend_)
			, name_(name)
		{ }

		logger_t(const std::string& name, backend_type&& backend)
			: backend_(std::move(backend))
			, frontend_(backend_)
			, name_(name)
		{ }

		void set_frontend(frontend_type&& frontend)
		{
			frontend_ = frontend;
			frontend_.set_backend(backend_);
		}

		void set_backend(backend_type&& backend)
		{
			backend_ = backend;
			frontend_.set_backend(backend_);
		}

		void consume(const std::string& message)
		{
			frontend_.consume(message);
		}

		const std::string& name()	const
		{
			return name_;
		}

		void flush()
		{
			frontend_.flush();
		}

	protected:
		backend_type  backend_;
		frontend_type frontend_;
		std::string   name_;
	};

	typedef sync_frontend<stdio_backend> stdio_frontend;
	typedef logger_t<stdio_frontend> stdio_logger;

	template <class LoggerT>
	struct record_pump
	{
		typedef LoggerT logger_type;
	
		explicit record_pump(logger_type& logger, size_t lvl)
			: logger_(std::addressof(logger))
			, stream_(new std::stringstream)
		{
			static const char* s_lvlstring[] = {
				"trace",
				"debug",
				"info ",
				"warn ",
				"error",
				"fatal"
			};
			*stream_ << time_now_string() << " " << "[" << logger.name() << "]-[" << s_lvlstring[lvl] << "] ";
		}
	
		record_pump(record_pump&& that)
			: logger_(that.logger_)
			, stream_(that.stream_.release())
		{
			that.logger_ = 0;
		}
	
		~record_pump()
		{
			if (logger_ && stream_)
			{
				std::string str = stream_->str();
				while (!str.empty() && isspace((int)(unsigned char)str.back())) str.pop_back();
				logger_->consume(str);
			}
		}
	
		std::stringstream& stream()
		{
			return *stream_;
		}
	
	protected:
		logger_type*      logger_;
		std::unique_ptr<std::stringstream> stream_;

	private:
		record_pump(const record_pump&);
		const record_pump& operator=(const record_pump&);
	};

	template <class LoggerT>
	inline record_pump<LoggerT> make_record_pump(LoggerT& logger, size_t lvl)
	{
		return record_pump<LoggerT>(logger, lvl);
	}
}


