#include <slk/utility/sequence.h>
#include <cctype>
#include <locale>

namespace slk
{
	namespace ctype
	{
		namespace detail
		{
			template<class CharT>
			struct ctype_facet_ptr
			{
				std::locale locale_;
				const std::ctype<CharT>* facet_ptr_;

				ctype_facet_ptr()
					: locale_()
					, facet_ptr_(&std::use_facet<std::ctype<CharT>>(locale_))
				{ }

				const std::ctype<CharT>* operator->() const
				{
					return facet_ptr_;
				}
			};


			template <typename _Ty>
			struct singleton
			{
			private:
				struct object_creator
				{
					object_creator() { singleton<_Ty>::instance(); }
					inline void do_nothing() const { }
				};
				static object_creator create_object;
				singleton();
				singleton(const singleton&);
				singleton& operator=(const singleton&);

			public:
				typedef _Ty object_type;
				static object_type& instance()
				{
					static object_type obj;
					create_object.do_nothing();
					return obj;
				}
			};

			template <typename _Ty>
			typename singleton<_Ty>::object_creator singleton<_Ty>::create_object;

		}

		template<> SLKLIB_API
		bool is_space::operator()<char>(char Ch) const
		{
			return detail::singleton<detail::ctype_facet_ptr<char>>::instance()->is(std::ctype_base::space, Ch);
		}
	}
}
