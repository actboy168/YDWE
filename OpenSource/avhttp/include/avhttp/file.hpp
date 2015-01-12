//
// file.hpp
// ~~~~~~~~
//
// Copyright (c) 2013 Jack (jack dot wgm at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// path LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AVHTTP_FILE_HPP
#define AVHTTP_FILE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/noncopyable.hpp>
#include <boost/filesystem/fstream.hpp>

namespace avhttp {

class file : public boost::noncopyable
{
public:
	enum
	{
		// when a file is opened with no_buffer
		// file offsets have to be aligned to
		// pos_alignment() and buffer addresses
		// to buf_alignment() and read/write sizes
		// to size_alignment()
		read_only = 0,
		write_only = 1,
		read_write = 2,
		rw_mask = read_only | write_only | read_write,
		no_buffer = 4,
		mode_mask = rw_mask | no_buffer,
		sparse = 8,

		attribute_hidden = 0x1000,
		attribute_executable = 0x2000,
		attribute_mask = attribute_hidden | attribute_executable
	};

#ifdef WIN32
	struct iovec_t
	{
		void* iov_base;
		size_t iov_len;
	};
#else
	typedef iovec iovec_t;
#endif

	// use a typedef for the type of iovec_t::iov_base
	// since it may differ
#ifdef __sparc__
	typedef char* iovec_base_t;
#else
	typedef void* iovec_base_t;
#endif

	typedef boost::int64_t size_type;
	typedef boost::uint64_t unsigned_size_type;

public:
	AVHTTP_DECL explicit file();
	AVHTTP_DECL explicit file(fs::path const& p, int m, boost::system::error_code& ec);
	AVHTTP_DECL ~file(void);

	AVHTTP_DECL void open(fs::path const& p, int m);
	AVHTTP_DECL void open(fs::path const& p, int m, boost::system::error_code& ec);
	AVHTTP_DECL bool is_open() const;
	AVHTTP_DECL void close();
	AVHTTP_DECL bool set_size(size_type size, boost::system::error_code& ec);

	AVHTTP_DECL int open_mode() const { return m_open_mode; }

	// when opened in unbuffered mode, this is the
	// required alignment of file_offsets. i.e.
	// any (file_offset & (pos_alignment()-1)) == 0
	// is a precondition to read and write operations
	AVHTTP_DECL int pos_alignment() const;

	// when opened in unbuffered mode, this is the
	// required alignment of buffer addresses
	AVHTTP_DECL int buf_alignment() const;

	// read/write buffer sizes needs to be aligned to
	// this when in unbuffered mode
	AVHTTP_DECL int size_alignment() const;

	AVHTTP_DECL size_type write(const char* buf, int size);
	AVHTTP_DECL size_type read(char* buf, int size);

	AVHTTP_DECL size_type write(size_type offset, const char* buf, int size);
	AVHTTP_DECL size_type read(size_type offset, char* buf, int size);

	AVHTTP_DECL size_type writev(size_type file_offset, iovec_t const* bufs, int num_bufs, boost::system::error_code& ec);
	AVHTTP_DECL size_type readv(size_type file_offset, iovec_t const* bufs, int num_bufs, boost::system::error_code& ec);

	AVHTTP_DECL bool flush();

	AVHTTP_DECL size_type offset(boost::system::error_code& ec);
	AVHTTP_DECL file::size_type offset(size_type offset, boost::system::error_code& ec);

	AVHTTP_DECL size_type get_size(boost::system::error_code& ec) const;

	// return the offset of the first byte that
	// belongs to a data-region
	AVHTTP_DECL size_type sparse_end(size_type start) const;

	AVHTTP_DECL size_type phys_offset(size_type offset);

#ifdef WIN32
	AVHTTP_DECL HANDLE native_handle() const { return m_file_handle; }
#else
	AVHTTP_DECL int native_handle() const { return m_fd; }
#endif

private:

#ifdef WIN32
	HANDLE m_file_handle;
	std::string m_path;
#else
	int m_fd;
#endif
#if defined WIN32 || defined __linux__ || defined DEBUG
	AVHTTP_DECL void init_file() const;
	mutable int m_page_size;
#endif
	int m_open_mode;
#if defined WIN32 || defined __linux__
	mutable int m_sector_size;
#endif
#if defined WIN32
	mutable int m_cluster_size;
#endif
};

} // namespace avhttp

#if defined(AVHTTP_HEADER_ONLY)
#	include "avhttp/impl/file.ipp"
#endif

#endif // AVHTTP_FILE_HPP
