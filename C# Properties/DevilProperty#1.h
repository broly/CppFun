// This holder-less C# property implementation
// It gives possibility (on several compilers) to fully get rid of presense of this property in memory
// On compiler like MSVC it still has size, but as little as possible

#include <functional>
#include <iostream>
#include <utility>
#include <cstddef>

using namespace std;

namespace evil
{
    // Helper that converts pointer-to-member to offset of member
    size_t FieldToOffset(auto Field)
    {
        union
        {
            unsigned int Result;
            decltype(Field) Field;
        } FieldOffset;
        FieldOffset.Field = Field;
        return FieldOffset.Result;
    }

    // Memory-zero-cost type
#if !defined(_MSC_VER)
    using ravage = unsigned char[0]; 
#else 
    using ravage = unsigned char;
#endif
}

// This is evil thing that makes property memory zero cost (sizeof(Property) == 0)
// NOTE: This does not works with MSVC
#if !defined(_MSC_VER)
    #define RAVAGE evil::ravage a;
#else
    #define RAVAGE
#endif

template<auto ThisField, class Cls, typename T, auto Getter, auto Setter>
class Property
{
public:
    RAVAGE // Make property memory-zero-cost if possible

    Cls* GetThis() const
    {
        return (Cls*)(reinterpret_cast<const unsigned char*>(this) - evil::FieldToOffset(ThisField));
    }

    T Get() const
    {
        return invoke(Getter, GetThis());
    }

    void Set(T&& V) const
    {
        invoke(Setter, GetThis(), std::forward<T>(V));
    }

    const Property& operator=(T&& V) const
    {
        Set(std::forward<T>(V));
        return *this;
    }

    operator T() const
    {
        return Get();
    }
};

// We need to navigate to property holder, so we pass dummy field as template parameter 
//    (with zero cost size if possible to avoid waste of memory for this property)
#define PROPERTY(Name, Class, Type, Getter, Setter) \
    evil::ravage __dummy_##Name;\
    Property<&Class::__dummy_##Name, Class, Type, &Class::Getter, &Class::Setter> Name;


class Test 
{
public:
    int Get()
    {
        return Val;
    }

    void Set(int A)
    {
        Val = A;
    }

    int Get2()
    {
        return Val2;
    }

    void Set2(int A)
    {
        Val2 = A;
    }

    int Val;
    int Val2;

    // Dirty layout stuff (try to break devil property)
    long long int a;
    char b[5];

    PROPERTY(Prop, Test, int, Get, Set);

    // Dirty layout stuff (try to break devil property)
    char c[5]; 

    PROPERTY(Prop2, Test, int, Get2, Set2);

};

int main()
{
    Test a;
    a.Prop = 333;
    int s = a.Prop;
    a.Prop2 = 444;
    int q = a.Prop2;

    std::cout << "Test FieldToOffset 1: " << evil::FieldToOffset(&Test::__dummy_Prop) << " " << offsetof(Test, Prop) << std::endl;
    std::cout << "Test FieldToOffset 2: " << evil::FieldToOffset(&Test::__dummy_Prop2) << " " <<  offsetof(Test, Prop2) << std::endl;

    std::cout << "Test FieldToOffset ptr diff 1: " << offsetof(Test, Prop) - evil::FieldToOffset(&Test::__dummy_Prop) << std::endl;
    std::cout << "Test FieldToOffset ptr diff 2: " << offsetof(Test, Prop2) - evil::FieldToOffset(&Test::__dummy_Prop2)<< std::endl;

    std::cout << "size of property: " << sizeof(Test::Prop) << std::endl;
    std::cout << "size of class: " << sizeof(Test) << std::endl;

    std::cout << "Property test 1: " << s << " " << q << std::endl;
    std::cout << "Property test 2: " << a.Val << " " << a.Val2 << std::endl;
    return 0;
}