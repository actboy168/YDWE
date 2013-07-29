/*
 * Useful iterators
 */

#ifndef AERO_FUNCTIONAL_ITERATOR_HPP_INCLUDED
#define AERO_FUNCTIONAL_ITERATOR_HPP_INCLUDED

#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include "aero/aero_basic_type.hpp"

namespace aero { namespace iterator {
	template <typename T>
	struct repeat_iterator
		: public boost::iterator_facade<
			repeat_iterator<T>,
			T,
			boost::random_access_traversal_tag
		>
	{
		typedef repeat_iterator<T> self_type;
		typedef const repeat_iterator<T> const_self_type;

		friend class boost::iterator_core_access;

		// Constructor
		repeat_iterator(T v, size_t c)	// end
			: value(v), count(c), index(c) {}

		repeat_iterator(T v, size_t c, size_t i)
			: value(v), count(c), index(i < c ? i : c) {}

		reference dereference() const { return (reference)value; }

		bool equal(const_self_type &other) const { return index == other.index; }
		
		void increment() { if (index < count) ++index; }
		void decrement() { if (index/* > 0*/) --index; }
		
		void advance(difference_type d) { index += d; }
		difference_type distance_to(const_self_type &other) const { return other.index - index; }
		
		T value;
		size_t count;
		size_t index;
	};
}
using iterator::repeat_iterator;
}

#endif // AERO_FUNCTIONAL_ITERATOR_HPP_INCLUDED
