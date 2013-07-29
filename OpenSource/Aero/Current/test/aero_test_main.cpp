#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>
#include <boost/unordered_map.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/typeof/typeof.hpp>

#include <aero/aero.hpp>

using namespace std;
using namespace boost;
using namespace aero;

BOOST_AUTO_TEST_CASE(AeroSetAlgorithmTestCase)
{
	typedef set<string> s_set;
	
	s_set s, s1, s2, s3, s4, s5, s6;
	s1.insert("a");
	s1.insert("b");
	s1.insert("c");
	s1.insert("a");
	s1.insert("b");
	s1.insert("d");
	
	s2.insert("a");
	s2.insert("d");
	s2.insert("e");
	s2.insert("f");
	s2.insert("g");
	s2.insert("h");
	
	s4.insert("a");
	s4.insert("b");
	s4.insert("c");
	s4.insert("d");
	s4.insert("e");
	s4.insert("f");
	s4.insert("g");
	s4.insert("h");
	
	s5.insert("a");
	s5.insert("d");
	
	naive_set_union_copy(s1, s2, s3);
	BOOST_CHECK(s3 == s4);
	s3.clear();
	naive_set_intersection_copy(s1, s2, s3);
	BOOST_CHECK(s3 == s5);
	s = s1;
	naive_set_intersection(s1, s2);
	BOOST_CHECK(s3 == s1);
	s1 = s;
}

BOOST_AUTO_TEST_CASE(AeroMapAlgorithmTestCase)
{
	typedef boost::unordered_map<string, string> ss_map;
	ss_map s, s1, s2, s3, s4, s5, s6;
	
	s1["a"] = "1";
	s1["b"] = "2";
	s1["c"] = "3";
	s1["d"] = "4";
	
	
	s2["a"] = "1";
	s2["d"] = "4";
	s2["e"] = "5";
	s2["f"] = "6";
	s2["g"] = "7";
	s2["h"] = "8";
	
	s4["a"] = "1";
	s4["b"] = "2";
	s4["c"] = "3";
	s4["d"] = "4";
	s4["e"] = "5";
	s4["f"] = "6";
	s4["g"] = "7";
	s4["h"] = "8";
	
	s5["a"] = "1";
	s5["d"] = "4";
	
	naive_map_union_copy(s1, s2, s3);
	BOOST_CHECK(s3 == s4);
	s3.clear();
	naive_map_intersection_copy(s1, s2, s3);
	BOOST_CHECK(s3 == s5);
	s = s1;
	naive_map_intersection(s1, s2);
	BOOST_CHECK(s3 == s1);
	s1 = s;
}

BOOST_AUTO_TEST_CASE(AeroFunctionPointerCallTestCase)
{
	HMODULE kernel32 = GetModuleHandle("kernel32.dll");
	FARPROC fp1 = GetProcAddress(kernel32, "AreFileApisANSI");
	
	BOOL result = std_call<BOOL>(make_fp(fp1));
	BOOST_CHECK(result);
	
	FARPROC fp2 = GetProcAddress(kernel32, "lstrlen");
	BOOST_CHECK(std_call<int>(make_fp(fp2), "abcdefg") == 7);
	
	FARPROC fp3 = GetProcAddress(kernel32, "lstrcmp");
	BOOST_CHECK(std_call<int>(make_fp(fp3), "abcdefg", "abcdefg") == 0);
}

BOOST_AUTO_TEST_CASE(AeroStreamAlgoTestCase)
{
	ifstream ifs("testdata.txt");
	
	BOOST_CHECK(stream_size(ifs) == 12);
	
	char buffer[32];
	flexable_getline(ifs, buffer, sizeof(buffer));
	BOOST_CHECK(strcmp(buffer, "hello") == 0);
	flexable_getline(ifs, buffer, sizeof(buffer));
	BOOST_CHECK(strcmp(buffer, "world") == 0);
	flexable_getline(ifs, buffer, sizeof(buffer));
	BOOST_CHECK(strcmp(buffer, "") == 0);
	BOOST_CHECK(!ifs);
}

BOOST_AUTO_TEST_CASE(AeroDataBitTestCase)
{
	boost::uint64_t n64 = 0x1234567878563412;
	BOOST_STATIC_ASSERT(sizeof(aero::number_split<2, 0>(n64)) == 4);
	BOOST_STATIC_ASSERT(sizeof(aero::number_split<4, 1>(n64)) == 2);

	boost::uint32_t nh32 = aero::number_split<2, 1>(n64);
	boost::uint32_t nl32 = aero::number_split<2, 0>(n64);
	BOOST_CHECK(nh32 == 0x12345678);
	BOOST_CHECK(nl32 == 0x78563412);
	BOOST_CHECK((aero::number_split<2, 0>(nh32) == 0x5678));
}

BOOST_AUTO_TEST_CASE(AeroIteratortTestCase)
{
	aero::repeat_iterator<int> b(1, 10, 0), e(1, 10);
	BOOST_CHECK_EQUAL(std::distance(b, e), 10);
	for (int i = 0; i < 10; ++i)
	{
		BOOST_CHECK_EQUAL(*b++, 1);
	}

	BOOST_CHECK(b == e);
}
