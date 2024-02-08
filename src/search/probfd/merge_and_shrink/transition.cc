#include "probfd/merge_and_shrink/transition.h"

#include "downward/utils/logging.h"

#include <iostream>

namespace probfd::merge_and_shrink {

namespace {
template <typename T, char LPAREN = '[', char RPAREN = ']'>
std::istream& operator>>(std::istream& in, std::vector<T>& list)
{
    using R = std::conditional_t<std::is_same_v<T, bool>, int, T>;

    if (in.get() != LPAREN) {
        in.setstate(std::ios::failbit);
        return in;
    }

    if ((in >> std::ws, in.peek() != RPAREN)) {
        {
            R element;
            if (!(in >> element)) return in;

            if constexpr (std::is_same_v<T, bool>) {
                list.push_back(element != 0);
            } else {
                list.push_back(element);
            }
        }

        while ((in >> std::ws, in.peek() != RPAREN)) {
            if (in.get() != ',') {
                in.setstate(std::ios::failbit);
                return in;
            }

            R element;
            if (!(in >> std::ws >> element)) return in;

            if constexpr (std::is_same_v<T, bool>) {
                list.push_back(element != 0);
            } else {
                list.push_back(element);
            }
        }
    }

    in.get();
    return in;
}
} // namespace

std::ostream& operator<<(std::ostream& os, const Transition& trans)
{
    os << trans.src << " -> (" << trans.targets.front();
    for (const int target : trans.targets | std::views::drop(1)) {
        os << "," << target;
    }
    return os << ")";
}

std::istream& operator>>(std::istream& is, Transition& trans)
{
    if (!(is >> trans.src)) return is;

    is >> std::ws;

    if (is.get() != '-' || is.get() != '>') {
        is.setstate(std::ios::failbit);
        return is;
    }

    is >> std::ws;

    return operator>> <int, '(', ')'>(is, trans.targets);
}

} // namespace probfd::merge_and_shrink