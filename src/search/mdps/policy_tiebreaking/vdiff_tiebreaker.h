#pragma once

#include "../policy_chooser.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace policy_tiebreaking {

class VDiffTiebreaker : public PolicyChooser {
public:
    explicit VDiffTiebreaker(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual unsigned choose(
        const GlobalState& state,
        const ProbabilisticOperator*,
        const std::vector<
            std::pair<const ProbabilisticOperator*, Distribution<GlobalState>>>&
            choices) override;

    virtual unsigned choose(
        const QuotientState& state,
        const QuotientAction& prev,
        const std::vector<
            std::pair<QuotientAction, Distribution<QuotientState>>>& choices)
        override;

};

}
}
