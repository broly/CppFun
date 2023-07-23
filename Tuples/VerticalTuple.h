// github.com/broly/CppFun
// This is minimal as possible vertical tuple implementation
#include <iostream>

template<typename... Ts>
struct vtuple;

namespace detail
{
	// Element accessing implementations
	template<size_t Index, typename T>
	struct GetHelper;

	// Accessing to first (0) element
	template<typename T, typename... Ts>
	struct GetHelper<0, vtuple<T, Ts... >>
	{
		static constexpr auto Get(vtuple<T, Ts...>& Data)
		{
			return Data.Value;
		}
	};

	// Accessing to any element (iter from 'Index' downto 0 and pass 'Rest' tuple for each iteration)
	// Finally go to first (0) accessor
	template<size_t Index, typename T, typename... Ts>
	struct GetHelper<Index, vtuple<T, Ts... >>
	{
		static constexpr auto Get(vtuple<T, Ts...>& Data)
		{
			return GetHelper<Index - 1, vtuple<Ts...>>::Get((vtuple<Ts...>&)Data);
		}
	};
}


// Last tuple element implementation
template<typename T>
struct vtuple<T>
{
	constexpr vtuple(const T& InValue)
		: Value(InValue)
	{}
	
	T Value;

	template<size_t Index>
	auto Get()
	{
		return Value;
	}

	template<size_t Index>
	auto GetLast()
	{
		return Value;
	}

	static constexpr size_t size = 1;
};

// Tuple element implementation
template<typename T, typename... Ts>
struct vtuple<T, Ts...> : vtuple<Ts...>
{
	constexpr vtuple(const T& InValue, const Ts&... InRest)
		: Value(InValue)
		, vtuple<Ts...>(InRest...)
	{}
	
	T Value;

	template<size_t Index>
	auto Get()
	{
		return detail::GetHelper<Index, vtuple<T, Ts...>>::Get(*this);
	}

	template<size_t Index = 0>
	auto GetLast()
	{
		return detail::GetHelper<sizeof...(Ts) - Index, vtuple<T, Ts...>>::Get(*this);
	}

	static constexpr size_t size = sizeof...(Ts) + 1;
};

// Empty tuple implementation
template<>
struct vtuple<>
{
	constexpr vtuple()
	{}

	static constexpr size_t size = 0;
};

int main()
{
    vtuple<int, float, bool, const char*> tup {33, 2.3f, true, "qwerty"};

    std::cout << "Tuple: " << tup.Get<0>() << " " << tup.Get<1>() << " " << tup.Get<2>() << " " << tup.Get<3>() << std::endl;
    std::cout << "Last: " << tup.GetLast() << std::endl;
    std::cout << "Tuple size: " << tup.size;
}