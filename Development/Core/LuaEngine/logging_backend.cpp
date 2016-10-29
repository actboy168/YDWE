#include "logging_backend.h"
#include <base/util/format.h>
#include <base/exception/exception.h>

namespace logging
{
	struct backend::implementation
	{
		fs::path root_;
		std::wstring name_;
		std::ofstream file_;
		uintmax_t written_;

		implementation(const fs::path& root, const std::wstring& name)
			: root_(root)
			, name_(name)
			, file_()
			, written_(0)
		{ }
	};

	backend::backend(const fs::path& root, const std::wstring& name)
		: impl_(new implementation(root, name))
	{ }

	backend::backend(backend&& that)
		: impl_(that.impl_)
	{
		that.impl_ = 0;
	}

	backend::~backend()
	{
		delete impl_;
		impl_ = 0;
	}

	void backend::consume(string_type const& formatted_message)
	{
		if (!impl_)
			return;

		if((impl_->file_.is_open() && (impl_->written_ + formatted_message.size() >= 512*1024) )
			|| !impl_->file_.good()
			)
		{
			rotate_file();
		}

		if (!impl_->file_.is_open())
		{
			fs::create_directories(impl_->root_);
			impl_->file_.open((impl_->root_ / (impl_->name_ + L".log")).c_str(), std::ios_base::app | std::ios_base::out);
			if (!impl_->file_.is_open())
			{
				throw base::exception("Failed to open file '%s' for writing.", (impl_->root_ / (impl_->name_ + L".log")).string().c_str());
			}

			impl_->written_ = static_cast<std::streamoff>(impl_->file_.tellp());
		}

		impl_->file_.write(formatted_message.data(), static_cast<std::streamsize>(formatted_message.size()));
		impl_->file_.put(static_cast<string_type::value_type>('\n'));
		impl_->written_ += formatted_message.size() + 1;
		impl_->file_.flush();
	}

	void backend::flush()
	{
		if (!impl_)
			return;
		if (impl_->file_.is_open())
			impl_->file_.flush();
	}

	void backend::rotate_file()
	{
		if (!impl_)
			return;
		impl_->file_.close();
		impl_->file_.clear();
		impl_->written_ = 0;

		size_t i = 1;
		for (; i < 10; ++i)
		{
			fs::path p = impl_->root_ / base::format(L"%s%03d.log", impl_->name_, i);
			if (!fs::exists(p))
			{
				break;
			}
		}
		if (i == 10) i = 1;
		fs::rename(impl_->root_ / (impl_->name_ + L".log"), impl_->root_ / base::format(L"%s%03d.log", impl_->name_, i));

		++i; if (i == 10) i = 1;
		fs::remove(impl_->root_ / base::format(L"%s%03d.log", impl_->name_, i));
	}
}
