avhttp [![Build Status](https://travis-ci.org/avplayer/avhttp.png?branch=master)](https://travis-ci.org/avplayer/avhttp)
======

avhttp是一个基于Boost.Asio实现的HTTP客户端开发工具库.

-
##### 简介


它支持HTTP(1.0/1.1)、HTTPS，断点续传，多线程并发下载，异步，HTTP/SOCKS4/SOCKS5代理支持等特性，开发者可以轻松的基于这个库开发其他相关应用。

-
##### 快速上手
``` c++
#include <iostream>
#include "avhttp.hpp"

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io;
		avhttp::http_stream h(io);

		h.open("http://www.boost.org/LICENSE_1_0.txt");
		std::cout << &h;
	}
	catch (std::exception& e)
	{
		std::cerr << "Error:" << e.what() << std::endl;
	}

	return 0;
}
```

OK，以上示例将http流通过标准std::cout输出，非常简单明确，这对熟悉使用c++标准流的朋友来说，是一件不可多得的好事！比如下载数据需要通过std::fstream保存，将std::fstream对象替换掉示例中的std::cout即可！

``` c++
#include <iostream>
#include <boost/array.hpp>
#include "avhttp.hpp"

int main()
{
	boost::asio::io_service io;
	avhttp::http_stream h(io);
	boost::system::error_code ec;

	// 打开url.
	h.open("http://www.boost.org/LICENSE_1_0.txt", ec);
	if (ec) { // 打开失败处理...
		std::cout << "Error: " << ec.message() << std::endl;
		return -1;
	}

	boost::array<char, 1024> buf;

	// 循环读取数据.
	while (!ec) {
		std::size_t bytes_transferred = h.read_some(boost::asio::buffer(buf), ec);
		// 将下载的数据打印到屏幕.
		std::cout.write(buf.data(), bytes_transferred);
	}

	std::cout.flush();
	h.close(ec); // 关闭.
	io.run();

	return 0;
}
``` 

OK，上面已经展示了一个简单却功能完善的示例用于同步方式HTTP下载，下面我来介绍异步并发下载多个URL的示范。

``` c++
#include <iostream>

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "avhttp.hpp"

class downloader : public boost::enable_shared_from_this<downloader>
{
public:
	downloader(boost::asio::io_service &io)
		: m_io_service(io)
		, m_stream(io)
	{}
	~downloader()
	{}

public:
	void start(const std::string &url)
	{
		// 异步打开链接, 完成操作时将调用handle_open.
		m_stream.async_open(url,
			boost::bind(&downloader::handle_open, shared_from_this(), boost::asio::placeholders::error));
	}

	void handle_open(const boost::system::error_code &ec)
	{
		if (!ec)
		{
			// 异步发起从http读取数据操作.
			m_stream.async_read_some(boost::asio::buffer(m_buffer, 1024),
				boost::bind(&downloader::handle_read, shared_from_this(),
					boost::asio::placeholders::bytes_transferred,
					boost::asio::placeholders::error
				)
			);
		}
	}

	void handle_read(int bytes_transferred, const boost::system::error_code &ec)
	{
		if (!ec)
		{
			// 输出到屏幕.
			std::cout.write(m_buffer.data(), bytes_transferred);
			// 继续读取http数据.
			m_stream.async_read_some(boost::asio::buffer(m_buffer),
				boost::bind(&downloader::handle_read, shared_from_this(),
					boost::asio::placeholders::bytes_transferred,
					boost::asio::placeholders::error
				)
			);
		}
		else if (ec == boost::asio::error::eof)
		{
			std::cout.write(m_buffer.data(), bytes_transferred);
		}
		std::cout.flush();
	}

private:
	boost::asio::io_service &m_io_service;
	avhttp::http_stream m_stream;
	boost::array<char, 1024> m_buffer;
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "usage: " << argv[0] << " <url1> [url2] ...\n";
		return -1;
	}

	try
	{
		boost::asio::io_service io;
		// 循环遍历url, 并创建下载.
		for (int i = 1; i < argc; i++)
		{
			std::string url = argv[i];
			if (url.empty())
				break;
			boost::shared_ptr<downloader> d(new downloader(io));
			d->start(url);
		}

		io.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
``` 

到现在为止，您已经了解了AVHTTP的同步和异步的基本用法，但事实上有时您还需要定制自己的HTTP请求，请继续往下看，下面介绍HTTP参数相关的设置。


-

##### 使用request_opts定制HTTP请求

``` c++
boost::asio::io_service io;
avhttp::http_stream h(io);

avhttp::request_opts opt;
// 可以insert多个选项.
opt.insert("Connection", "Keep-Alive");

// 在这里设置到request_options.
h.request_options(opt);

// 然后再发起其它相关操作.
h.open("http://www.boost.org/LICENSE_1_0.txt");
// ...
``` 

avhttp::request_opts 在发起HTTP请求之前的设定HTTP选项，它可以实现让您定制自己的http header。

-


##### 使用proxy_settings设置代理

``` c++
boost::asio::io_service io;
avhttp::http_stream h(io);

avhttp::proxy_settings p;
// 这里可以设置3种代理, socks4/socks5/http, 具体可以查看avhttp::proxy_settings的声明.
p.type = avhttp::proxy_settings::http;
p.hostname = "127.0.0.1";
p.port = 8080;
h.proxy(p); // 设置代理.

// 然后再发起其它相关操作.
h.open("http://www.boost.org/LICENSE_1_0.txt");
// ...
``` 

想了解更多的关于avhttp的使用(断点续传并发下载等)，请参考avhttp的example代码。

-


##### 常用问题

* 如果需要支持https，它依赖openssl，请自行编译openssl或到 http://sourceforge.net/projects/avplayer/files/develop/OpenSSL-dev/ 下载已经编译好的ssl开发包，并在项目中设置，启用AVHTTP_ENABLE_OPENSSL。
* 如果需要支持gzip，它依赖zlib，需要在项目中启用AVHTTP_ENABLE_ZLIB，当然您还需要使用avhttp::request_opts指定相应Accept-Encoding。
* 如果您只有一个线程运行io_service，那么定义AVHTTP_DISABLE_THREAD可以避免锁来提高工作效率。
* 如果您还有其它任何问题，请加QQ群：3597082或IRC #avplayer @ irc.freenode.net，或直接mailto：jack.wgm@gmail.com。
