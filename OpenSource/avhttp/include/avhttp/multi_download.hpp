//
// multi_download.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2013 Jack (jack dot wgm at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// path LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AVHTTP_MULTI_DOWNLOAD_HPP
#define AVHTTP_MULTI_DOWNLOAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <vector>
#include <list>
#include <algorithm>    // for std::min/std::max

#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/crc.hpp>  // for boost::crc_32_type

#include "avhttp/http_stream.hpp"
#include "avhttp/rangefield.hpp"
#include "avhttp/entry.hpp"
#include "avhttp/settings.hpp"


namespace avhttp {

// multi_download类的具体实现.
class multi_download : public boost::noncopyable
{
	// 重定义http_stream_ptr指针.
	typedef boost::shared_ptr<http_stream> http_stream_ptr;

	// 定义http_stream_obj.
	struct http_stream_object;

	// 重定义http_object_ptr指针.
	typedef boost::shared_ptr<http_stream_object> http_object_ptr;

	// 用于计算下载速率.
	struct download_stat;
	typedef boost::shared_ptr<download_stat> download_stat_ptr;

	// 用于帮助multi_download自动计算outstranding.
	struct auto_outstanding;
	friend struct auto_outstanding;

public:

	/// Constructor.
	AVHTTP_DECL explicit multi_download(boost::asio::io_service& io);

	/// Destructor.
	AVHTTP_DECL ~multi_download();

public:

	///开始multi_download开始下载.
	// @param u指定的url.
	// @param ec当发生错误时, 包含详细的错误信息.
	// @备注: 直接使用内部的file.hpp下载数据到文件, 若想自己指定数据下载到指定的地方
	// 可以通过调用另一个open来完成, 具体见另一个open的详细说明.
	AVHTTP_DECL void start(const std::string& u, boost::system::error_code& ec);

	///开始multi_download开始下载, 打开失败抛出一个异常.
	// @param u指定的url.
	// @备注: 直接使用内部的file.hpp下载数据到文件, 若想自己指定数据下载到指定的地方
	// 可以通过调用另一个open来完成, 具体见另一个open的详细说明.
	AVHTTP_DECL void start(const std::string& u);

	///开始multi_download开始下载.
	// @param u指定的url.
	// @param s指定的设置信息.
	// @失败抛出一个boost::system::system_error异常, 包含详细的错误信息.
	AVHTTP_DECL void start(const std::string& u, const settings& s);

	///开始multi_download开始下载.
	// @param u指定的url.
	// @param s指定的设置信息.
	// @返回error_code, 包含详细的错误信息.
	AVHTTP_DECL void start(const std::string& u, const settings& s, boost::system::error_code& ec);

	///异步启动下载, 启动完成将回调对应的Handler.
	// @param u 将要下载的URL.
	// @param handler 将被调用在启动完成时. 它必须满足以下条件:
	// @begin code
	//  void handler(
	//    const boost::system::error_code& ec // 用于返回操作状态.
	//  );
	// @end code
	// @begin example
	//  void start_handler(const boost::system::error_code& ec)
	//  {
	//    if (!ec)
	//    {
	//      // 启动下载成功!
	//    }
	//  }
	//  ...
	//  avhttp::multi_download h(io_service);
	//  h.async_open("http://www.boost.org", start_handler);
	// @end example
	// @备注: handler也可以使用boost.bind来绑定一个符合规定的函数作
	// 为async_start的参数handler.
	template <typename Handler>
	void async_start(const std::string& u, Handler handler);

	///异步启动下载, 启动完成将回调对应的Handler.
	// @param u 将要下载的URL.
	// @param s 下载设置参数信息.
	// @param handler 将被调用在启动完成时. 它必须满足以下条件:
	// @begin code
	//  void handler(
	//    const boost::system::error_code& ec // 用于返回操作状态.
	//  );
	// @end code
	// @begin example
	//  void start_handler(const boost::system::error_code& ec)
	//  {
	//    if (!ec)
	//    {
	//      // 启动下载成功!
	//    }
	//  }
	//  ...
	//  avhttp::multi_download h(io_service);
	//  settings s;
	//  h.async_open("http://www.boost.org", s, start_handler);
	// @end example
	// @备注: handler也可以使用boost.bind来绑定一个符合规定的函数作
	// 为async_start的参数handler.
	template <typename Handler>
	void async_start(const std::string& u, const settings& s, Handler handler);

	// stop当前所有连接, 停止工作.
	AVHTTP_DECL void stop();

	///获取指定的数据, 并改变下载点的位置.
	// @param buffers 指定的数据缓冲. 这个类型必须满足MutableBufferSequence的定义,
	//          MutableBufferSequence的定义在boost.asio文档中.
	// @param offset 读取数据的指定偏移位置, 注意: offset影响内部下载位置从offset开始下载.
	// 返回读取数据的大小.
	template <typename MutableBufferSequence>
	std::size_t fetch_data(const MutableBufferSequence& buffers,
		boost::int64_t offset);

	///返回当前设置信息.
	AVHTTP_DECL const settings& set() const;

	///是否停止下载.
	AVHTTP_DECL bool stopped() const;

	///等待直接下载完成.
	// @完成返回true, 否则返回false.
	AVHTTP_DECL bool wait_for_complete();

	///设置是否检查证书, 默认检查证书.
	// @param check指定是否检查ssl证书.
	AVHTTP_DECL void check_certificate(bool check);

	///返回当前下载的文件大小.
	// @如果服务器不支持多点下载, 则可能文件大小为-1.
	AVHTTP_DECL boost::int64_t file_size() const;

	///根据url计算出对应的meta文件名.
	// @param url是指定的url地址.
	// @返回一串由crc32编码url后的16进制字符串meta文件名.
	AVHTTP_DECL std::string meta_name(const std::string& url) const;

	///得到当前下载的文件名.
	// @如果请求的url不太规则, 则可能返回错误的文件名.
	AVHTTP_DECL std::string file_name() const;

	///当前已经下载的字节总数.
	AVHTTP_DECL boost::int64_t bytes_download() const;

	///当前下载速率, 单位byte/s.
	AVHTTP_DECL int download_rate() const;

	///设置下载速率, -1为无限制, 单位byte/s.
	AVHTTP_DECL void download_rate_limit(int rate);

	///返回当前限速.
	AVHTTP_DECL int download_rate_limit() const;

protected:

	AVHTTP_DECL void handle_open(const int index,
		http_object_ptr object_ptr, const boost::system::error_code& ec);

	AVHTTP_DECL void handle_read(const int index,
		http_object_ptr object_ptr, int bytes_transferred, const boost::system::error_code& ec);

	AVHTTP_DECL void handle_request(const int index,
		http_object_ptr object_ptr, const boost::system::error_code& ec);

	template <typename Handler>
	void handle_start(Handler handler, http_object_ptr object_ptr, const boost::system::error_code& ec);

	AVHTTP_DECL void on_tick(const boost::system::error_code& e);

	AVHTTP_DECL bool allocate_range(range& r);

	AVHTTP_DECL bool open_meta(const fs::path& file_path);

	AVHTTP_DECL void update_meta();

private:

	AVHTTP_DECL void change_outstranding(bool addref = true);

	// 默认根据文件大小自动计算分片大小.
	AVHTTP_DECL std::size_t default_piece_size(const boost::int64_t& file_size) const;

private:
	// io_service引用.
	boost::asio::io_service& m_io_service;

	// 每一个http_stream_obj是一个http连接.
	// 注意: 容器中的http_object_ptr只能在on_tick一处进行写操作, 并且确保其它地方
	// 是新的副本, 这主要体现在发起新的异步操作的时候将http_object_ptr作为参数形式
	// 传入, 这样在异步回调中只需要访问http_object_ptr的副本指针, 而不是直接访问
	// m_streams!!!
	std::vector<http_object_ptr> m_streams;

#ifndef AVHTTP_DISABLE_THREAD
	// 为m_streams在多线程环境下线程安全.
	mutable boost::mutex m_streams_mutex;
#endif

	// 最终的url, 如果有跳转的话, 是跳转最后的那个url.
	url m_final_url;

	// 是否支持多点下载.
	bool m_accept_multi;

	// 是否支持长连接.
	bool m_keep_alive;

	// 文件大小, 如果没有文件大小值为-1.
	boost::int64_t m_file_size;

	// 保存的文件名.
	mutable std::string m_file_name;

	// 当前用户设置.
	settings m_settings;

	// 定时器, 用于定时执行一些任务, 比如检查连接是否超时之类.
	boost::asio::deadline_timer m_timer;

	// 动态计算速率.
	download_stat_ptr m_download_rate;

	// 实际连接数.
	int m_number_of_connections;

	// 下载计时.
	int m_time_total;

	// 下载数据存储接口指针, 可由用户定义, 并在open时指定.
	boost::scoped_ptr<storage_interface> m_storage;

	// meta文件, 用于续传.
	file m_file_meta;

	// 下载点位置.
	boost::int64_t m_download_point;

	// 文件区间图, 每次请求将由m_rangefield来分配空间区间.
	rangefield m_rangefield;

	// 已经下载的文件区间.
	rangefield m_downlaoded_field;

	// 保证分配空闲区间的唯一性.
#ifndef AVHTTP_DISABLE_THREAD
	boost::mutex m_rangefield_mutex;
#endif

	// 用于限速.
	int m_drop_size;

	// 用于异步工作计数.
	int m_outstanding;

#ifndef AVHTTP_DISABLE_THREAD
	mutable boost::mutex m_outstanding_mutex;
#endif

	// 用于通知wait_for_complete退出.
	mutable boost::mutex m_quit_mtx;
	mutable boost::condition m_quit_cond;

	// 是否中止工作.
	bool m_abort;
};

} // avhttp

#if defined(AVHTTP_HEADER_ONLY)
#	include "avhttp/impl/multi_download.ipp"
#endif

#endif // AVHTTP_MULTI_DOWNLOAD_HPP
