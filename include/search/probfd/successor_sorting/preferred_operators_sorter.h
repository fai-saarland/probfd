#ifndef MDPS_SUCCESSOR_SORTING_PREFERRED_OPERATORS_SORTER_H
#define MDPS_SUCCESSOR_SORTING_PREFERRED_OPERATORS_SORTER_H

#include "probfd/successor_sort.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

namespace new_state_handlers {
class StorePreferredOperators;
}

namespace successor_sorting {

class PreferredOperatorsSorter : public ProbabilisticOperatorSuccessorSorting {
public:
    explicit PreferredOperatorsSorter(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        std::vector<Distribution<StateID>>& successors) override;

    std::shared_ptr<new_state_handlers::StorePreferredOperators> pref_ops_;
};

} // namespace successor_sorting
} // namespace probfd

#endif // __PREFERRED_OPERATORS_TIEBREAKER_H__