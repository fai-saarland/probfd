#include "probfd/heuristics/pdb_max_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/heuristics/max_heuristic.h"

#include "probfd/cost_function.h"
#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

#include <algorithm>

using namespace downward;
using namespace probfd::pdbs;

using namespace std::chrono_literals;

namespace probfd::heuristics {

PDBMaxHeuristic::PDBMaxHeuristic(PPDBCollection pdbs, value_t termination_cost)
    : pdbs_(std::move(pdbs))
    , termination_cost_(termination_cost)
{
}

value_t PDBMaxHeuristic::evaluate(const State& state) const
{
    value_t result = -INFINITE_VALUE;

    for (const auto& pdb : pdbs_) {
        const value_t est = pdb->lookup_estimate(state);

        if (est == termination_cost_) { return est; }

        result = std::max(result, est);
    }

    return result;
}

PDBMaxHeuristicFactory::PDBMaxHeuristicFactory(
    std::shared_ptr<PatternCollectionGenerator> generator,
    utils::Verbosity verbosity)
    : generator_(std::move(generator))
    , log_(get_log_for_verbosity(verbosity))
{
}

std::unique_ptr<FDRHeuristic>
PDBMaxHeuristicFactory::create_object(const SharedProbabilisticTask& task)
{
    const utils::Timer construction_timer;

    const utils::Timer generator_timer;
    auto pattern_collection_info = generator_->generate(task);
    const utils::FSeconds generator_time = generator_timer();

    auto pdbs = std::move(pattern_collection_info.get_pdbs());

    utils::FSeconds dominance_pruning_time = 0s;

    if (log_.is_at_least_normal()) {
        // Gather statistics.
        const utils::FSeconds construction_time = construction_timer();

        size_t largest_pattern = 0;
        size_t variables = 0;
        size_t abstract_states = 0;

        for (const auto& pdb : pdbs) {
            size_t vars = pdb->get_pattern().size();
            largest_pattern = std::max(largest_pattern, vars);
            variables += vars;
            abstract_states += pdb->num_states();
        }

        const double avg_variables =
            static_cast<double>(variables) / static_cast<double>(pdbs.size());
        const double avg_abstract_states =
            static_cast<double>(abstract_states) /
            static_cast<double>(pdbs.size());

        log_.println();
        log_.println(
            "Pattern Databases Statistics:\n"
            "  Total number of PDBs: {}\n"
            "  Total number of variables: {}\n"
            "  Total number of abstract states: {}\n"
            "  Average number of variables per PDB: {}\n"
            "  Average number of abstract states per PDB: {}\n"
            "  Largest pattern size: {}\n"
            "  Generator time: {}\n"
            "  Dominance pruning time: {}\n"
            "  Total construction time: {}",
            pdbs.size(),
            variables,
            abstract_states,
            avg_variables,
            avg_abstract_states,
            largest_pattern,
            generator_time,
            dominance_pruning_time,
            construction_time);
    }

    const auto& term_costs = get_termination_costs(task);

    if (pdbs.empty()) {
        const auto& operators = get_operators(task);
        const auto& cost_function = get_cost_function(task);

        return std::make_unique<BlindHeuristic<State>>(
            operators,
            cost_function,
            term_costs);
    }

    return std::make_unique<PDBMaxHeuristic>(
        std::move(pdbs),
        term_costs.get_non_goal_termination_cost());
}

} // namespace probfd::heuristics
