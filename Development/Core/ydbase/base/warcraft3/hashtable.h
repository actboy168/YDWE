#pragma once

#include <base/config.h>
#include <cstdint>
#include <base/warcraft3/detail/string_hash.h>
#include <iterator>
#include <cstring>

namespace base { namespace warcraft3 {

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

			class iterator;

			Node* get(uint32_t hash)
			{
				Node* fnode_ptr = nullptr;

				if (this->mask_ == 0xFFFFFFFF)
					return nullptr;

				fnode_ptr = this->entry_[hash & this->mask_].end_;

				if (!fnode_ptr->is_vaild())
					return nullptr;

				for (;;)
				{
					if (fnode_ptr->hash_ == hash)
						return fnode_ptr;
					fnode_ptr = (Node*)(uintptr_t)(fnode_ptr->prev_);

					if (!fnode_ptr->is_vaild())
						return nullptr;
				}
			}

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

		template <class Node>
		class table<Node>::iterator
			: public std::iterator<std::input_iterator_tag, Node>
		{
		public:
			iterator()
				: ptr_(nullptr)
				, index_(0)
				, current_(nullptr)
			{ }

			explicit iterator(const table<Node>* ptr)
				: ptr_(ptr)
				, index_(0)
				, current_(nullptr)
			{
				operator++();
			}

			~iterator()
			{ }

			reference operator*() const
			{
				return *current_;
			}

			pointer operator->() const
			{
				return current_;
			}

			iterator operator++(int)
			{
				auto result = *this;
				++(*this);
				return result;
			}

			iterator& operator++()
			{
				if (!current_)
				{
					if (index_ > ptr_->mask_)
					{
						return *this;
					}

					current_ = ptr_->entry_[index_].end_;
				}
				else
				{
					current_ = (Node*)(uintptr_t)(current_->prev_);
				}

				if (!current_->is_vaild())
				{
					index_++;
					current_ = nullptr;
					return operator++();
				}
				return *this;
			}

			bool operator==(const iterator& other) const
			{
				return current_ == other.current_;
			}

			bool operator!=(const iterator& other) const
			{
				return !operator==(other);
			}

		private:
			const table<Node>* ptr_;
			uint32_t     index_;
			Node*        current_;
		};

		struct variable_node : public node
		{
			uint32_t    unk_;
			uint32_t    type_;
			uint32_t    value_;
		};

		struct variable_table
		{
			typedef table<variable_node> table_t;
			typedef table_t::iterator iterator;
			table_t table_;

			iterator begin() const
			{
				return iterator(&table_);
			}

			iterator end() const
			{
				return iterator();
			}

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
			typedef table<reverse_node> table_t;
			typedef table_t::iterator iterator;

			uint32_t            unk0_;
			uint32_t            size;
			reverse_node**      node_array_;
			uint32_t            unk3_;
			table_t             table_;

			iterator begin()
			{
				return iterator(&table_);
			}

			iterator end()
			{
				return iterator();
			}

			reverse_node* at(uint32_t index)
			{
				return node_array_[index];
			}

			reverse_node* get(uint32_t hash)
			{
				return table_.get(hash);
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

	struct symbol_table_t {
		uint32_t unk0;
		uint32_t unk1;
		hashtable::reverse_table* string_table;
	};

	struct code_table_t {
		uint32_t unk0;
		uint32_t unk1;
		uintptr_t table;
	};

	struct handle_table_t {
		char unk[0x194];
		hashtable::reverse_table table; // 0x194
	};

	struct jass_vm_t
	{
		char                         unk1[0x2858];
		symbol_table_t*              symbol_table;          //0x2858
		hashtable::variable_table*   global_table;          //0x285C
		char                         unk2[0x14];
		hashtable::string_fasttable* string_table;		    //0x2874
		char                         unk3[0x10];
		code_table_t*                code_table;		    //0x2888
		char                         unk4[0x14];
		uintptr_t                    set_handle_reference;	//0x28A0
		handle_table_t**             handle_table;	        //0x28A4
	};

	_BASE_API jass_vm_t*                    get_jass_vm();
	_BASE_API jass_vm_t*                    get_jass_vm_nofix();
	_BASE_API jass_vm_t*                    get_current_jass_vm();
	_BASE_API jass_vm_t*                    get_current_jass_vm_nofix();
	_BASE_API hashtable::native_func_table* get_native_function_hashtable();
	_BASE_API uintptr_t                     get_current_jass_pos();
}}
