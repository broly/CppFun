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

	namespace helpers
	{
		// Just deduces the type of the element by given index
        template<size_t Index>
        struct elem_getter
        {
            template<typename DeducedType>
            static decltype(auto) Get(const htuple_elem<DeducedType, Index>& Tup)
            {
                return Tup.Value;
            }
        };
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
            return helpers::elem_getter<Index>::Get(*this);
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