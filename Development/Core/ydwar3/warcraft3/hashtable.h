#pragma once

#include <warcraft3/config.h>
#include <cstdint>
#include <warcraft3/detail/string_hash.h>
#include <cstring>

namespace warcraft3 {
	namespace jass {
		struct opcode;
	}
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
			uint32_t    key;

			bool is_vaild() const
			{
				return ((intptr_t)this > 0x10000);
			}
		};

		template <class Node = node>
		struct entry
		{
			uint32_t step;
			node_1*  tail;
			Node*    head;

			node* convert(Node* ptr) const
			{
				return (node*)((uintptr_t)ptr + step - 4);
			}
		};

		template <class Node = node>
		struct table
		{
			uint32_t     unk0;
			uint32_t     step;
			uint32_t     tail;
			Node*        head;
			uint32_t     unk4;
			uint32_t     unk5;
			uint32_t     unk6;
			entry<Node>* buckets;
			uint32_t     unk8;
			uint32_t     mask;

			class iterator;
			class iterator_v1;

			Node* find(uint32_t hash)
			{
				Node* fnode_ptr = nullptr;

				if (mask == 0xFFFFFFFF)
					return nullptr;

				fnode_ptr = buckets[hash & mask].head;

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

			Node* find(const char* str)
			{
				uint32_t hash;
				Node* fnode_ptr = nullptr;

				if (mask == 0xFFFFFFFF) 
					return nullptr;

				hash = detail::string_hash(str);

				fnode_ptr = buckets[hash & mask].head;

				if (!fnode_ptr->is_vaild()) 
					return nullptr;

				for (;;)
				{
					if (fnode_ptr->hash_ == hash)
					{
						if ((const char*)fnode_ptr->key == str)
							return fnode_ptr;

						if (0 == strcmp((const char*)fnode_ptr->key, str)) 
							return fnode_ptr;
					}
					fnode_ptr = (Node*)(uintptr_t)(fnode_ptr->prev_);

					if (!fnode_ptr->is_vaild()) 
						return nullptr;
				}
			}


			iterator begin() const
			{
				return iterator(this);
			}

			iterator end() const
			{
				return iterator();
			}
		};

		template <class Node>
		class table<Node>::iterator
		{
		public:
			typedef Node value_type;
			typedef value_type& reference;
			typedef value_type* pointer;

		public:
			iterator()
				: ptr_(nullptr)
				, current_(nullptr)
			{ }

			explicit iterator(const table<Node>* ptr)
				: ptr_(ptr)
				, current_(ptr->head)
			{
				if (!current_->is_vaild()) {
					current_ = nullptr;
				}
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
				current_ = (Node*)((uint32_t*)current_)[ptr_->step / 4 + 1];
				if (!current_->is_vaild()) {
					current_ = nullptr;
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
			Node*              current_;
		};

		template <class Node>
		class table<Node>::iterator_v1
		{
		public:
			typedef Node value_type;
			typedef value_type& reference;
			typedef value_type* pointer;

		public:
			iterator_v1()
				: ptr_(nullptr)
				, index_(0)
				, current_(nullptr)
			{ }

			explicit iterator_v1(const table<Node>* ptr)
				: ptr_(ptr)
				, index_(0)
				, current_(nullptr)
			{
				if (!ptr_->buckets)
				{
					ptr_ = nullptr;
					return;
				}
				operator++();
			}

			~iterator_v1()
			{ }

			reference operator*() const
			{
				return *current_;
			}

			pointer operator->() const
			{
				return current_;
			}

			iterator_v1 operator++(int)
			{
				auto result = *this;
				++(*this);
				return result;
			}

			iterator_v1& operator++()
			{
				if (!current_)
				{
					if (index_ > ptr_->mask)
					{
						return *this;
					}

					current_ = ptr_->buckets[index_].head;
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

			bool operator==(const iterator_v1& other) const
			{
				return current_ == other.current_;
			}

			bool operator!=(const iterator_v1& other) const
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
			uint32_t    array_;
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

			variable_node* find(const char* str)
			{
				return table_.find(str);
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

			native_func_node* find(const char* str)
			{
				return table_.find(str);
			}
		};

		struct reverse_node : public node
		{
			uint32_t     value;
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

			reverse_node* find(uint32_t hash)
			{
				return table_.find(hash);
			}

			reverse_node* find(const char* str)
			{
				return table_.find(str);
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

	struct stackframe_unk_t {
		uint32_t unk0;
		uint32_t unk1;
		uint32_t unk2;
		uint32_t unk3;
		uint32_t unk4;
		uint32_t unk5;
		uint32_t unk6;
		uint32_t unk7;
		uint32_t code;
	};

	struct stackframe_t {
		uint32_t                   unk0;
		stackframe_t*              next;
		stackframe_unk_t*          codes[0x21];
		uint32_t                   index;
		hashtable::variable_table  local_table;
	};

	struct jass_vm_t
	{
		char                         unk0[0x20];
		jass::opcode*                opcode;                // 0x20
		char                         unk1[0x10];
		uint32_t                     has_sleep;             // 0x34
		char                         unk2[0x2818];
		uint32_t                     index;                 //0x2850
		uint32_t                     unk3;
		symbol_table_t*              symbol_table;          //0x2858
		hashtable::variable_table*   global_table;          //0x285C
		uint32_t                     unk4;
		uint32_t                     unk5;
		stackframe_t*                stackframe;            //0x2868
		uint32_t                     unk6;
		uint32_t                     unk7;
		hashtable::string_fasttable* string_table;		    //0x2874
		char                         unk8[0x10];
		code_table_t*                code_table;		    //0x2888
		char                         unk9[0x14];
		uintptr_t                    set_handle_reference;	//0x28A0
		handle_table_t**             handle_table;	        //0x28A4
	};

	_WAR3_API jass_vm_t*                    get_jass_vm(int index = 1);
	_WAR3_API jass_vm_t*                    get_jass_thread();
	_WAR3_API hashtable::native_func_table* get_native_function_hashtable();
}
