// github.com/broly/CppFun
// This is property pattern that gives possibility to access inner state of object 
// Property imitates class field
// You can specify getter and optionally setter
// You can also use direct field instead of getter and make field private

#include <type_traits>
#include <functional>
#include <iostream>

namespace details
{
	template<typename Owner>
	struct property_owner
	{
		constexpr explicit property_owner(Owner* owner)
			: _owner(owner)
		{}
	
		Owner* _owner;
	};
}

template<typename T, typename Owner, auto...>
struct property;

// Getter implementation
template<typename T, typename Owner, auto Getter>
struct property<T, Owner, Getter> : protected details::property_owner<Owner>
{
	using parent = details::property_owner<Owner>;
	using parent::parent;
	
	T Get() const
	{
		return std::invoke(Getter, parent::_owner);
	}
	
	operator T() const
	{
		return Get();
	}

	std::remove_reference_t<T>* GetPtr() const
	{
		return &Get();
	}

	std::remove_reference_t<T>* operator->() const
	{
		return GetPtr();
	}
};

// Setter with getter implementation
template<typename T, typename OwnerType, auto Getter, auto Setter>
struct property<T, OwnerType, Getter, Setter> : property<T, OwnerType, Getter>
{
	using parent = property<T, OwnerType, Getter>;
	using parent::parent;
	
	template<typename U = T>
	void Set(U&& Value)
	{
		return std::invoke(Setter, parent::_owner, std::forward<U>(Value));
	}

	template<typename U = T>
	const property& operator=(T&& Value)
	{
		Set(std::forward<T>(Value));
		return *this;
	}
};

// Helpers to parse holder class and property type
namespace details
{
	template<class C, typename T>
	C get_holder_class(T C::*);
	
	template<class C, typename T>
	T get_field_type(T C::*);

	template<class C, typename R, typename... Args>
	C get_holder_class(R (C::*)(Args...));
	
	template<class C, typename R, typename... Args>
	R get_field_type(R (C::*)(Args...));
	
	template<class C, typename R, typename... Args>
	C get_holder_class(R (C::*)(Args...) const);
	
	template<class C, typename R, typename... Args>
	R get_field_type(R (C::*)(Args...) const);
	
	template<auto V>
	using get_holder_class_t = decltype(get_holder_class(V));
	
	template<auto V>
	using get_field_type_t = decltype(get_field_type(V));
}


template<auto getter, auto... maybe_setter>
using auto_property = property<
	details::get_field_type_t<getter>,
	details::get_holder_class_t<getter>,
	getter, maybe_setter...>;



// Use cases
class PropertySamples
{
public:
    PropertySamples()
        : Var1(0)
        , Var2(0)
        , Var3(0)
    {}

    int Var1;
    int Var2;
    int Var3;

    int Var1_Getter()
    {
        return Var1;
    }

    void Var1_Setter(int Val)
    {
        Var1 = Val;
    }

    void Var2_Setter(int Val)
    {
        Var2 = Val;
    }

    // Sample with getter and setter
    auto_property<
        &PropertySamples::Var1_Getter, 
        &PropertySamples::Var1_Setter> 
        Prop1{this};

    // Sample with direct get access and setter
    auto_property<
        &PropertySamples::Var2, 
        &PropertySamples::Var2_Setter> 
        Prop2{this};

    // Sample with only direct get access
    auto_property<
        &PropertySamples::Var3>
        Prop3{this};
};

int main()
{
    PropertySamples S;

    S.Prop1 = 123;
    int Var1 = S.Prop1;
    std::cout << Var1 << " " << S.Var1 << std::endl;

    S.Prop2 = 321;
    int Var2 = S.Prop2;
    std::cout << Var2 << " " << S.Var2 << std::endl;

    S.Var3 = 444;
    int Var3 = S.Prop3;
    std::cout << Var3 << " " << S.Var3 << std::endl;

}
