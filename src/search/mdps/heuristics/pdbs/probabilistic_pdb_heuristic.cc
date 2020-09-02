#include "probabilistic_pdb_heuristic.h"

#include "../../../global_state.h"
#include "../../../globals.h"
#include "../../../operator_cost.h"
#include "../../../option_parser.h"
#include "../../../pdbs/pattern_collection_information.h"
#include "../../../pdbs/pattern_generator.h"
#include "../../../plugin.h"
#include "../../../utils/countdown_timer.h"
#include "../../../utils/timer.h"
#include "../../analysis_objective.h"
#include "../../globals.h"
#include "../../logging.h"
#include "probabilistic_projection.h"
#include "qualitative_result_store.h"
#include "quantitative_result_store.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#ifndef NDEBUG
#define DEBUG_CHECK_ADMISSIBILITY 0
#endif

#if DEBUG_CHECK_ADMISSIBILITY
#include "../../../state_registry.h"
#include "../../algorithms/topological_value_iteration.h"
#include "../../task/applicable_actions_generator.h"
#include "../../task/property.h"
#include "../../task/state_valuation.h"
#include "../../task/state_value_estimator.h"
#include "../../task/storage.h"
#include "../../task/transition_generator.h"
#include "../../task/transition_valuation.h"
#include "../../task/types.h"

#include <unordered_map>
class Heuristic;
#endif

namespace probabilistic {
namespace pdbs {

#if DEBUG_CHECK_ADMISSIBILITY
std::unordered_map<StateID, probabilistic::value_type::value_t>
    g_reference_values;
#endif

ProbabilisticPDBHeuristic::ProjectionInfo::ProjectionInfo(
    std::shared_ptr<AbstractStateMapper> state_mapper,
    AbstractAnalysisResult& result)
    : state_mapper(state_mapper)
    , values(result.value_table)
    , dead_ends(result.dead_ends)
    , one_states(result.one_states)
{
}

ProbabilisticPDBHeuristic::ProbabilisticPDBHeuristic(
    const options::Options& opts)
    : initial_state_is_dead_end_(false)
{
#if DEBUG_CHECK_ADMISSIBILITY
    task::ApplicableActionsGenerator aops_gen(false);
    std::vector<std::shared_ptr<Heuristic>> aux;
    task::ProbabilisticTransitionGenerator tgen(aux, false);
    struct StatePruningFunction {
        bool operator()(const task::StateRef&) const { return false; }
    };
    StatePruningFunction prune;
    value_type::less compare;
    topological_vi::TopologicalValueIteration<
        task::StateIndexedVector,
        task::StateRef,
        task::OperatorRef,
        task::ApplicableActionsGenerator,
        task::ProbabilisticTransitionGenerator,
        task::StateValuation,
        task::TransitionValuation,
        StatePruningFunction,
        value_type::less>
        vi(aops_gen,
           tgen,
           *(g_property->get_state_valuation()),
           *(g_property->get_transition_valuation()),
           prune,
           compare,
           g_property->get_pessimistic_bound());

    task::StateRef sref = g_state_registry->get_initial_state().get_id();
    vi(sref, g_reference_values);
#endif

    g_log << "Initializing Probabilistic PDB Heuristic..." << std::endl;
    ::verify_no_axioms_no_conditional_effects();
    utils::Timer t_init;

    std::shared_ptr<::pdbs::PatternCollectionGenerator> patterns_generator =
        opts.get<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
            "patterns");
    ::pdbs::PatternCollectionInformation patterns_info =
        patterns_generator->generate(NORMAL);
    std::shared_ptr<::pdbs::PatternCollection> patterns =
        patterns_info.get_patterns();

    database_.reserve(patterns->size());

    const bool countdown_enabled = opts.get<double>("time_limit") > 0;
    const unsigned max_states = opts.get<int>("max_states");
    utils::CountdownTimer countdown(opts.get<double>("time_limit"));
    unsigned states = 0;
    // value_type::value_t ival = g_property->get_optimistic_bound();

    unsigned reachable = 0;
    unsigned dead_ends = 0;
    unsigned one_states = 0;
    unsigned deterministic = 0;

    for (unsigned i = 0; i < patterns->size(); i++) {
        ProbabilisticProjection projection(
            patterns->at(i), ::g_variable_domain);
        auto state_mapper = projection.get_abstract_state_mapper();

        bool terminate = false;
        if (max_states - state_mapper->size() < states) {
            terminate = true;
        }
        states += state_mapper->size();

        if (opts.get<bool>("dump_projections")) {
            std::ostringstream path;
            path << "pattern" << i << ".dot";
            const bool print_transition_labels = true;
            const bool print_values = true;
            dump_graphviz(
                &projection,
                g_analysis_objective.get(),
                path.str(),
                print_transition_labels,
                print_values);
        }

        AbstractState s0 = state_mapper->operator()(g_initial_state_values);
        AbstractAnalysisResult result;

        if (opts.get<bool>("precompute_dead_ends")) {
            QualitativeResultStore dead_ends = projection.compute_dead_ends();
            initial_state_is_dead_end_ = dead_ends.get(s0);
            if (!initial_state_is_dead_end_) {
                result = compute_value_table(
                    &projection, g_analysis_objective.get(), &dead_ends);
                result.dead_ends =
                    new QualitativeResultStore(std::move(dead_ends));
            }
        } else {
            result =
                compute_value_table(&projection, g_analysis_objective.get());
            initial_state_is_dead_end_ = result.dead_ends->get(s0);
        }

#ifndef NDEBUG
        {
            const auto& p = patterns->at(i);
            g_debug << "pattern[" << i << "]: vars = [";
            for (unsigned j = 0; j < p.size(); j++) {
                g_debug << (j > 0 ? ", " : "") << p[j];
            }
            g_debug << "] ({";
            for (unsigned j = 0; j < p.size(); j++) {
                g_debug << (j > 0 ? ", " : "") << ::g_fact_names[p[j]][0];
            }
            g_debug << "})" << std::flush;
        }
#endif

        if (initial_state_is_dead_end_) {
            database_.clear();
#if defined(NDEBUG)
            g_log << "Pattern [";
            const auto& p = patterns->at(i);
            for (unsigned j = 0; j < p.size(); j++) {
                g_log << (j > 0 ? ", " : "") << p[j];
            }
            g_log << "]";
#endif
            g_log << " identifies initial state as dead-end!" << std::endl;
            if (result.value_table)
                delete (result.value_table);
            if (result.one_states)
                delete (result.one_states);
            if (result.dead_ends)
                delete (result.dead_ends);
            break;
        }

        if (result.one == result.reachable_states) {
#ifndef NDEBUG
            g_debug << " **trivial projection**"
                    << " ~~> estimate(s0) = " << result.one_state_reward
                    << std::endl;
#endif
            delete (result.value_table);
            delete (result.one_states);
            delete (result.dead_ends);
            continue;
        } else if (result.one + result.dead == result.reachable_states) {
#ifndef NDEBUG
            g_debug << " **deterministic projection**";
#endif
            ++deterministic;
        }

        database_.emplace_back(state_mapper, result);

        reachable += result.reachable_states;
        dead_ends += result.dead;
        one_states += result.one;

        one_state_reward_ = result.one_state_reward;
#ifndef NDEBUG
        {
            assert(
                (result.one_states != nullptr && result.one_states->get(s0))
                || result.value_table->has_value(s0));
            g_debug << " ~~> estimate(s0) = "
                    << ((result.one_states != nullptr
                         && result.one_states->get(s0))
                            ? one_state_reward_
                            : result.value_table->get(s0))
                    << std::endl;
        }
#endif

        if (terminate || (countdown_enabled && countdown.is_expired())) {
            break;
        }
    }

    g_log << "probabilistic PDB initialization completed after " << t_init
          << " [t=" << ::utils::g_timer << "]" << std::endl;
    g_log << "Stored " << database_.size() << "/" << patterns->size()
          << " projections (" << deterministic << " are non-probabilistic)"
          << std::endl;
    g_log << "Abstract states: " << states << " (" << reachable
          << " reachable, " << dead_ends << " dead ends, " << one_states
          << " one states)" << std::endl;
    g_log << "Initial state value estimate: ";
    if (initial_state_is_dead_end_) {
        g_log << "dead-end";
    } else if (database_.empty()) {
        g_log << g_analysis_objective->max();
    } else {
        value_type::value_t min_val = g_analysis_objective->max();
        ProjectionInfo* info = &database_[0];
        for (int i = database_.size(); i > 0; --i, ++info) {
            auto state = info->state_mapper->operator()(g_initial_state_values);
            const value_type::value_t val = lookup(info, state);
            if (val < min_val) {
                min_val = val;
            }
        }
        g_log << min_val;
    }
    g_log << std::endl;
}

value_type::value_t
ProbabilisticPDBHeuristic::lookup(
    const ProjectionInfo* info,
    const AbstractState& s) const
{
    assert(!info->dead_ends->get(s));
    if (info->one_states != nullptr && info->one_states->get(s)) {
        return one_state_reward_;
    }
    return info->values->get(s);
}

void
ProbabilisticPDBHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "patterns", "", "systematic(pattern_max_size=2)");
    parser.add_option<bool>("precompute_dead_ends", "", "false");
    parser.add_option<double>("time_limit", "", "0");
    parser.add_option<int>("max_states", "", "-1");
    parser.add_option<bool>("dump_projections", "", "false");
}

EvaluationResult
ProbabilisticPDBHeuristic::evaluate(const GlobalState& state)
{
    if (initial_state_is_dead_end_) {
        return EvaluationResult(true, g_analysis_objective->min());
    } else if (database_.empty()) {
        return EvaluationResult(false, g_analysis_objective->max());
    }
    value_type::value_t result = g_analysis_objective->max();
    for (int i = database_.size() - 1; i >= 0; i--) {
        const auto& store = database_[i];
        const AbstractState x = (*store.state_mapper)(state);
        if (store.dead_ends->get(x)) {
            return EvaluationResult(true, g_analysis_objective->min());
        }
        const value_type::value_t estimate = lookup(&store, x);
#if DEBUG_CHECK_ADMISSIBILITY
        if (value_type::approx_less()(
                estimate, g_reference_values[state.get_id()])) {
            const auto& pattern = store.state_mapper->get_variables();
            std::cerr << "ERROR: Pattern #" << i
                      << " returned inadmissible estimation!" << std::endl;
            std::cerr << "Pattern = [";
            for (unsigned i = 0; i < pattern.size(); i++) {
                std::cerr << (i > 0 ? ", " : "") << pattern[i];
            }
            std::cerr << "]" << std::endl;
            std::cerr << "State: [";
            for (unsigned var = 0; var < g_fact_names.size(); var++) {
                std::cerr << (var > 0 ? ", " : "")
                          << g_fact_names[var][state[var]];
            }
            std::cerr << "]" << std::endl;
            std::cerr << "Abstract state: "
                      << (AbstractStateToString(store.state_mapper)(x))
                      << std::endl;
            std::cerr << "                [";
            for (unsigned i = 0; i < pattern.size(); i++) {
                std::cerr << (i > 0 ? ", " : "")
                          << g_fact_names[pattern[i]][state[pattern[i]]];
            }
            std::cerr << "]" << std::endl;
            std::cerr << "V* = " << (g_reference_values[state.get_id()])
                      << std::endl;
            std::cerr << "h = " << (estimate) << std::endl;
            assert(false);
        }
#endif
        if (estimate < result) {
            result = estimate;
        }
    }
    return EvaluationResult(false, result);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "ppdb",
    options::parse<GlobalStateEvaluator, ProbabilisticPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic

#undef DEBUG_CHECK_ADMISSIBILITY
