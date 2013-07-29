/*
 * Implements map algorithms
 */

#ifndef MAP_ALGO_HPP_INCLUDED
#define MAP_ALGO_HPP_INCLUDED

namespace aero
{
	// Map copy
	template <typename MapType, typename OutputMapType>
	inline OutputMapType &naive_map_copy(const MapType &s, OutputMapType &o)
	{		
		if (&s != &o)
		{
			for (MapType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
			{
				o.insert(*itr);
			}
		}
		
		return o;
	}
	
	template <typename MapType, typename OutputMapType, typename _Pred>
	inline OutputMapType &naive_map_copy_if(const MapType &s, OutputMapType &o, _Pred pred)
	{
		typedef MapType::value_type value_type;
		
		if (&s != &o)
		{
			for (MapType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
			{
				const value_type &e = *itr;
				if (pred(e))
					o.insert(e);
			}
		}
		
		return o;
	}
	
	// Map copy output iterator
	template <typename MapType, typename _OutputIteratorType>
	inline _OutputIteratorType naive_map_copy_to(const MapType &s, _OutputIteratorType o)
	{		
		for (MapType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
		{
			*o = itr->second;
			++o;
		}
		
		return o;
	}
	
	template <typename MapType, typename _OutputIteratorType, typename _Pred>
	inline _OutputIteratorType naive_map_copy_to_if(const MapType &s, _OutputIteratorType o, _Pred pred)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		for (MapType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
		{
			const value_type &e = *itr;
			if (pred(e))
			{
				*o = e.second;
				++o;
			}
		}
		
		return o;
	}

	// Map copy unique
	template <typename MapType, typename OutputMapType>
	inline OutputMapType &naive_map_copy_unique(const MapType &s, OutputMapType &o)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		if (&s != &o)
		{
			for (MapType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
			{
				const value_type &e = *itr;
				if (o.find(e.first) == o.end())
					o.insert(e);
			}
		}
		
		return o;
	}
	
	template <typename MapType, typename OutputMapType, typename _Pred>
	inline OutputMapType &naive_map_copy_unique_if(const MapType &s, OutputMapType &o, _Pred pred)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		if (&s != &o)
		{
			for (MapType::const_iterator itr = s.begin(); itr != s.end(); ++itr)
			{
				const value_type &e = *itr;
				if (o.find(e.first) == o.end() && pred(e))
					o.insert(e);
			}
		}
		
		return o;
	}
	
	// Algorithms
	
	// map union
	template <typename MapType, typename OutputMapType>
	inline OutputMapType &naive_map_union_copy(const MapType &a, const MapType &b, OutputMapType &o)
	{
		naive_map_copy_unique(a, o);
		naive_map_copy_unique(b, o);
		
		return o;
	}
	
	template <typename MapType, typename OutputMapType, typename _Pred>
	inline OutputMapType &naive_map_union_copy_if(const MapType &a, const MapType &b, OutputMapType &o, _Pred pred)
	{
		naive_map_copy_unique_if(a, o, pred);
		naive_map_copy_unique_if(b, o, pred);
		
		return o;
	}
	
	template <typename MapType>
	inline void naive_map_union(MapType &a, const MapType &b)
	{
		naive_map_copy_unique(b, a);
	}
	
	// map intersection
	
	template <typename MapType, typename OutputMapType>
	inline OutputMapType &naive_map_intersection_copy(const MapType &a, const MapType &b, OutputMapType &o)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		for (MapType::const_iterator itr = a.begin(); itr != a.end(); ++itr)
		{
			const value_type &e = *itr;
			if (b.find(e.first) != b.end())
				o.insert(e);
		}
		
		return o;
	}
	
	template <typename MapType, typename OutputMapType, typename _Pred>
	inline OutputMapType &naive_map_intersection_copy_if(const MapType &a, const MapType &b, OutputMapType &o, _Pred pred)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		for (MapType::const_iterator itr = a.begin(); itr != a.end(); ++itr)
		{
			const value_type &e = *itr;
			if (b.find(e.first) != b.end() && pred(e))
				o.insert(e);
		}
		
		return o;
	}

	template <typename MapType>
	inline void naive_map_intersection(MapType &a, const MapType &b)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		if (&a != &b)
		{
			MapType temp;
			a.swap(temp);
			
			for (MapType::const_iterator itr = b.begin(); itr != b.end(); ++itr)
			{
				const value_type &e = *itr;
				if (temp.find(e.first) != temp.end())
					a.insert(e);
			}
		}
	}
	
	// map difference
	template <typename MapType, typename OutputMapType>
	inline OutputMapType &naive_map_difference_copy(const MapType &a, const MapType &b, OutputMapType &o)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		for (MapType::const_iterator itr = a.begin(); itr != a.end(); ++itr)
		{
			const value_type &e = *itr;
			if (b.find(e.first) == b.end())
				o.insert(e);
		}
		
		return o;
	}
	
	template <typename MapType, typename OutputMapType, typename _Pred>
	inline OutputMapType &naive_map_difference_copy_if(const MapType &a, const MapType &b, OutputMapType &o, _Pred pred)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		for (MapType::const_iterator itr = a.begin(); itr != a.end(); ++itr)
		{
			const value_type &e = *itr;
			if (b.find(e.first) == b.end() && pred(e))
				o.insert(e);
		}
		
		return o;
	}

	template <typename MapType>
	inline void naive_map_difference(MapType &a, const MapType &b)
	{
		typedef MapType::key_type key_type;
		typedef MapType::value_type value_type;
		
		if (&a != &b)
		{
			for (MapType::const_iterator itr = b.begin(); itr != b.end(); ++itr)
			{
				const value_type &e = *itr;
				if (a.find(e.first) != a.end())
					a.erase(e.first);
			}
		}
		else
			a.clear();
	}
}

#endif // MAP_ALGO_HPP_INCLUDED
