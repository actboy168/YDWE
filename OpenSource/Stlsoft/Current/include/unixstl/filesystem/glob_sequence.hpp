/* /////////////////////////////////////////////////////////////////////////
 * File:        unixstl/filesystem/glob_sequence.hpp
 *
 * Purpose:     glob_sequence class.
 *
 * Created:     15th January 2002
 * Updated:     12th January 2010
 *
 * Thanks:      To Carlos Santander Bernal for helping with Mac compatibility.
 *              To Nevin Liber for pressing upon me the need to lead by
 *              example when writing books about good design/implementation.
 *
 * Home:        http://stlsoft.org/
 *
 * Copyright (c) 2002-2010, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the names of
 *   any contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


/** \file unixstl/filesystem/glob_sequence.hpp
 *
 * \brief [C++ only] Definition of the unixstl::glob_sequence class
 *   (\ref group__library__filesystem "File System" Library).
 */

#ifndef UNIXSTL_INCL_UNIXSTL_FILESYSTEM_HPP_GLOB_SEQUENCE
#define UNIXSTL_INCL_UNIXSTL_FILESYSTEM_HPP_GLOB_SEQUENCE

#ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION
# define UNIXSTL_VER_UNIXSTL_FILESYSTEM_HPP_GLOB_SEQUENCE_MAJOR     5
# define UNIXSTL_VER_UNIXSTL_FILESYSTEM_HPP_GLOB_SEQUENCE_MINOR     1
# define UNIXSTL_VER_UNIXSTL_FILESYSTEM_HPP_GLOB_SEQUENCE_REVISION  2
# define UNIXSTL_VER_UNIXSTL_FILESYSTEM_HPP_GLOB_SEQUENCE_EDIT      142
#endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef UNIXSTL_INCL_UNIXSTL_H_UNIXSTL
# include <unixstl/unixstl.h>
#endif /* !UNIXSTL_INCL_UNIXSTL_H_UNIXSTL */
#ifndef UNIXSTL_INCL_UNIXSTL_FILESYSTEM_HPP_FILESYSTEM_TRAITS
# include <unixstl/filesystem/filesystem_traits.hpp>
#endif /* !UNIXSTL_INCL_UNIXSTL_FILESYSTEM_HPP_FILESYSTEM_TRAITS */
#ifndef UNIXSTL_INCL_UNIXSTL_FILESYSTEM_HPP_FILE_PATH_BUFFER
# include <unixstl/filesystem/file_path_buffer.hpp>
#endif /* !UNIXSTL_INCL_UNIXSTL_FILESYSTEM_HPP_FILE_PATH_BUFFER */
#ifndef STLSOFT_INCL_STLSOFT_UTIL_STD_HPP_ITERATOR_HELPER
# include <stlsoft/util/std/iterator_helper.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_UTIL_STD_HPP_ITERATOR_HELPER */
#ifndef STLSOFT_INCL_STLSOFT_MEMORY_HPP_AUTO_BUFFER
# include <stlsoft/memory/auto_buffer.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_MEMORY_HPP_AUTO_BUFFER */
#ifndef STLSOFT_INCL_STLSOFT_MEMORY_HPP_ALLOCATOR_SELECTOR
# include <stlsoft/memory/allocator_selector.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_MEMORY_HPP_ALLOCATOR_SELECTOR */
#ifndef STLSOFT_INCL_STLSOFT_SMARTPTR_HPP_SCOPED_HANDLE
# include <stlsoft/smartptr/scoped_handle.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_SMARTPTR_HPP_SCOPED_HANDLE */
#ifndef STLSOFT_INCL_STLSOFT_STRING_HPP_TOKENISER_FUNCTIONS
//# include <stlsoft/string/tokeniser_functions.hpp> // for find_next_token
#endif /* !STLSOFT_INCL_STLSOFT_STRING_HPP_TOKENISER_FUNCTIONS */
#ifndef STLSOFT_INCL_STLSOFT_COLLECTIONS_UTIL_HPP_COLLECTIONS
# include <stlsoft/collections/util/collections.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_COLLECTIONS_UTIL_HPP_COLLECTIONS */
#ifndef STLSOFT_INCL_STLSOFT_UTIL_HPP_STD_SWAP
# include <stlsoft/util/std_swap.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_UTIL_HPP_STD_SWAP */

#ifndef STLSOFT_INCL_SYS_H_TYPES
# define STLSOFT_INCL_SYS_H_TYPES
# include <sys/types.h>
#endif /* !STLSOFT_INCL_SYS_H_TYPES */
#ifndef STLSOFT_INCL_SYS_H_STAT
# define STLSOFT_INCL_SYS_H_STAT
# include <sys/stat.h>                      // for stat()
#endif /* !STLSOFT_INCL_SYS_H_STAT */
#ifndef STLSOFT_INCL_H_ERRNO
# define STLSOFT_INCL_H_ERRNO
# include <errno.h>
#endif /* !STLSOFT_INCL_H_ERRNO */
#ifndef STLSOFT_INCL_H_GLOB
# define STLSOFT_INCL_H_GLOB
# include <glob.h>                           // for glob(), globfree()
#endif /* !STLSOFT_INCL_H_GLOB */

#ifndef STLSOFT_INCL_ALGORITHM
# define STLSOFT_INCL_ALGORITHM
# include <algorithm>                        // for std::sort
#endif /* !STLSOFT_INCL_ALGORITHM */
#ifndef STLSOFT_INCL_EXCEPTION
# define STLSOFT_INCL_EXCEPTION
# include <exception>                        // for std::exception
#endif /* !STLSOFT_INCL_EXCEPTION */
#ifndef STLSOFT_INCL_STDEXCEPT
# define STLSOFT_INCL_STDEXCEPT
# include <stdexcept>                        // for std::runtime_error
#endif /* !STLSOFT_INCL_STDEXCEPT */

#ifdef STLSOFT_UNITTEST
# include <stlsoft/string/simple_string.hpp>
#endif /* STLSOFT_UNITTEST */

/* /////////////////////////////////////////////////////////////////////////
 * Library compatibility
 */

#ifndef GLOB_ONLYDIR
# undef UNIXSTL_GLOB_SEQUENCE_TRUST_ONLYDIR
#endif /* GLOB_ONLYDIR */

// Define UNIXSTL_GLOB_SEQUENCE_DONT_TRUST_MARK to not trust GLOB_MARK
//#define UNIXSTL_GLOB_SEQUENCE_DONT_TRUST_MARK

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatibility
 */

#ifdef STLSOFT_DOCUMENTATION_SKIP_SECTION
# define GLOB_SEQUENCE_CTOR_PRIMARY_FORM
#elif defined(STLSOFT_COMPILER_IS_CUSTOM)
# define GLOB_SEQUENCE_CTOR_PRIMARY_FORM
#elif defined(STLSOFT_COMPILER_IS_UNKNOWN)
# define GLOB_SEQUENCE_CTOR_PRIMARY_FORM
#elif defined(STLSOFT_COMPILER_IS_BORLAND)
# define GLOB_SEQUENCE_CTOR_OLD_FORM
#elif defined(STLSOFT_COMPILER_IS_COMO)
# define GLOB_SEQUENCE_CTOR_PRIMARY_FORM
#elif defined(STLSOFT_COMPILER_IS_DMC)
# define GLOB_SEQUENCE_CTOR_PRIMARY_FORM
#elif defined(STLSOFT_COMPILER_IS_GCC)
# define GLOB_SEQUENCE_CTOR_PRIMARY_FORM
#elif defined(STLSOFT_COMPILER_IS_INTEL)
# define GLOB_SEQUENCE_CTOR_PRIMARY_FORM
#elif defined(STLSOFT_COMPILER_IS_MSVC)
# if _MSC_VER >= 1310
#  define GLOB_SEQUENCE_CTOR_PRIMARY_FORM
# elif _MSC_VER >= 1020
#  define GLOB_SEQUENCE_CTOR_ALT_FORM
# else /* ? compiler */
#  define GLOB_SEQUENCE_CTOR_OLD_FORM
# endif /* compiler */
#elif defined(STLSOFT_COMPILER_IS_MWERKS)
# define GLOB_SEQUENCE_CTOR_ALT_FORM
#elif defined(STLSOFT_COMPILER_IS_VECTORC)
# define GLOB_SEQUENCE_CTOR_ALT_FORM
#elif defined(STLSOFT_COMPILER_IS_WATCOM)
# define GLOB_SEQUENCE_CTOR_OLD_FORM
#else /* ? compiler */
# error Unrecognised compiler
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifndef _UNIXSTL_NO_NAMESPACE
# if defined(_STLSOFT_NO_NAMESPACE) || \
     defined(STLSOFT_DOCUMENTATION_SKIP_SECTION)
/* There is no stlsoft namespace, so must define ::unixstl */
namespace unixstl
{
# else
/* Define stlsoft::unixstl_project */

namespace stlsoft
{

namespace unixstl_project
{

# endif /* _STLSOFT_NO_NAMESPACE */
#endif /* !_UNIXSTL_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
/** \brief The exception-type thrown by unixstl::glob_sequence
 *
 * \ingroup group__library__filesystem
 */
class glob_sequence_exception
#if defined(STLSOFT_COMPILER_IS_DMC)
    : public std::exception
#else /* ? compiler */
    : public unixstl_ns_qual_std(exception)
#endif /* compiler */
{
/// \name Types
/// @{
public:
#if defined(STLSOFT_COMPILER_IS_DMC)
    typedef std::exception                  parent_class_type;
#else /* ? compiler */
    typedef unixstl_ns_qual_std(exception)  parent_class_type;
#endif /* compiler */
    typedef glob_sequence_exception         class_type;
/// @}

/// \name Construction
/// @{
public:
    ss_explicit_k glob_sequence_exception(us_int_t globStatus, us_int_t errno_) stlsoft_throw_0()
        : m_globStatus(globStatus)
        , m_errno(errno_)
    {}
/// @}

/// \name Accessors
/// @{
public:
#if defined(STLSOFT_COMPILER_IS_DMC)
    char const  *what() const throw()
#else /* ? compiler */
    char const  *what() const stlsoft_throw_0()
#endif /* compiler */
    {
        return "glob_sequence failure";
    }

    us_int_t get_globstatus() const stlsoft_throw_0()
    {
        return m_globStatus;
    }
    us_int_t get_errno() const stlsoft_throw_0()
    {
        return m_errno;
    }
/// @}

/// \name Members
/// @{
private:
    us_int_t const  m_globStatus;
    us_int_t const  m_errno;
/// @}

/// \name Not to be implemented
/// @{
private:
    class_type& operator =(class_type const&);
/// @}
};

#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

/** \brief STL-like readonly sequence based on the results of file-system wildcard matches
 *
 * \ingroup group__library__filesystem
 *
 * This class presents and STL-like readonly sequence interface to allow the
 * iteration over the results of file-system wildcard matches.
 */
class glob_sequence
    : public stlsoft_ns_qual(stl_collection_tag)
{
/// \name Types
/// @{
public:
    /// \brief This class
    typedef glob_sequence                                                   class_type;
    /// \brief The char type
    typedef us_char_a_t                                                     char_type;
    // The traits type
    typedef filesystem_traits<char_type>                                    traits_type;
    /// \brief The value type
    typedef char_type const                                                 *value_type;
    /// \brief The non-mutating (const) reference type
    typedef value_type const                                                &const_reference;
    /// \brief The non-mutating (const) pointer type
    typedef value_type const                                                *const_pointer;
    /// \brief The size type
    typedef us_size_t                                                       size_type;
    /// \brief The difference type
    typedef us_ptrdiff_t                                                    difference_type;
    /// \brief The allocator type
    typedef stlsoft_ns_qual(allocator_selector)<value_type>::allocator_type allocator_type;

    /// \brief The non-mutating (const) iterator type
    typedef stlsoft_ns_qual(pointer_iterator)<   value_type const
                                             ,   const_pointer
                                             ,   const_reference
                                             >::type                        const_iterator;

#ifdef STLSOFT_LF_BIDIRECTIONAL_ITERATOR_SUPPORT
    /// \brief The type of the const (non-mutating) reverse iterator
    typedef stlsoft_ns_qual(reverse_iterator_base)  <   const_iterator
                                                    ,   value_type
                                                    ,   const_reference
                                                    ,   const_pointer
                                                    ,   difference_type
                                                    >                       const_reverse_iterator;
#endif /* STLSOFT_LF_BIDIRECTIONAL_ITERATOR_SUPPORT */
/// @}

/// \name Member Constants
/// @{
public:
    enum search_flags
    {
            includeDots     =   0x0008  /*!< \brief Requests that dots directories be included in the returned sequence */
        ,   directories     =   0x0010  /*!< \brief Causes the search to include directories */
        ,   files           =   0x0020  /*!< \brief Causes the search to include files */
        ,   noSort          =   0x0100  /*!< \brief Does not sort entries. Corresponds to GLOB_NOSORT. */
        ,   markDirs        =   0x0200  /*!< \brief Mark directories with a trailing path name separator. Corresponds to GLOB_MARK. */
        ,   absolutePath    =   0x0400  /*!< \brief Return all entries in absolute format */

        ,   breakOnError    =   0x0800  /*!< \brief Causes processing to stop on the first filesystem error. Corresponds to GLOB_ERR. */
        ,   noEscape        =   0x1000  /*!< \brief Treats backslashes literally. Corresponds to GLOB_NOESCAPE. */

#ifdef GLOB_PERIOD
        ,   matchPeriod     =   0x2000  /*!< \brief Leading '.' can be matched by metacharacters. Corresponds to GLOB_PERIOD. */
#endif /* GLOB_PERIOD */
#ifdef GLOB_BRACE
        ,   bracePatterns   =   0x4000  /*!< \brief Allow "{*.cpp;makefile*}" style multi-part patterns. Corresponds to GLOB_BRACE. */
#endif /* GLOB_BRACE */
#ifdef GLOB_TILDE
        ,   expandTilde     =   0x8000  /*!< \brief Expand ~ and ~<user> directories. Corresponds to GLOB_TILDE. */
#endif /* GLOB_TILDE */
    };
/// @}

/// \name Construction
/// @{
public:
#if defined(GLOB_SEQUENCE_CTOR_PRIMARY_FORM)
    /// \brief Constructs a sequence according to the given criteria
    ///
    /// The constructor initialises a glob_sequence instance on the given
    /// pattern with the given flags.
    ///
    /// \param pattern The pattern against which to match the file-system contents
    /// \param flags Flags to alter the behaviour of the search
    ///
    /// \note If exceptions are supported, then this will throw a glob_sequence_exception
    /// on failure of any underlying functions
    template<ss_typename_param_k S>
    ss_explicit_k glob_sequence(S const& pattern, us_int_t flags = noSort)
        : m_flags(validate_flags_(flags))
        , m_buffer(1)
    {
        m_cItems = init_glob_(NULL, stlsoft_ns_qual(c_str_ptr)(pattern));

        UNIXSTL_ASSERT(is_valid());
    }

# ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION
    template<ss_typename_param_k S>
    glob_sequence(S const& pattern, search_flags flag)
        : m_flags(validate_flags_(flag))
        , m_buffer(1)
    {
        m_cItems = init_glob_(NULL, stlsoft_ns_qual(c_str_ptr)(pattern));

        UNIXSTL_ASSERT(is_valid());
    }
# endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */

    /// \brief Constructs a sequence according to the given criteria
    ///
    /// The constructor initialises a glob_sequence instance on the given
    /// pattern with the given flags.
    ///
    /// \param directory The directory in which the pattern is located
    /// \param pattern The pattern against which to match the file-system contents
    /// \param flags Flags to alter the behaviour of the search
    ///
    /// \note If exceptions are supported, then this will throw a glob_sequence_exception
    /// on failure of any underlying functions
    template<   ss_typename_param_k S1
            ,   ss_typename_param_k S2
            >
    glob_sequence(S1 const& directory, S2 const& pattern, us_int_t flags = noSort)
        : m_flags(validate_flags_(flags))
        , m_buffer(1)
    {
        m_cItems = init_glob_(stlsoft_ns_qual(c_str_ptr)(directory), stlsoft_ns_qual(c_str_ptr)(pattern));

        UNIXSTL_ASSERT(is_valid());
    }

# ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION
    template<   ss_typename_param_k S1
            ,   ss_typename_param_k S2
            >
    glob_sequence(S1 const& directory, S2 const& pattern, search_flags flag)
        : m_flags(validate_flags_(flag))
        , m_buffer(1)
    {
        m_cItems = init_glob_(stlsoft_ns_qual(c_str_ptr)(directory), stlsoft_ns_qual(c_str_ptr)(pattern));

        UNIXSTL_ASSERT(is_valid());
    }
# endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */

#elif defined(GLOB_SEQUENCE_CTOR_ALT_FORM)

    template<ss_typename_param_k S>
    ss_explicit_k glob_sequence(S const& pattern)
        : m_flags(validate_flags_())
        , m_buffer(1)
    {
        m_cItems = init_glob_(NULL, stlsoft_ns_qual(c_str_ptr)(pattern));

        UNIXSTL_ASSERT(is_valid());
    }

    template<ss_typename_param_k S>
    glob_sequence(S const& pattern, us_int_t flags /* = noSort */)
        : m_flags(validate_flags_(flags))
        , m_buffer(1)
    {
        m_cItems = init_glob_(NULL, stlsoft_ns_qual(c_str_ptr)(pattern));

        UNIXSTL_ASSERT(is_valid());
    }

    glob_sequence(char_type const* directory, char_type const* pattern, us_int_t flags = noSort);

    template<ss_typename_param_k S>
    glob_sequence(S const& directory, char const* pattern, us_int_t flags = noSort)
        : m_flags(validate_flags_(flags))
        , m_buffer(1)
    {
        m_cItems = init_glob_(stlsoft_ns_qual(c_str_ptr)(directory), stlsoft_ns_qual(c_str_ptr)(pattern));

        UNIXSTL_ASSERT(is_valid());
    }

    template<ss_typename_param_k S>
    glob_sequence(S const& directory, S const& pattern, us_int_t flags = noSort)
        : m_flags(validate_flags_(flags))
        , m_buffer(1)
    {
        m_cItems = init_glob_(stlsoft_ns_qual(c_str_ptr)(directory), stlsoft_ns_qual(c_str_ptr)(pattern));

        UNIXSTL_ASSERT(is_valid());
    }

#elif defined(GLOB_SEQUENCE_CTOR_OLD_FORM)

    ss_explicit_k glob_sequence(char_type const* pattern, us_int_t flags = noSort);

    glob_sequence(char_type const* directory, char_type const* pattern, us_int_t flags = noSort);

#else /* ? constructor form */
# error Constructor form not recognised
#endif /* constructor form */

#if 0
    /// \brief Constructs a sequence according to the given criteria
    ///
    /// The constructor initialises a glob_sequence instance on the given
    /// pattern with the given flags.
    ///
    /// \param directory The directory in which the pattern is located
    /// \param pattern The pattern against which to match the file-system contents
    /// \param flags Flags to alter the behaviour of the search
    ///
    /// \note If exceptions are supported, then this will throw a glob_sequence_exception
    /// on failure of any underlying functions
    glob_sequence(char_type const* directory, char_type const* pattern, char_type delim, us_int_t flags = noSort);
#endif /* 0 */

    /// \brief Releases any acquired resources
    ~glob_sequence() stlsoft_throw_0();
/// @}

/// \name Attributes
/// @{
public:
    /// \brief Returns the number of elements in the sequence
    us_size_t size() const;

    /// \brief Indicates whether the search sequence is empty
    us_bool_t empty() const;
/// @}

/// \name Element Access
/// @{
public:
    /// \brief Returns the value corresponding to the given index
    ///
    /// \note In debug-mode a runtime assert is applied to enforce that the index is valid. There is <b>no</b> release-time checking on the index validity!
    const_reference operator [](size_type index) const;
/// @}

/// \name Iteration
/// @{
public:
    /// \brief Begins the iteration
    ///
    /// \return An iterator representing the start of the sequence
    const_iterator  begin() const;
    /// \brief Ends the iteration
    ///
    /// \return An iterator representing the end of the sequence
    const_iterator  end() const;

#ifdef STLSOFT_LF_BIDIRECTIONAL_ITERATOR_SUPPORT
    /// \brief Begins the reverse iteration
    ///
    /// \return An iterator representing the start of the reverse sequence
    const_reverse_iterator  rbegin() const;
    /// \brief Ends the reverse iteration
    ///
    /// \return An iterator representing the end of the reverse sequence
    const_reverse_iterator  rend() const;
#endif /* STLSOFT_LF_BIDIRECTIONAL_ITERATOR_SUPPORT */
/// @}

/// \name Implementation
/// @{
private:
    // Tests the class invariant
    us_bool_t           is_valid() const;

    // Validates the flags, and sets up defaults
    static us_int_t     validate_flags_(us_int_t flags = noSort);

    // Returns true if pch is a path separator "/" (or "\\"); false otherwise
    static us_bool_t    is_path_separator_(char_type ch);

    // Returns true if pch == "" or "/" (or "\\"); false otherwise
    static us_bool_t    is_end_of_path_elements_(char_type const* pch, difference_type index);

    // Returns true if s points to a path that is a dots directory; false otherwise
    static us_bool_t    is_dots_maybe_slashed_(char_type const* s, us_bool_t &bTwoDots);

    // Calls glob() and process the results
    us_size_t           init_glob_(char_type const* directory, char_type const* pattern);
/// @}

/// \name Members
/// @{
private:
    typedef stlsoft_ns_qual(auto_buffer_old)<   char_type const*
                                            ,   allocator_type
                                            ,   128
                                            >       buffer_type_;

    us_int_t const      m_flags;
    char_type const**   m_base;
    us_size_t           m_cItems;
    buffer_type_        m_buffer;
    glob_t              m_glob;
/// @}

/// \name Not to be implemented
/// @{
private:
    glob_sequence(class_type const&);
    class_type const& operator =(class_type const&);
/// @}
};

////////////////////////////////////////////////////////////////////////////
// Unit-testing

#ifdef STLSOFT_UNITTEST
# include "./unittest/glob_sequence_unittest_.h"
#endif /* STLSOFT_UNITTEST */

/* /////////////////////////////////////////////////////////////////////////
 * Implementation
 */

#ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION

#if defined(GLOB_SEQUENCE_CTOR_OLD_FORM)
inline /* ss_explicit_k */ glob_sequence::glob_sequence(glob_sequence::char_type const* pattern, us_int_t flags /* = noSort */)
    : m_flags(validate_flags_(flags))
    , m_buffer(1)
{
    m_cItems = init_glob_(NULL, pattern);

    UNIXSTL_ASSERT(is_valid());
}
#endif /* GLOB_SEQUENCE_CTOR_OLD_FORM */

#if defined(GLOB_SEQUENCE_CTOR_OLD_FORM) || \
    defined(GLOB_SEQUENCE_CTOR_ALT_FORM)
inline glob_sequence::glob_sequence(glob_sequence::char_type const* directory, glob_sequence::char_type const* pattern, us_int_t flags /* = noSort */)
    : m_flags(validate_flags_(flags))
    , m_buffer(1)
{
    m_cItems = init_glob_(directory, pattern);

    UNIXSTL_ASSERT(is_valid());
}
#endif /* GLOB_SEQUENCE_CTOR_OLD_FORM || GLOB_SEQUENCE_CTOR_ALT_FORM */

#if 0
template<   ss_typename_param_k S1
        ,   ss_typename_param_k S2
        >
inline glob_sequence::glob_sequence(S1 const& directory, S2 const& pattern, us_int_t flags /* = noSort */)
    : m_flags(validate_flags_(flags))
    , m_buffer(1)
{
    m_cItems = init_glob_(stlsoft_ns_qual(c_str_ptr)(directory), stlsoft_ns_qual(c_str_ptr)(pattern));

    UNIXSTL_ASSERT(is_valid());
}
#endif /* 0 */

#if 0
inline glob_sequence::glob_sequence(char_type const* directory, char_type const* pattern, char_type delim, us_int_t flags /* = noSort */)
    : m_flags(validate_flags_(flags))
    , m_buffer(1)
{

    m_cItems = init_glob_(NULL, pattern);

    UNIXSTL_ASSERT(is_valid());
}
#endif /* 0 */

inline glob_sequence::~glob_sequence() stlsoft_throw_0()
{
    UNIXSTL_ASSERT(is_valid());

    if(NULL != m_base)
    {
        ::globfree(&m_glob);
    }
}

inline us_size_t glob_sequence::size() const
{
    return m_cItems;
}

inline us_bool_t glob_sequence::empty() const
{
    return 0 == size();
}

inline glob_sequence::const_reference glob_sequence::operator [](glob_sequence::size_type index) const
{
    UNIXSTL_MESSAGE_ASSERT("index access out of range in glob_sequence", index < 1 + size());   // Has to be +1, since legitimate to take address of one-past-the-end

    return m_base[index];
}

inline glob_sequence::const_iterator glob_sequence::begin() const
{
    return m_base;
}

inline glob_sequence::const_iterator glob_sequence::end() const
{
    return m_base + m_cItems;
}

#ifdef STLSOFT_LF_BIDIRECTIONAL_ITERATOR_SUPPORT
inline glob_sequence::const_reverse_iterator glob_sequence::rbegin() const
{
    return const_reverse_iterator(end());
}

inline glob_sequence::const_reverse_iterator glob_sequence::rend() const
{
    return const_reverse_iterator(begin());
}
#endif /* STLSOFT_LF_BIDIRECTIONAL_ITERATOR_SUPPORT */

inline us_bool_t glob_sequence::is_valid() const
{
    if((0 != m_cItems) && (NULL == m_base))
    {
        return false;
    }

    return true;
}


inline /* static */ us_int_t glob_sequence::validate_flags_(us_int_t flags)
{
    const us_int_t  validFlags  =   0
                                |   includeDots
                                |   directories
                                |   files
                                |   noSort
                                |   markDirs
                                |   absolutePath
                                |   breakOnError
                                |   noEscape
#ifdef GLOB_PERIOD
                                |   matchPeriod
#endif /* GLOB_PERIOD */
#ifdef GLOB_BRACE
                                |   bracePatterns
#endif /* GLOB_BRACE */
#ifdef GLOB_TILDE
                                |   expandTilde
#endif /* GLOB_TILDE */
                                |   0;

    UNIXSTL_MESSAGE_ASSERT("Specification of unrecognised/unsupported flags", flags == (flags & validFlags));
    STLSOFT_SUPPRESS_UNUSED(validFlags);

    if(0 == (flags & (directories | files)))
    {
        flags |= (directories | files);
    }

#ifndef UNIXSTL_GLOB_SEQUENCE_DONT_TRUST_MARK
    // If we're not searching for directories, then we can optimise the
    // subsequent filtering by asking for the dots directories (so we
    // skip that filtering) and asking for directories to be marked (so
    // we can detect the mark rather than making a system call (stat())
    if(0 == (flags & directories))
    {
        // It's more efficient to not bother doing a separate dots check if all
        // directories are being elided.
        flags |= includeDots;

        // Since we're not going to be returning directories to the caller, and
        // it's more efficient to believe the glob() directory marking rather
        // than calling stat, we add the markDirs flag here.
        flags |= markDirs;
    }
#endif /* !UNIXSTL_GLOB_SEQUENCE_DONT_TRUST_MARK */

    return flags;
}

inline /* static */ us_bool_t glob_sequence::is_path_separator_(glob_sequence::char_type ch)
{
    return  ch == '/'
#if defined(_UNIXSTL_COMPILER_IS_UNKNOWN) && \
    !defined(_UNIXSTL_GLOB_SEQUENCE_NO_BACK_SLASH_TERMINATOR)
            || ch == '\\' /* Allow for testing on Win32 systems */
#endif /* _UNIXSTL_COMPILER_IS_UNKNOWN && !_UNIXSTL_GLOB_SEQUENCE_NO_BACK_SLASH_TERMINATOR */
            ;
}


inline /* static */ us_bool_t glob_sequence::is_end_of_path_elements_(glob_sequence::char_type const* pch, glob_sequence::difference_type index)
{
    return  pch[index] == '\0' ||
            (   pch[index + 1] == '\0' &&
                is_path_separator_(pch[index]));
}

inline /* static */ us_bool_t glob_sequence::is_dots_maybe_slashed_(glob_sequence::char_type const* s, us_bool_t &bTwoDots)
{
    UNIXSTL_ASSERT(NULL != s);

    // This must match all patterns like the following:
    //
    // "."          l=1, s[0] = .
    // ".."         l=2, s[0] = ., s[1] = .
    // "./"         l=2, s[0] = ., s[1] = sep
    // "../"        l=3, s[0] = ., s[1] = ., s[2] = sep
    // "abc/."
    // "abc/.."
    // "abc/./"
    // "abc/../"

    const us_size_t len             =   traits_type::str_len(s);
    us_size_t       lastNameChar    =   len -1;

    UNIXSTL_ASSERT(len > 0);

    if(is_path_separator_(s[lastNameChar]))
    {
        --lastNameChar;
    }
    if( 0 < lastNameChar &&
        '.' == s[lastNameChar])
    {
        --lastNameChar;

        if( 0 == lastNameChar ||
            is_path_separator_(s[lastNameChar]))
        {
            bTwoDots = false;
            return true;
        }
        else if( 0 < lastNameChar &&
                '.' == s[lastNameChar])
        {
            --lastNameChar;

            if( 0 == lastNameChar ||
                is_path_separator_(s[lastNameChar]))
            {
                bTwoDots = true;
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}

inline us_size_t glob_sequence::init_glob_(glob_sequence::char_type const* directory, glob_sequence::char_type const* pattern)
{
    UNIXSTL_MESSAGE_ASSERT("Null pattern given to glob_sequence", NULL != pattern);

    us_int_t                            glob_flags  =   0;
    basic_file_path_buffer<char_type>   scratch_;   // Scratch buffer for directory / pattern

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
    if(0 == scratch_.size())
    {
        m_base = NULL;

        return 0;
    }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

    if( NULL == directory &&
        absolutePath == (m_flags & absolutePath))
    {
        static const char_type  s_thisDir[] = { '.', '\0' };

        directory = s_thisDir;
    }

    // If a directory is given, then ...
    if( NULL != directory &&
        '\0' != *directory)
    {
        us_size_t dirLen;

        // ... optionally turn it into an absolute directory, ...
        if(absolutePath == (m_flags & absolutePath))
        {
            dirLen = traits_type::get_full_path_name(directory, scratch_.size(), &scratch_[0]);
        }
        else
        {
            dirLen = traits_type::str_len(directory);

            traits_type::char_copy(&scratch_[0], directory, dirLen);
            scratch_[dirLen] = '\0';
        }

        // ... ensure that it has a trailing path name-separator, and ...
        traits_type::ensure_dir_end(&scratch_[0] + (dirLen ? dirLen - 1 : 0));

        // ... prefix directory onto pattern.
        size_type scratchLen = traits_type::str_len(scratch_.data());
        size_type patternLen = traits_type::str_len(pattern);
        traits_type::char_copy(&scratch_[0] + scratchLen, pattern, patternLen);
        scratch_[scratchLen + patternLen] = '\0';
        pattern = scratch_.c_str();
    }

    if(m_flags & noSort)
    {
        // Don't bother sorting
        glob_flags |= GLOB_NOSORT;
    }

    if(m_flags & markDirs)
    {
        // Ask for trailing slashes on directories
        glob_flags |= GLOB_MARK;
    }

#ifdef GLOB_ONLYDIR // If this is not defined, we rely on stat
    if(directories == (m_flags & (directories | files)))
    {
        // Ask for only directories
        glob_flags |= GLOB_ONLYDIR;
    }
#endif /* GLOB_ONLYDIR */

#ifdef GLOB_ONLYFILE // If this is not defined, we rely on stat
    if(files == (m_flags & (directories | files)))
    {
        // Ask for only files
        glob_flags |= GLOB_ONLYFILE;
    }
#endif /* GLOB_ONLYFILE */

#ifdef GLOB_ERR
    if(m_flags & breakOnError)
    {
        glob_flags |= GLOB_ERR;
    }
#endif /* GLOB_ERR */

#ifdef GLOB_NOESCAPE
    if(m_flags & noEscape)
    {
        glob_flags |= GLOB_NOESCAPE;
    }
#endif /* GLOB_NOESCAPE */

#ifdef GLOB_PERIOD
    if(m_flags & matchPeriod)
    {
        glob_flags |= GLOB_PERIOD;
    }
#endif /* GLOB_PERIOD */

#ifdef GLOB_BRACE
    if(m_flags & bracePatterns)
    {
        glob_flags |= GLOB_BRACE;
    }
#endif /* GLOB_BRACE */

#ifdef GLOB_TILDE
    if(m_flags & expandTilde)
    {
        glob_flags |= GLOB_TILDE;
    }
#endif /* GLOB_TILDE */

    int gr = ::glob(pattern, glob_flags, NULL, &m_glob);

    if(0 != gr)
    {
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
# ifdef GLOB_NOMATCH
        // When GLOB_NOMATCH is not defined, we can reasonably infer that
        // there is no replacement value, so throwing on a non-zero
        // return from glob() is appropriate
        if(GLOB_NOMATCH != gr)
# endif /* GLOB_NOMATCH */
        {
            STLSOFT_THROW_X(glob_sequence_exception(gr, 0));
        }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        m_base = NULL;

        return 0;
    }
    else
    {
        // Sink the glob_t instance into a local scoped_handle, to invoke
        // ::globfree() automatically if we encounter any problems during
        // the processing. At the end we'll call detach(), to "give it"
        // to the glob_sequence instance.

        stlsoft_ns_qual(scoped_handle)<glob_t*>     cleanup(&m_glob, ::globfree);

        char_type   **base  =   m_glob.gl_pathv;
        us_size_t   cItems  =   static_cast<us_size_t>(m_glob.gl_pathc);

        // This section performs a COW on the entry pointers, into
        // the m_buffer member, if any of the following hold:
        //
        // 1. we're eliding dots
        // 2. we want directories only, and so will need to
        //     remove files, AND we do not trust GLOB_ONLYDIR
        // 3. we want files only, and so will need
        //     to remove directories
        //
        if( 0 == (m_flags & includeDots) ||                     // 1
#ifndef UNIXSTL_GLOB_SEQUENCE_TRUST_ONLYDIR
            directories == (m_flags & (directories | files)) || // 2
#endif /* !UNIXSTL_GLOB_SEQUENCE_TRUST_ONLYDIR */
            files == (m_flags & (directories | files)))         // 3
        {
            if(!m_buffer.resize(cItems))
            {
#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
                m_base = NULL;

                return 0;
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */
            }

            UNIXSTL_ASSERT(m_buffer.size() == cItems);

            base = static_cast<char_type**>(memcpy(&m_buffer[0], base, m_buffer.size() * sizeof(char_type*)));
        }

        // This section elides dots directories.
        if(0 == (m_flags & includeDots))
        {
            // Now remove the dots. If located at the start of
            // the gl buffer, then simply increment m_base to
            // be above that. If not then rearrange the base
            // two pointers such that they are there.

            us_bool_t   bFoundDot1   =   false;
            us_bool_t   bFoundDot2   =   false;
            char_type   **begin     =   base;
            char_type   **end       =   begin + cItems;

            for(; begin != end; ++begin)
            {
                us_bool_t   bTwoDots = false;

                if(is_dots_maybe_slashed_(*begin, bTwoDots))
                {
                    // Swap with whatever is at base[0]
                    if(begin != base)
                    {
                        stlsoft_ns_qual(std_swap)(*begin, *base);
                    }
                    ++base;
                    --cItems;

                     // We're only going to get one "." and one ".."
                    (bTwoDots ? bFoundDot2 : bFoundDot1) = true;

                    if( bFoundDot1 &&
                        bFoundDot2)
                    {
                        break;
                    }
                }
            }
        }

        // This section performs the filtering, if any of the following
        // hold:
        //
        // 1. we want directories only, and so will need to
        //     remove files, AND we do not trust GLOB_ONLYDIR
        // 2. we want files only, and so will need
        //     to remove directories
        //
#ifndef UNIXSTL_GLOB_SEQUENCE_TRUST_ONLYDIR
        if((m_flags & (directories | files)) != (directories | files))  // 1 & 2
#else /* ? UNIXSTL_GLOB_SEQUENCE_TRUST_ONLYDIR */
        if((m_flags & (directories | files)) == files)                  // 2
#endif /* !UNIXSTL_GLOB_SEQUENCE_TRUST_ONLYDIR */
        {
            // Declare a path buffer which will be used as a scratch area
            // in case we need to remove trailing path name separators to
            // perform stat() on a (suspected) directory entry
            basic_file_path_buffer<char_type>   buffer;

            char_type                           **begin =   base;
            char_type                           **end   =   begin + cItems;

#ifndef STLSOFT_CF_EXCEPTION_SUPPORT
            if(0 == buffer.size())
            {
                m_base = NULL;

                return 0;
            }
#endif /* !STLSOFT_CF_EXCEPTION_SUPPORT */

            // This loop enumerates through the list of entries, and attempts
            // to match each with the requested file type.
            //
            // 1. If we're wanting to return only files, then markDirs (GLOB_MARK)
            //     will have been applied by validate_flags_(), so we test that
            //     first
            // 2. If we're wanting to return only directories, then we must not
            //     be trusting GLOB_ONLYDIR. Either:
            // 2.1. The user specified markDirs (GLOB_MARK), so use that, or
            // 2.2. The user did not specify, and we must stat()
            //
            for(; begin != end; ++begin)
            {
                // Now need to process the file, by using stat
                struct stat         st;
                int                 res;
                char_type const*    entry  =   *begin;

                // 1.
                if(files == (m_flags & (directories | files)))
                {
                    UNIXSTL_ASSERT(markDirs == (m_flags & markDirs));

                    if(!traits_type::has_dir_end(entry))
                    {
                        continue;   // A non-marked entry, hence a file, so accept
                    }
#ifndef UNIXSTL_GLOB_SEQUENCE_DONT_TRUST_MARK
                    else
                    {
                        res = ::stat(entry, &st);

                        if(0 != res)
                        {
                            // We could throw an exception here, but it might just be
                            // the case that a file has been deleted subsequent to its
                            // having been included in the glob list. As such, it makes
                            // more sense to just kick it from the list
                        }
                        else
                        { // stat() succeeded
                            if(S_IFREG == (st.st_mode & S_IFREG))
                            {
                                continue; // A file, so accept it
                            }
                        }
                    }
#endif /* !UNIXSTL_GLOB_SEQUENCE_DONT_TRUST_MARK */
                }
                else // 2.
                {
                    UNIXSTL_ASSERT(directories == (m_flags & (directories | files)));

#ifdef UNIXSTL_GLOB_SEQUENCE_TRUST_ONLYDIR
                    UNIXSTL_MESSAGE_ASSERT("Cannot get here, since trusting GLOB_ONLYDIR", 0);
#endif /* UNIXSTL_GLOB_SEQUENCE_TRUST_ONLYDIR */

                    if(markDirs == (m_flags & markDirs))
                    {
                        if(traits_type::has_dir_end(entry))
                        {
                            continue;   // A marked entry, hence a directory, so accept
                        }
                    }
#ifndef UNIXSTL_GLOB_SEQUENCE_DONT_TRUST_MARK
                    else
#endif /* !UNIXSTL_GLOB_SEQUENCE_DONT_TRUST_MARK */
                    {
                        // Now we must stat()
                        res = stat(entry, &st);

                        if(0 != res)
                        {
                            // We could throw an exception here, but it might just be
                            // the case that a file has been deleted subsequent to its
                            // having been included in the glob list. As such, it makes
                            // more sense to just kick it from the list
                        }
                        else
                        { // stat() succeeded
                            if(S_IFDIR == (st.st_mode & S_IFDIR))
                            {
                                continue; // A directory, so accept it
                            }
                        }
                    }

                }

                // This section elides the entry from the list
                //
                // Note that there is no test here to determine whether or not
                // begin == base. It is assumed that most cases of file elision
                // will involve several files - how many directories have just
                // one file in them? - so the test would actually be a
                // pessimisation

                // Swap with whatever is at base[0]
                stlsoft_ns_qual(std_swap)(*begin, *base);
                ++base;
                --cItems;
            }
        }

        // Ensure we've not corrupted the sort order
        if( 0 == (m_flags & noSort) &&
            cItems != static_cast<us_size_t>(m_glob.gl_pathc))
        {
            unixstl_ns_qual_std(sort)(base, base + cItems);
        }

        // Set m_base and m_cItems to the correct values, with
        // or without dots. m_base is cast here to remove the
        // need for const-casting throughout the rest of the
        // class
        m_base = const_cast<char_type const**>(base);

        // Everything has succeeded, so give ownership over to the
        // glob_sequence instance
        cleanup.detach();

        return cItems;
    }
}

#endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

#ifndef _UNIXSTL_NO_NAMESPACE
# if defined(_STLSOFT_NO_NAMESPACE) || \
     defined(STLSOFT_DOCUMENTATION_SKIP_SECTION)
} // namespace unixstl
# else
} // namespace unixstl_project
} // namespace stlsoft
# endif /* _STLSOFT_NO_NAMESPACE */
#endif /* !_UNIXSTL_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !UNIXSTL_INCL_UNIXSTL_FILESYSTEM_HPP_GLOB_SEQUENCE */

/* ///////////////////////////// end of file //////////////////////////// */
