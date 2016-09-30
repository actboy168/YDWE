#pragma once

#include <set>
#include <string>		
#include <boost/interprocess/sync/windows/named_mutex.hpp> 
#include <boost/filesystem.hpp>

struct file_set
{
	typedef boost::interprocess::ipcdetail::windows_named_mutex mutex_t;
	file_set(const std::string& name);
	std::set<int> get();
	void          insert(int v);
	void          erase(int v);

private:
	boost::filesystem::path name_;
	mutex_t                 mutex_;
};
