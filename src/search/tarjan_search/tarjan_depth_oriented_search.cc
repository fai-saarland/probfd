#include "tarjan_depth_oriented_search.h"

#include "../global_state.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../plugin.h"
#include "../state_component.h"
#include "../state_component_listener.h"
#include "../state_registry.h"
#include "../successor_generator.h"
#include "../utils/system.h"
#include "../utils/timer.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>

#ifndef NDEBUG
#define NDEBUG_VERIFY_RECOGNIZED_NEIGHBORS 1
#define NDEBUG_VERIFY_REFINEMENT 1
#endif

using namespace successor_generator;

struct GlobalStateLess {
    bool operator()(const GlobalState& s1, const GlobalState& s2) const
    {
        return s1.get_id().hash() < s2.get_id().hash();
    }
};

using StateSet = std::set<GlobalState, GlobalStateLess>;

namespace tarjan_search {

TarjanDepthOrientedSearch::CallStackElement::CallStackElement(
    StateID state_id,
    unsigned open_list_offset,
    unsigned rn_offset)
    : state_id(state_id)
    , open_list_offset(open_list_offset)
    , rn_offset(rn_offset)
    , succ_result(DFSResult::Failed)
{
}

TarjanDepthOrientedSearch::TarjanDepthOrientedSearch(
    const options::Options& opts)
    : SearchEngine(opts)
    , recompute_u_(opts.get<bool>("recompute_u"))
    , ignore_initial_state_(opts.get<bool>("ignore_initial_state"))
    , stop_dead_end_notification_(opts.get<bool>("stop_dead_end_notification"))
    , eval_dead_ends_compatible_(opts.get<bool>("eval_dead_ends_compatible"))
    , consider_eval_dead_ends_(opts.get<bool>("consider_eval_dead_ends"))
    , identify_dead_end_components_(opts.contains("on_dead_end_scc"))
    , compute_recognized_neighbors_(identify_dead_end_components_)
    , guidance_(
          opts.contains("eval") ? opts.get<std::shared_ptr<Heuristic>>("eval")
                                : nullptr)
    , preferred_(
          opts.contains("preferred")
              ? opts.get<std::shared_ptr<Heuristic>>("preferred")
              : nullptr)
    , dead_end_identifier_(
          opts.contains("dead_end_eval")
              ? opts.get<std::shared_ptr<Heuristic>>("dead_end_eval")
              : nullptr)
    , dead_end_listener_(
          opts.contains("on_dead_end_scc")
              ? opts.get<
                  std::shared_ptr<state_component::StateComponentListener>>(
                  "on_dead_end_scc")
              : nullptr)
    , eval_value_(0)
    , is_preferred_(g_operators.size(), false)
    , search_space_()
    , open_states_(0)
    , current_depth_(0)
    , current_index_(0)
    , backtracked_(false)
    , result_(DFSResult::Failed)
{
    compute_recognized_neighbors_ = dead_end_listener_ != nullptr
        && dead_end_listener_->requires_recognized_neighbors_explicitly();
    if (guidance_) {
        guidance_->get_involved_heuristics(path_dependent_evaluators_);
    }
    if (preferred_) {
        preferred_->get_involved_heuristics(path_dependent_evaluators_);
    }
    if (dead_end_identifier_) {
        dead_end_identifier_->get_involved_heuristics(
            path_dependent_evaluators_);
    }

    num_backtracks_ = 0;
    num_dead_end_components_ = 0;
    num_dead_end_component_notifications_ = 0;
    sum_dead_end_component_sizes_ = 0;
    sum_notified_dead_end_component_sizes_ = 0;

    last_progress_print_ = 1;
}

void
TarjanDepthOrientedSearch::initialize()
{
    std::cout << "Initializing tarjan search ..." << std::endl;
    std::cout << (compute_recognized_neighbors_ ? "M" : "Not m")
              << "aintaining recognized neighbors." << std::endl;

    current_index_ = 0;
    backtracked_ = false;
    result_ = DFSResult::Failed;
    open_states_ = 1;
    current_depth_ = 0;

    const GlobalState istate = state_registry->get_initial_state();

    if (::test_goal(istate)) {
        std::cout << "Initial state satisfies goal!" << std::endl;
        result_ = DFSResult::Solved;
    } else {
        SearchNode inode = search_space_[istate];
        if (!evaluate(istate)) {
            std::cout << "Initial state is dead-end!" << std::endl;
            inode.mark_recognized_dead_end();
        } else {
            inode.open_initial_state(get_h_value());
            if (!expand(istate)) {
                search_progress.inc_dead_ends();
                std::cout << "Initial state is dead-end!" << std::endl;
            }
        }
    }
}

bool
TarjanDepthOrientedSearch::evaluate(const GlobalState& state)
{
    return evaluate(state, guidance_);
}

bool
TarjanDepthOrientedSearch::evaluate(
    const GlobalState& state,
    std::shared_ptr<Heuristic> eval)
{
    search_progress.inc_evaluations();
    if (eval == nullptr) {
        eval_value_ = 0;
        return true;
    }
    eval_value_ = static_cast<int>(~((unsigned)3 << 30));
    eval->evaluate(state);
    if (eval->is_dead_end()) {
        return false;
    }
    eval_value_ = eval->get_heuristic();
    return true;
}

bool
TarjanDepthOrientedSearch::evaluate_dead_end_heuristic(const GlobalState& state)
{
    if (dead_end_identifier_ == nullptr) {
        return false;
    }
    dead_end_identifier_->evaluate(state);
    return dead_end_identifier_->is_dead_end();
}

int
TarjanDepthOrientedSearch::get_h_value() const
{
    return eval_value_;
}

void
TarjanDepthOrientedSearch::get_preferred_operators()
{
    if (preferred_) {
        std::fill(is_preferred_.begin(), is_preferred_.end(), false);
        std::vector<const GlobalOperator*> ops;
        preferred_->get_preferred_operators(ops);
        for (int i = ops.size() - 1; i >= 0; i--) {
            const unsigned idx =
                static_cast<unsigned>(ops[i] - (&g_operators[0]));
            is_preferred_[idx] = true;
        }
    }
}

bool
TarjanDepthOrientedSearch::is_preferred(const GlobalOperator* op) const
{
    const unsigned idx = static_cast<unsigned>(op - (&g_operators[0]));
    return is_preferred_[idx];
}

bool
TarjanDepthOrientedSearch::handle_transition_check_open(
    CallStackElement& info,
    SearchNode& node,
    SearchNode& successor)
{
    assert(!successor.is_new());
    if (successor.is_recognized_dead_end()) {
        if (compute_recognized_neighbors_) {
            recognized_neighbors_.push_back(successor.get_state_id());
        }
    } else if (successor.is_dead_end()) {
        assert(info.succ_result <= DFSResult::BacktrackedUnrecognized);
        info.succ_result = DFSResult::BacktrackedUnrecognized;
    } else if (successor.is_onstack()) {
        node.update_lowlink(successor.get_index());
    } else {
        assert(!successor.is_closed());
        return true;
    }
    return false;
}

bool
TarjanDepthOrientedSearch::expand(const GlobalState& state)
{
    SearchNode node = search_space_[state];
    assert(!node.is_dead_end() && !node.is_closed());
    open_states_--;

    if (evaluate_dead_end_heuristic(state)) {
        node.mark_recognized_dead_end();
        return false;
    }

    search_progress.inc_expanded();

    node.close(current_index_);
    current_index_++;
    stack_.push_front(state);
    call_stack_.emplace_back(
        state.get_id(), open_list_.size(), recognized_neighbors_.size());

    std::vector<const GlobalOperator*> aops;

    using Key = std::pair<bool, int>;
    using PriorityList = std::map<Key, std::deque<StateID>>;
    PriorityList successors;

    g_successor_generator->generate_applicable_ops(state, aops);
    search_progress.inc_generated_ops(aops.size());
    search_progress.inc_generated(aops.size());
    get_preferred_operators();

    for (unsigned i = 0; i < aops.size(); i++) {
        GlobalState succ = state_registry->get_successor_state(state, *aops[i]);
        for (Heuristic* pde : path_dependent_evaluators_) {
            pde->reach_state(state, *aops[i], succ);
        }
        SearchNode succ_node = search_space_[succ];
        // assert(
        //     succ_node.is_new()
        //     || succ_node.get_lowlink() <= succ_node.get_index());
        if (succ_node.is_new()) {
            search_progress.inc_evaluated_states();
            if (::test_goal(succ)) {
                Plan plan;
                search_space_.trace_path(node, plan);
                plan.push_back(aops[i]);
                set_plan(plan);
                result_ = DFSResult::Solved;
                successors.clear();
                break;
            }
            if (evaluate(succ) || consider_eval_dead_ends_) {
                succ_node.open(node, aops[i], get_h_value());
                open_states_++;
            } else {
                if (eval_dead_ends_compatible_) {
                    search_progress.inc_dead_ends();
                    succ_node.mark_recognized_dead_end();
                } else {
                    search_progress.inc_dead_ends();
                    succ_node.mark_dead_end();
                }
            }
        }
        if (handle_transition_check_open(call_stack_.back(), node, succ_node)) {
            const Key key(!is_preferred(aops[i]), succ_node.get_h());
            successors[key].push_front(succ.get_id());
        }
    }

    for (auto it = successors.rbegin(); it != successors.rend(); it++) {
        open_list_.insert(
            open_list_.end(), it->second.begin(), it->second.end());
    }

    current_depth_++;
    return true;
}

SearchStatus
TarjanDepthOrientedSearch::step()
{
    report_progress();

    if (result_ == DFSResult::Solved) {
        return SOLVED;
    }

    if (call_stack_.empty()) {
        return FAILED;
    }

    CallStackElement& elem = call_stack_.back();
    GlobalState elem_state = state_registry->lookup_state(elem.state_id);
    SearchNode elem_node = search_space_[elem_state];

    bool already_recognized = false;
    // if backtracked
    if (backtracked_) {
        assert(last_successor_node_.is_closed());
        elem_node.update_lowlink(last_successor_node_.get_lowlink());
        elem.succ_result = std::max(elem.succ_result, result_);
        assert(
            !last_successor_node_.is_dead_end()
            || elem.succ_result == DFSResult::BacktrackedUnrecognized
            || last_successor_node_.is_recognized_dead_end());
        assert(
            result_ != DFSResult::Failed || last_successor_node_.is_onstack());
        if (last_successor_node_.is_recognized_dead_end()) {
            if (result_ == DFSResult::DeadEndSCC
                || (result_ == DFSResult::BacktrackedSCC && recompute_u_
                    && evaluate_dead_end_heuristic(elem_state))) {
                already_recognized = true;
                elem_node.mark_recognized_dead_end();
                while (open_list_.size() > elem.open_list_offset) {
                    StateID state_id = open_list_.back();
                    open_list_.pop_back();
                    SearchNode node =
                        search_space_[state_registry->lookup_state(state_id)];
                    if (!node.is_closed()) {
                        node.mark_recognized_dead_end();
                        search_progress.inc_dead_ends();
                    } else if (node.is_onstack()) {
                        elem_node.update_lowlink(node.get_index());
                    } else {
                        assert(node.is_dead_end());
                        node.mark_recognized_dead_end();
                    }
                }
            } else if (compute_recognized_neighbors_) {
                recognized_neighbors_.push_back(
                    last_successor_node_.get_state_id());
            }
        }
    }

    backtracked_ = true;
    while (open_list_.size() > elem.open_list_offset) {
        StateID succ_id = open_list_.back();
        open_list_.pop_back();
        GlobalState succ = state_registry->lookup_state(succ_id);
        SearchNode succ_node = search_space_[succ];

        if (handle_transition_check_open(elem, elem_node, succ_node)) {
            bool is_alive = expand(succ);
            if (is_alive) {
                backtracked_ = false;
                break;
            } else {
                if (compute_recognized_neighbors_) {
                    assert(succ_node.is_recognized_dead_end());
                    recognized_neighbors_.push_back(succ_id);
                }
                search_progress.inc_dead_ends();
            }
        }
    }

    if (backtracked_) {
        if (already_recognized) {
            result_ = DFSResult::DeadEndSCC;
        } else if (elem.succ_result == DFSResult::BacktrackedUnrecognized) {
            result_ = DFSResult::BacktrackedUnrecognized;
        } else {
            result_ = DFSResult::Failed;
        }

        assert(elem_node.get_lowlink() <= elem_node.get_index());
        if (elem_node.get_index() == elem_node.get_lowlink()) {
            size_t scc_size = 0;

            auto stack_it = stack_.begin();
            while (true) {
                scc_size++;
                SearchNode snode = search_space_[*stack_it];
                snode.popped_from_stack();
                if (already_recognized) {
                    /* progress_.inc_partially_expanded_dead_ends(); */
                    snode.mark_recognized_dead_end();
                } else {
                    assert(!snode.is_dead_end());
                    snode.mark_dead_end();
                    /* progress_.inc_expanded_dead_ends(); */
                }
                if ((stack_it++)->get_id() == elem_node.get_state_id()) {
                    break;
                }
            }

            num_dead_end_components_++;
            sum_dead_end_component_sizes_ += scc_size;

            bool dead_end_notification = identify_dead_end_components_
                && !already_recognized
                && (open_states_ > 0 || !ignore_initial_state_)
                && (elem.succ_result != DFSResult::BacktrackedUnrecognized
                    || !dead_end_listener_
                            ->requires_recognized_neighbors_implicitly());
            if (dead_end_notification) {
                num_dead_end_component_notifications_++;
                sum_notified_dead_end_component_sizes_ += scc_size;

                // progress_.inc_dead_end_refinements();
                StateSet recognized_neighbors;
                if (compute_recognized_neighbors_) {
                    auto rnid_it =
                        recognized_neighbors_.begin() + elem.rn_offset;
                    while (rnid_it != recognized_neighbors_.end()) {
                        recognized_neighbors.insert(
                            state_registry->lookup_state(*rnid_it));
                        rnid_it++;
                    }
                }
#if NDEBUG_VERIFY_RECOGNIZED_NEIGHBORS
                {
                    StateSet component;
                    auto compit = stack_.begin();
                    while (compit != stack_it) {
                        component.insert(*compit);
                        compit++;
                    }
                    std::vector<const GlobalOperator*> aops;
                    compit = stack_.begin();
                    while (compit != stack_it) {
                        g_successor_generator->generate_applicable_ops(
                            *compit, aops);
                        for (unsigned i = 0; i < aops.size(); i++) {
                            GlobalState succ =
                                state_registry->get_successor_state(
                                    *compit, *aops[i]);
                            /*
                            if (!component.count(succ)
                                && !recognized_neighbors.count(succ)) {
                                SearchNode succnode = search_space_[succ];
                                std::cout
                                    << "STATE NOT FOUND! " << succ.get_id()
                                    << ": " << succnode.is_new() << "|"
                                    << succnode.is_closed() << "|"
                                    << succnode.is_dead_end() << "|"
                                    << succnode.is_onstack() << " "
                                    << succnode.get_index() << " "
                                    << succnode.get_lowlink() << std::endl;
                            }*/
                            assert(
                                component.count(succ)
                                || !compute_recognized_neighbors_
                                || recognized_neighbors.count(succ));
                            assert(
                                component.count(succ)
                                || search_space_[succ].is_dead_end());
                            if (dead_end_listener_
                                    ->requires_recognized_neighbors_implicitly()) {
                                assert(
                                    component.count(succ)
                                    || (search_space_[succ]
                                            .is_recognized_dead_end()));
                                assert(
                                    component.count(succ)
                                    || evaluate_dead_end_heuristic(succ));
                            }
                        }
                        aops.clear();
                        compit++;
                    }
                }
#endif

                // std::cout << "Refinement " << (scc_size) << " -> ";
                // print_progress();

                state_component::StateComponentListener::Status res =
                    dead_end_listener_->notify_new_component(
                        state_component::StateComponentIterator<decltype(
                            stack_it)>(stack_.begin(), stack_it),
                        state_component::StateComponentIterator<
                            StateSet::iterator>(
                            recognized_neighbors.begin(),
                            recognized_neighbors.end()));

                // std::cout << "  => result = " << ((int) res) << std::endl;

                dead_end_notification = false;
                if (res
                    == state_component::StateComponentListener::Status::Mark) {
                    dead_end_notification = true;
                    for (auto compit = stack_.begin(); compit != stack_it;
                         compit++) {
#if NDEBUG_VERIFY_REFINEMENT
                        assert(evaluate_dead_end_heuristic(*compit));
#endif
                        assert(search_space_[*compit].is_dead_end());
                        search_space_[*compit].mark_recognized_dead_end();
                    }
                } else if (
                    res
                        == state_component::StateComponentListener::Status::
                            StopListening
                    || stop_dead_end_notification_) {
                    std::cout << "Stopping dead-end notification!" << std::endl;
                    identify_dead_end_components_ = false;
                    if (compute_recognized_neighbors_) {
                        compute_recognized_neighbors_ = false;
                        recognized_neighbors_.clear();
                    }
                }
            }

            stack_.erase(stack_.begin(), stack_it);
            if (compute_recognized_neighbors_) {
                recognized_neighbors_.erase(
                    recognized_neighbors_.begin() + elem.rn_offset,
                    recognized_neighbors_.end());
            }

            result_ = (already_recognized || dead_end_notification)
                ? DFSResult::BacktrackedSCC
                : DFSResult::BacktrackedUnrecognized;
        }

        last_successor_node_ = elem_node;

        call_stack_.pop_back();
        current_depth_--;

        num_backtracks_++;
    }

    return IN_PROGRESS;
}

void
TarjanDepthOrientedSearch::report_progress()
{
    if (2 * last_progress_print_ < state_registry->size()) {
        last_progress_print_ = state_registry->size();
        print_progress();
    }
}

void
TarjanDepthOrientedSearch::print_progress() const
{
    std::cout << "["
              << "registered="
              << state_registry->size()
              // << ", expansions=" << statistics.get_expanded()
              << ", open=" << open_list_.size() << "(" << open_states_
              << ")"
              // << ", dead_ends=" << statistics.get_dead_ends()
              << ", backtracked=" << num_dead_end_components_ << "("
              << num_dead_end_component_notifications_ << ")"
              << ", d=" << call_stack_.size() << ", t=" << utils::g_timer
              << ", m=" << utils::get_peak_memory_in_kb() << "KB"
              << "]" << std::endl
              << std::flush;
}

void
TarjanDepthOrientedSearch::print_statistics() const
{
    std::cout << "**** Depth-Oriented Search ****" << std::endl;
    SearchEngine::print_statistics();
    search_progress.print_statistics();
    std::cout << "Registered: " << state_registry->size() << " state(s)"
              << std::endl;
    std::cout << "Number of search backtracks: " << num_backtracks_
              << std::endl;
    std::cout << "Explored dead-end components: " << num_dead_end_components_
              << std::endl;
    std::cout << "Dead-end component notifications: "
              << num_dead_end_component_notifications_ << std::endl;
    std::cout << "Average dead-end component size: "
              << (((double)sum_dead_end_component_sizes_)
                  / (double)num_dead_end_components_)
              << std::endl;
    std::cout << "Average notification dead-end component size: "
              << (((double)sum_notified_dead_end_component_sizes_)
                  / (double)num_dead_end_component_notifications_)
              << std::endl;
    if (dead_end_listener_ != nullptr) {
        dead_end_listener_->print_statistics();
    }
}

bool
TarjanDepthOrientedSearch::is_dead_end_notification_enabled() const
{
    return identify_dead_end_components_;
}

void
TarjanDepthOrientedSearch::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>(
        "eval", "", options::OptionParser::NONE);
    parser.add_option<std::shared_ptr<Heuristic>>(
        "preferred", "", options::OptionParser::NONE);
    parser.add_option<std::shared_ptr<Heuristic>>(
        "dead_end_eval", "", options::OptionParser::NONE);
    parser.add_option<std::shared_ptr<state_component::StateComponentListener>>(
        "on_dead_end_scc", "", options::OptionParser::NONE);
    parser.add_option<bool>("stop_dead_end_notification", "", "false");
    parser.add_option<bool>(
        "recompute_u",
        "recompute dead-end detection heuristic after each refinement",
        "true");
    parser.add_option<bool>("ignore_initial_state", "", "true");
    parser.add_option<bool>("eval_dead_ends_compatible", "", "true");
    parser.add_option<bool>("consider_eval_dead_ends", "", "false");
    SearchEngine::add_options_to_parser(parser);
}

static std::shared_ptr<SolverInterface>
_parse(options::OptionParser& parser)
{
    TarjanDepthOrientedSearch::add_options_to_parser(parser);
    options::Options opts = parser.parse();
    if (!parser.dry_run()) {
        return std::make_shared<TarjanDepthOrientedSearch>(opts);
    }
    return nullptr;
}

static Plugin<SolverInterface> _plugin("dfs", _parse);

} // namespace tarjan_search
