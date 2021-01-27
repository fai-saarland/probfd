#include "expected_cost_pdb_heuristic.h"

#include "../../../../globals.h"
#include "../../../../operator_cost.h"
#include "../../../../option_parser.h"
#include "../../../../pdbs/pattern_collection_information.h"
#include "../../../../pdbs/pattern_generator.h"
#include "../../../../plugin.h"
#include "../../../../utils/countdown_timer.h"
#include "../../../analysis_objective.h"
#include "../../../globals.h"
#include "../../../logging.h"
#include "../probabilistic_projection.h"
#include "../../../analysis_objectives/expected_cost_objective.h"

#include <cassert>
#include <iostream>
#include <string>

namespace probabilistic {
namespace pdbs {

static void dumpProjection(unsigned int projection_id,
                           ProbabilisticProjection& projection)
{
    std::ostringstream path;
    path << "pattern" << projection_id << ".dot";
    const bool print_transition_labels = true;
    const bool print_values = true;
    dump_graphviz(
        &projection,
        g_analysis_objective.get(),
        path.str(),
        print_transition_labels,
        print_values);
}

ExpectedCostPDBHeuristic::ProjectionInfo::ProjectionInfo(
    std::shared_ptr<AbstractStateMapper> state_mapper,
    AbstractAnalysisResult& result)
    : state_mapper(std::move(state_mapper))
    , values(result.value_table)
{
}

ExpectedCostPDBHeuristic::ExpectedCostPDBHeuristic(
    const options::Options& opts)
{
    assert(dynamic_cast<ExpectedCostObjective*>(g_analysis_objective.get()));

    ::verify_no_axioms_no_conditional_effects();

    logging::out << "Initializing Probabilistic PDB Heuristic..." << std::endl;
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

    // Statistics.
    unsigned num_states = 0;
    unsigned num_reachable = 0;

    bool terminate = false;

    for (unsigned i = 0; i < patterns->size() && !terminate; i++) {
        const auto& p = patterns->operator[](i);

        // Check if the time ran out
        if (countdown_enabled && countdown.is_expired()) {
            break;
        }

        // Set up the projection state space
        ProbabilisticProjection projection(p, ::g_variable_domain);
        auto state_mapper = projection.get_abstract_state_mapper();

        // If we exceed max_states, this will be the last projection
        if (max_states - state_mapper->size() < num_states) {
            terminate = true;
        }

        if (opts.get<bool>("dump_projections")) {
            dumpProjection(i, projection);
        }

        // Compute the value table for this projection
        AbstractAnalysisResult result =
            compute_value_table(&projection, g_analysis_objective.get());

#ifndef NDEBUG
        {
            logging::out << "pattern[" << i << "]: vars = [";
            for (unsigned j = 0; j < p.size(); j++) {
                logging::out << (j > 0 ? ", " : "") << p[j];
            }
            logging::out << "] ({";
            for (unsigned j = 0; j < p.size(); j++) {
                logging::out << (j > 0 ? ", " : "") << ::g_fact_names[p[j]][0];
            }
            logging::out << "})" << std::flush;
        }
#endif

        // Add to the list of PDB heuristics.
        database_.emplace_back(state_mapper, result);

        // Update statistics.
        num_states += state_mapper->size();
        num_reachable += result.reachable_states;

#ifndef NDEBUG
        {
            AbstractState s0 = state_mapper->operator()(g_initial_state_values);

            assert(result.value_table && result.value_table->has_value(s0));

            logging::out << " ~~> estimate(s0) = "
                         << result.value_table->get(s0)
                         << std::endl;
        }
#endif
    }

    // Dump statistics.
    logging::out << "Expected-Cost PDB initialization completed after "
                 << t_init << " [t=" << ::utils::g_timer << "]" << std::endl;
    logging::out << "Stored " << database_.size() << " projections."
                 << std::endl;
    logging::out << "Abstract states: " << num_states << " (" << num_reachable
                 << " reachable)" << std::endl;

    const auto val = static_cast<value_type::value_t>(
        ExpectedCostPDBHeuristic::evaluate(g_initial_state()));

    logging::out << "Initial state value estimate: " << val << std::endl;
}

value_type::value_t
ExpectedCostPDBHeuristic::ProjectionInfo::lookup(const AbstractState& s) const
{
    assert(values);
    return values->get(s);
}

void
ExpectedCostPDBHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<::pdbs::PatternCollectionGenerator>>(
        "patterns", "", "systematic(pattern_max_size=2)");
    parser.add_option<double>("time_limit", "", "0");
    parser.add_option<int>("max_states", "", "-1");
    parser.add_option<bool>("dump_projections", "", "false");
}

EvaluationResult
ExpectedCostPDBHeuristic::evaluate(const GlobalState& state)
{
    if (database_.empty()) {
        return { false, g_analysis_objective->max() };
    }

    value_type::value_t result = g_analysis_objective->max();
    for (const ProjectionInfo& store : database_) {
        const AbstractState x = store.state_mapper->operator()(state);
        result = std::min(result, store.lookup(x));
    }

    return { false, result };
}

static Plugin<GlobalStateEvaluator> _plugin(
    "ecpdb",
    options::parse<GlobalStateEvaluator, ExpectedCostPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
