#include "state_listener.h"

#include "../heuristic.h"
#include "../option_parser.h"
#include "../state_component_listener.h"
#include "logging.h"

#include <string>
#include <vector>

namespace probabilistic {
namespace algorithms {

StateComponentListener<GlobalState>::Statistics::Statistics(
    std::shared_ptr<::state_component::StateComponentListener> l)
    : listener(l)
{
}

void
StateComponentListener<GlobalState>::Statistics::print(std::ostream& out) const
{
    out << "Dead ends identified: " << calls << std::endl;
    out << "Number of dead-end evaluator refinements: " << refinements
        << std::endl;
    listener->print_statistics();
}

StateComponentListener<GlobalState>::StateComponentListener(
    std::shared_ptr<::state_component::StateComponentListener> listener,
    std::shared_ptr<Heuristic> evaluator)
    : stats_(listener)
    , stop_listening_(false)
    , listener_(listener)
    , evaluator_(evaluator)
{
}

StateComponentListener<GlobalState>::StateComponentListener(
    const options::Options& opts)
    : StateComponentListener(
        opts.get<std::shared_ptr<::state_component::StateComponentListener>>(
            "on_dead_end"),
        opts.contains("dead_end_eval")
            ? opts.get<std::shared_ptr<Heuristic>>("dead_end_eval")
            : nullptr)
{
}

const IPrintable&
StateComponentListener<GlobalState>::get_statistics() const
{
    return stats_;
}

bool
StateComponentListener<GlobalState>::neighbor_dependency() const
{
    return listener_->requires_recognized_neighbors_explicitly();
}

bool
StateComponentListener<GlobalState>::operator()(
    const GlobalState& state,
    std::unordered_set<GlobalState>::const_iterator child_begin,
    std::unordered_set<GlobalState>::const_iterator child_end)
{
    if (stop_listening_) {
        return false;
    }
    stats_.calls++;
    if (evaluator_ != nullptr) {
        evaluator_->evaluate(state);
        if (evaluator_->is_dead_end()) {
            return true;
        }
    }
    stats_.refinements++;
    ::state_component::SingletonComponent<GlobalState> component(state);
    ::state_component::ComponentIterator<
        GlobalState,
        std::unordered_set<GlobalState>::const_iterator>
        neighbors(child_begin, child_end);
    ::state_component::StateComponentListener::Status res =
        listener_->notify_new_component(component, neighbors);
    stop_listening_ =
        res == ::state_component::StateComponentListener::Status::StopListening
        || (res
                == ::state_component::StateComponentListener::Status::
                    KeepListening
            && listener_->requires_recognized_neighbors_implicitly());
    return !stop_listening_;
}

bool
StateComponentListener<GlobalState>::operator()(
    std::deque<GlobalState>::const_iterator begin,
    std::deque<GlobalState>::const_iterator end,
    std::unordered_set<GlobalState>::const_iterator child_begin,
    std::unordered_set<GlobalState>::const_iterator child_end)
{
    if (stop_listening_) {
        return false;
    }
    stats_.calls++;
    if (evaluator_ != nullptr) {
        evaluator_->evaluate(*begin);
        if (evaluator_->is_dead_end()) {
            return true;
        }
    }
    stats_.refinements++;
    ::state_component::
        ComponentIterator<GlobalState, std::deque<GlobalState>::const_iterator>
            component(begin, end);
    ::state_component::ComponentIterator<
        GlobalState,
        std::unordered_set<GlobalState>::const_iterator>
        neighbors(child_begin, child_end);
    ::state_component::StateComponentListener::Status res =
        listener_->notify_new_component(component, neighbors);
    stop_listening_ =
        res == ::state_component::StateComponentListener::Status::StopListening
        || (res
                == ::state_component::StateComponentListener::Status::
                    KeepListening
            && listener_->requires_recognized_neighbors_implicitly());
    return !stop_listening_;
}

void
StateComponentListener<GlobalState>::add_options_to_parser(
    options::OptionParser& parser)
{
    parser
        .add_option<std::shared_ptr<::state_component::StateComponentListener>>(
            "on_dead_end", "", options::OptionParser::NONE);
    parser.add_option<std::shared_ptr<Heuristic>>(
        "dead_end_eval", "", options::OptionParser::NONE);
    add_dead_end_level_option(parser);
}

std::shared_ptr<StateComponentListener<GlobalState>>
StateComponentListener<GlobalState>::from_options(const options::Options& opts)
{
    if (!opts.contains("on_dead_end")) {
        return nullptr;
    }
    if (!opts.contains("dead_end_level")
        || opts.get_enum("dead_end_level") == 0) {
        return nullptr;
    }
    return std::shared_ptr<StateComponentListener<GlobalState>>(
        new StateComponentListener<GlobalState>(opts));
}

StateListener<GlobalState, const ProbabilisticOperator*>*
StateComponentListener<GlobalState>::create_state_listener(
    std::shared_ptr<StateComponentListener<GlobalState>> x,
    ApplicableActionsGenerator<GlobalState, const ProbabilisticOperator*>*
        aops_gen,
    TransitionGenerator<GlobalState, const ProbabilisticOperator*>*
        transition_generator)
{
    if (x == nullptr) {
        return nullptr;
    }
    x->state_listeners_.emplace_back(
        new StateListener<GlobalState, const ProbabilisticOperator*>(
            x.get(), aops_gen, transition_generator));
    return x->state_listeners_.back().get();
}

} // namespace algorithms

void
add_dead_end_level_option(options::OptionParser& p)
{
    std::vector<std::string> opts { "off", "policy", "visited", "complete" };
    p.add_enum_option("dead_end_level", opts, "", "off");
}

algorithms::DeadEndIdentificationLevel
get_dead_end_level(const options::Options& opts)
{
    return algorithms::DeadEndIdentificationLevel(
        opts.get_enum("dead_end_level"));
}

} // namespace probabilistic
