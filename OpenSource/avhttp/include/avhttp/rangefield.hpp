//
// rangefield.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2013 Jack (jack dot wgm at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AVHTTP_RANGEFIELD_HPP
#define AVHTTP_RANGEFIELD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <map>
#include <vector>
#include <algorithm>    // for std::min/std::max

#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "avhttp/bitfield.hpp"



namespace avhttp {

// 定义请求数据范围类型.
struct range
{
	inline range(boost::int64_t l, boost::int64_t r)
		: left(l)
		, right(r)
	{}
	inline range()
		: left(0)
		, right(0)
	{}

	inline boost::int64_t size()
	{
		return right - left + 1;	// 注意: 由于http请求带右边界, 所以必须把右边界计算进去!!!
	}

	inline bool operator ==(const range& r) const
	{
		if (left == r.left && right == r.right)
			return true;
		return false;
	}

	inline bool operator!=(const range& r) const
	{
		if (left == r.left && right == r.right)
			return false;
		return true;
	}

	boost::int64_t left;
	boost::int64_t right;
};

// 一个按区域来划分的位图实现(类似Range tree).
class rangefield
{
	typedef std::map<boost::int64_t, boost::int64_t> range_map;

public:
	// @param size表示区间的总大小.
	inline rangefield(boost::int64_t size = 0)
		: m_need_merge(false)
		, m_size(size)
#ifndef AVHTTP_DISABLE_THREAD
		, m_mutex(boost::make_shared<boost::mutex>())
#endif
	{}

	inline ~rangefield()
	{}

	rangefield(const rangefield& rhs)
	{
		m_need_merge = rhs.m_need_merge;
		m_ranges = rhs.m_ranges;
		m_size = rhs.m_size;
#ifndef AVHTTP_DISABLE_THREAD
		m_mutex = boost::make_shared<boost::mutex>();
#endif
	}

	const rangefield& operator=(const rangefield& rhs)
	{
		m_need_merge = rhs.m_need_merge;
		m_ranges = rhs.m_ranges;
		m_size = rhs.m_size;
#ifndef AVHTTP_DISABLE_THREAD
		m_mutex = boost::make_shared<boost::mutex>();
#endif
		return *this;
	}

public:

	///重置rangefield.
	// @param size表示区间的总大小.
	void reset(boost::int64_t size = 0)
	{
#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif
		m_size = size;
		m_need_merge = false;
		m_ranges.clear();
	}

	///获得区域的大小.
	// @返回这个区间的总大小.
	boost::int64_t size() const
	{
		return m_size;
	}

	///添加或更新range, 区间为[r.left, r.right)
	// @param r区间, 不包含右边界处.
	// @备注: 添加一个区间到range中, 可以重叠添加, 成功返回true.
	inline bool update(const range& r)
	{
		return update(r.left, r.right);
	}

	///添加或更新range, 区间为[left, right)
	// @param left左边边界.
	// @param right右边边界, 不包含边界处.
	// @备注: 添加一个区间到range中, 可以重叠添加, 成功返回true.
	inline bool update(const boost::int64_t& left, const boost::int64_t& right)
	{
		BOOST_ASSERT((left >= 0 && left < right) && right <= m_size);

#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif

		if ((left < 0 || right > m_size) || (right <= left))
			return false;
		m_ranges[left] = right;
		m_need_merge = true;
		return true;
	}

	///检查是否在区间里.
	// @param r区间, 不包含右边界处.
	// @返回这个range这个区间是否完整的被包含在range中.
	// @备注: 检查的区间是一个半开区间[left, right), 即不包含右边界.
	inline bool check_range(const range& r)
	{
		return check_range(r.left, r.right);
	}

	///检查是否在区间里.
	// @param left左边边界.
	// @param right右边边界, 不包含边界处.
	// @返回这个[left, right)这个区间是否完整的被包含在range中.
	// @备注: 检查的区间是一个半开区间[left, right), 即不包含右边界.
	inline bool check_range(const boost::int64_t& left, const boost::int64_t& right)
	{
		BOOST_ASSERT((left >= 0 && left < right) && right <= m_size);

		// 先整理.
		if (m_need_merge)
			merge();

#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif

		for (range_map::iterator i = m_ranges.begin();
			i != m_ranges.end(); i++)
		{
			if (left >= i->first && right <= i->second)
				return true;
		}

		return false;
	}

	///获取在[left, right)区间的最大段.
	// @param left左边边界, 也是输出参数.
	// @param right右边边界, 不包含边界处, 同为输出参数.
	// @返回false表示所请求的空间是空的.
	// @备注: 请求的区间是一个半开区间[left, right), 即不包含右边界.
	inline bool get_range(boost::int64_t& left, boost::int64_t& right)
	{
		BOOST_ASSERT((left >= 0 && left < right) && right <= m_size);

		// 先整理.
		if (m_need_merge)
			merge();

#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif

		for (range_map::iterator i = m_ranges.begin();
			i != m_ranges.end(); i++)
		{
			if (left >= i->first)
			{
				if (right <= i->second)
					return true;
				if (right > i->first && i->second > left)
				{
					right = i->second;
					return true;
				}
			}
		}

		return false;
	}

	///输出空隙空间.
	// @param r区间, 不包含右边界处.
	// @返回false表示没有空间或失败.
	// @备注: 输出的区间是一个半开区间[left, right), 即不包含右边界.
	inline bool out_space(range& r)
	{
		return out_space(r.left, r.right);
	}

	///输出空隙空间.
	// @param left 表示从左开始起始边界.
	// @param right 表示右边的边界, 不包括边界处.
	// @返回false表示没有空间或失败.
	// @备注: 输出的区间是一个半开区间[left, right), 即不包含右边界.
	inline bool out_space(boost::int64_t& left, boost::int64_t& right)
	{
		// 先整理.
		if (m_need_merge)
			merge();
		return out_space(0, left, right);
	}

	///从指定位置开始, 输出空隙空间.
	// @param offset 表示输出空间在offset后面开始.
	// @param left 表示从左开始起始边界.
	// @param right 表示右边的边界, 不包括边界处.
	// @返回false表示没有空间或失败.
	// @备注: 输出的区间是一个半开区间[left, right), 即不包含右边界.
	inline bool out_space(boost::int64_t offset, boost::int64_t& left, boost::int64_t& right)
	{
		// 先整理.
		if (m_need_merge)
			merge();

#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif

		// inv里的每一个元素都是空间, 从元素中返回区间.
		range_map inv = inverse_impl();

		// 没有空间.
		if (inv.size() == 0)
			return false;
		range_map::iterator i;
		for (i = inv.begin(); i != inv.end(); i++)
		{
			// 偏移位置在空间里, 就返回这段空间.
			if (offset >= i->first)
			{
				if (offset < i->second)
				{
					left = offset;
					right = i->second;
					return true;
				}
			}
			else // offset不在区间内, 直接返回第一个空间.
			{
				left = i->first;
				right = i->second;
				return true;
			}
		}

		// 走到这, 还没找到有效的空间, 则直接返回第一块空间.
		i = inv.begin();
		left = i->first;
		right = i->second;

		return true;
	}

	///检查位图是否已经满了.
	inline bool is_full()
	{
		// 先整理.
		if (m_need_merge)
			merge();

#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif

		// 直接判断区间边界.
		if (m_ranges.size() == 1)
		{
			range_map::iterator i = m_ranges.begin();
			if (i->first == 0 && i->second == m_size)
				return true;
		}
		return false;
	}

	///得到区间所有大小.
	inline boost::int64_t range_size() const
	{
#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif
		boost::int64_t size = 0;
		for (range_map::const_iterator i = m_ranges.begin();
			i != m_ranges.end(); i++)
		{
			size += i->second - i->first;
		}

		return size;
	}

	///按指定大小输出为位图块.
	// @param bitfield以int为单位的位图数组, 每个元素表示1个piece, 为0表示空, 为1表示满.
	// @param piece_size指定的piece大小.
	inline void range_to_bitfield(bitfield& bf, int piece_size)
	{
		// 先整理连续的空间, 将连续的区间合并为一个区间.
		if (m_need_merge)
			merge();

#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif

		// 计算出分片总数.
		int piece_num = (m_size / piece_size) + (m_size % piece_size == 0 ? 0 : 1);

		// 分配位图空间, 默认为0.
		bf.resize(piece_num, 0);

		boost::int64_t l = 0;
		boost::int64_t r = 0;
		range_map::iterator it = m_ranges.begin();
		range_map::iterator end = m_ranges.end();

		// 从0位置开始, 每次检查piece_size大小的空间是否数据完整, 数据
		// 完整则设置到位图为1, 表示有数据.
		for (int i = 0; i < piece_num; i++)
		{
			l = i * piece_size;
			r = (i + 1) * piece_size;
			r = r > m_size ? m_size : r;

			bool is_complete = false;
			if (it != end)
			{
				// 在已经下载的区间中.
				if (l >= it->first && r <= it->second)
				{
					is_complete = true;
				}
				else
				{
					// 除非右边界大于等于当前区间的右边界, 否则一直使用当前区间判断.
					// 这样就避免了每次从头循环遍历.
					if (r >= it->second)
					{
						if (++it != end)
						{
							if (l >= it->first && r <= it->second)
							{
								is_complete = true;
							}
						}
					}
				}
			}

			if (is_complete)
			{
				bf.set_bit(i);
			}
		}
	}

	///按指定的分片大小bitfield更新rangefield.
	// @param bf为指定的bitfield.
	// @param piece_size是指定的分片大小.
	inline void bitfield_to_range(const bitfield& bf, int piece_size)
	{
#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif
		m_ranges.clear();

		boost::int64_t left = 0;
		boost::int64_t right = 0;
		bool left_record = false;
		int index = 0;

		for (bitfield::const_iterator i = bf.begin(); i != bf.end(); i++, index++)
		{
			BOOST_ASSERT(index * piece_size < m_size);

			if (*i && !left_record)
			{
				left = index * piece_size;
				right = left + piece_size;

				left_record = true;
				continue;
			}

			if (*i && left_record)
			{
				right += piece_size;
			}

			if (left_record && !(*i))
			{
				// 得到区间.
				right = (std::min)(right, m_size);
				m_ranges[left] = right;
				left_record = false;
			}
		}

		if (left_record)
		{
			right = (std::min)(right, m_size);
			m_ranges[left] = right;
			left_record = false;
		}

		m_need_merge = true;
	}

	///输出range内容, 调试使用.
	inline void print()
	{
#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif
		for (range_map::iterator i = m_ranges.begin();
			i != m_ranges.end(); i++)
		{
			std::cout << i->first << "   ---    " << i->second << "\n";
		}
	}

	///翻转rangefield.
	inline rangefield inverse()
	{
#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif

		range_map m = inverse_impl();
		rangefield f(m_size);

		f.m_need_merge = m_need_merge;
		f.m_ranges = m;

		return f;
	}

protected:

	///返回翻转的range_item.
	inline range_map inverse_impl()
	{
		// 先整理.
		if (m_need_merge)
			merge();

		range_map reverse_map;
		boost::int64_t point = 0;

		// 将空隙添加到reverse_map中.
		for (range_map::iterator i = m_ranges.begin();
			i != m_ranges.end(); i++)
		{
			if (i == m_ranges.begin())
			{
				if (i->first != 0)
				{
					point = 0;
					reverse_map.insert(std::make_pair(point, i->first));
					point = i->second;
					continue;
				}
				else
				{
					point = i->second;
					continue;
				}
			}

			reverse_map.insert(std::make_pair(point, i->first));
			point = i->second;
		}

		// 尾部判断.
		if (point != m_size || m_ranges.size() == 0)
		{
			reverse_map.insert(std::make_pair(point, m_size));
		}

		return reverse_map;
	}

	///整理回收range中的重叠部分.
	inline void merge()
	{
#ifndef AVHTTP_DISABLE_THREAD
		boost::mutex::scoped_lock lock(*m_mutex);
#endif
		std::map<boost::int64_t, boost::int64_t> result;
		std::pair<boost::int64_t, boost::int64_t> max_value;
		max_value.first = 0;
		max_value.second = 0;
		for (range_map::iterator i = m_ranges.begin();
			i != m_ranges.end(); i++)
		{
			// i 在max_value 之间, 忽略之.
			// [        ]
			//   [    ]
			// --------------
			// [         ]
			if (i->first >= max_value.first && i->second <= max_value.second)
				continue;
			// 情况如下:
			//      [   ]
			// [       ]
			// --------------
			// [        ]
			if (i->first <= max_value.first && (i->second >= max_value.first && i->second <= max_value.second))
			{
				max_value.first = i->first;	// 更新首部位置.
				continue;
			}
			// 情况如下:
			// [   ]
			//     [     ]
			// --------------
			// [         ]
			if ((i->first > max_value.first && i->first <= max_value.second))
			{
				max_value.second = i->second;
				continue;
			}
			// 情况如下:
			// [   ]
			//       [     ]
			// --------------
			// [   ] [     ]
			if ((i->first > max_value.second) || (max_value.first == 0 && max_value.second == 0))
			{
				if (max_value.first != 0 || max_value.second != 0)
					result.insert(max_value);
				max_value = *i;
			}
		}
		result.insert(max_value);
		m_ranges = result;
		m_need_merge = false;
	}

private:
	bool m_need_merge;
	boost::int64_t m_size;
	range_map m_ranges;
#ifndef AVHTTP_DISABLE_THREAD
	typedef boost::shared_ptr<boost::mutex> mutex_ptr;
	mutex_ptr m_mutex;
#endif
};

} // namespace avhttp

#endif // AVHTTP_RANGEFIELD_HPP
