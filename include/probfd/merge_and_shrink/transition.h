#ifndef PROBFD_MERGE_AND_SHRINK_TRANSITION_H
#define PROBFD_MERGE_AND_SHRINK_TRANSITION_H

#include <compare>
#include <iosfwd>
#include <vector>

namespace probfd::merge_and_shrink {

struct Transition {
    int src;
    std::vector<int> targets;

    friend auto
    operator<=>(const Transition& lhs, const Transition& rhs) = default;

    friend std::ostream&
    operator<<(std::ostream& os, const Transition& transition);

    friend std::istream& operator>>(std::istream& is, Transition& transition);
};

} // namespace probfd::merge_and_shrink

#endif
