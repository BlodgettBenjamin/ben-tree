#pragma once

#include <type_traits>

#include "type.h"

namespace btl
{
	class pack
	{
	private:
		template <u64 N, class T, class NextArg, class... PackArgs>
		struct index_rec
		{
			static constexpr u64 const value =
				std::is_same<T, NextArg>::value ? N : index_rec<N + 1, T, PackArgs...>::value;
		};

		template <u64 N, class T, class ArgFinal>
		struct index_rec<N, T, ArgFinal>
		{
			static constexpr u64 const value = N;
		};
	public:
		template <class T, class... Args>
		using index = index_rec<0, T, Args...>;
	};
}