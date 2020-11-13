#include "probabilistic_pdb_heuristic.h"

#include "../../../globals.h"
#include "../../../operator_cost.h"
#include "../../../option_parser.h"
#include "../../../pdbs/pattern_collection_information.h"
#include "../../../pdbs/pattern_generator.h"
#include "../../../plugin.h"
#include "../../../utils/countdown_timer.h"
#include "../../analysis_objective.h"
#include "../../globals.h"
#include "../../logging.h"
#include "probabilistic_projection.h"
#include "qualitative_result_store.h"

#include <cassert>
#include <iostream>
#include <string>

namespace probabilistic {
namespace pdbs {

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
    bool terminate = false;

    for (unsigned i = 0; i < patterns->size(); i++) {
        if (terminate || (countdown_enabled && countdown.is_expired())) {
            break;
        }
        ProbabilisticProjection projection(
            patterns->at(i), ::g_variable_domain);
        auto state_mapper = projection.get_abstract_state_mapper();

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
            delete (result.value_table);
            delete (result.one_states);
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
            delete (result.value_table);
            delete (result.one_states);
            result.value_table = nullptr;
            result.one_states = nullptr;
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
        for (ProjectionInfo& info : database_) {
            auto state = info.state_mapper->operator()(g_initial_state_values);
            const value_type::value_t val = lookup(info, state);
            min_val = std::min(min_val, val);
        }
        g_log << min_val;
    }
    g_log << std::endl;
}

value_type::value_t
ProbabilisticPDBHeuristic::lookup(
    const ProjectionInfo& info,
    const AbstractState& s) const
{
    assert(!info.dead_ends->get(s));

    if (info.one_states == nullptr && info.values == nullptr) { // Dead end projection
        return g_analysis_objective->max();
    }

    if (info.one_states != nullptr && info.one_states->get(s)) {
        return one_state_reward_;
    }

    assert(info.values);
    return info.values->get(s);
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
    }

    value_type::value_t result = g_analysis_objective->max();
    for (const ProjectionInfo& store : database_) {
        const AbstractState x = store.state_mapper->operator()(state);
        if (store.dead_ends->get(x)) {
            return EvaluationResult(true, g_analysis_objective->min());
        }
        const value_type::value_t estimate = lookup(store, x);
        result = std::min(result, estimate);
    }

    return EvaluationResult(false, result);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "ppdb",
    options::parse<GlobalStateEvaluator, ProbabilisticPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
