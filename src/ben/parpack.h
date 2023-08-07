#pragma once

#include <assert.h>

#include <type_traits>

#include "type.h"

// x----------------------------------------------------------------------------------------------x
// |   - parpack.h contains utility functionality for recursively unpacking parameter packs       |
// x----------------------------------------------------------------------------------------------x
//
namespace btl
{
	template <class StartArgs, class... Args>
	class pack
	{
	private:
		template <class T, class NextArg, class... PackArgs>
		struct contains_rec
		{
			static constexpr bool const value =
				std::is_same<T, NextArg>::value ? true : contains_rec<T, PackArgs...>::value;
		};
		template <class T, class ArgFinal>
		struct contains_rec<T, ArgFinal>
		{
			static constexpr bool const value =
				std::is_same<T, ArgFinal>::value ? true : false;
		};

		template <u64 N, class T, class NextArg, class... PackArgs>
		struct index_rec
		{
			static constexpr u64 const value =
				std::is_same<T, NextArg>::value ? N : index_rec<N + 1, T, PackArgs...>::value;
		};
		template <u64 N, class T, class ArgFinal>
		struct index_rec<N, T, ArgFinal>
		{
			static constexpr u64 const value =
				std::is_same<T, ArgFinal>::value ? N : U64_MAX;
		};


		template <class T, class NextArg, class... PackArgs>
		struct is_set_rec
		{
			static constexpr bool const value =
				std::is_same<T, NextArg>::value ? false : is_set_rec<T, PackArgs...>::value;
		};
		template <class T, class ArgFinal>
		struct is_set_rec<T, ArgFinal>
		{
			static constexpr bool const value =
				std::is_same<T, ArgFinal>::value ? false : true;
		};
	public:
		struct alias
		{
			template <class T>
			using contains = contains_rec<T, StartArgs, Args...>;

			template <class T>
			using index = index_rec<0, T, StartArgs, Args...>;

			using is_set = is_set_rec<StartArgs, Args...>;
		};

		template <class T>
		static constexpr bool contains = alias::contains<T>::value;

		template <class T>
		static constexpr u64 index = alias::index<T>::value;

		static constexpr bool is_set = alias::is_set::value;

		static constexpr u64 count = 1 + sizeof...(Args);
	};
}