
#include "port/config.h"
#include "SlkReader.hpp"
#include "Sequence.hpp"
#include "TextReader.hpp"

namespace slk
{
	namespace
	{
#pragma pack(push, 1)
		struct SLK_HEADER
		{
			char szMagic[11]; //"ID;PWXL;N;E"

			bool IsValid()
			{
				return 0 == memcmp("ID;PWXL;N;E", szMagic, sizeof(szMagic)/sizeof(szMagic[0]));
			}
		};
#pragma pack(pop)


		class SlkDataArray : public std::vector<std::vector<std::string>>
		{
		public:
			typedef std::vector<std::vector<std::string>> _Mybase;

			SlkDataArray(buffer& buf)
				: max_x_(0)
				, max_y_(0)
				, cur_x_(0)
				, cur_y_(0)
			{
				read(buf);
			}

			void convert(SlkTable& table)
			{
				for (size_t y = 1; y < max_y_; ++y)
				{
					if (id_[y].vaild())
					{
						SlkSingle& obj = table[id_[y]];
						for (size_t x = 1; x < max_x_; ++x)
						{
							if ((!(*this)[x][y].empty()) && (!tag_[x].empty()))
							{
								obj[tag_[x]] = (*this)[x][y];
							}
						};
					}
				}
			}

		private:
			void assign(size_t max_x, size_t max_y)
			{
				max_x_ = max_x;
				max_y_ = max_y;
				tag_.resize(max_x_);
				id_.resize(max_y_);
				_Mybase::assign(max_x_, std::vector<std::string>(max_y_));
			}

			void set_x(size_t x)
			{
				cur_x_ = x;
			}

			void set_y(size_t y)
			{
				cur_y_ = y;
			}

			void set_k(std::string&& val)
			{
				if (cur_y_ == 1)
				{
					if (val.size() >= 2 && val[0] == '"' && val[val.size()-1] == '"')
					{
						tag_[cur_x_-1].assign(val, 1, val.size()-2);
					}
					else
					{
						tag_[cur_x_-1].assign(val);
					}
				}
				else if (cur_x_ == 1)
				{
					if (val.size() >= 2 && val[0] == '"' && val[val.size()-1] == '"')
					{
						id_[cur_y_-1] = ObjectId(std::string(val, 1, val.size()-2));
					}
					else
					{
						id_[cur_y_-1] = ObjectId(val);
					}
				}
				else
				{
					(*this)[cur_x_-1][cur_y_-1] = std::move(val);
				}
			}

			template <typename SequenceT, typename ResultT>
			void split_callback(SequenceT const& Input, ResultT Callback)
			{
				typedef typename SequenceT::const_iterator ForwardIteratorT;
				ForwardIteratorT InBegin = Input.begin();
				ForwardIteratorT InEnd = Input.end();
				ForwardIteratorT InCur = Input.begin();
				int stat = 0;

				for (ForwardIteratorT It = InBegin; It != InEnd; )
				{
					for (; It != InEnd; ++It)
					{
						if (stat == 0)
						{
							if (*It == '"')
							{
								stat = 1;
							}
							else if (*It == ';')
							{
								Callback(InCur, It);
								break;
							}
						}
						else if (stat == 1)
						{
							if (*It == '"')
							{
								stat = 0;
							}
							else if (*It == '\\')
							{
								stat = 2;
							}
						}
						else if (stat == 2)
						{
							stat = 1;
						}
					}

					if (It != InEnd)
					{
						InCur = ++It;
					}
				}
				if (InCur != InEnd)
				{
					Callback(InCur, InEnd);
				}
			}

			void read_line_b(std::string& line)
			{
				size_t x = 0, y = 0;

				split_callback(line,
					[&](std::string::const_iterator const& beg, std::string::const_iterator const& end)
				{
					switch (*beg)
					{
					case 'X':
						{
							x = Str2UInt(trim_copy<std::string>(beg+1, end, ctype::is_space()));
						}
						break;
					case 'Y':
						{
							y = Str2UInt(trim_copy<std::string>(beg+1, end, ctype::is_space()));
						}
						break;
					default:
						break;
					}
				});

				if ((x == 0) || (y == 0))
				{
					throw ydwe::exception(L"slk data corrupted.") ;
				}

				this->assign(x, y);
			}

			void read_line_c(std::string& line)
			{
				split_callback(line,
					[&](std::string::const_iterator const& beg, std::string::const_iterator const& end)
				{
					switch (*beg)
					{
					case 'X':
						{
							this->set_x(Str2UInt(trim_copy<std::string>(beg+1, end, ctype::is_space())));
						}
						break;
					case 'Y':
						{
							this->set_y(Str2UInt(trim_copy<std::string>(beg+1, end, ctype::is_space())));
						}
						break;
					case 'K':
						{
							this->set_k(trim_copy<std::string>(beg+1, end, ctype::is_space()));
						}
						break;
					default:
						break;
					}
				});
			}

			void read_line_f(std::string& line)
			{
				split_callback(line,
					[&](std::string::const_iterator const& beg, std::string::const_iterator const& end)
				{
					switch (*beg)
					{
					case 'X':
						{
							this->set_x(Str2UInt(trim_copy<std::string>(beg+1, end, ctype::is_space())));
						}
						break;
					case 'Y':
						{
							this->set_y(Str2UInt(trim_copy<std::string>(beg+1, end, ctype::is_space())));
						}
						break;
					default:
						break;
					}
				});
			}

			uint8_t read_type(std::string& line)
			{
				auto It = find_begin(line, char_equal(';'));

				if (It == line.end())
				{
					return 0;
				}

				std::string key = trim_copy<std::string>(line.begin(), It, ctype::is_space());

				line.erase(line.begin(), ++It);
				return key.front();
			}

			void read(buffer& buf)
			{
				bool is_found_b = false;
				TextReader::EachLine<std::string>(buf, [&](std::string& line)
				{
					uint8_t type = read_type(line);
					if (!is_found_b)
					{
						if (type == 'B')
						{
							read_line_b(line);
							is_found_b = true;
						}
					}
					else
					{
						if (type == 'C')
						{
							read_line_c(line);
						}
						else if (type == 'F')
						{
							read_line_f(line);
						}
					}
				});
			}

		private:
			size_t max_x_;
			size_t max_y_;
			size_t cur_x_;
			size_t cur_y_;
			std::vector<ObjectId>    id_;
			std::vector<std::string> tag_;
		};

	}

	void SlkReader::Read(buffer&& buf, SlkTable& table)
	{
		if (!buf.read_ptr<SLK_HEADER>()->IsValid())
		{
			throw ydwe::exception(L"slk data corrupted.") ;
		}

		SlkDataArray(buf).convert(table);
	}
}
