#ifndef MDPS_SUCCESSOR_SORTING_VDIFF_SORTER_H
#define MDPS_SUCCESSOR_SORTING_VDIFF_SORTER_H

#include "probfd/policy_picker.h"
#include "probfd/successor_sort.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

/// Namespace dedicated to policy tiebreaker implementations.
namespace successor_sorting {

class VDiffSorter : public ProbabilisticOperatorSuccessorSorting {
public:
    explicit VDiffSorter(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<OperatorID>& action_choices,
        std::vector<Distribution<StateID>>& successors) override;

    const value_type::value_t favor_large_gaps_;
};

} // namespace successor_sorting
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__