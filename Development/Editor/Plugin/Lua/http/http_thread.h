#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <boost/asio.hpp>
#include <sstream>
#include <functional>
#include <stdint.h>

namespace http
{
	typedef uintptr_t thread_t;

	class task
	{
	public:
		typedef std::function<void(int, const std::string&)> finish_t;

		void then(const finish_t& finish);

	public:
		task();
		std::ostream& get_ostream();
		void on_success();
		void on_fail(int code, const std::string& msg);

	private:
		task(task&);
		task& operator=(task&);

	private:
		std::stringstream m_result;
		finish_t          m_finish;
	};

	class thread
	{
	public:
		// 创建http线程(并不会创建OS线程)
		thread();

		// 释放http线程
		~thread();

		// 增加http线程的引用，让线程不会因为当前没有请求就终止
		void     push();

		// 减少http线程的引用，让线程在当前没有请求时就终止
		void     pop();

		// 执行http线程，非阻塞执行一次
		size_t   poll_one();

		// 执行http线程，阻塞执行一次
		size_t   run_one();
			 
		// 异步访问http服务器
		task&    get(const std::string& url);
		task&    post(const std::string& url, const std::string& body);

	private:
		boost::asio::io_service io_service;
		std::unique_ptr<boost::asio::io_service::work> work;
	};
}
