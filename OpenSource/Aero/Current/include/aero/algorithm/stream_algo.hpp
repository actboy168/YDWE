/*
 * Stream helper functions
 */
#ifndef STREAM_ALGO_HPP_INCLUDED
#define STREAM_ALGO_HPP_INCLUDED

#include <istream>
#include <boost/detail/endian.hpp>

namespace aero
{
	// Utility for retrieving stream size
	template<typename Elem, typename Traits>
	inline std::streamsize stream_size(std::basic_istream<Elem, Traits> &is)
	{
		// Check
		if (!is)
			return 0;

		std::streamsize theSize;
		std::streamsize curPos;
		
		// Get current position
		curPos = is.tellg();
		is.seekg(0, std::ios::end);
		theSize = is.tellg();
		
		// Set back
		is.seekg(curPos, std::ios::beg);

		return theSize;
	}
	
	// Stream extracters

	template <typename Type>
	inline Type stream_extract(std::istream &is, bool isBigEndian = 
#ifdef BOOST_BIG_ENDIAN
		true
#else
		false
#endif
		)
	{
		Type value;
#ifdef BOOST_BIG_ENDIAN
		// Machine is big endian
		if (isBigEndian)
		{
			// Data is big endian too
			
			// Let's directly read
			is.read((char *)&value, sizeof(Type));
		}
		else
		{
			// Data is little endian
			
			// Oh god, let's convert
			char *rend = (char *)&value - 1;
			char *rbegin = (char *)&value + sizeof(Type) - 1;
			for (char *itr = rbegin; itr != rend; --itr)
			{
				is.read(itr, sizeof(char));
			}
		}
#else
		// Machine is little endian
		if (!isBigEndian)
		{
			// Data is little endian
			
			// Let's directly read
			is.read((char *)&value, sizeof(Type));
		}
		else
		{
			// Data is big endian
			
			// Oh god, let's convert
			char *rend = (char *)&value - 1;
			char *rbegin = (char *)&value + sizeof(Type) - 1;
			for (char *itr = rbegin; itr != rend; --itr)
			{
				is.read(itr, sizeof(char));
			}
		}
#endif

		return value;
	}
	
#ifdef _MSC_VER
	// get characters into string, discard delimiter
	template<typename Elem,
		class Traits,
		class Alloc
	>
	inline std::basic_istream<Elem, Traits>& flexable_getline(
		std::basic_istream<Elem, Traits>& is,
		std::basic_string<Elem, Traits, Alloc>& str
	)
	{
		typedef std::basic_istream<Elem, Traits> MyInputStreamType;

		std::ios_base::iostate streamState = std::ios_base::goodbit;
		bool streamChanged = false;
		const typename MyInputStreamType::sentry streamOk(is, true);

		if (streamOk)
		{	// state okay, extract characters
			try
			{
				str.erase();
				const typename Traits::int_type metaDelimReturn = Traits::to_int_type(0xD);
				const typename Traits::int_type metaDelimNewline = Traits::to_int_type(0xA);
				typename Traits::int_type metaChar = is.rdbuf()->sgetc();

				for (; ; metaChar = is.rdbuf()->snextc())
				{
					if (Traits::eq_int_type(Traits::eof(), metaChar))
					{	// end of file, quit
						streamState |= std::ios_base::eofbit;
						break;
					}
					else if (Traits::eq_int_type(metaChar, metaDelimReturn))
					{	// got a delimiter, discard it and quit
						streamChanged = true;
						is.rdbuf()->sbumpc();

						// try to discard the next one
						if (Traits::eq_int_type(is.rdbuf()->sgetc(), metaDelimNewline))
							is.rdbuf()->sbumpc();

						break;
					}
					else if (Traits::eq_int_type(metaChar, metaDelimNewline))
					{	// got a delimiter, discard it and quit
						streamChanged = true;
						is.rdbuf()->sbumpc();

						break;
					}
					else if (str.max_size() <= str.size())
					{	// string too large, quit
						streamState |= std::ios_base::failbit;
						break;
					}
					else
					{	// got a character, add it to string
						str += Traits::to_char_type(metaChar);
						streamChanged = true;
					}
				}
			}
			catch (...)
			{
				is.setstate(std::ios_base::badbit);
				throw;
			}
		}

		if (!streamChanged)
			streamState |= std::ios_base::failbit;
		is.setstate(streamState);

		return is;
	}

	// get characters into string, discard delimiter
	template<typename Elem,
		class Traits
	>
	inline std::basic_istream<Elem, Traits>& flexable_getline(
		std::basic_istream<Elem, Traits>& is,
		Elem *buffer,
		size_t bufferLength
	)
	{
		typedef std::basic_istream<Elem, Traits> MyInputStreamType;

		std::ios_base::iostate streamState = std::ios_base::goodbit;
		bool streamChanged = false;
		const typename MyInputStreamType::sentry streamOk(is, true);
		Elem *end = buffer;

		if (streamOk)
		{	// state okay, extract characters
			try
			{
				const typename Traits::int_type metaDelimReturn = Traits::to_int_type(0xD);
				const typename Traits::int_type metaDelimNewline = Traits::to_int_type(0xA);
				typename Traits::int_type metaChar = is.rdbuf()->sgetc();

				for (; ; metaChar = is.rdbuf()->snextc())
				{
					if (Traits::eq_int_type(Traits::eof(), metaChar))
					{	// end of file, quit
						streamState |= std::ios_base::eofbit;
						break;
					}
					else if (Traits::eq_int_type(metaChar, metaDelimReturn))
					{	// got a delimiter, discard it and quit
						streamChanged = true;
						is.rdbuf()->sbumpc();

						// try to discard the next one
						if (Traits::eq_int_type(is.rdbuf()->sgetc(), metaDelimNewline))
							is.rdbuf()->sbumpc();

						break;
					}
					else if (Traits::eq_int_type(metaChar, metaDelimNewline))
					{	// got a delimiter, discard it and quit
						streamChanged = true;
						is.rdbuf()->sbumpc();

						break;
					}
					else if ((size_t)(end - buffer) >= bufferLength)
					{	// string too large, quit
						streamState |= std::ios_base::failbit;
						break;
					}
					else
					{	// got a character, add it to string
						*(end++) = Traits::to_char_type(metaChar);
						streamChanged = true;
					}
				}
			}
			catch (...)
			{
				is.setstate(std::ios_base::badbit);
				if ((size_t)(end - buffer) >= bufferLength)
					buffer[bufferLength - 1] = 0;
				else
					*end = 0;
				throw;
			}
		}

		if (!streamChanged)
			streamState |= std::ios_base::failbit;
		is.setstate(streamState);

		if ((size_t)(end - buffer) >= bufferLength)
			buffer[bufferLength - 1] = 0;
		else
			*end = 0;

		return is;
	}
#endif
	
	template <typename Type>
	inline void stream_send(std::ostream &os, Type value, bool isBigEndian = 
#ifdef BOOST_BIG_ENDIAN
		true
#else
		false
#endif
	)
	{
#ifdef BOOST_BIG_ENDIAN
		// Machine is big endian
		if (isBigEndian)
		{
			// Data is big endian too
			
			// Let's directly write
			os.write((char *)&value, sizeof(Type));
		}
		else
		{
			// Data is little endian
			
			// Oh god, let's convert
			char *rend = (char *)&value - 1;
			char *rbegin = (char *)&value + sizeof(Type) - 1;
			for (char *itr = rbegin; itr != rend; --itr)
			{
				os.write(itr, sizeof(char));
			}
		}
#else
		// Machine is little endian
		if (!isBigEndian)
		{
			// Data is little endian
			
			// Let's directly write
			os.write((char *)&value, sizeof(Type));
		}
		else
		{
			// Data is big endian
			
			// Oh god, let's convert
			char *rend = (char *)&value - 1;
			char *rbegin = (char *)&value + sizeof(Type) - 1;
			for (char *itr = rbegin; itr != rend; --itr)
			{
				os.write(itr, sizeof(char));
			}
		}
#endif
	}
}
#endif // STREAM_ALGO_HPP_INCLUDED
