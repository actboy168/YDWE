#pragma once

#include <slk/port/config.h>
#include <string>
#include <memory.h>
#include <base/util/string_view.h>

namespace slk
{
	class object_id
	{
	public:
		struct not_swap_t {};

		object_id()
			: _v(0)
		{ }

		object_id(const object_id& that)
			: _v(that._v)
		{ }

		object_id(const std::string& id)
		{
			if (id.size() >= 4)
			{
				memcpy(&_c[0], &*id.begin(), 4*sizeof(char));
			}
			else
			{
				_v = 0;
				memcpy(&_c[0], &*id.begin(), id.size()*sizeof(char));
			}
		}

		object_id(const std::string_view& id)
		{
			if (id.size() >= 4)
			{
				memcpy(&_c[0], &*id.begin(), 4*sizeof(char));
			}
			else
			{
				_v = 0;
				memcpy(&_c[0], &*id.begin(), id.size()*sizeof(char));
			}
		}

		object_id(const uint32_t& id)
			: _v(id)
		{
			swap();
		}

		object_id(const uint32_t& id, not_swap_t const&)
			: _v(id)
		{ }

		bool operator < (const object_id& that) const
		{
			return _v < that._v;
		}

		operator uint32_t () const
		{
			return _v;
		}

		std::string to_string() const
		{
			return std::move(std::string(_c, 4));
		}

		bool is_hero() const
		{
			return 0 != isupper(static_cast<unsigned char>(_c[0])); 
		}

		bool vaild() const
		{
			return _v >= '000';
		}

	private:
		union
		{
			uint32_t  _v;
			char      _c[4];
		};

		void swap()
		{
			std::swap(_c[0], _c[3]);
			std::swap(_c[1], _c[2]);
		}
	};
}
