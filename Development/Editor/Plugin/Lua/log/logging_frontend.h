#pragma once

#include <boost/log/sinks/basic_sink_frontend.hpp>
#include <base/thread/lock/spin.h> 

namespace logging
{
	using namespace boost::log;

	template <typename SinkBackendT>
	class logging_frontend
		: public boost::log::BOOST_LOG_VERSION_NAMESPACE::sinks::basic_formatting_sink_frontend<typename SinkBackendT::char_type>
	{
	public:
		typedef boost::log::BOOST_LOG_VERSION_NAMESPACE::sinks::basic_formatting_sink_frontend<typename SinkBackendT::char_type> base_type;
		typedef SinkBackendT sink_backend_type;
		typedef base::lock::spin<> backend_mutex_type;

	private:
		const boost::shared_ptr<sink_backend_type> m_pBackend;
		backend_mutex_type                         m_BackendMutex;

	public:
		logging_frontend()
			: base_type(false)
			, m_pBackend(boost::make_shared<sink_backend_type>())
		{ }

		explicit logging_frontend(boost::shared_ptr<sink_backend_type> const& backend)
			: base_type(false)
			, m_pBackend(backend)
		{ }

		template <class T1, class T2>
		explicit logging_frontend(const T1& arg1, const T2& arg2)
			: base_type(false)
			, m_pBackend(boost::make_shared<sink_backend_type>(arg1, arg2))
		{ }

		void consume(record_view const& rec)
		{
			base_type::feed_record(rec, m_BackendMutex, *m_pBackend);
		}

		bool try_consume(record_view const& rec)
		{
			return base_type::try_feed_record(rec, m_BackendMutex, *m_pBackend);
		}

		void flush()
		{
			base_type::flush_backend(m_BackendMutex, *m_pBackend);
		}
	};
}
