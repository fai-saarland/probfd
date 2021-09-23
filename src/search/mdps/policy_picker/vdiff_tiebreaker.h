#ifndef MDPS_POLICY_PICKER_VDIFF_TIEBREAKER_H
#define MDPS_POLICY_PICKER_VDIFF_TIEBREAKER_H

#include "../policy_picker.h"
#include "../successor_sort.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

/// Namespace dedicated to policy tiebreaker implementations.
namespace policy_tiebreaking {

class VDiffTiebreaker : public ProbabilisticOperatorPolicyPicker {
public:
    explicit VDiffTiebreaker(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        const std::vector<Distribution<StateID>>& successors) override;

    const value_type::value_t favor_large_gaps_;
};

class VDiffSort : public ProbabilisticOperatorSuccessorSorting {
public:
    explicit VDiffSort(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        std::vector<Distribution<StateID>>& successors) override;

    const value_type::value_t favor_large_gaps_;
};

} // namespace policy_tiebreaking
} // namespace probabilistic

#endif // __VDIFF_TIEBREAKER_H__