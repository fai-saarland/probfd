#ifndef MDPS_SUCCESSOR_SORTING_HBASED_SORTER_H
#define MDPS_SUCCESSOR_SORTING_HBASED_SORTER_H

#include "probfd/successor_sort.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

namespace new_state_handlers {
class StoreHeuristic;
}

namespace successor_sorting {

class HBasedSorter : public ProbabilisticOperatorSuccessorSorting {
public:
    explicit HBasedSorter(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        std::vector<Distribution<StateID>>& successors) override;

    std::shared_ptr<new_state_handlers::StoreHeuristic> heuristic_;
};

} // namespace successor_sorting
} // namespace probfd

#endif // __HBASED_TIEBREAKER_H__