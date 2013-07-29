/*
 * Implements set algorithms
 */

#ifndef SET_ALGO_HPP_INCLUDED
#define SET_ALGO_HPP_INCLUDED

namespace aero
{
	// Set copy
	template <typename SetType, typename OutputSetType>
	inline OutputSetType &naive_set_copy(const SetType &s, OutputSetType &o)
	{
		if (&s != &o)
		{
			for (SetType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
			{
				o.insert(*itr);
			}
		}
		
		return o;
	}
	
	template <typename SetType, typename OutputSetType, typename _Pred>
	inline OutputSetType &naive_set_copy_if(const SetType &s, OutputSetType &o, _Pred pred)
	{
		typedef SetType::key_type key_type;
		
		if (&s != &o)
		{
			for (SetType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
			{
				const key_type &e = *itr;
				if (pred(e))
					o.insert(e);
			}
		}
		
		return o;
	}
	
	// Set copy output iterator
	template <typename SetType, typename _OutputIteratorType>
	inline _OutputIteratorType naive_set_copy_to(const SetType &s, _OutputIteratorType o)
	{
		for (SetType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
		{
			*o = *itr;
			++o;
		}
		
		return o;
	}
	
	template <typename SetType, typename _OutputIteratorType, typename _Pred>
	inline _OutputIteratorType naive_set_copy_to_if(const SetType &s, _OutputIteratorType o, _Pred pred)
	{
		typedef SetType::key_type key_type;
		
		for (SetType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
		{
			const key_type &e = *itr;
			if (pred(e))
			{
				*o = e;
				++o;
			}
		}
		
		return o;
	}

	// Set copy unique
	template <typename SetType, typename OutputSetType>
	inline OutputSetType &naive_set_copy_unique(const SetType &s, OutputSetType &o)
	{
		typedef SetType::key_type key_type;
		
		if (&s != &o)
		{
			for (SetType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
			{
				const key_type &e = *itr;
				if (o.find(e) == o.end())
					o.insert(e);
			}
		}
		
		return o;
	}
	
	template <typename SetType, typename OutputSetType, typename _Pred>
	inline OutputSetType &naive_set_copy_unique_if(const SetType &s, OutputSetType &o, _Pred pred)
	{
		typedef SetType::key_type key_type;
		
		if (&s != &o)
		{
			for (SetType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
			{
				const key_type &e = *itr;
				if (o.find(e) == o.end() && pred(e))
					o.insert(e);
			}
		}
		
		return o;
	}
	
	// Algorithms
	
	// set union
	template <typename SetType, typename OutputSetType>
	inline OutputSetType &naive_set_union_copy(const SetType &a, const SetType &b, OutputSetType &o)
	{
		naive_set_copy_unique(a, o);
		naive_set_copy_unique(b, o);
		
		return o;
	}
	
	template <typename SetType, typename OutputSetType, typename _Pred>
	inline OutputSetType &naive_set_union_copy_if(const SetType &a, const SetType &b, OutputSetType &o, _Pred pred)
	{
		naive_set_copy_unique_if(a, o, pred);
		naive_set_copy_unique_if(b, o, pred);
		
		return o;
	}
	
	template <typename SetType>
	inline void naive_set_union(SetType &a, const SetType &b)
	{
		naive_set_copy_unique(b, a);
	}
	
	// set intersection
	
	template <typename SetType, typename OutputSetType>
	inline OutputSetType &naive_set_intersection_copy(const SetType &a, const SetType &b, OutputSetType &o)
	{
		typedef SetType::key_type key_type;
		
		for (SetType::const_iterator itr = a.begin(); itr != a.end(); ++itr)
		{
			const key_type &e = *itr;
			if (b.find(e) != b.end())
				o.insert(e);
		}
		
		return o;
	}
	
	template <typename SetType, typename OutputSetType, typename _Pred>
	inline OutputSetType &naive_set_intersection_copy_if(const SetType &a, const SetType &b, OutputSetType &o, _Pred pred)
	{
		typedef SetType::key_type key_type;
		
		for (SetType::const_iterator itr = a.begin(); itr != a.end(); ++itr)
		{
			const key_type &e = *itr;
			if (b.find(e) != b.end() && pred(e))
				o.insert(e);
		}
		
		return o;
	}

	template <typename SetType>
	inline void naive_set_intersection(SetType &a, const SetType &b)
	{
		typedef SetType::key_type key_type;
		
		if (&a != &b)
		{
			SetType temp;
			a.swap(temp);
			
			for (SetType::const_iterator itr = b.begin(); itr != b.end(); ++itr)
			{
				const key_type &e = *itr;
				if (temp.find(e) != temp.end())
					a.insert(e);
			}
		}
	}
	
	// set difference
	template <typename SetType, typename OutputSetType>
	inline OutputSetType &naive_set_difference_copy(const SetType &a, const SetType &b, OutputSetType &o)
	{
		typedef SetType::key_type key_type;
		
		for (SetType::const_iterator itr = a.begin(); itr != a.end(); ++itr)
		{
			const key_type &e = *itr;
			if (b.find(e) == b.end())
				o.insert(e);
		}
		
		return o;
	}
	
	template <typename SetType, typename OutputSetType, typename _Pred>
	inline OutputSetType &naive_set_difference_copy_if(const SetType &a, const SetType &b, OutputSetType &o, _Pred pred)
	{
		typedef SetType::key_type key_type;
		
		for (SetType::const_iterator itr = a.begin(); itr != a.end(); ++itr)
		{
			const key_type &e = *itr;
			if (b.find(e) == b.end() && pred(e))
				o.insert(e);
		}
		
		return o;
	}

	template <typename SetType>
	inline void naive_set_difference(SetType &a, const SetType &b)
	{
		typedef SetType::key_type key_type;
		
		if (&a != &b)
		{
			for (SetType::const_iterator itr = b.begin(); itr != b.end(); ++itr)
			{
				const key_type &e = *itr;
				if (a.find(e) != a.end())
					a.erase(e);
			}
		}
		else
			a.clear();
	}
}

#endif // SET_ALGO_HPP_INCLUDED
