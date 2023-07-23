// github.com/broly/CppFun
// This is minimal as possible tuple implementation
#include <iostream>

template<typename... Ts>
struct minituple;

namespace detail
{
	// Element accessing implementations
	template<size_t Index, typename T>
	struct GetHelper;

	// Accessing to first (0) element (0 is just access to 'First' field)
	template<typename T, typename... Ts>
	struct GetHelper<0, minituple<T, Ts... >>
	{
		static constexpr auto Get(minituple<T, Ts...>& Data)
		{
			return Data.First;
		}
	};

	// Accessing to any element (iter from 'Index' downto 0 and pass 'Rest' tuple for each iteration)
	// Finally go to first (0) accessor
	template<size_t Index, typename T, typename... Ts>
	struct GetHelper<Index, minituple<T, Ts... >>
	{
		static constexpr auto Get(minituple<T, Ts...>& Data)
		{
			return GetHelper<Index - 1, minituple<Ts ...>>::Get(Data.Rest);
		}
	};
}


// Last tuple element implementation
template<typename T>
struct minituple<T>
{
	constexpr minituple(const T& InFirst)
		: First(InFirst)
	{}
	
	T First;

	template<size_t Index>
	auto Get()
	{
		return First;
	}

	template<size_t Index>
	auto GetLast()
	{
		return First;
	}

	static constexpr size_t size = 1;
};

// Tuple element implementation
template<typename T, typename... Ts>
struct minituple<T, Ts...>
{
	constexpr minituple(const T& InFirst, const Ts&... InRest)
		: First(InFirst)
		, Rest(InRest...)
	{}
	
	T First;
	minituple<Ts...> Rest;

	template<size_t Index>
	auto Get()
	{
		return detail::GetHelper<Index, minituple<T, Ts...>>::Get(*this);
	}

	template<size_t Index = 0>
	auto GetLast()
	{
		return detail::GetHelper<sizeof...(Ts) - Index, minituple<T, Ts...>>::Get(*this);
	}

	static constexpr size_t size = sizeof...(Ts) + 1;
};

// Empty tuple implementation
template<>
struct minituple<>
{
	constexpr minituple()
	{}

	static constexpr size_t size = 0;
};

int main()
{
    minituple<int, float, bool, const char*> tup {33, 2.3f, true, "qwerty"};

    std::cout << "Tuple: " << tup.Get<0>() << " " << tup.Get<1>() << " " << tup.Get<2>() << " " << tup.Get<3>() << std::endl;
    std::cout << "Last: " << tup.GetLast() << std::endl;
    std::cout << "Tuple size: " << tup.size;
}
