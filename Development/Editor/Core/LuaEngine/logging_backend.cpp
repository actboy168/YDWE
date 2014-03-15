#include "logging_backend.h"
#include <boost/filesystem/fstream.hpp>
#include <base/util/format.h>
#include <base/exception/exception.h>

namespace logging
{
	struct logging_backend::implementation
	{
		boost::filesystem::path root_path_;
		boost::filesystem::ofstream file_;
		uintmax_t written_;

		implementation(const boost::filesystem::path& root_path)
			: root_path_(root_path)
			, file_()
			, written_(0)
		{ }
	};

	logging_backend::logging_backend(const boost::filesystem::path& root_path)
		: impl_(new implementation(root_path))
	{ }

	logging_backend::~logging_backend()
	{
		delete impl_;
	}

	void logging_backend::consume(record_view const& rec, string_type const& formatted_message)
	{
		if((impl_->file_.is_open() && (impl_->written_ + formatted_message.size() >= 512*1024) )
			|| !impl_->file_.good()
			)
		{
			rotate_file();
		}

		if (!impl_->file_.is_open())
		{
			boost::filesystem::create_directories(impl_->root_path_);
			impl_->file_.open(impl_->root_path_ / L"ydwe.log", std::ios_base::app | std::ios_base::out);
			if (!impl_->file_.is_open())
			{
				throw base::exception("Failed to open file '%s' for writing.", (impl_->root_path_ / L"ydwe.log").string().c_str());
			}

			impl_->written_ = static_cast<std::streamoff>(impl_->file_.tellp());
		}

		impl_->file_.write(formatted_message.data(), static_cast<std::streamsize>(formatted_message.size()));
		impl_->file_.put(static_cast<string_type::value_type>('\n'));
		impl_->written_ += formatted_message.size() + 1;
	}

	void logging_backend::flush()
	{
		if (impl_->file_.is_open())
			impl_->file_.flush();
	}

	void logging_backend::rotate_file()
	{
		impl_->file_.close();
		impl_->file_.clear();
		impl_->written_ = 0;

		size_t i = 1;
		for (; i < 10; ++i)
		{
			boost::filesystem::path p = impl_->root_path_ / base::format(L"ydwe%03d.log", i);
			if (!boost::filesystem::exists(p))
			{
				break;
			}
		}
		if (i == 10) i = 1;
		boost::filesystem::rename(impl_->root_path_ / L"ydwe.log", impl_->root_path_ / base::format(L"ydwe%03d.log", i));

		++i; if (i == 10) i = 1;
		boost::filesystem::remove(impl_->root_path_ / base::format(L"ydwe%03d.log", i));
	}
}
