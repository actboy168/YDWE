#pragma once

#include <boost/version.hpp>

#define BOOST_UTF8_BEGIN_NAMESPACE namespace ydwe { namespace util { namespace detail {
#define BOOST_UTF8_END_NAMESPACE   }}}
#define BOOST_UTF8_DECL 
#if BOOST_VERSION  > 104900
#include <boost/detail/utf8_codecvt_facet.ipp>
#else
#include <boost/detail/utf8_codecvt_facet.hpp>
#endif
#undef BOOST_UTF8_BEGIN_NAMESPACE
#undef BOOST_UTF8_END_NAMESPACE
#undef BOOST_UTF8_DECL
