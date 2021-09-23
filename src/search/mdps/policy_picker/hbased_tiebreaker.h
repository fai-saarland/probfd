#ifndef MDPS_POLICY_PICKER_HBASED_TIEBREAKER_H
#define MDPS_POLICY_PICKER_HBASED_TIEBREAKER_H

#include "../policy_picker.h"
#include "../successor_sort.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

namespace new_state_handlers {
class StoreHeuristic;
}

namespace policy_tiebreaking {

class HBasedTiebreaker : public ProbabilisticOperatorPolicyPicker {
public:
    explicit HBasedTiebreaker(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        const std::vector<Distribution<StateID>>& successors) override;

    std::shared_ptr<new_state_handlers::StoreHeuristic> heuristic_;
};

class HBasedSort : public ProbabilisticOperatorSuccessorSorting {
public:
    explicit HBasedSort(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        std::vector<Distribution<StateID>>& successors) override;

    std::shared_ptr<new_state_handlers::StoreHeuristic> heuristic_;
};

} // namespace policy_tiebreaking
} // namespace probabilistic

#endif // __HBASED_TIEBREAKER_H__