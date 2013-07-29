#pragma once

#include <ydwe/config.h>
#include <cstdint>
#include <ydwe/warcraft3/detail/string_hash.h>
#include <cstring>

namespace ydwe { namespace warcraft3 {

	namespace hashtable
	{
		struct node;

		struct node_2
		{
			node_2*     lft_;
			node*       rht_;
			const char* str_;
		};

		struct node_1
		{
			node_1*     next_;
			node*       prev_; 
			node_2*     lft_;
			node*       rht_;
			const char* str_;
		};

		struct node
		{
			uint32_t    hash_;
			node_1*     next_;
			node*       prev_; 
			node_2*     lft_;
			node*       rht_;
			const char* str_;

			bool is_vaild() const
			{
				return ((intptr_t)this > 0x10000);
			}
		};

		template <class Node = node>
		struct entry
		{
			uint32_t step_;
			node_1*  beg_;
			Node*    end_;

			node* convert(Node* ptr) const
			{
				return (node*)((uintptr_t)ptr + step_ - 4);
			}
		};

		template <class Node = node>
		struct table
		{
			uint32_t     unk0_;
			uint32_t     unk1_;
			uint32_t     unk2_;
			uint32_t     unk3_;
			uint32_t     unk4_;
			uint32_t     unk5_;
			uint32_t     unk6_;
			entry<Node>* entry_;
			uint32_t     unk8_;
			uint32_t     mask_;

			Node* get(const char* str)
			{
				uint32_t hash;
				Node* fnode_ptr = nullptr;

				if (this->mask_ == 0xFFFFFFFF) 
					return nullptr;

				hash = detail::string_hash(str);

				fnode_ptr = this->entry_[hash & this->mask_].end_;

				if (!fnode_ptr->is_vaild()) 
					return nullptr;

				for (;;)
				{
					if (fnode_ptr->hash_ == hash)
					{
						if (fnode_ptr->str_ == str) 
							return fnode_ptr;

						if (0 == strcmp(fnode_ptr->str_, str)) 
							return fnode_ptr;
					}
					fnode_ptr = (Node*)(uintptr_t)(fnode_ptr->prev_);

					if (!fnode_ptr->is_vaild()) 
						return nullptr;
				}
			}
		};

		struct variable_node : public node
		{
			uint32_t    unk_;
			uint32_t    type_;
			uint32_t    value_;
		};

		struct variable_table
		{
			table<variable_node> table_;

			variable_node* get(const char* str)
			{
				return table_.get(str);
			}
		};

		struct virtual_func_table 
		{
			uint32_t vfn_;
		};

		struct native_func_node : public virtual_func_table, public node
		{
			uint32_t    func_address_;
			uint32_t    param_num_;
			const char* param_list_;
		};

		struct native_func_table
		{
			table<native_func_node> table_;

			native_func_node* get(const char* str)
			{
				return table_.get(str);
			}
		};

		struct reverse_node : public node
		{
			uint32_t     index_;
		};

		struct reverse_table
		{
			uint32_t            unk0_;
			uint32_t            unk1_;
			reverse_node**      node_array_;
			uint32_t            unk3_;
			table<reverse_node> table_;

			reverse_node* get(uint32_t index)
			{
				return node_array_[index];
			}

			reverse_node* get(const char* str)
			{
				return table_.get(str);
			}
		};

		struct string_fasttable
		{
			uint32_t unk_;
			uint32_t max_size_;
			uint32_t array_;

			uint32_t get(uint32_t idx)
			{
				uint32_t result = 0;
				if (idx < max_size_)
				{
					result = array_ + 0x10 * idx;
					++*(uint32_t*)(result + 0xC);
				}
				return result;
			}
		};
	}

	YDWE_BASE_API uintptr_t                     get_jass_virtual_machine();
	YDWE_BASE_API hashtable::native_func_table* get_native_function_hashtable();
	YDWE_BASE_API hashtable::variable_table*    get_variable_hashtable();
	YDWE_BASE_API hashtable::reverse_table*     get_string_hashtable();
	YDWE_BASE_API uintptr_t                     get_code_table();
	YDWE_BASE_API hashtable::string_fasttable*  get_string_fasttable();
}}
