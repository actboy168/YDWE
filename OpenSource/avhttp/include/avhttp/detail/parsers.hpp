//
// parsers.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2009 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2013 Jack (jack dot wgm at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// * $Id: parsers.hpp 49 2011-07-15 03:00:34Z jack $
//

#ifndef AVHTTP_PARSERS_HPP
#define AVHTTP_PARSERS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <map>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cstring>

#include <boost/date_time.hpp>
#include <boost/algorithm/string.hpp>

#include "avhttp/settings.hpp"
#include "avhttp/detail/escape_string.hpp"

namespace avhttp {
namespace detail {

#ifndef atoi64
# ifdef _MSC_VER
#  define atoi64 _atoi64
# else
#  define atoi64(x) strtoll(x, (char**)NULL, 10)
# endif
#endif // atoi64

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
// for parser http date.
#  ifndef _USE_32BIT_TIME_T
#    define SIZEOF_TIME_T 8
#  else
#    define SIZEOF_TIME_T 4
#  endif
#endif

#if defined(_WIN32)
#	define strcasecmp lstrcmpiA
#endif

inline bool headers_equal(const std::string& a, const std::string& b)
{
	if (a.length() != b.length())
		return false;
	return std::equal(a.begin(), a.end(), b.begin(), tolower_compare);
}

inline void check_header(const std::string& name, const std::string& value,
	std::string& content_type, boost::int64_t& content_length,
	std::string& location)
{
	if (headers_equal(name, "Content-Type"))
		content_type = value;
	else if (headers_equal(name, "Content-Length"))
		content_length = (std::max)((boost::int64_t)atoi64(value.c_str()), content_length);
	else if (headers_equal(name, "Location"))
		location = value;
	else if (headers_equal(name, "Content-Range"))
	{
		std::string::size_type f = value.find('/');
		if (f != std::string::npos && f++ != std::string::npos)
		{
			std::string tmp = value.substr(f);
			if (!tmp.empty())
			{
				boost::int64_t length = atoi64(tmp.c_str());
				content_length = (std::max)(length, content_length);
			}
		}
	}
}

#ifdef atoi64
#undef atoi64
#endif

template <typename Iterator>
bool parse_http_status_line(Iterator begin, Iterator end,
	int& version_major, int& version_minor, int& status)
{
	enum
	{
		http_version_h,
		http_version_t_1,
		http_version_t_2,
		http_version_p,
		http_version_slash,
		http_version_major_start,
		http_version_major,
		http_version_minor_start,
		http_version_minor,
		status_code_start,
		status_code,
		reason_phrase,
		linefeed,
		fail
	} state = http_version_h;

	Iterator iter = begin;
	std::string reason;
	while (iter != end && state != fail)
	{
		char c = *iter++;
		switch (state)
		{
		case http_version_h:
			state = (c == 'H') ? http_version_t_1 : fail;
			break;
		case http_version_t_1:
			state = (c == 'T') ? http_version_t_2 : fail;
			break;
		case http_version_t_2:
			state = (c == 'T') ? http_version_p : fail;
			break;
		case http_version_p:
			state = (c == 'P') ? http_version_slash : fail;
			break;
		case http_version_slash:
			state = (c == '/') ? http_version_major_start : fail;
			break;
		case http_version_major_start:
			if (is_digit(c))
			{
				version_major = version_major * 10 + c - '0';
				state = http_version_major;
			}
			else
				state = fail;
			break;
		case http_version_major:
			if (c == '.')
				state = http_version_minor_start;
			else if (is_digit(c))
				version_major = version_major * 10 + c - '0';
			else
				state = fail;
			break;
		case http_version_minor_start:
			if (is_digit(c))
			{
				version_minor = version_minor * 10 + c - '0';
				state = http_version_minor;
			}
			else
				state = fail;
			break;
		case http_version_minor:
			if (c == ' ')
				state = status_code_start;
			else if (is_digit(c))
				version_minor = version_minor * 10 + c - '0';
			else
				state = fail;
			break;
		case status_code_start:
			if (is_digit(c))
			{
				status = status * 10 + c - '0';
				state = status_code;
			}
			else
				state = fail;
			break;
		case status_code:
			if (c == ' ')
				state = reason_phrase;
			else if (is_digit(c))
				status = status * 10 + c - '0';
			else
				state = fail;
			break;
		case reason_phrase:
			if (c == '\r')
				state = linefeed;
			else if (is_ctl(c))
				state = fail;
			else
				reason.push_back(c);
			break;
		case linefeed:
			return (c == '\n');
		default:
			return false;
		}
	}
	return false;
}

template <typename Iterator>
bool parse_http_headers(Iterator begin, Iterator end,
	std::string& content_type, boost::int64_t& content_length,
	std::string& location)
{
	enum
	{
		first_header_line_start,
		header_line_start,
		header_lws,
		header_name,
		space_before_header_value,
		header_value,
		linefeed,
		final_linefeed,
		fail
	} state = first_header_line_start;

	Iterator iter = begin;
	std::string reason;
	std::string name;
	std::string value;
	while (iter != end && state != fail)
	{
		char c = *iter++;
		switch (state)
		{
		case first_header_line_start:
			if (c == '\r')
				state = final_linefeed;
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
				state = fail;
			else
			{
				name.push_back(c);
				state = header_name;
			}
			break;
		case header_line_start:
			if (c == '\r')
			{
				boost::trim(name);
				boost::trim(value);
				check_header(name, value, content_type, content_length, location);
				name.clear();
				value.clear();
				state = final_linefeed;
			}
			else if (c == ' ' || c == '\t')
				state = header_lws;
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
				state = fail;
			else
			{
				boost::trim(name);
				boost::trim(value);
				check_header(name, value, content_type, content_length, location);
				name.clear();
				value.clear();
				name.push_back(c);
				state = header_name;
			}
			break;
		case header_lws:
			if (c == '\r')
				state = linefeed;
			else if (c == ' ' || c == '\t')
				; // Discard character.
			else if (is_ctl(c))
				state = fail;
			else
			{
				state = header_value;
				value.push_back(c);
			}
			break;
		case header_name:
			if (c == ':')
				state = space_before_header_value;
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
				state = fail;
			else
				name.push_back(c);
			break;
		case space_before_header_value:
			if (c == ' ')
				state = header_value;
			else if (is_ctl(c))
				state = fail;
			else
			{
				value.push_back(c);
				state = header_value;
			}
			break;
		case header_value:
			if (c == '\r')
				state = linefeed;
			else if (is_ctl(c))
				state = fail;
			else
				value.push_back(c);
			break;
		case linefeed:
			state = (c == '\n') ? header_line_start : fail;
			break;
		case final_linefeed:
			return (c == '\n');
		default:
			return false;
		}
	}
	return false;
}

typedef avhttp::option::option_item_list http_headers;

template <typename Iterator>
bool parse_http_headers(Iterator begin, Iterator end,
	std::string& content_type, boost::int64_t& content_length,
	std::string& location, http_headers& headers)
{
	enum
	{
		first_header_line_start,
		header_line_start,
		header_lws,
		header_name,
		space_before_header_value,
		header_value,
		linefeed,
		final_linefeed,
		fail
	} state = first_header_line_start;

	Iterator iter = begin;
	std::string reason;
	std::string name;
	std::string value;
	while (iter != end && state != fail)
	{
		char c = *iter++;
		switch (state)
		{
		case first_header_line_start:
			if (c == '\r')
				state = final_linefeed;
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
				state = fail;
			else
			{
				name.push_back(c);
				state = header_name;
			}
			break;
		case header_line_start:
			if (c == '\r')
			{
				boost::trim(name);
				boost::trim(value);
				check_header(name, value, content_type, content_length, location);
				headers.push_back(std::make_pair(name, value));
				name.clear();
				value.clear();
				state = final_linefeed;
			}
			else if (c == ' ' || c == '\t')
				state = header_lws;
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
				state = fail;
			else
			{
				boost::trim(name);
				boost::trim(value);
				check_header(name, value, content_type, content_length, location);
				headers.push_back(std::make_pair(name, value));
				name.clear();
				value.clear();
				name.push_back(c);
				state = header_name;
			}
			break;
		case header_lws:
			if (c == '\r')
				state = linefeed;
			else if (c == ' ' || c == '\t')
				; // Discard character.
			else if (is_ctl(c))
				state = fail;
			else
			{
				state = header_value;
				value.push_back(c);
			}
			break;
		case header_name:
			if (c == ':')
				state = space_before_header_value;
			else if (!is_char(c) || is_ctl(c) || is_tspecial(c))
				state = fail;
			else
				name.push_back(c);
			break;
		case space_before_header_value:
			if (c == ' ')
				state = header_value;
			if (c == '\r')	// 当value没有值的时候, 直接进入读取value完成逻辑, 避免失败.
				state = linefeed;
			else if (is_ctl(c))
				state = fail;
			else
			{
				value.push_back(c);
				state = header_value;
			}
			break;
		case header_value:
			if (c == '\r')
				state = linefeed;
			else if (is_ctl(c))
				state = (c == '\n') ? header_line_start : fail;
			else
				value.push_back(c);
			break;
		case linefeed:
			state = (c == '\n') ? header_line_start : fail;
			break;
		case final_linefeed:
			return (c == '\n');
		default:
			return false;
		}
	}
	return false;
}

// 从Content-Disposition解析filename字段, 示例:
// attachment=other; filename="file.zip"; 这样的字符串中匹配到
// filename项, 将它的value保存到filename变量.
template <typename Iterator>
bool content_disposition_filename(Iterator begin, Iterator end, std::string& filename)
{
	enum
	{
		parse_key_start,
		parse_key,
		parse_value_start,
		parse_value,
		parse_fail
	} state = parse_key_start;

	Iterator iter = begin;
	std::string name;
	std::string value;
	char c;

	while (iter != end && state != parse_fail)
	{
		c = *iter++;
		switch (state)
		{
		case parse_key_start:
			if (c == ' ')
				continue;
			if (is_char(c))
			{
				name.push_back(c);
				state = parse_key;
			}
			else
				state = parse_fail;
			break;
		case parse_key:
			if (c == ';')
			{
				name = "";
				state = parse_key_start;
			}
			else if (c == '=')
			{
				value = "";
				state = parse_value_start;
			}
			else if (is_tspecial(c) || c == ':')
			{
				name = "";
				state = parse_key_start;
			}
			else if (is_char(c) || c == '_')
				name.push_back(c);
			break;
		case parse_value_start:
			if (c == ';' || c == '\"' || c == '\'')
				continue;
			if (is_char(c))
			{
				value.push_back(c);
				state = parse_value;
			}
			else
				state = parse_fail;
			break;
		case parse_value:
			if (c == ';' || c == '\"' || c == '\'')
			{
				if (name == "filename")
					filename = value;
				state = parse_key_start;
			}
			else if (is_char(c))
				value.push_back(c);
			else
				state = parse_fail;
			break;
		case parse_fail:
			break;
		}
	}
	if (name == "filename" && !value.empty())
		filename = value;
	if (filename.empty())
		return false;
	return true;
}

// 转换ptime到time_t.
inline time_t ptime_to_time_t(const boost::posix_time::ptime& pt)
{
	struct tm tm = boost::posix_time::to_tm(pt);
	return std::mktime(&tm);
}

namespace {

static const char * const wkday[] = {
	"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
static const char * const weekday[] = {
	"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
};
static const char * const month[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
typedef struct {
	char name[5];
	int offset;		/* +/- in minutes */
} tzinfo;
#define TMP_DAYZONE -60
static const tzinfo tz[] = {
	{ "GMT", 0 },              /* Greenwich Mean */
	{ "UTC", 0 },              /* Universal (Coordinated) */
	{ "WET", 0 },              /* Western European */
	{ "BST", 0 TMP_DAYZONE },  /* British Summer */
	{ "WAT", 60 },             /* West Africa */
	{ "AST", 240 },            /* Atlantic Standard */
	{ "ADT", 240 TMP_DAYZONE },/* Atlantic Daylight */
	{ "EST", 300 },            /* Eastern Standard */
	{ "EDT", 300 TMP_DAYZONE },/* Eastern Daylight */
	{ "CST", 360 },            /* Central Standard */
	{ "CDT", 360 TMP_DAYZONE },/* Central Daylight */
	{ "MST", 420 },            /* Mountain Standard */
	{ "MDT", 420 TMP_DAYZONE },/* Mountain Daylight */
	{ "PST", 480 },            /* Pacific Standard */
	{ "PDT", 480 TMP_DAYZONE },/* Pacific Daylight */
	{ "YST", 540 },            /* Yukon Standard */
	{ "YDT", 540 TMP_DAYZONE },/* Yukon Daylight */
	{ "HST", 600 },            /* Hawaii Standard */
	{ "HDT", 600 TMP_DAYZONE },/* Hawaii Daylight */
	{ "CAT", 600 },            /* Central Alaska */
	{ "AHST", 600 },           /* Alaska-Hawaii Standard */
	{ "NT", 660 },             /* Nome */
	{ "IDLW", 720 },           /* International Date Line West */
	{ "CET", -60 },            /* Central European */
	{ "MET", -60 },            /* Middle European */
	{ "MEWT", -60 },           /* Middle European Winter */
	{ "MEST", -60 TMP_DAYZONE },  /* Middle European Summer */
	{ "CEST", -60 TMP_DAYZONE },  /* Central European Summer */
	{ "MESZ", -60 TMP_DAYZONE },  /* Middle European Summer */
	{ "FWT", -60 },               /* French Winter */
	{ "FST", -60 TMP_DAYZONE },   /* French Summer */
	{ "EET", -120 },              /* Eastern Europe, USSR Zone 1 */
	{ "WAST", -420 },             /* West Australian Standard */
	{ "WADT", -420 TMP_DAYZONE }, /* West Australian Daylight */
	{ "CCT", -480 },              /* China Coast, USSR Zone 7 */
	{ "JST", -540 },              /* Japan Standard, USSR Zone 8 */
	{ "EAST", -600 },             /* Eastern Australian Standard */
	{ "EADT", -600 TMP_DAYZONE }, /* Eastern Australian Daylight */
	{ "GST", -600 },              /* Guam Standard, USSR Zone 9 */
	{ "NZT", -720 },              /* New Zealand */
	{ "NZST", -720 },             /* New Zealand Standard */
	{ "NZDT", -720 TMP_DAYZONE }, /* New Zealand Daylight */
	{ "IDLE", -720 },             /* International Date Line East */
	/* Next up: Military timezone names. RFC822 allowed these, but (as noted in
	RFC 1123) had their signs wrong. Here we use the correct signs to match
	actual military usage.
	*/
	{ "A", +1 * 60 },         /* Alpha */
	{ "B", +2 * 60 },         /* Bravo */
	{ "C", +3 * 60 },         /* Charlie */
	{ "D", +4 * 60 },         /* Delta */
	{ "E", +5 * 60 },         /* Echo */
	{ "F", +6 * 60 },         /* Foxtrot */
	{ "G", +7 * 60 },         /* Golf */
	{ "H", +8 * 60 },         /* Hotel */
	{ "I", +9 * 60 },         /* India */
	/* "J", Juliet is not used as a timezone, to indicate the observer's local
	time */
	{ "K", +10 * 60 },        /* Kilo */
	{ "L", +11 * 60 },        /* Lima */
	{ "M", +12 * 60 },        /* Mike */
	{ "N", -1 * 60 },         /* November */
	{ "O", -2 * 60 },         /* Oscar */
	{ "P", -3 * 60 },         /* Papa */
	{ "Q", -4 * 60 },         /* Quebec */
	{ "R", -5 * 60 },         /* Romeo */
	{ "S", -6 * 60 },         /* Sierra */
	{ "T", -7 * 60 },         /* Tango */
	{ "U", -8 * 60 },         /* Uniform */
	{ "V", -9 * 60 },         /* Victor */
	{ "W", -10 * 60 },        /* Whiskey */
	{ "X", -11 * 60 },        /* X-ray */
	{ "Y", -12 * 60 },        /* Yankee */
	{ "Z", 0 },               /* Zulu, zero meridian, a.k.a. UTC */
};

typedef enum {
	DATE_MDAY,
	DATE_YEAR,
	DATE_TIME
} assume;

// this is a clone of 'struct tm' but with all fields we don't need or use cut out.
typedef struct {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
} short_tm;

inline int checkday(const char *check, size_t len)
{
	int i;
	const char * const *what;
	bool found = false;

	if (len > 3)
		what = &weekday[0];
	else
		what = &wkday[0];
	for (i = 0; i < 7; i++)
	{
		if (strcasecmp(check, what[0]) == 0)
		{
			found = true;
			break;
		}
		what++;
	}
	return found ? i : -1;
}

inline int checkmonth(const char *check)
{
	int i;
	const char * const *what;
	bool found = false;

	what = &month[0];
	for (i = 0; i < 12; i++)
	{
		if (strcasecmp(check, what[0]) == 0)
		{
			found = true;
			break;
		}
		what++;
	}

	// return the offset or -1, no real offset is -1.
	return found ? i : -1;
}

// return the time zone offset between GMT and the input one, in number
// of seconds or -1 if the timezone wasn't found/legal
inline int checktz(const char *check)
{
	unsigned int i;
	const tzinfo *what;
	bool found = false;

	what = tz;
	for (i = 0; i < sizeof(tz) / sizeof(tz[0]); i++)
	{
		if (strcasecmp(check, what->name) == 0)
		{
			found = true;
			break;
		}
		what++;
	}

	return found ? what->offset * 60 : -1;
}

inline int sltosi(long slnum)
{
	const long curl_mask_sint = 0x7FFFFFFF;
	return (int)(slnum & (long)curl_mask_sint);
}

inline void skip(const char** date)
{
	// skip everything that aren't letters or digits.
	while (**date && !std::isalnum(**date))
		(*date)++;
}

inline time_t short_tm_time_t(short_tm *tm)
{
	int month, year, leap_days;
	static const int month_days_cumulative[12] = {
		0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
	};

	// we don't support years before 1970 as they will cause this function
	// to return a negative value.
	if (tm->tm_year < 70)
		return -1;

	year = tm->tm_year + 1900;
	month = tm->tm_mon;
	if (month < 0)
	{
		year += (11 - month) / 12;
		month = 11 - (11 - month) % 12;
	}
	else if (month >= 12)
	{
		year -= month / 12;
		month = month % 12;
	}

	leap_days = year - (tm->tm_mon <= 1);
	leap_days = ((leap_days / 4) - (leap_days / 100) + (leap_days / 400)
		- (1969 / 4) + (1969 / 100) - (1969 / 400));

	return ((((time_t)(year - 1970) * 365
		+ leap_days + month_days_cumulative[month] + tm->tm_mday - 1) * 24
		+ tm->tm_hour) * 60 + tm->tm_min) * 60 + tm->tm_sec;
}

inline bool parse(const char* date, time_t* output)
{
	time_t t = 0;
	// day of the week number, 0-6 (mon-sun)
	int wdaynum = -1;
	// month of the year number, 0-11
	int monnum = -1;
	// day of month, 1 - 31
	int mdaynum = -1;
	int hournum = -1;
	int minnum = -1;
	int secnum = -1;
	int yearnum = -1;
	int tzoff = -1;
	short_tm tm;
	assume dignext = DATE_MDAY;
	// save the original pointer
	const char *indate = date;
	// max 6 parts
	int part = 0;

	while (*date && (part < 6))
	{
		bool found = false;

		skip(&date);

		if (std::isalpha(*date))
		{
			// a name coming up
			char buf[32] = "";
			size_t len;

			sscanf(date, "%31[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]", buf);
			len = strlen(buf);

			if (wdaynum == -1)
			{
				wdaynum = checkday(buf, len);
				if (wdaynum != -1)
					found = true;
			}

			if (!found && (monnum == -1))
			{
				monnum = checkmonth(buf);
				if (monnum != -1)
					found = true;
			}

			if (!found && (tzoff == -1))
			{
				// this just must be a time zone string.
				tzoff = checktz(buf);
				if (tzoff != -1)
					found = true;
			}

			// bad string.
			if (!found)
				return false;

			date += len;
		}
		else if (std::isdigit(*date))
		{
			// a digit.
			int val;
			char *end;
			// time stamp!
			if ((secnum == -1) && (3 == sscanf(date, "%02d:%02d:%02d", &hournum, &minnum, &secnum)))
				date += 8;
			else if ((secnum == -1) && (2 == sscanf(date, "%02d:%02d", &hournum, &minnum)))
			{
				// time stamp without seconds.
				date += 5;
				secnum = 0;
			}
			else
			{
				val = sltosi(strtol(date, &end, 10));

				if ((tzoff == -1) &&
					((end - date) == 4) &&
					(val <= 1400) &&
					(indate < date) &&
					((date[-1] == '+' || date[-1] == '-')))
				{
					// four digits and a value less than or equal to 1400 (to take into
					// account all sorts of funny time zone diffs) and it is preceded
					// with a plus or minus. This is a time zone indication.  1400 is
					// picked since +1300 is frequently used and +1400 is mentioned as
					// an edge number in the document "ISO C 200X Proposal: Timezone
					// Functions" at http://david.tribble.com/text/c0xtimezone.html If
					// anyone has a more authoritative source for the exact maximum time
					// zone offsets, please speak up!
					found = true;
					tzoff = (val / 100 * 60 + val % 100) * 60;

					// the + and - prefix indicates the local time compared to GMT
					// this we need ther reversed math to get what we want.
					tzoff = date[-1] == '+' ? -tzoff : tzoff;
				}

				if (((end - date) == 8) &&
					(yearnum == -1) &&
					(monnum == -1) &&
					(mdaynum == -1))
				{
					// 8 digits, no year, month or day yet. This is YYYYMMDD
					found = true;
					yearnum = val / 10000;
					// month is 0 - 11.
					monnum = (val % 10000) / 100 - 1;
					mdaynum = val % 100;
				}

				if (!found && (dignext == DATE_MDAY) && (mdaynum == -1))
				{
					if ((val > 0) && (val < 32))
					{
						mdaynum = val;
						found = true;
					}
					dignext = DATE_YEAR;
				}

				if (!found && (dignext == DATE_YEAR) && (yearnum == -1))
				{
					yearnum = val;
					found = true;
					if (yearnum < 1900)
					{
						if (yearnum > 70)
							yearnum += 1900;
						else
							yearnum += 2000;
					}
					if (mdaynum == -1)
						dignext = DATE_MDAY;
				}

				if (!found)
					return false;

				date = end;
			}
		}

		part++;
	}

	if (-1 == secnum)
		secnum = minnum = hournum = 0; // no time, make it zero.

	// lacks vital info, fail.
	if ((-1 == mdaynum) || (-1 == monnum) || (-1 == yearnum))
		return false;

#if SIZEOF_TIME_T < 5
	// 32 bit time_t can only hold dates to the beginning of 2038
	if (yearnum > 2037)
	{
		*output = 0x7fffffff;
		return false;
	}
#endif

	if (yearnum < 1970)
	{
		*output = 0;
		return false;
	}

	tm.tm_sec = secnum;
	tm.tm_min = minnum;
	tm.tm_hour = hournum;
	tm.tm_mday = mdaynum;
	tm.tm_mon = monnum;
	tm.tm_year = yearnum - 1900;

	// short_tm_time_t() returns a time_t. time_t is often 32 bits, even on many
	// architectures that feature 64 bit 'long'.
	// Some systems have 64 bit time_t and deal with years beyond 2038. However,
	// even on some of the systems with 64 bit time_t mktime() returns -1 for
	// dates beyond 03:14:07 UTC, January 19, 2038. (Such as AIX 5100-06)

	t = short_tm_time_t(&tm);

	// time zone adjust (cast t to int to compare to negative one)
	if (-1 != (int)t)
	{
		// Add the time zone diff between local time zone and GMT.
		long delta = (long)(tzoff != -1 ? tzoff : 0);
		// time_t overflow
		if ((delta > 0) && (t + delta < t))
			return false;

		t += delta;
	}

	*output = t;

	return true;
}

class gmt_time_input_face : public boost::posix_time::time_input_facet
{
	template <typename charT>
	inline static std::vector<std::basic_string<charT> >
	gather_month_strings(const std::locale& locale, bool short_strings=true)
	{
		typedef std::basic_string<charT> string_type;
		typedef std::vector<string_type> collection_type;
		typedef std::basic_ostringstream<charT> ostream_type;
		typedef std::ostreambuf_iterator<charT> ostream_iter_type;
		typedef std::basic_ostringstream<charT> stringstream_type;
		typedef std::time_put<charT>           time_put_facet_type;
		charT short_fmt[3] = { '%', 'b' };
		charT long_fmt[3]  = { '%', 'B' };
		collection_type months;
		string_type outfmt(short_fmt);
		if (!short_strings) {
			outfmt = long_fmt;
		}
		{
			//grab the needed strings by using the locale to
			//output each month
			const charT* p_outfmt = outfmt.c_str(), *p_outfmt_end = p_outfmt + outfmt.size();
			tm tm_value = { 0 };
			for (int m = 0; m < 12; m++) {
				tm_value.tm_mon = m;
				stringstream_type ss;
				ss.imbue(locale);
				ostream_iter_type oitr(ss);
				std::use_facet<time_put_facet_type>(locale).put(oitr, ss, ss.fill(),
					&tm_value,
					p_outfmt,
					p_outfmt_end);
				months.push_back(ss.str());
			}
		}
		return months;
	}

	template <typename charT>
	inline static std::vector<std::basic_string<charT> >
	gather_weekday_strings(const std::locale& locale, bool short_strings=true)
	{
		typedef std::basic_string<charT> string_type;
		typedef std::vector<string_type> collection_type;
		typedef std::basic_ostringstream<charT> ostream_type;
		typedef std::ostreambuf_iterator<charT> ostream_iter_type;
		typedef std::basic_ostringstream<charT> stringstream_type;
		typedef std::time_put<charT>           time_put_facet_type;
		charT short_fmt[3] = { '%', 'a' };
		charT long_fmt[3]  = { '%', 'A' };
		collection_type weekdays;
		string_type outfmt(short_fmt);
		if (!short_strings) {
			outfmt = long_fmt;
		}
		{
			//grab the needed strings by using the locale to
			//output each month / weekday
			const charT* p_outfmt = outfmt.c_str(), *p_outfmt_end = p_outfmt + outfmt.size();
			tm tm_value = { 0 };
			for (int i = 0; i < 7; i++) {
				tm_value.tm_wday = i;
				stringstream_type ss;
				ss.imbue(locale);
				ostream_iter_type oitr(ss);
				std::use_facet<time_put_facet_type>(locale).put(oitr, ss, ss.fill(),
					&tm_value,
					p_outfmt,
					p_outfmt_end);
				weekdays.push_back(ss.str());
			}
		}
		return weekdays;
	}

	class gmt_format_date_parser : public format_date_parser_type
	{
	public:
		gmt_format_date_parser(std::string fmt)
			: format_date_parser_type(fmt, std::locale::classic())
		{
			short_month_names(gmt_time_input_face::gather_month_strings<char>(std::locale::classic()));
			long_month_names(gmt_time_input_face::gather_month_strings<char>(std::locale::classic(), false));
			short_weekday_names(gmt_time_input_face::gather_weekday_strings<char>(std::locale::classic()));
			long_weekday_names(gmt_time_input_face::gather_weekday_strings<char>(std::locale::classic(), false));
		}
	};

public:
	gmt_time_input_face(std::string fmt)
		: boost::posix_time::time_input_facet(fmt)
	{
		m_parser = gmt_format_date_parser(fmt);
	}
};

} // namespace

// 解析http-date字符串.
// 注: 根据RFC2616规定HTTP-date 格式是 rfc1123-date | rfc850-date | asctime-date之一.
// 参数s 指定了需要解析的字符串.
// 参数t 返回解析出的time.
// 返回true表示解析成功, 返回false表示解析失败.
inline bool parse_http_date(const std::string& s, boost::posix_time::ptime& t)
{
	time_t st = 0;
	if (!parse(s.c_str(), &st))
		return false;
	std::tm* gmt = std::gmtime((const time_t*)&st);
	t = boost::posix_time::ptime_from_tm(*gmt);
	if (t != boost::posix_time::not_a_date_time)
		return true;
	return false;
}

// 解析http-date字符串.
// 注: 根据RFC2616规定HTTP-date 格式是 rfc1123-date | rfc850-date | asctime-date之一.
// 参数s 指定了需要解析的字符串.
// 参数t 返回解析出的UTC time.
// 返回true表示解析成功, 返回false表示解析失败.
inline bool parse_http_date(const std::string& s, time_t& t)
{
	boost::posix_time::ptime pt;
	if (!parse_http_date(s, pt))
		return false;
	t = ptime_to_time_t(pt);
	return true;
}

} // namespace detail
} // namespace avhttp

#endif // AVHTTP_PARSERS_HPP
