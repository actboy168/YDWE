//
// storage_interface.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2013 Jack (jack dot wgm at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// path LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AVHTTP_STORAGE_INTERFACE_HPP
#define AVHTTP_STORAGE_INTERFACE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <boost/cstdint.hpp>

namespace avhttp {

namespace fs = boost::filesystem;

// 数据存储接口.
struct storage_interface
{
	storage_interface() {}
	virtual ~storage_interface() {}

	// 存储组件初始化.
	// @param file_path指定了文件名路径信息.
	// @param ec在出错时保存了详细的错误信息.
	virtual void open(const fs::path& file_path, boost::system::error_code& ec) = 0;

	// 关闭存储组件.
	virtual void close() = 0;

	// 写入数据到文件.
	// @param buf是需要写入的数据缓冲.
	// @param size指定了写入的数据缓冲大小.
	// @返回值为实际写入的字节数, 返回-1表示写入失败.
	// 备注: 在文件指针当前位置写入, 写入完成将自动移动文件指针到完成位置, 保证和fwrite行为一至.
	virtual std::streamsize write(const char* buf, int size) = 0;

	// 写入数据.
	// @param buf是需要写入的数据缓冲.
	// @param offset是写入的偏移位置.
	// @param size指定了写入的数据缓冲大小.
	// @返回值为实际写入的字节数, 返回-1表示写入失败.
	virtual std::streamsize write(const char* buf, boost::int64_t offset, int size) = 0;

	// 从文件读取数据.
	// @param buf是需要读取的数据缓冲.
	// @param size指定了读取的数据缓冲大小.
	// @返回值为实际读取的字节数, 返回-1表示读取失败.
	// 备注: 在文件指针当前位置开始读取, 读取完成将自动移动文件指针到完成位置, 保证和fread行为一至.
	virtual std::streamsize read(char* buf, int size) = 0;

	// 读取数据.
	// @param buf是需要读取的数据缓冲.
	// @param offset是读取的偏移位置.
	// @param size指定了读取的数据缓冲大小.
	// @返回值为实际读取的字节数, 返回-1表示读取失败.
	virtual std::streamsize read(char* buf, boost::int64_t offset, int size) = 0;

	// 判断是否文件结束.
	// 返回值true表示文件结束.
	virtual bool eof() = 0;
};

// 重定义storage_interface创建函数指针, 在multi_download内部通过调用它来完成创建storage_interface.
typedef storage_interface* (*storage_constructor_type)();

}

#endif // AVHTTP_STORAGE_INTERFACE_HPP
