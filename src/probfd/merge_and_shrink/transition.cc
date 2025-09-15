#include "probfd/merge_and_shrink/transition.h"

#include "probfd/json/json.h"

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

Transition::Transition(const json::JsonObject& object)
    : src(object.read<int>("src"))
    , targets(object.read<std::vector<int>>("targets"))
{
}

Transition::Transition(int src, std::vector<int> targets)
    : src(src)
    , targets(std::move(targets))
{
}

std::ostream& operator<<(std::ostream& os, const Transition& transition)
{
    std::print(os, "{} -> ({:n})", transition.src, transition.targets);
    return os;
}

std::unique_ptr<json::JsonObject> to_json(const Transition& transition)
{
    return json::make_object(
        "src",
        transition.src,
        "targets",
        transition.targets);
}

} // namespace probfd::merge_and_shrink