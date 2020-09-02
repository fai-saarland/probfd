#pragma once

#include "../global_state.h"
#include "../heuristic.h"
#include "../search_engine.h"
#include "search_node.h"
#include "search_node_info.h"
#include "search_space.h"

#include <deque>
#include <memory>
#include <set>
#include <vector>

class GlobalOperator;

namespace state_component {
class StateComponentListener;
}

namespace tarjan_search {

using SearchSpace = SearchSpaceBase<SearchNodeInfo, SearchNode>;

class TarjanDepthOrientedSearch : public SearchEngine {
public:
    explicit TarjanDepthOrientedSearch(const options::Options& opts);

    virtual void print_statistics() const override;

    void report_progress();
    void print_progress() const;

    bool is_dead_end_notification_enabled() const;

    static void add_options_to_parser(options::OptionParser& parser);

protected:
    enum class DFSResult {
        Failed = 0,
        BacktrackedSCC = 1,
        BacktrackedUnrecognized = 2,
        DeadEndSCC = 3,
        Solved = 4,
    };

    struct CallStackElement {
        CallStackElement(
            StateID state_id,
            unsigned open_list_offset,
            unsigned rn_offset);

        const StateID state_id;
        const unsigned open_list_offset;
        const unsigned rn_offset;
        DFSResult succ_result;
    };

    virtual void initialize() override;
    virtual SearchStatus step() override;

    bool expand(const GlobalState& state);
    bool handle_transition_check_open(
        CallStackElement& info,
        SearchNode& node,
        SearchNode& successor);

    bool evaluate(const GlobalState& state);
    bool evaluate(const GlobalState& state, std::shared_ptr<Heuristic> eval);
    bool evaluate_dead_end_heuristic(const GlobalState& state);

    int get_h_value() const;
    void get_preferred_operators();
    bool is_preferred(const GlobalOperator* op) const;

    const bool recompute_u_;
    const bool ignore_initial_state_;
    const bool stop_dead_end_notification_;
    const bool eval_dead_ends_compatible_;
    const bool consider_eval_dead_ends_;
    bool identify_dead_end_components_;
    bool compute_recognized_neighbors_;

    std::shared_ptr<Heuristic> guidance_;
    std::shared_ptr<Heuristic> preferred_;
    std::set<Heuristic*> path_dependent_evaluators_;
    std::shared_ptr<Heuristic> dead_end_identifier_;
    std::shared_ptr<state_component::StateComponentListener> dead_end_listener_;

    int eval_value_;
    std::vector<bool> is_preferred_;

    SearchSpace search_space_;

    std::deque<StateID> open_list_;
    std::deque<CallStackElement> call_stack_;
    size_t open_states_;
    int current_depth_;

    unsigned current_index_;
    std::deque<GlobalState> stack_;
    bool backtracked_;
    DFSResult result_;
    SearchNode last_successor_node_;

    std::deque<StateID> recognized_neighbors_;

    size_t num_backtracks_;
    size_t num_dead_end_components_;
    size_t num_dead_end_component_notifications_;
    size_t sum_dead_end_component_sizes_;
    size_t sum_notified_dead_end_component_sizes_;

    unsigned last_progress_print_;
};

} // namespace tarjan_search
