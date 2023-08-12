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

		template <class T, class NextArg, class... PackArgs>
		struct contains_rec_u64
		{
			static constexpr u64 const value =
				std::is_same<T, NextArg>::value ? 1 : contains_rec_u64<T, PackArgs...>::value;
		};
		template <class T, class ArgFinal>
		struct contains_rec_u64<T, ArgFinal>
		{
			static constexpr u64 const value =
				std::is_same<T, ArgFinal>::value ? 1 : 0;
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


		template <class NextArg, class... PackArgs>
		struct is_set_rec
		{
			static constexpr bool const value =
				contains_rec<NextArg, PackArgs...>::value ? false : is_set_rec<PackArgs...>::value;
		};
		template <class ArgFinal>
		struct is_set_rec<ArgFinal>
		{
			static constexpr bool const value = true;
		};

		template <class NextArg, class... PackArgs>
		struct triviality_rec
		{
			static constexpr u64 const value =
				std::is_trivially_copyable<NextArg>::value + triviality_rec<PackArgs...>::value;
		};
		template <class ArgFinal>
		struct triviality_rec<ArgFinal>
		{
			static constexpr u64 const value =
				std::is_trivially_copyable<ArgFinal>::value ? 1 : 0;
		};

		template <class NextArg, class... PackArgs>
		struct pointedness_rec
		{
			static constexpr u64 const value =
				std::is_pointer<NextArg>::value + pointedness_rec<PackArgs...>::value;
		};
		template <class ArgFinal>
		struct pointedness_rec<ArgFinal>
		{
			static constexpr u64 const value =
				std::is_pointer<ArgFinal>::value ? 1 : 0;
		};

		template <class NextArg, class... PackArgs>
		struct containment_rec
		{
			static constexpr u64 const value =
				contains_rec_u64<NextArg, PackArgs...>::value + containment_rec<PackArgs...>::value;
		};
		template <class ArgFinal>
		struct containment_rec<ArgFinal>
		{
			static constexpr u64 const value = 1;
		};

		template <class NextArg, class... PackArgs>
		struct reflexiveness_rec
		{
			static constexpr u64 const value =
				std::is_same<NextArg, NextArg>::value + reflexiveness_rec<PackArgs...>::value;
		};
		template <class ArgFinal>
		struct reflexiveness_rec<ArgFinal>
		{
			static constexpr u64 const value =
				std::is_same<ArgFinal, ArgFinal>::value;
		};
	public:
		struct alias
		{
			template <class T>
			using contains = contains_rec<T, StartArgs, Args...>;

			template <class T>
			using index = index_rec<0, T, StartArgs, Args...>;

			using is_set = is_set_rec<StartArgs, Args...>;

			using triviality = triviality_rec<StartArgs, Args...>;

			using pointedness = pointedness_rec<StartArgs, Args...>;

			using containment = containment_rec<StartArgs, Args...>;

			using reflexiveness = reflexiveness_rec<StartArgs, Args...>;
		};

		template <class T>
		static constexpr bool contains = alias::contains<T>::value;

		template <class T>
		static constexpr u64 index = alias::index<T>::value;

		static constexpr bool is_set = alias::is_set::value;

		static constexpr u64 count = 1 + sizeof...(Args);

		static constexpr u64 triviality = alias::triviality::value;

		static constexpr u64 pointedness = alias::pointedness::value;

		static constexpr u64 containment = alias::containment::value;

		static constexpr u64 reflexiveness = alias::reflexiveness::value;
	};
}