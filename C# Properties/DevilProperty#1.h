// github.com/broly/CppFun
// This is holder-less C# property implementation
// It gives possibility (on several compilers) to fully get rid of presense of this property in memory
// On compiler like MSVC it still has size, but as little as possible
// WARNING: DO NOT USE IT IN YOUR CODE! It is not compatible with non-standard class layouts

#include <functional>
#include <iostream>
#include <utility>
#include <cstddef>

using namespace std;

namespace evil
{
    // Helper that converts pointer-to-member-field to offset of member
    template<typename Class, typename FieldType>
    unsigned int MemberPtrToOffset(FieldType Class::* PointerToField)
    {
        // A pointer-to-member-field could not be cast to an integral type via any known cast (even reinterpret_cast)
        // So use the union to break this limitation
        union
        {
            unsigned int Offset;
            FieldType Class::* PtrToField;
        } FieldOffset;
        FieldOffset.PtrToField = PointerToField;
        return FieldOffset.Offset;
    }

    // Memory-zero-cost type
#if !defined(_MSC_VER)
    using ravage = unsigned char[0];  // "ravage" means that a class that only has a variable of that type becomes zero-sized
#else 
    using ravage = unsigned char;  // on MSVC it does not works
#endif
}

// This is evil thing that makes property memory zero cost (sizeof(Property) == 0)
// NOTE: This does not works with MSVC
#if !defined(_MSC_VER)
    #define RAVAGE evil::ravage __RAVAGE__;
#else
    #define RAVAGE
#endif

template<auto ThisField, class Cls, typename T, auto Getter, auto Setter>
class Property
{
public:
    RAVAGE // Make property memory-zero-cost if possible

    Cls* GetHolderPtr() const
    {
        // Go from current memory location to holder location
        return (Cls*)(reinterpret_cast<const unsigned char*>(this) - evil::MemberPtrToOffset(ThisField));
    }

    T Get() const
    {
        return invoke(Getter, GetHolderPtr());
    }

    void Set(T&& V) const
    {
        invoke(Setter, GetHolderPtr(), std::forward<T>(V));
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

    std::cout << "Test MemberPtrToOffset 1: " << evil::MemberPtrToOffset(&Test::__dummy_Prop) << " " << offsetof(Test, Prop) << std::endl;
    std::cout << "Test MemberPtrToOffset 2: " << evil::MemberPtrToOffset(&Test::__dummy_Prop2) << " " <<  offsetof(Test, Prop2) << std::endl;

    std::cout << "Test MemberPtrToOffset ptr diff 1: " << offsetof(Test, Prop) - evil::MemberPtrToOffset(&Test::__dummy_Prop) << std::endl;
    std::cout << "Test MemberPtrToOffset ptr diff 2: " << offsetof(Test, Prop2) - evil::MemberPtrToOffset(&Test::__dummy_Prop2)<< std::endl;

    std::cout << "size of property: " << sizeof(Test::Prop) << std::endl;
    std::cout << "size of class: " << sizeof(Test) << std::endl;

    std::cout << "Property test 1: " << s << " " << q << std::endl;
    std::cout << "Property test 2: " << a.Val << " " << a.Val2 << std::endl;
    return 0;
}
