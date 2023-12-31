// This is compile-time counter implementation
// It uses friend function injection that caches some generated code for each instantiation to check for existance
// Works since C++20
// github.com/broly/CppFun

#include <iostream>

namespace detail
{
    // special dummy type for function select
    enum check_t { check };

    // Index        - counting index (0, 1, 2, 3, ...)
    // CounterId    - unique counter type (usually it is lambda)
    template<size_t Index, auto CounterId>
    struct counter_index_cacher 
    {
        // this template instance
        using this_inst = counter_index_cacher<Index, CounterId>;

        // this struct contains special friend dummy function 
        // it exists only if struct is really instantiated
        // we use `this_inst` as tag for defer instatiation
        struct helper 
        {
            friend consteval bool is_instantiated(this_inst)
            { 
                return true; // doesn't make sense (only for instance)
            }
        };

        friend consteval bool is_instantiated(this_inst);

        // substitution produces this function if `is_instantiated` is really exists, 
        //   `helper` is instantiated - current `Index` already exists
        template<typename T = this_inst, bool = is_instantiated(T{})>
        static consteval bool exists(check_t check)
        { 
            return true; 
        }

        // otherwise we produce `helper` and return false - current `Index` not exists yet
        // When `helper` is instantiated, it caches `is_instantiated` function, so it switches `exists` to another overloading
        static consteval bool exists(...)
        {
            // just instantiate for cache (and use it with comma operator to avoid optimization out)
            return helper(), false;
        }
    };
}

// Counter class. 
// Each instance gives possibility to generate unique integer sequence from 0
// CounterUniqueId - unique identifier that gives possibility to generate unique counter_index_cacher(s)
template<auto CounterUniqueId = []{}>
struct Counter
{
    // Recursive integer sequence counter
    // Index  - CountingIndex
    // NextId - Unique `next` member function tag (lambda is always unique)
    template<size_t Index = 0, auto NextId = []{}>
    static consteval size_t next() 
    {
        // if cacher (by CounterUniqueId and Index) contains Index, we check next cacher...
        // otherwise return this index
        if constexpr (detail::counter_index_cacher<Index, CounterUniqueId>::exists(detail::check)) 
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
    static consteval size_t next() 
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


// Special counter for integral functions
template<auto Func = [](int) -> int {}, auto CounterUniqueId = []{}>
struct CounterFunc : private Counter<CounterUniqueId>
{
    // We should make `next` unique always, so we use lambda as template parameter
    template<auto = []{}>
    static consteval size_t next() 
    {
        return Func(Counter<CounterUniqueId>::next());
    }
};

using sqcnt = CounterFunc<[] (size_t a) -> size_t { return a * a; }>;


enum class SqrEnum
{
    a = sqcnt::next(),
    b = sqcnt::next(),
    c = sqcnt::next(),
    d = sqcnt::next(),
    e = sqcnt::next(),
};

int main()
{
    std::cout << (int)Enum1::a;  // 0
    std::cout << (int)Enum1::b;  // 1
    std::cout << (int)Enum1::c;  // 2
    std::cout << (int)Enum1::d;  // 3

    std::cout << "\n";
    
    std::cout << (int)Enum2::a;  // 0
    std::cout << (int)Enum2::b;  // 1
    std::cout << (int)Enum2::c;  // 2
    std::cout << (int)Enum2::d;  // 3

    std::cout << "\n";

    std::cout << (int)MaskEnum::a;  // 1
    std::cout << (int)MaskEnum::b;  // 2
    std::cout << (int)MaskEnum::c;  // 4
    std::cout << (int)MaskEnum::d;  // 8
    
    std::cout << "\n";

    std::cout << (int)SqrEnum::a; // 0
    std::cout << (int)SqrEnum::b; // 1
    std::cout << (int)SqrEnum::c; // 4
    std::cout << (int)SqrEnum::d; // 9
    std::cout << (int)SqrEnum::e; // 16
}
