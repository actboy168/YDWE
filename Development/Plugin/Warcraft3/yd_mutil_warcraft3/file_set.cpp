#include "file_set.h"
#include <base/path/self.h>	 
#include <base/file/stream.h>

static boost::filesystem::path file_set_make_name(const std::string& name)
{
	return base::path::self().remove_filename() / (name + ".txt");
}

template <class T, class FuncT>
static void split(const T& Input, typename T::value_type Char, FuncT FoundFunc)
{
	typedef typename T::const_iterator ForwardIteratorT;
	ForwardIteratorT InBegin = Input.begin();
	ForwardIteratorT InEnd = Input.end();
	ForwardIteratorT InCur = Input.begin();
	for (ForwardIteratorT It = InBegin; It != InEnd;)
	{
		for (; It != InEnd; ++It)
		{
			if (Char == *It)
			{
				FoundFunc(T(InCur, It));
				break;
			}
		}

		if (It != InEnd)
		{
			InCur = ++It;
		}
	}
	if (InCur != InEnd)
	{
		FoundFunc(T(InCur, InEnd));
	}
}

static std::set<int> file_set_load(const boost::filesystem::path& path)
{
	std::set<int> s;
	try {
		std::string buf = base::file::read_stream(path).read<std::string>();
		split(buf, '\n', [&](const std::string& str){
			s.insert(std::stoi(str));
		});
	}
	catch (...) {
	}
	return std::move(s);
}

static void file_set_save(const boost::filesystem::path& path, const std::set<int>& s)
{
	try {
		std::string buf = "";
		for (auto it = s.begin(); it != s.end(); ++it)
		{
			buf += std::to_string(*it) + "\n";
		}
		base::file::write_stream(path).write<std::string>(buf);
	}
	catch (...) {
	}
}

file_set::file_set(const std::string& name)
	: name_(file_set_make_name(name))
	, mutex_(boost::interprocess::open_or_create, name.c_str())
{ }

std::set<int> file_set::get()
{
	boost::interprocess::scoped_lock<mutex_t> lock(mutex_);
	return file_set_load(name_);
}

void file_set::insert(int v)
{
	boost::interprocess::scoped_lock<mutex_t> lock(mutex_);
	std::set<int> s = file_set_load(name_);
	s.insert(v);
	file_set_save(name_, s);
}

void file_set::erase(int v)
{
	boost::interprocess::scoped_lock<mutex_t> lock(mutex_);
	std::set<int> s = file_set_load(name_);
	s.erase(v);
	file_set_save(name_, s);
}
