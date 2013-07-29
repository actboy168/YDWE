#ifndef LUABIND_TABLE_POLICY_HPP_INCLUDED
#define LUABIND_TABLE_POLICY_HPP_INCLUDED

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>

namespace luabind {
namespace detail {
struct table_convert
{
	int const consumed_args(...)
	{
		return 1;
	}

   template<class ContainerT>
   ContainerT apply(lua_State *L, by_value<ContainerT>
                    data, int index)
   {
      luabind::object tbl(from_stack(L,
                                     index));
      ContainerT result;

      for (luabind::iterator itr(tbl), end; itr !=
           end; ++itr)
      {
         boost::optional<ContainerT::value_type>
         v = object_cast_nothrow<ContainerT::value_type>(*itr);

         if (v)
         {
            result.push_back(*v);
         }
      }

      return(result);
   }


   template<class ContainerT>
   ContainerT apply(lua_State *L,
                    by_const_reference<ContainerT> data, int index)
   {
      return(apply(L, by_value<ContainerT>(), index));
   }


   template<class ContainerT>
   ContainerT apply(lua_State *L, by_reference<ContainerT>
                    data, int index)
   {
      return(apply(L, by_value<ContainerT>(), index));
   }


   template<class ContainerT>
   static int match(lua_State *L, by_value<ContainerT>, int
                    index)
   {
      if (lua_type(L, index) == LUA_TTABLE)
      {
         return(0);
      }

      return(-1);
   }


   template<class ContainerT>
   static int match(lua_State *L,
                    by_const_reference<ContainerT>, int index)
   {
      return(match(L, by_value<ContainerT>(), index));
   }


   template<class ContainerT>
   static int match(lua_State *L, by_reference<ContainerT>,
                    int index)
   {
      return(match(L, by_value<ContainerT>(), index));
   }


   template<class ContainerT>
   void apply(lua_State *L, const ContainerT& container)
   {
      lua_createtable(L, container.size(), 0);

      luabind::object tbl(from_stack(L, -1));
      int             n = 0;

      for (ContainerT::const_iterator itr =
              container.begin(); itr != container.end(); ++itr)
      {
         tbl[++n] = *itr;
      }
   }


   template<class T>
   void converter_postcall(lua_State *, T, int) {}
};

struct table_convert_assoc
{
	int const consumed_args(...)
	{
		return 1;
	}

   template<class ContainerT>
   ContainerT apply(lua_State *L, by_value<ContainerT>
                    data, int index)
   {
      luabind::object tbl(from_stack(L,
                                     index));
      ContainerT result;

      for (luabind::iterator itr(tbl), end; itr !=
           end; ++itr)
      {
         typedef ContainerT::key_type
         KeyT;
         typedef ContainerT::mapped_type
         ValueT;

         boost::optional<KeyT> k =
            object_cast_nothrow<KeyT>(itr.key());

         if (k)
         {
            boost::optional<ValueT>
            v = object_cast_nothrow<ValueT>(*itr);

            if (v)
            {
               result.insert(std::make_pair(*k, *v));
            }
         }
      }

      return(result);
   }


   template<class ContainerT>
   ContainerT apply(lua_State *L,
                    by_const_reference<ContainerT> data, int index)
   {
      return(apply(L, by_value<ContainerT>(), index));
   }


   template<class ContainerT>
   ContainerT apply(lua_State *L, by_reference<ContainerT>
                    data, int index)
   {
      return(apply(L, by_value<ContainerT>(), index));
   }


   template<class ContainerT>
   static int match(lua_State *L, by_value<ContainerT>, int
                    index)
   {
      if (lua_type(L, index) == LUA_TTABLE)
      {
         return(0);
      }

      return(-1);
   }


   template<class ContainerT>
   static int match(lua_State *L,
                    by_const_reference<ContainerT>, int index)
   {
      return(match(L, by_value<ContainerT>(), index));
   }


   template<class ContainerT>
   static int match(lua_State *L, by_reference<ContainerT>,
                    int index)
   {
      return(match(L, by_value<ContainerT>(), index));
   }


   template<class ContainerT>
   void apply(lua_State *L, const ContainerT& container)
   {
      lua_createtable(L, 0, container.size());

      luabind::object tbl(from_stack(L, -1));
      int             n = 0;

      for (ContainerT::const_iterator itr =
              container.begin(); itr != container.end(); ++itr)
      {
         tbl[itr->first] = itr->second;
      }
   }


   template<class T>
   void converter_postcall(lua_State *, T, int) {}
};

template<int N, bool Assoc>
struct table_policy : conversion_policy<N>
{
   struct only_accepts_values_or_references {};

   static void precall(lua_State *, const index_map&) {}
   static void postcall(lua_State *, const index_map&) {}

   template<class T, class Direction>
   struct apply
   {
      static const bool IsPtr =
         luabind::detail::is_nonconst_pointer<T>::value ||
         luabind::detail::is_const_pointer<T>::value;

      typedef typename boost::mpl::if_c<
         IsPtr,

         only_accepts_values_or_references,
         typename boost::mpl::if_c<Assoc,
                                   table_convert_assoc, table_convert>::type
         >::type type;
   };
};
}
}

namespace luabind
{
template<int N>
detail::policy_cons<detail::table_policy<N, false>,
                    detail::null_type>
copy_table(LUABIND_PLACEHOLDER_ARG(N))
{
   return(detail::policy_cons<detail::table_policy<N,
                                                   false>, detail::null_type>());
}


template<int N>
detail::policy_cons<detail::table_policy<N, true>,
                    detail::null_type>
copy_table_assoc(LUABIND_PLACEHOLDER_ARG(N))
{
   return(detail::policy_cons<detail::table_policy<N,
                                                   true>, detail::null_type>());
}
}

#endif // LUABIND_TABLE_POLICY_HPP_INCLUDED
