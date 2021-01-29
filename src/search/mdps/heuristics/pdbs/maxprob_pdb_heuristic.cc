#include "maxprob_pdb_heuristic.h"

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
#include "utils.h"

#include <cassert>
#include <iostream>
#include <string>

namespace probabilistic {
namespace pdbs {

using ::pdbs::PatternCollectionGenerator;
using ::pdbs::PatternCollectionInformation;

namespace {
void dump_projection(std::size_t i, ProbabilisticProjection& projection) {
    const bool print_transition_labels = true;
    const bool print_values = true;

    std::ostringstream path;
    path << "pattern" << i << ".dot";

    dump_graphviz(
        &projection,
        g_analysis_objective.get(),
        path.str(),
        print_transition_labels,
        print_values);
}
}

struct MaxProbPDBHeuristic::ProjectionInfo {
    ProjectionInfo(
        std::shared_ptr<AbstractStateMapper> state_mapper,
        AbstractAnalysisResult& result);

    std::shared_ptr<AbstractStateMapper> state_mapper;
    std::unique_ptr<QuantitativeResultStore> values;
    std::unique_ptr<QualitativeResultStore> dead_ends;
    std::unique_ptr<QualitativeResultStore> one_states;

    [[nodiscard]] value_type::value_t
    lookup(const AbstractState& s) const;
};

MaxProbPDBHeuristic::ProjectionInfo::ProjectionInfo(
    std::shared_ptr<AbstractStateMapper> state_mapper,
    AbstractAnalysisResult& result)
    : state_mapper(std::move(state_mapper))
    , values(result.value_table)
    , dead_ends(result.dead_ends)
    , one_states(result.one_states)
{
}

void MaxProbPDBHeuristic::Statistics::dump(std::ostream& out) const {
    out << "MaxProb-PDB initialization complete: " << std::endl;
    out << "  Initialization time: " << init_time << std::endl;
    out << "  Stored " << stored_projections << "/" << total_projections
        << " projections (" << deterministic_projections
        << " are non-probabilistic)" << std::endl;
    out << "  Abstract states: " << abstract_states << " ("
        << abstract_reachable_states << " reachable, "
        << abstract_dead_ends << " dead ends, "
        << abstract_one_states << " one states)" << std::endl;
}

MaxProbPDBHeuristic::MaxProbPDBHeuristic(
    const options::Options& opts)
{
    ::verify_no_axioms_no_conditional_effects();

    g_log << "Initializing Probabilistic PDB Heuristic..." << std::endl;
    utils::Timer t_init;

    auto patterns_generator =
        opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns");
    auto patterns_info = patterns_generator->generate(NORMAL);

    const PatternCollection& patterns = *patterns_info.get_patterns();

    database_.reserve(patterns.size());

    const bool countdown_enabled = opts.get<double>("time_limit") > 0;
    const unsigned max_states = opts.get<int>("max_states");
    utils::CountdownTimer countdown(opts.get<double>("time_limit"));

    bool terminate = false;

    for (unsigned i = 0; i < patterns.size() || terminate; i++) {
        const Pattern& p = patterns[i];

        if (countdown_enabled && countdown.is_expired()) {
            break;
        }

        ProbabilisticProjection projection(p, ::g_variable_domain);

        auto state_mapper = projection.get_abstract_state_mapper();

        if (max_states - state_mapper->size() < statistics_.abstract_states) {
            terminate = true;
        }

        if (opts.get<bool>("dump_projections")) {
            dump_projection(i, projection);
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
        dump_pattern(g_log, i, p);
#endif

        if (initial_state_is_dead_end_) {
            database_.clear();
#if defined(NDEBUG)
            dump_pattern_short(g_log, i, p);
#endif
            g_log << " identifies initial state as dead-end!" << std::endl;
            delete (result.value_table);
            delete (result.one_states);
            delete (result.dead_ends);
            break;
        }

        if (result.one == result.reachable_states) {
#ifndef NDEBUG
            g_debug << " **trivial projection** ~~> estimate(s0) = "
                    << result.one_state_reward << std::endl;
#endif
            delete (result.value_table);
            delete (result.one_states);
            delete (result.dead_ends);
            continue;
        } else if (result.one + result.dead == result.reachable_states) {
#ifndef NDEBUG
            g_debug << " **deterministic projection**";
#endif
            ++statistics_.deterministic_projections;
            delete (result.value_table);
            delete (result.one_states);
            result.value_table = nullptr;
            result.one_states = nullptr;
        }

        database_.emplace_back(state_mapper, result);

        statistics_.abstract_states += state_mapper->size();
        statistics_.abstract_reachable_states += result.reachable_states;
        statistics_.abstract_dead_ends += result.dead;
        statistics_.abstract_one_states += result.one;

#ifndef NDEBUG
        const auto eval = database_.back().lookup(s0);
        g_debug << " ~~> estimate(s0) = " << eval << std::endl;
#endif
    }

    statistics_.total_projections = patterns.size();
    statistics_.stored_projections = database_.size();
    statistics_.init_time = t_init();

    statistics_.dump(g_log);

    auto eval = MaxProbPDBHeuristic::evaluate(g_initial_state());
    g_log << "  Initial state value estimate: "
          << eval.operator value_type::value_t() << std::endl;
}

value_type::value_t
MaxProbPDBHeuristic::ProjectionInfo::lookup(const AbstractState& s) const
{
    assert(!dead_ends->get(s));

    if (one_states == nullptr && values == nullptr) { // Dead end projection
        return g_analysis_objective->max();
    }

    if (one_states != nullptr && one_states->get(s)) {
        return value_type::one;
    }

    assert(values);
    return values->get(s);
}

void
MaxProbPDBHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "patterns", "", "systematic(pattern_max_size=2)");
    parser.add_option<bool>("precompute_dead_ends", "", "false");
    parser.add_option<double>("time_limit", "", "0");
    parser.add_option<int>("max_states", "", "-1");
    parser.add_option<bool>("dump_projections", "", "false");
}

EvaluationResult
MaxProbPDBHeuristic::evaluate(const GlobalState& state)
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
        result = std::min(result, store.lookup(x));
    }

    return EvaluationResult(false, result);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "maxprob_pdb",
    options::parse<GlobalStateEvaluator, MaxProbPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
