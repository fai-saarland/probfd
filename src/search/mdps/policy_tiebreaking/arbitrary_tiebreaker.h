#pragma once

#include "../policy_chooser.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace policy_tiebreaking {

class ArbitraryTiebreaker : public PolicyChooser {
public:
    explicit ArbitraryTiebreaker();
    explicit ArbitraryTiebreaker(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

protected:
    virtual unsigned choose(
        const GlobalState& state,
        const std::vector<
            std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
            choices) override;

    virtual unsigned choose(
        const QuotientState& state,
        const std::vector<std::pair<QuotientAction, Distribution<QuotientState>>>&
            choices) override;
};

} // namespace policy_tiebreaking
} // namespace probabilistic
