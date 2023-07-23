// This is compile-time counter implementation
// It uses friend function injection that caches some generated code for each instantiation to check for existance
// Works since C++20
// github.com/broly/CppFun

#include <iostream>

namespace detail
{
    enum { check };

    // Index        - counting index (0, 1, 2, 3, ...)
    // CounterId    - unique counter type (usually it is lambda)
    template<size_t Index, auto CounterId>
    struct counter_cacher 
    {
        // this template instance
        using this_inst = counter_cacher<Index, CounterId>;

        // this struct contains special friend function that always returns true
        // it exists only if struct is really instantiated
        // we use `this_inst` as tag for defer instatiation
        struct helper 
        {
            friend consteval bool is_instantiated(this_inst)
            { 
                return true; 
            }
        };

        friend consteval bool is_instantiated(this_inst);

        // substitution produces this function if `is_instantiated` is really exists, 
        //   `helper` is instantiated - current `Index` already exists
        template<typename T = this_inst, bool = is_instantiated(T{})>
        static consteval bool exists(decltype(check))
        { 
            return true; 
        }

        // otherwise we produce `generator` and return false - current `Index` not exists yet
        static consteval bool exists(...)
        { 
            return helper(), false; 
        }
    };
}

// Counter class. 
// Each instance gives possibility to generate unique integer sequence from 0
// CounterUniqueId - unique identifier that gives possibility to generate unique counter_cacher(s)
template<auto CounterUniqueId = []{}>
struct Counter
{
    // Recursive integer sequence counter
    // Index  - CountingIndex
    // NextId - Unique `next` member function tag (lambda is always unique)
    template<auto Index = size_t{}, auto NextId = []{}>
    static consteval auto next() 
    {
        // if cacher (by CounterUniqueId and Index) contains Index, we check next cacher...
        // otherwise return this index
        if constexpr (detail::counter_cacher<Index, CounterUniqueId>::exists(detail::check)) 
        {
            return next<Index + 1>();
        } else 
        {
            return Index;
        }
    }
};

// Unqiue counters
using cnt = Counter<>;
using cnt2 = Counter<>;

// Special counter for enum masks
template<auto CounterUniqueId = []{}>
struct Masker : private Counter<CounterUniqueId>
{
    // We should make `next` unique always, so we use lambda as template parameter
    template<auto = []{}>
    static consteval auto next() 
    {
        return 1 << Counter<CounterUniqueId>::next();
    }
};

enum class Enum1
{
    a = cnt::next(),
    b = cnt::next(),
    c = cnt::next(),
    d = cnt::next(),
};

enum class Enum2
{
    a = cnt2::next(),
    b = cnt2::next(),
    c = cnt2::next(),
    d = cnt2::next(),
};

// Yet another special unique counter
using msk = Masker<>;

enum class MaskEnum
{
    a = msk::next(),
    b = msk::next(),
    c = msk::next(),
    d = msk::next(),
};

int main()
{
    std::cout << (int)Enum1::a;
    std::cout << (int)Enum1::b;
    std::cout << (int)Enum1::c;
    std::cout << (int)Enum1::d;

    std::cout << "\n";
    
    std::cout << (int)Enum2::a;
    std::cout << (int)Enum2::b;
    std::cout << (int)Enum2::c;
    std::cout << (int)Enum2::d;

    std::cout << "\n";

    std::cout << (int)MaskEnum::a;
    std::cout << (int)MaskEnum::b;
    std::cout << (int)MaskEnum::c;
    std::cout << (int)MaskEnum::d;
}