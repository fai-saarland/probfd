#pragma once

#include "../global_state.h"
#include "algorithms/types_heuristic_search.h"
#include "interfaces/i_printable.h"
#include "probabilistic_operator.h"

#include <memory>
#include <vector>

class Heuristic;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace state_component {
class StateComponentListener;
}

namespace probabilistic {
namespace algorithms {

template<>
class StateComponentListener<GlobalState> {
public:
    ~StateComponentListener() = default;
    bool neighbor_dependency() const;
    bool operator()(
        const GlobalState& state,
        std::unordered_set<GlobalState>::const_iterator child_begin,
        std::unordered_set<GlobalState>::const_iterator child_end);
    bool operator()(
        std::deque<GlobalState>::const_iterator begin,
        std::deque<GlobalState>::const_iterator end,
        std::unordered_set<GlobalState>::const_iterator child_begin,
        std::unordered_set<GlobalState>::const_iterator child_end);

    const IPrintable& get_statistics() const;

    static std::shared_ptr<StateComponentListener<GlobalState>>
    from_options(const options::Options& opts);

    static StateListener<GlobalState, const ProbabilisticOperator*>*
    create_state_listener(
        std::shared_ptr<StateComponentListener<GlobalState>> x,
        ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>*
            aops_gen,
        TransitionGenerator<GlobalState, const ProbabilisticOperator*>*
            transition_generator);

    static void add_options_to_parser(options::OptionParser& parser);

private:
    struct Statistics : public IPrintable {
        unsigned long long calls = 0;
        unsigned long long refinements = 0;
        std::shared_ptr<::state_component::StateComponentListener> listener;
        explicit Statistics(
            std::shared_ptr<::state_component::StateComponentListener> l);
        virtual void print(std::ostream& out) const override;
    };

    explicit StateComponentListener(
        std::shared_ptr<::state_component::StateComponentListener> listener,
        std::shared_ptr<Heuristic> evaluator);
    explicit StateComponentListener(const options::Options& opts);

    Statistics stats_;
    bool stop_listening_;
    std::shared_ptr<::state_component::StateComponentListener> listener_;
    std::shared_ptr<Heuristic> evaluator_;

    std::vector<std::unique_ptr<
        StateListener<GlobalState, const ProbabilisticOperator*>>>
        state_listeners_;
};

} // namespace algorithms

using GlobalStateComponentListener =
    algorithms::StateComponentListener<GlobalState>;
using GlobalStateListener =
    algorithms::StateListener<GlobalState, const ProbabilisticOperator*>;

extern void add_dead_end_level_option(options::OptionParser& p);

extern algorithms::DeadEndIdentificationLevel
get_dead_end_level(const options::Options& opts);

} // namespace probabilistic
