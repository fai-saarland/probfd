#ifndef PROBFD_UTILS_LANGUAGE_H
#define PROBFD_UTILS_LANGUAGE_H

#include <iterator>
#include <utility>

namespace probfd {

/**
 * Can be used to combine multiple lambdas:
 *
 * overloaded{
 *     [](int x){ return std::to_string(x); },
 *     [](std::string x){ return x; },
 * }
 */
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts... ts) -> overloaded<Ts...>;

template <size_t i>
decltype(auto) select(auto&&... args)
    requires(i < sizeof...(args))
{
    return std::get<i>(std::tie(args...));
}

template <size_t i>
decltype(auto) select_opt(auto&&... args)
{
    if constexpr (i < sizeof...(args)) {
        return std::get<i>(std::tie(args...));
    } else {
        return nullptr;
    }
}

template <typename Derived>
class add_postfix_inc_dec {
public:
    Derived operator++(int)
        requires requires(Derived d) { ++d; }
    {
        auto& self = static_cast<Derived&>(*this);

        Derived tmp(self);
        ++self;
        return tmp;
    }

    Derived operator--(int)
        requires requires(Derived d) { --d; }
    {
        auto& self = static_cast<Derived&>(*this);

        Derived tmp(self);
        --self;
        return tmp;
    }
};

template <typename Derived>
class add_pm_assignment {
public:
    Derived& operator+=(int n)
        requires requires(Derived d, int x) {
            {
                d + x
            } -> std::convertible_to<Derived>;
        }
    {
        auto& self = static_cast<Derived&>(*this);
        self = self + n;
        return self;
    }

    Derived& operator-=(int n)
        requires requires(Derived d, int x) {
            {
                d - x
            } -> std::convertible_to<Derived>;
        }
    {
        auto& self = static_cast<Derived&>(*this);
        self = self - n;
        return self;
    }
};

template <typename Derived>
class add_indirection {
public:
    auto* operator->()
    {
        auto& self = static_cast<Derived&>(*this);
        return &*self;
    }
};

template <typename Derived>
class add_subscript {
public:
    auto& operator[](int n)
        requires requires(Derived d, int x) {
            {
                d + x
            } -> std::convertible_to<Derived>;
            {
                *d
            };
        }
    {
        auto& self = static_cast<Derived&>(*this);
        return *(self + n);
    }
};

} // namespace probfd

#endif