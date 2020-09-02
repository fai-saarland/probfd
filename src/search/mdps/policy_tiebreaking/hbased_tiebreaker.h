#pragma once

#include "../policy_chooser.h"

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

class HBasedTiebreaker : public PolicyChooser {
public:
    explicit HBasedTiebreaker(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual unsigned choose(
        const GlobalState& state,
        const ProbabilisticOperator* prev,
        const std::vector<
            std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
            choices) override;

    virtual unsigned choose(
        const QuotientState& state,
        const QuotientAction& a,
        const std::vector<
            std::pair<QuotientAction, Distribution<QuotientState>>>& choices)
        override;

    std::shared_ptr<new_state_handlers::StoreHeuristic> heuristic_;
};

} // namespace policy_tiebreaking
} // namespace probabilistic
