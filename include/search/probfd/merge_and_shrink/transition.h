#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_TRANSITION_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_TRANSITION_H

#include "probfd/utils/json.h"

#include <compare>
#include <iosfwd>
#include <vector>

namespace probfd::merge_and_shrink {

struct Transition {
    int src;
    std::vector<int> targets;

    friend auto
    operator<=>(const Transition& lhs, const Transition& rhs) = default;

    friend std::ostream& operator<<(std::ostream& out, const Transition& t);
    friend std::istream& operator>>(std::istream& in, Transition& t);

    friend void dump_json(std::ostream& os, const Transition& transition);
    static Transition read_json(std::istream& is);
};

void dump_json(std::ostream& os, const Transition& transition);

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_TRANSITION_H
