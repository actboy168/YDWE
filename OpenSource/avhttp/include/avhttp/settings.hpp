//
// settings.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2013 Jack (jack dot wgm at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// path LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AVHTTP_SETTINGS_HPP
#define AVHTTP_SETTINGS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <vector>
#include <map>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

#include "avhttp/storage_interface.hpp"

namespace avhttp {

// 如果没有定义最大重定向次数, 则默认为5次最大重定向.
#ifndef AVHTTP_MAX_REDIRECTS
#define AVHTTP_MAX_REDIRECTS 5
#endif

// 常用有以下http选项.
namespace http_options {

	// 下面为avhttp内定的一些选项.
	static const std::string request_method("_request_method"); // 请求方式(GET/POST)
	static const std::string http_version("_http_version");		// HTTP/1.0|HTTP/1.1
	static const std::string request_body("_request_body");		// 一般用于POST一些数据如表单之类时使用.
	static const std::string status_code("_status_code");	// HTTP状态码.
	static const std::string path("_path");		// 请求的path, 如http://abc.ed/v2/cma.txt中的/v2/cma.txt.
	static const std::string url("_url");		// 在启用keep-alive的时候, 请求host上不同的url时使用.
	// 以下是常用的标准http head选项.
	static const std::string host("Host");
	static const std::string accept("Accept");
	static const std::string range("Range");
	static const std::string cookie("Cookie");
	static const std::string referer("Referer");
	static const std::string user_agent("User-Agent");
	static const std::string content_type("Content-Type");
	static const std::string content_length("Content-Length");
	static const std::string content_range("Content-Range");
	static const std::string connection("Connection");
	static const std::string proxy_connection("Proxy-Connection");
	static const std::string accept_encoding("Accept-Encoding");
	static const std::string transfer_encoding("Transfer-Encoding");
	static const std::string content_encoding("Content-Encoding");

} // namespace http_options


// 具体的http的option选项实现.

class option
{
public:
	// 定义option_item类型.
	typedef std::pair<std::string, std::string> option_item;
	// 定义option_item_list类型.
	typedef std::vector<option_item> option_item_list;
	// for boost::assign::insert
	typedef option_item value_type;
	// 定义选项回调.
	typedef boost::function<void (boost::system::error_code&)> body_callback_func;

public:

	option()
		: m_fake_continue(false)
	{}

	~option()
	{}

public:

	// 这样就允许这样的应用:
	// http_stream s;
	// s.request_options(request_opts()("cookie", "XXXXXX"));
	option& operator()(const std::string& key, const std::string& val)
	{
		insert(key, val);
		return *this;
	}

	// 添加选项, 由key/value形式添加.
	void insert(const std::string& key, const std::string& val)
	{
		m_opts.push_back(option_item(key, val));
	}

	// 添加选项，由 std::part 形式.
	void insert(const value_type& item)
	{
		m_opts.push_back(item);
	}

	// 删除选项.
	option& remove(const std::string& key)
	{
		for (option_item_list::iterator i = m_opts.begin(); i != m_opts.end(); i++)
		{
			if (i->first == key)
			{
				m_opts.erase(i);
				return *this;
			}
		}
		return *this;
	}

	// 查找指定key的value.
	bool find(const std::string& key, std::string& val) const
	{
		std::string s = key;
		boost::to_lower(s);
		for (option_item_list::const_iterator f = m_opts.begin(); f != m_opts.end(); f++)
		{
			std::string temp = f->first;
			boost::to_lower(temp);
			if (temp == s)
			{
				val = f->second;
				return true;
			}
		}
		return false;
	}

	// 查找指定的 key 的 value. 没找到返回 "", 这是个偷懒的帮助.
	std::string find(const std::string& key) const
	{
		std::string v;
		find(key, v);
		return v;
	}

	// 得到Header字符串.
	std::string header_string() const
	{
		std::string str;
		for (option_item_list::const_iterator f = m_opts.begin(); f != m_opts.end(); f++)
		{
			if (f->first != http_options::status_code)
				str += (f->first + ": " + f->second + "\r\n");
		}
		return str;
	}

	// 清空.
	void clear()
	{
		m_opts.clear();
	}

	// 返回所有option.
	option_item_list& option_all()
	{
		return m_opts;
	}

	// 返回当前option个数.
	int size() const
	{
		return m_opts.size();
	}

	// 返回fake_continue.
	bool fake_continue() const
	{
		return m_fake_continue;
	}

	// 设置fake_continue.
	void fake_continue(bool b)
	{
		m_fake_continue = b;
	}

protected:
	// 选项列表.
	option_item_list m_opts;

	// 是否启用假100 continue消息, 如果启用, 则在发送完成http request head
	// 之后, 返回一个fake continue消息.
	bool m_fake_continue;
};

// 请求时的http选项.
// _http_version, 取值 "HTTP/1.0" / "HTTP/1.1", 默认为"HTTP/1.1".
// _request_method, 取值 "GET/POST/HEAD", 默认为"GET".
// _request_body, 请求中的body内容, 取值任意, 默认为空.
// Host, 取值为http服务器, 默认为http服务器.
// Accept, 取值任意, 默认为"*/*".
// 这些比较常用的选项被定义在http_options中.
typedef option request_opts;

// http服务器返回的http选项.
// 一般会包括以下几个选项:
// _status_code, http返回状态.
// Server, 服务器名称.
// Content-Length, 数据内容长度.
// Connection, 连接状态标识.
typedef option response_opts;



// Http请求的代理设置.

struct proxy_settings
{
	proxy_settings()
		: type (none)
	{}

	std::string hostname;
	int port;

	std::string username;
	std::string password;

	enum proxy_type
	{
		// 没有设置代理.
		none,
		// socks4代理, 需要username.
		socks4,
		// 不需要用户密码的socks5代理.
		socks5,
		// 需要用户密码认证的socks5代理.
		socks5_pw,
		// http代理, 不需要认证.
		http,
		// http代理, 需要认证.
		http_pw,
	};

	proxy_type type;
};


// 一些默认的值.
static const int default_request_piece_num = 10;
static const int default_time_out = 11;
static const int default_connections_limit = 5;
static const int default_buffer_size = 1024;

// multi_download下载设置.

struct settings
{
	settings ()
		: download_rate_limit(-1)
		, connections_limit(default_connections_limit)
		, piece_size(-1)
		, time_out(default_time_out)
		, request_piece_num(default_request_piece_num)
		, allow_use_meta_url(true)
		, disable_multi_download(false)
		, check_certificate(true)
		, storage(NULL)
	{}

	// 下载速率限制, -1为无限制, 单位为: byte/s.
	int download_rate_limit;

	// 连接数限制, -1为默认.
	int connections_limit;

	// 分块大小, 默认根据文件大小自动计算.
	int piece_size;

	// 超时断开, 默认为11秒.
	int time_out;

	// 每次请求的分片数, 默认为10.
	int request_piece_num;

	// meta_file路径, 默认为当前路径下同文件名的.meta文件.
	fs::path meta_file;

	// 允许使用meta中保存的url, 默认为允许. 针对一些变动的url, 我们应该禁用.
	bool allow_use_meta_url;

	// 禁止使用并发下载.
	// NOTE: 比如用于动态页面下载, 因为动态页面不能使用并发下载, 如果还想继续使用
	// multi_download进行下载, 则需要设置这个参数, 否则并发下载动态数据的行为, 是
	// 未定义的, 其结果可能因为数据长度不一至导致断言, 也可能数据错误.
	// NOTE: 不推荐使用multi_download进行下载, 而应该使用http_stream进行下载,
	// multi_download主要应用在大文件, 静态页面下载!
	bool disable_multi_download;

	// 下载文件路径, 默认为当前目录.
	fs::path save_path;

	// 设置是否检查证书, 默认检查证书.
	bool check_certificate;

	// 存储接口创建函数指针, 默认为multi_download提供的file.hpp实现.
	storage_constructor_type storage;

	// 请求选项.
	request_opts opts;

	// 代理设置.
	proxy_settings proxy;
};

} // namespace avhttp

#endif // AVHTTP_SETTINGS_HPP
