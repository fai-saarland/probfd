#include "probfd/merge_and_shrink/transition.h"

#include "probfd/json/json.h"

#include "downward/utils/logging.h"

#include <iostream>

namespace probfd::merge_and_shrink {

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