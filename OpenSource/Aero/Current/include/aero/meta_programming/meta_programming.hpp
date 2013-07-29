/*
 * Some meta-programming stuff
 */
#ifndef META_PROGRAMMING_HPP_INCLUDED
#define META_PROGRAMMING_HPP_INCLUDED

namespace aero
{
	// Statically calculating power by template metaprogramming
	template <int n, unsigned int e>
	struct static_pow
	{
		enum { value = static_pow<n, e - 1>::value * n };
	};

	// Template partial specialization for stopping recursion
	template <int n>
	struct static_pow<n, 0>
	{
		enum { value = 1 };
	};

	// Similar, un-functional version
	template <typename TypeA, typename TypeB>
	inline TypeA pow(TypeA n, TypeB e)
	{
		TypeA result = 1;
		for (TypeB i = 0; i < e; ++i)
			result *= n;

		return result;
	}
}
#endif // META_PROGRAMMING_HPP_INCLUDED
