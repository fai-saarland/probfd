#pragma once

#include "../policy_picker.h"
#include "../successor_sort.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

namespace new_state_handlers {
class StorePreferredOperators;
}

namespace policy_tiebreaking {

class PreferredOperatorsTiebreaker : public ProbabilisticOperatorPolicyPicker {
public:
    explicit PreferredOperatorsTiebreaker(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        const std::vector<Distribution<StateID>>& successors) override;

    std::shared_ptr<new_state_handlers::StorePreferredOperators> pref_ops_;
};

class PreferredOperatorsSort : public ProbabilisticOperatorSuccessorSorting {
public:
    explicit PreferredOperatorsSort(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<const ProbabilisticOperator*>& action_choices,
        std::vector<Distribution<StateID>>& successors) override;

    std::shared_ptr<new_state_handlers::StorePreferredOperators> pref_ops_;
};

} // namespace policy_tiebreaking
} // namespace probabilistic
