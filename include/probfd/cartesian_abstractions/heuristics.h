#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_HEURISTICS_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_HEURISTICS_H

#include "probfd/heuristic.h"
#include "probfd/value_type.h"

#include <vector>

namespace probfd::cartesian_abstractions {

class CartesianHeuristic : public Heuristic<int> {
    std::vector<value_t> h_values_ = {0.0_vt};

public:
    [[nodiscard]]
    value_t evaluate(int state) const final;

    [[nodiscard]]
    value_t get_h_value(int v) const;
    void set_h_value(int v, value_t h);
    void on_split(int v);
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_HEURISTICS_H
