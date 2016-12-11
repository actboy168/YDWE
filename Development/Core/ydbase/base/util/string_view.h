#pragma once

#if _HAS_CXX17

#include <string_view>

#else

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <string>
#include <iosfwd>

// see http://cplusplus.github.io/fundamentals-ts/main.html#string.view

#if 0
#    define BASE_CONSTEXPR          constexpr
#    define BASE_NOEXCEPT           noexcept
#    define BASE_CONSTEXPR_OR_CONST constexpr
#else
#    define BASE_CONSTEXPR
#    define BASE_NOEXCEPT
#    define BASE_CONSTEXPR_OR_CONST const
#endif

namespace std {
    namespace string_view_detail {
        template <class charT, class traits>
        class traits_eq
        {
        public:
            traits_eq(charT ch) : ch_(ch) {}
            bool operator() (charT val) const { return traits::eq(ch_, val); }
            charT ch_;
        };
    }

    template <class charT, class traits = char_traits<charT>>
    class basic_string_view
    {
    public:
        typedef traits traits_type;
        typedef charT value_type;
        typedef charT* pointer;
        typedef const charT* const_pointer;
        typedef charT& reference;
        typedef const charT& const_reference;
        typedef const charT* const_iterator; // impl-defined
        typedef const_iterator iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef const_reverse_iterator reverse_iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        static BASE_CONSTEXPR_OR_CONST size_type npos = size_type(-1);

        BASE_CONSTEXPR basic_string_view() BASE_NOEXCEPT
            : data_(nullptr)
            , size_(0)
            { }

        BASE_CONSTEXPR basic_string_view(const basic_string_view &rhs) BASE_NOEXCEPT // = default
            : data_(rhs.data_)
            , size_(rhs.size_)
            { }

        basic_string_view& operator=(const basic_string_view &rhs) BASE_NOEXCEPT // = default
        {
            data_ = rhs.data_;
            size_ = rhs.size_;
            return *this;
        }

        template <class Allocator>
        basic_string_view(const basic_string<charT, traits, Allocator>& str) BASE_NOEXCEPT
            : data_(str.data())
            , size_(str.size())
            { }

        BASE_CONSTEXPR basic_string_view(const charT* str)
            : data_(str)
            , size_(traits::length(str))
            { }

        BASE_CONSTEXPR basic_string_view(const charT* str, size_type len)
            : data_(str)
            , size_(len)
            { }

        BASE_CONSTEXPR const_iterator    begin() const BASE_NOEXCEPT { return data_; }
        BASE_CONSTEXPR const_iterator   cbegin() const BASE_NOEXCEPT { return data_; }
        BASE_CONSTEXPR const_iterator      end() const BASE_NOEXCEPT { return begin() + size(); }
        BASE_CONSTEXPR const_iterator     cend() const BASE_NOEXCEPT { return begin() + size(); }
                const_reverse_iterator  rbegin() const BASE_NOEXCEPT { return const_reverse_iterator(end()); }
                const_reverse_iterator crbegin() const BASE_NOEXCEPT { return const_reverse_iterator(end()); }
                const_reverse_iterator    rend() const BASE_NOEXCEPT { return const_reverse_iterator(begin()); }
                const_reverse_iterator   crend() const BASE_NOEXCEPT { return const_reverse_iterator(begin()); }

        BASE_CONSTEXPR size_type size()     const BASE_NOEXCEPT { return size_; }
        BASE_CONSTEXPR size_type length()   const BASE_NOEXCEPT { return size_; }
        BASE_CONSTEXPR size_type max_size() const BASE_NOEXCEPT { return size_; }
        BASE_CONSTEXPR bool empty()         const BASE_NOEXCEPT { return size() == 0; }

        BASE_CONSTEXPR const_reference operator[](size_type pos) const { assert(pos < size()); return data_[pos]; }
        BASE_CONSTEXPR const_reference at(size_t pos) const
        {
            if (pos >= size())
                throw out_of_range("std::string_view::at");
            return data_[pos];
        }
        BASE_CONSTEXPR const_reference front() const { assert(!empty()); return data_[0]; }
        BASE_CONSTEXPR const_reference back()  const { assert(!empty()); return data_[size()-1]; }
        BASE_CONSTEXPR const_pointer   data()  const BASE_NOEXCEPT { return data_; }

        BASE_CONSTEXPR void clear() BASE_NOEXCEPT
        {
            data_ = nullptr;
            size_ = 0;
        }
        BASE_CONSTEXPR void remove_prefix(size_type n)
        {
            assert(n <= size());
            data_ += n;
            size_ -= n;
        }
        BASE_CONSTEXPR void remove_suffix(size_type n)
        {
            assert(n <= size());
            size_ -= n;
        }
        BASE_CONSTEXPR void swap(basic_string_view& s) BASE_NOEXCEPT
        {
            swap(data_, s.data_);
            swap(size_, s.size_);
        }

#if 0
        template <class Allocator>
        explicit operator basic_string<charT, traits, Allocator>() const
        {
            return basic_string<charT, traits, Allocator>(begin(), end());
        }
        template <class Allocator = allocator<charT>>
        basic_string<charT, traits, Allocator> to_string(const Allocator& a = Allocator()) const
        {
            return basic_string<charT, traits, Allocator>(begin(), end(), a);
        }
#else
        basic_string<charT, traits> to_string() const
        {
            return basic_string<charT, traits>(begin(), end());
        }
#endif

        size_type copy(charT* s, size_type n, size_type pos = 0) const
        {
            if (pos > size())
                throw out_of_range("std::string_view::copy");
            size_type rlen = (std::min)(n, size_ - pos);
            copy_n(begin() + pos, rlen, s);
            return rlen;
        }

        BASE_CONSTEXPR basic_string_view substr(size_type pos = 0, size_type n = npos) const
        {
            if (pos > size())
                throw out_of_range("std::string_view::substr");
            size_type rlen = (std::min)(n, size_ - pos);
            return basic_string_view(data() + pos, rlen);
        }

        BASE_CONSTEXPR int compare(basic_string_view s) const BASE_NOEXCEPT
        {
            const int cmp = traits::compare(data(), s.data(), (std::min)(size(), s.size()));
            return cmp != 0 ? cmp: (size() == s.size() ? 0: (size() < s.size()? -1: 1));
        }
        BASE_CONSTEXPR int compare(size_type pos1, size_type n1, basic_string_view s) const
        {
            return substr(pos1, n1).compare(s);
        }
        BASE_CONSTEXPR int compare(size_type pos1, size_type n1, basic_string_view s, size_type pos2, size_type n2) const
        {
            return substr(pos1, n1).compare(s.substr(pos2, n2));
        }
        BASE_CONSTEXPR int compare(const charT* s) const BASE_NOEXCEPT
        {
            return compare(basic_string_view(s));
        }
        BASE_CONSTEXPR int compare(size_type pos1, size_type n1, const charT* s) const
        {
            return substr(pos1, n1).compare(basic_string_view(s));
        }
        BASE_CONSTEXPR int compare(size_type pos1, size_type n1, const charT* s, size_type n2) const
        {
            return substr(pos1, n1).compare(basic_string_view(s, n2));
        }

        BASE_CONSTEXPR size_type find(basic_string_view s, size_type pos = 0) const BASE_NOEXCEPT
        {
            const_iterator iter = search(cbegin() + pos, cend(), s.cbegin(), s.cend(), traits::eq);
            return iter == cend() ? npos: distance(cbegin(), iter);
        }
        BASE_CONSTEXPR size_type find(charT c, size_type pos = 0) const BASE_NOEXCEPT
        {
            if (pos >= size()) return npos;
            const_pointer p = traits::find(cbegin() + pos, size() - pos, c);
            return p == nullptr ? npos: static_cast<size_type>(p - data());
        }
        BASE_CONSTEXPR size_type find(const charT* s, size_type pos, size_type n) const
        {
            return find(basic_string_view(s, n), pos);
        }
        BASE_CONSTEXPR size_type find(const charT* s, size_type pos = 0) const
        {
            return find(basic_string_view(s), pos);
        }

        BASE_CONSTEXPR size_type rfind(basic_string_view s, size_type pos = npos) const BASE_NOEXCEPT
        {
            const_reverse_iterator iter = search(crbegin() + (size() - pos), crend(), s.crbegin(), s.crend(), traits::eq);
            return iter == crend() ? npos: reverse_distance(crbegin (), iter);
        }
        BASE_CONSTEXPR size_type rfind(charT c, size_type pos = npos) const BASE_NOEXCEPT
        {
            const_reverse_iterator iter = find_if(crbegin() + (size() - pos), crend(), string_view_detail::traits_eq<charT, traits>(c));
            return iter == crend() ? npos: reverse_distance(crbegin(), iter);
        }
        BASE_CONSTEXPR size_type rfind(const charT* s, size_type pos, size_type n) const
        {
            return rfind(basic_string_view(s, n), pos);
        }
        BASE_CONSTEXPR size_type rfind(const charT* s, size_type pos = npos) const
        {
            return rfind(basic_string_view(s), pos);
        }

        BASE_CONSTEXPR size_type find_first_of(basic_string_view s, size_type pos = 0) const BASE_NOEXCEPT
        {
            const_iterator iter = find_first_of(cbegin() + pos, cend(), s.cbegin(), s.cend(), traits::eq);
            return iter == cend() ? npos: distance(cbegin(), iter);
        }
        BASE_CONSTEXPR size_type find_first_of(charT c, size_type pos = 0) const BASE_NOEXCEPT
        {
            return find(c, pos);
        }
        BASE_CONSTEXPR size_type find_first_of(const charT* s, size_type pos, size_type n) const
        {
            return find_first_of(basic_string_view(s, n), pos);
        }
        BASE_CONSTEXPR size_type find_first_of(const charT* s, size_type pos = 0) const
        {
            return find_first_of(basic_string_view(s), pos);
        }

        BASE_CONSTEXPR size_type find_last_of(basic_string_view s, size_type pos = npos) const BASE_NOEXCEPT
        {
            const_reverse_iterator iter = find_first_of(crbegin() + (size() - pos), crend(), s.cbegin(), s.cend(), traits::eq);
            return iter == crend() ? npos: reverse_distance(crbegin(), iter);
        }
        BASE_CONSTEXPR size_type find_last_of(charT c, size_type pos = npos) const BASE_NOEXCEPT
        {
            return rfind(c, pos);
        }
        BASE_CONSTEXPR size_type find_last_of(const charT* s, size_type pos, size_type n) const
        {
            return find_last_of(basic_string_view(s, n), pos);
        }
        BASE_CONSTEXPR size_type find_last_of(const charT* s, size_type pos = npos) const
        {
            return find_last_of(basic_string_view(s), pos);
        }

        BASE_CONSTEXPR size_type find_first_not_of(basic_string_view s, size_type pos = 0) const BASE_NOEXCEPT
        {
            const_iterator iter = find_not_of(cbegin() + pos, cend(), s);
            return iter == cend () ? npos: distance(cbegin(), iter);
        }
        BASE_CONSTEXPR size_type find_first_not_of(charT c, size_type pos = 0) const BASE_NOEXCEPT
        {
            for (const_iterator iter = cbegin() + pos; iter != cend(); ++iter)
                if (!traits::eq(c, *iter))
                    return distance(cbegin(), iter);
            return npos;
        }
        BASE_CONSTEXPR size_type find_first_not_of(const charT* s, size_type pos, size_type n) const
        {
            return find_first_not_of(basic_string_view(s, n), pos);
        }
        BASE_CONSTEXPR size_type find_first_not_of(const charT* s, size_type pos = 0) const
        {
            return find_first_not_of(basic_string_view(s), pos);
        }

        BASE_CONSTEXPR size_type find_last_not_of(basic_string_view s, size_type pos = npos) const BASE_NOEXCEPT
        {
            const_reverse_iterator iter = find_not_of(crbegin() + (size() - pos), crend(), s);
            return iter == crend () ? npos: reverse_distance(crbegin(), iter);
        }
        BASE_CONSTEXPR size_type find_last_not_of(charT c, size_type pos = npos) const BASE_NOEXCEPT
        {
            for (const_reverse_iterator iter = crbegin() + (size() - pos); iter != crend(); ++iter)
                if (!traits::eq(c, *iter))
                    return reverse_distance(crbegin(), iter);
            return npos;
        }
        BASE_CONSTEXPR size_type find_last_not_of(const charT* s, size_type pos, size_type n) const
        {
            return find_last_not_of(basic_string_view(s, n), pos);
        }
        BASE_CONSTEXPR size_type find_last_not_of(const charT* s, size_type pos = npos) const
        {
            return find_last_not_of(basic_string_view(s), pos);
        }

    private:
        template <class Iterator>
        size_type reverse_distance(Iterator first, Iterator last) const
        {
            return size() - 1 - distance(first, last);
        }

        template <class Iterator>
        Iterator find_not_of(Iterator first, Iterator last, basic_string_view s) const
        {
            for (; first != last; ++first)
                if (0 == traits::find(s.data(), s.size(), *first))
                    return first;
            return last;
        }

        const_pointer data_;
        size_type     size_;
    };

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator==(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        if (x.size () != y.size ()) return false;
        return x.compare(y) == 0;
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator==(basic_string_view<charT, traits> x, const basic_string<charT, traits, Allocator> & y) BASE_NOEXCEPT
    {
        return x == basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator==(const basic_string<charT, traits, Allocator> & x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) == y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator==(basic_string_view<charT, traits> x, const charT * y) BASE_NOEXCEPT
    {
        return x == basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator==(const charT * x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) == y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator!=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        if (x.size () != y.size ()) return true;
        return x.compare(y) != 0;
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator!=(basic_string_view<charT, traits> x, const basic_string<charT, traits, Allocator> & y) BASE_NOEXCEPT
    {
        return x != basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator!=(const basic_string<charT, traits, Allocator> & x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) != y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator!=(basic_string_view<charT, traits> x, const charT * y) BASE_NOEXCEPT
    {
        return x != basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator!=(const charT * x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) != y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator<(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return x.compare(y) < 0;
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator<(basic_string_view<charT, traits> x, const basic_string<charT, traits, Allocator> & y) BASE_NOEXCEPT
    {
        return x < basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator<(const basic_string<charT, traits, Allocator> & x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) < y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator<(basic_string_view<charT, traits> x, const charT * y) BASE_NOEXCEPT
    {
        return x < basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator<(const charT * x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) < y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator>(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return x.compare(y) > 0;
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator>(basic_string_view<charT, traits> x, const basic_string<charT, traits, Allocator> & y) BASE_NOEXCEPT
    {
        return x > basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator>(const basic_string<charT, traits, Allocator> & x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) > y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator>(basic_string_view<charT, traits> x, const charT * y) BASE_NOEXCEPT
    {
        return x > basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator>(const charT * x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) > y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator<=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return x.compare(y) <= 0;
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator<=(basic_string_view<charT, traits> x, const basic_string<charT, traits, Allocator> & y) BASE_NOEXCEPT
    {
        return x <= basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator<=(const basic_string<charT, traits, Allocator> & x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) <= y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator<=(basic_string_view<charT, traits> x, const charT * y) BASE_NOEXCEPT
    {
        return x <= basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator<=(const charT * x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) <= y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator>=(basic_string_view<charT, traits> x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return x.compare(y) >= 0;
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator>=(basic_string_view<charT, traits> x, const basic_string<charT, traits, Allocator> & y) BASE_NOEXCEPT
    {
        return x >= basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits, typename Allocator>
    BASE_CONSTEXPR inline bool operator>=(const basic_string<charT, traits, Allocator> & x, basic_string_view<charT, traits> y)  BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) >= y;
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator>=(basic_string_view<charT, traits> x, const charT * y) BASE_NOEXCEPT
    {
        return x >= basic_string_view<charT, traits>(y);
    }

    template<typename charT, typename traits>
    BASE_CONSTEXPR inline bool operator>=(const charT * x, basic_string_view<charT, traits> y) BASE_NOEXCEPT
    {
        return basic_string_view<charT, traits>(x) >= y;
    }

    namespace string_view_detail {
        template<class charT, class traits>
        inline void insert_fill_chars(basic_ostream<charT, traits>& os, size_t n)
        {
            enum { chunk_size = 8 };
            charT fill_chars[chunk_size];
            fill_n(fill_chars, static_cast< size_t >(chunk_size), os.fill());
            for (; n >= chunk_size && os.good(); n -= chunk_size)
                os.write(fill_chars, static_cast< size_t >(chunk_size));
            if (n > 0 && os.good())
                os.write(fill_chars, n);
        }

        template<class charT, class traits>
        void insert_aligned(basic_ostream<charT, traits>& os, const basic_string_view<charT,traits>& str)
        {
            const size_t size = str.size();
            const size_t alignment_size = static_cast< size_t >(os.width()) - size;
            const bool align_left = (os.flags() & basic_ostream<charT, traits>::adjustfield) == basic_ostream<charT, traits>::left;
            if (!align_left)
            {
                insert_fill_chars(os, alignment_size);
                if (os.good())
                    os.write(str.data(), size);
            }
            else
            {
                os.write(str.data(), size);
                if (os.good())
                    insert_fill_chars(os, alignment_size);
            }
        }
    }

    template<class charT, class traits>
    inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const basic_string_view<charT,traits>& str)
    {
        if (os.good())
        {
            const size_t size = str.size();
            const size_t w = static_cast< size_t >(os.width());
            if (w <= size)
                os.write(str.data(), size);
            else
                string_view_detail::insert_aligned(os, str);
            os.width(0);
        }
        return os;
    }

    template<class charT, class traits>
    struct hash<basic_string_view<charT, traits>>
        : public unary_function<basic_string_view<charT, traits>, size_t>
    {
        size_t operator()(const basic_string_view<charT, traits>& key) const
        {
            size_t h = 0;
            for (auto iter = key.cbegin(); iter != key.cend(); ++iter)
                h = (h * 131) + *iter;
            return h;
        }
    };

    typedef basic_string_view<char>     string_view;
    typedef basic_string_view<wchar_t>  wstring_view;
#if 0
    typedef basic_string_view<char16_t> u16string_view;
    typedef basic_string_view<char32_t> u32string_view;
#endif

}

#undef BASE_CONSTEXPR
#undef BASE_NOEXCEPT
#undef BASE_CONSTEXPR_OR_CONST

#endif
