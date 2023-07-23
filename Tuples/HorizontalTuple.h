// github.com/broly/CppFun
// This is minimal as possible horizontal tuple implementation
#include <iostream>

template<typename... Ts>
struct htuple;

namespace detail
{
	// Element holder type
	template<typename ElemType, size_t ElemIndex>
	struct htuple_elem
	{
		constexpr htuple_elem(ElemType InValue)
			: Value(InValue)
		{}
		ElemType Value;
		static constexpr size_t Index = ElemIndex;
	};

	// tuple implementation
	template<typename IndexSequence, typename... Ts>
	struct htuple_impl;

	// type selector helpers from type list
	namespace helpers
	{
		template<size_t Index, typename... Ts>
		struct get_type;

		template<typename T, typename... Ts>
		struct get_type<0, T, Ts...>
		{
			using type = T;
		};

		template<size_t Index, typename T, typename... Ts>
		struct get_type<Index, T, Ts...>
		{
			using type = typename get_type<Index - 1, Ts...>::type;
		};

		template<size_t Index, typename... Ts>
		using get_type_t = typename get_type<Index, Ts...>::type;
	}
	
	// horizontal tuple implementation
	template<typename... Ts, size_t... Indices>
	struct htuple_impl<std::index_sequence<Indices...>, Ts...> : htuple_elem<Ts, Indices>...
	{
		constexpr htuple_impl(const Ts&... InValues)
			: htuple_elem<Ts, Indices>(InValues)...
		{}
		
		static constexpr size_t size = sizeof...(Ts);

		template<size_t Index>
		auto Get()
		{
			using type = helpers::get_type_t<size - Index - 1, Ts...>;
			auto* as_elem = static_cast<htuple_elem<type, size - Index - 1>*>(this);
			return as_elem->Value;
		}

		template<size_t Index = 0>
		auto GetLast()
		{
			return Get<size - Index - 1>();
		}
	};
}

// horizontal tuple type
template<typename... Ts>
struct htuple : detail::htuple_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...>
{
	constexpr htuple(Ts... Vs)
		: detail::htuple_impl<std::make_index_sequence<sizeof...(Ts)>, Ts...>(Vs...)
	{}
};

int main()
{
    htuple tup {33, 2.3f, true, "qwerty"};
    std::cout << "Tuple: " << tup.Get<0>() << " " << tup.Get<1>() << " " << tup.Get<2>() << " " << tup.Get<3>() << std::endl;
    std::cout << "Last: " << tup.GetLast() << std::endl;
    std::cout << "Tuple size: " << tup.size;
}