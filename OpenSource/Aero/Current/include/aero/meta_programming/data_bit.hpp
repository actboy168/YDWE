/*
 * Low level data bit manipulation
 */

#ifndef DATA_BIT_HPP_INCLUCED
#define DATA_BIT_HPP_INCLUCED

#include <boost/mpl/at.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map.hpp>
#include <aero/aero_basic_type.hpp>

namespace aero
{
	namespace databit { namespace detail {
		
		struct underflow_traits 
		{
			typedef underflow_traits this_traits;
			typedef this_traits value_type;
			
			typedef this_traits half_traits;
			typedef half_traits::value_type half_value_type;
			
			enum { size = -1 };
		};
				
		struct data8_traits
		{
			typedef data8_traits this_traits;
			typedef boost::uint8_t value_type;
			
			typedef underflow_traits half_traits;
			typedef half_traits::value_type half_value_type;
			
			enum { size = 8 * sizeof(value_type) };
		};
		
		struct data16_traits
		{
			typedef data16_traits this_traits;
			typedef boost::uint16_t value_type;
			
			typedef data8_traits half_traits;
			typedef half_traits::value_type half_value_type;
			
			enum { size = 8 * sizeof(value_type) };
		};
		
		struct data32_traits
		{
			typedef data32_traits this_traits;
			typedef boost::uint32_t value_type;

			typedef data16_traits half_traits;
			typedef half_traits::value_type half_value_type;
			
			enum { size = 8 * sizeof(value_type) };
		};
		
		struct data64_traits
		{
			typedef data64_traits this_traits;
			typedef boost::uint64_t value_type;
			
			typedef data32_traits::value_type half_value_type;
			typedef data32_traits half_traits;
			
			enum { size = 8 * sizeof(value_type) };
		};
		
		template <typename TypeTraits, unsigned int splitCount>
		struct split_type_worker
		{
			typedef typename split_type_worker
			<
				typename TypeTraits::half_traits,
				splitCount / 2
			>::type_traits type_traits;
		};

		template <typename TypeTraits>
		struct split_type_worker<TypeTraits, 1>
		{
			typedef typename TypeTraits type_traits;
		};

		template <typename TypeTraits>
		struct split_type_worker<TypeTraits, 0>
		{
			typedef typename TypeTraits type_traits;
		};
		
		typedef boost::mpl::map
		<
#if defined(UCHAR_MAX) && UCHAR_MAX == 0xFF
			boost::mpl::pair<char, data8_traits>, 
			boost::mpl::pair<unsigned char, data8_traits>,
#elif defined(UCHAR_MAX) && UCHAR_MAX == 0xFFFF
			boost::mpl::pair<char, data16_traits>, 
			boost::mpl::pair<unsigned char, data16_traits>,
#elif defined(UCHAR_MAX) && UCHAR_MAX == 0xFFFFFFFF
			boost::mpl::pair<char, data32_traits>, 
			boost::mpl::pair<unsigned char, data32_traits>,
#elif defined(UCHAR_MAX) && UCHAR_MAX == 0xFFFFFFFFFFFFFFFF
			boost::mpl::pair<char, data64_traits>, 
			boost::mpl::pair<unsigned char, data64_traits>,
#else
			boost::mpl::pair<char, data8_traits>, 
			boost::mpl::pair<unsigned char, data8_traits>,
#endif

#if defined(USHRT_MAX) && USHRT_MAX == 0xFF
			boost::mpl::pair<short, data8_traits>, 
			boost::mpl::pair<unsigned short, data8_traits>,
#elif defined(USHRT_MAX) && USHRT_MAX == 0xFFFF
			boost::mpl::pair<short, data16_traits>, 
			boost::mpl::pair<unsigned short, data16_traits>,
#elif defined(USHRT_MAX) && USHRT_MAX == 0xFFFFFFFF
			boost::mpl::pair<short, data32_traits>, 
			boost::mpl::pair<unsigned short, data32_traits>,
#elif defined(USHRT_MAX) && USHRT_MAX == 0xFFFFFFFFFFFFFFFF
			boost::mpl::pair<short, data64_traits>, 
			boost::mpl::pair<unsigned short, data64_traits>,
#else
			boost::mpl::pair<short, data16_traits>, 
			boost::mpl::pair<unsigned short, data16_traits>,
#endif

#if defined(UINT_MAX) && UINT_MAX == 0xFF
			boost::mpl::pair<int, data8_traits>, 
			boost::mpl::pair<unsigned int, data8_traits>,
#elif defined(UINT_MAX) && UINT_MAX == 0xFFFF
			boost::mpl::pair<int, data16_traits>, 
			boost::mpl::pair<unsigned int, data16_traits>,
#elif defined(UINT_MAX) && UINT_MAX == 0xFFFFFFFF
			boost::mpl::pair<int, data32_traits>, 
			boost::mpl::pair<unsigned int, data32_traits>,
#elif defined(UINT_MAX) && UINT_MAX == 0xFFFFFFFFFFFFFFFF
			boost::mpl::pair<int, data64_traits>, 
			boost::mpl::pair<unsigned int, data64_traits>,
#else
			boost::mpl::pair<int, data32_traits>, 
			boost::mpl::pair<unsigned int, data32_traits>,
#endif

#if defined(ULONG_MAX) && ULONG_MAX == 0xFF
			boost::mpl::pair<long, data8_traits>, 
			boost::mpl::pair<unsigned long, data8_traits>,
#elif defined(ULONG_MAX) && ULONG_MAX == 0xFFFF
			boost::mpl::pair<long, data16_traits>, 
			boost::mpl::pair<unsigned long, data16_traits>,
#elif defined(ULONG_MAX) && ULONG_MAX == 0xFFFFFFFF
			boost::mpl::pair<long, data32_traits>, 
			boost::mpl::pair<unsigned long, data32_traits>,
#elif defined(ULONG_MAX) && ULONG_MAX == 0xFFFFFFFFFFFFFFFF
			boost::mpl::pair<long, data64_traits>, 
			boost::mpl::pair<unsigned long, data64_traits>,
#else
			boost::mpl::pair<long, data32_traits>, 
			boost::mpl::pair<unsigned long, data32_traits>,
#endif

#ifdef _MSC_VER
			boost::mpl::pair<__int64, data64_traits>,
			boost::mpl::pair<unsigned __int64, data64_traits>,
#endif
			boost::mpl::pair<long long, data64_traits>,
			boost::mpl::pair<unsigned long long, data64_traits>
		> type_traits_map;
				
		template <typename IntType, unsigned int i>
		struct split_type
		{
			typedef IntType int_type;
			typedef typename boost::mpl::if_
			<
				boost::is_same
				<
					typename boost::mpl::at<type_traits_map, int_type>::type,
					boost::mpl::void_
				>,
				boost::mpl::void_,
				typename boost::mpl::at<type_traits_map, int_type>::type
			>::type original_type_traits;
			typedef typename original_type_traits::value_type original_value_type;  // Change from signed to unsigned, if any.
			
			typedef typename split_type_worker<original_type_traits, i>::type_traits type_traits;
			typedef typename type_traits::value_type value_type;
		};		
	} }
	
	template <unsigned int splitCount, typename Type>
	inline 
	typename databit::detail::split_type<Type, splitCount>::value_type number_split(Type n, unsigned int splitIndex)
	{
		typedef typename databit::detail::split_type<Type, splitCount> split_type;
		
		return (split_type::value_type)
		(
			((split_type::original_value_type)((split_type::value_type)-1)) /* mask*/ &
			(
				(split_type::original_type_traits::size * splitIndex / splitCount) ? 
					(((split_type::original_value_type)n) >> 
						(split_type::original_type_traits::size * splitIndex / splitCount))
					: ((split_type::original_value_type)n)
			)
		);
	}

	template <unsigned int splitCount, unsigned int splitIndex, typename Type>
	inline 
	typename databit::detail::split_type<Type, splitCount>::value_type number_split(Type n)
	{
		typedef typename databit::detail::split_type<Type, splitCount> split_type;
		
		return (split_type::value_type)
		(
			((split_type::original_value_type)((split_type::value_type)-1)) /* mask*/ &
			(
				(split_type::original_type_traits::size * splitIndex / splitCount) ? 
					(((split_type::original_value_type)n) >> 
						(split_type::original_type_traits::size * splitIndex / splitCount))
					: ((split_type::original_value_type)n)
			)
		);
	}
}

#endif // DATA_BIT_HPP_INCLUCED
