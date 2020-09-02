#include "critical_path_heuristic_nogoods.h"

#include "../global_state.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../partial_state.h"
#include "../plugin.h"
#include "../strips.h"
#include "critical_path_heuristic.h"

#include <algorithm>
#include <cassert>

namespace critical_path_heuristic {

CriticalPathHeuristicNoGoods::CriticalPathHeuristicNoGoods(
    const options::Options& opts)
    : Heuristic(opts)
    , hc_(std::dynamic_pointer_cast<CriticalPathHeuristic>(
          opts.get<std::shared_ptr<Heuristic>>("heuristic")))
    , task_(hc_->get_task())
    , nogoods_(task_->num_facts())
{
    refinement_time_.stop();
    evaluation_time_.stop();
    evaluations_ = 0;
    saved_heuristic_computations_ = 0;
    refinements_ = 0;
}

void
CriticalPathHeuristicNoGoods::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("heuristic");
    Heuristic::add_options_to_parser(parser);
}

bool
CriticalPathHeuristicNoGoods::supports_partial_state_evaluation() const
{
    return true;
}

std::shared_ptr<CriticalPathHeuristic>
CriticalPathHeuristicNoGoods::get_underlying_heuristic() const
{
    return hc_;
}

int
CriticalPathHeuristicNoGoods::compute_heuristic(const GlobalState& state)
{
    evaluations_++;
    evaluation_time_.resume();
    strips_utils::FactSet state_facts = task_->get_fact_set(state);
    if (nogoods_.contains_superset(state_facts)) {
        evaluation_time_.stop();
        saved_heuristic_computations_++;
#ifndef NDEBUG
        hc_->evaluate(state);
        assert(hc_->is_dead_end());
#endif
        return DEAD_END;
    }
    evaluation_time_.stop();
    hc_->evaluate(state);
    if (hc_->is_dead_end()) {
        update_nogoods(state, state_facts);
        return DEAD_END;
    }
    return hc_->get_heuristic();
}

int
CriticalPathHeuristicNoGoods::compute_heuristic(const PartialState& state)
{
    evaluations_++;
    evaluation_time_.resume();
    strips_utils::FactSet state_facts = task_->get_fact_set(state);
    if (nogoods_.contains_superset(state_facts)) {
        evaluation_time_.stop();
        saved_heuristic_computations_++;
#ifndef NDEBUG
        hc_->evaluate(state);
        assert(hc_->is_dead_end());
#endif
        return DEAD_END;
    }
    evaluation_time_.stop();
    hc_->evaluate(state);
    if (hc_->is_dead_end()) {
        refinements_++;
        refinement_time_.resume();
        for (int var = g_variable_domain.size() - 1; var >= 0; --var) {
            if (!state.is_defined(var)) {
                continue;
            }
            unsigned size = state_facts.size();
            strips_utils::FactSet extended_state(state_facts);
            for (int val = 0; val < g_variable_domain[var]; val++) {
                if (state[var] != val) {
                    extended_state.push_back(task_->get_fact(var, val));
                }
            }
            std::inplace_merge(
                extended_state.begin(),
                extended_state.begin() + size,
                extended_state.end());
            std::vector<unsigned> conjunctions =
                hc_->get_satisfied_conjunctions(extended_state);
            unsigned i = 0;
            for (unsigned j = 0; j < conjunctions.size(); j++) {
                if (!hc_->is_conjunction_reachable(conjunctions[j])) {
                    if (i != j) {
                        conjunctions[i] = conjunctions[j];
                    }
                    i++;
                }
            }
            conjunctions.resize(i);
            if (conjunctions.empty()) {
                extended_state.swap(state_facts);
            } else {
                int res = hc_->incremental_exploration_unit_cost(conjunctions);
                if (res == DEAD_END) {
                    extended_state.swap(state_facts);
                } else {
                    hc_->reset_exploration_cache(conjunctions);
                }
            }
        }
        nogoods_.insert(state_facts);
        refinement_time_.stop();
        return DEAD_END;
    }
    return hc_->get_heuristic();
}

void
CriticalPathHeuristicNoGoods::update_nogoods(const GlobalState& state)
{
    strips_utils::FactSet state_facts = task_->get_fact_set(state);
    update_nogoods(state, state_facts);
}

void
CriticalPathHeuristicNoGoods::update_nogoods(
    const GlobalState& state,
    strips_utils::FactSet& state_facts)
{
    refinements_++;
    refinement_time_.resume();
    for (int var = g_variable_domain.size() - 1; var >= 0; --var) {
        unsigned size = state_facts.size();
        strips_utils::FactSet extended_state(state_facts);
        for (int val = 0; val < g_variable_domain[var]; val++) {
            if (state[var] != val) {
                extended_state.push_back(task_->get_fact(var, val));
            }
        }
        std::inplace_merge(
            extended_state.begin(),
            extended_state.begin() + size,
            extended_state.end());
        std::vector<unsigned> conjunctions =
            hc_->get_satisfied_conjunctions(extended_state);
        unsigned i = 0;
        for (unsigned j = 0; j < conjunctions.size(); j++) {
            if (!hc_->is_conjunction_reachable(conjunctions[j])) {
                if (i != j) {
                    conjunctions[i] = conjunctions[j];
                }
                i++;
            }
        }
        conjunctions.resize(i);
        if (conjunctions.empty()) {
            extended_state.swap(state_facts);
        } else {
            int res = hc_->incremental_exploration_unit_cost(conjunctions);
            if (res == DEAD_END) {
                extended_state.swap(state_facts);
            } else {
                hc_->reset_exploration_cache(conjunctions);
            }
        }
    }
    if (state_facts.size() > g_variable_domain.size()) {
        nogoods_.insert(state_facts);
    }
    refinement_time_.stop();
}

void
CriticalPathHeuristicNoGoods::print_statistics() const
{
    std::cout << "**** Critical-Path NoGoods Heuristic ****" << std::endl;
    std::cout << "Formula evaluations: " << evaluations_ << std::endl;
    std::cout << "Evaluated to true: " << saved_heuristic_computations_
              << std::endl;
    std::cout << "Formula refinements: " << refinements_ << std::endl;
    std::cout << "NoGoods: " << nogoods_.size() << std::endl;
    std::cout << "Formula evaluation time: " << evaluation_time_ << std::endl;
    std::cout << "Formula refinement time: " << refinement_time_ << std::endl;
}

static Plugin<Heuristic> _plugin(
    "hc_nogoods",
    options::parse<Heuristic, CriticalPathHeuristicNoGoods>);

} // namespace critical_path_heuristic
