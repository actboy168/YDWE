#pragma once

#include <fstream>
#include <map>
#include <base/warcraft3/jass/global_variable.h>

namespace handles {
	struct node
	{
		uint32_t handle = 0;
		uint32_t object = 0;
		uint32_t reference = 0;
		std::vector<std::string> global_reference;
		std::vector<std::string> local_reference;
		std::vector<std::string> hashtable_reference;
	};

	struct table
		: public std::map<uint32_t, node>
	{
		std::map<uint32_t, uint32_t> htmgr;

		enum e_type {
			local,
			global,
			hashtable,
		};

		void add_handle(uint32_t handle, uint32_t object, uint32_t reference);
		void add_global_reference(base::warcraft3::hashtable::variable_node& var);
		void add_local_reference(base::warcraft3::hashtable::variable_node& var, const std::string& funcname);
		void add_hashtable_reference(uint32_t ht, uint32_t t, uint32_t k, uint32_t handle);
		void add_reference(e_type type, uint32_t handle, const std::string& name);
	};

	void scanner(std::fstream& fs);
}

