#include "http_thread.h"
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <queue>

// 支持http gzip
#define AVHTTP_ENABLE_ZLIB
// 单线程执行io_service
#define AVHTTP_DISABLE_THREAD
// 禁用上传文件功能
#define AVHTTP_DISABLE_FILE_UPLOAD
// 禁用下载文件功能
#define AVHTTP_DISABLE_MULTI_DOWNLOAD  
// 禁用日志
#define DISABLE_LOGGER
#include <avhttp.hpp>

namespace http
{
	task::task()
		: m_result()
		, m_finish()
	{ }

	void task::then(const finish_t& finish)
	{
		m_finish = finish;
	}

	std::ostream& task::get_ostream()
	{
		return m_result;
	}

	void task::on_success()
	{
		if (m_finish)
		{
			m_finish(0, m_result.str());
		}
	}

	void task::on_fail(int code, const std::string& msg)
	{
		if (m_finish)
		{
			m_finish(code, msg);
		}
	}

	class ctask
		: public boost::enable_shared_from_this<ctask>
	{
	public:
		ctask(boost::asio::io_service& io)
			: m_io_service(io)
			, m_stream(io)
			, m_buffer()
			, m_task()
		{ }

		~ctask()
		{ }

	public:
		void initialize()
		{
			m_status.push(boost::bind(&ctask::async_read, shared_from_this(), boost::asio::placeholders::error));
		}

		void get(const std::string& url)
		{
			initialize();
			m_stream.check_certificate(false);
			m_stream.async_open(url, boost::bind(&ctask::async_next, shared_from_this(), boost::asio::placeholders::error));
		}

		void post(const std::string& url, const std::string& body)
		{
			avhttp::request_opts options;
			options.insert(avhttp::http_options::request_method, "POST");
			options.insert(avhttp::http_options::request_body, body);
			options.insert(avhttp::http_options::content_length, std::to_string(body.size()));
			m_stream.request_options(options);
			get(url);
		}

		void async_next(const boost::system::error_code &ec)
		{
			if (!m_status.empty())
			{
				callback_t& callback = m_status.front();
				callback(ec);
				m_status.pop();
			}
		}

		void async_read(const boost::system::error_code &ec)
		{
			if (!ec)
			{
				m_stream.async_read_some(
					boost::asio::buffer(m_buffer, 1024)
					, boost::bind(&ctask::async_read_continue, shared_from_this(), boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error)
					);
			}
			else
			{
				m_task.on_fail(ec.value(), ec.message());
			}
		}

		void async_read_continue(int bytes_transferred, const boost::system::error_code &ec)
		{
			if (!ec)
			{
				m_task.get_ostream().write(m_buffer.data(), bytes_transferred);
				m_stream.async_read_some(
					boost::asio::buffer(m_buffer)
					, boost::bind(&ctask::async_read_continue, shared_from_this(), boost::asio::placeholders::bytes_transferred, boost::asio::placeholders::error)
					);
			}
			else if (ec == boost::asio::error::eof)
			{
				m_task.get_ostream().write(m_buffer.data(), bytes_transferred);
				m_task.on_success();
			}
			else
			{
				m_task.on_fail(ec.value(), ec.message());
			}
		}

		void async_end(const boost::system::error_code &ec)
		{
			if (!ec)
			{
				m_task.on_success();
			}
			else
			{
				m_task.on_fail(ec.value(), ec.message());
			}
		}

		task& get()
		{
			return m_task;
		}

	private:
		typedef boost::function<void(const boost::system::error_code &)> callback_t;

		boost::asio::io_service& m_io_service;
		avhttp::http_stream      m_stream;
		boost::array<char, 1024> m_buffer;
		std::queue<callback_t>   m_status;
		task                     m_task;
	};

	thread::thread()
		: io_service()
		, work()
	{ }

	thread::~thread()
	{ }

	void thread::push()
	{
		work.reset(new boost::asio::io_service::work(io_service));
	}

	void thread::pop()
	{
		work.reset();
	}

	size_t thread::run_one()
	{
		return io_service.run_one();
	}

	size_t thread::poll_one()
	{
		return io_service.poll_one();
	}

	task& thread::get(const std::string& url)
	{
		boost::shared_ptr<ctask> t(new ctask(io_service));
		t->get(url);
		return t->get();
	}

	task& thread::post(const std::string& url, const std::string& body)
	{
		boost::shared_ptr<ctask> t(new ctask(io_service));
		t->post(url, body);
		return t->get();
	}
}
