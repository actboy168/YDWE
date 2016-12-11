#pragma once

#include <base/config.h>
#include <base/util/foreach.h>
#include <functional>
#include <vector>

namespace base {
	template <class ResultT, class ParamT>
	class signal
	{
	public:
		std::vector<ResultT> operator ()(ParamT param)
		{
			std::vector<ResultT> retvel;
			foreach (auto it, funclist_)
			{
				retvel.push_back(it(param));
			}
			return std::move(retvel);
		}

		void connect(std::function<ResultT(ParamT)> func)
		{
			funclist_.push_back(func);
		}

	private:
		std::vector<std::function<ResultT(ParamT)>> funclist_;
	};

	template <class ResultT>
	class signal<ResultT, void>
	{
	public:
		std::vector<ResultT> operator ()()
		{
			std::vector<ResultT> retvel;
			foreach (auto it, funclist_)
			{
				retvel.push_back(it());
			}
			return std::move(retvel);
		}

		void connect(std::function<ResultT(void)> func)
		{
			funclist_.push_back(func);
		}

	private:
		std::vector<std::function<ResultT(void)>> funclist_;
	};

	template <class ParamT>
	class signal<void, ParamT>
	{
	public:
		void operator ()(ParamT param)
		{
			foreach (auto it, funclist_)
			{
				it(param);
			}
		}

		void connect(std::function<void(ParamT)> func)
		{
			funclist_.push_back(func);
		}

	private:
		std::vector<std::function<void(ParamT)>> funclist_;
	};

	template <>
	class signal<void, void>
	{
	public:
		void operator ()()
		{
			foreach (auto it, funclist_)
			{
				it();
			}
		}

		void connect(std::function<void(void)> func)
		{
			funclist_.push_back(func);
		}

	private:
		std::vector<std::function<void(void)>> funclist_;
	};
}
