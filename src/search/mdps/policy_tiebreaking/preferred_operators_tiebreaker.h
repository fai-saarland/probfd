#pragma once

#include "../policy_chooser.h"

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

class PreferredOperatorsTiebreaker : public PolicyChooser {
public:
    explicit PreferredOperatorsTiebreaker(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual unsigned choose(
        const GlobalState& state,
        const ProbabilisticOperator*,
        const std::vector<
            std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
            choices) override;

    virtual unsigned choose(
        const QuotientState& state,
        const QuotientAction&,
        const std::vector<
            std::pair<QuotientAction, Distribution<QuotientState>>>& choices)
        override;

    std::shared_ptr<new_state_handlers::StorePreferredOperators> pref_ops_;
};

} // namespace policy_tiebreaking
} // namespace probabilistic

