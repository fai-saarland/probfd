#include "probfd/heuristics/probability_aware_pdb_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/cost_function.h"
#include "probfd/task_heuristic_factory.h"

#include "downward/pdbs/dominance_pruning.h"

#include "downward/utils/logging.h"

#include <algorithm>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

ProbabilityAwarePDBHeuristic::ProbabilityAwarePDBHeuristic(
    PPDBCollection pdbs,
    std::vector<PatternSubCollection> subcollections,
    std::shared_ptr<SubCollectionFinder> subcollection_finder,
    value_t termination_cost)
    : pdbs_(std::move(pdbs))
    , subcollections_(std::move(subcollections))
    , subcollection_finder_(std::move(subcollection_finder))
    , termination_cost_(termination_cost)
{
}

value_t ProbabilityAwarePDBHeuristic::evaluate(const State& state) const
{
    return subcollection_finder_
        ->evaluate(pdbs_, subcollections_, state, termination_cost_);
}

ProbabilityAwarePDBHeuristicFactory::ProbabilityAwarePDBHeuristicFactory(
    std::shared_ptr<PatternCollectionGenerator> generator,
    double max_time_dominance_pruning,
    utils::Verbosity verbosity)
    : generator_(std::move(generator))
    , max_time_dominance_pruning_(max_time_dominance_pruning)
    , log_(get_log_for_verbosity(verbosity))
{
}

std::unique_ptr<FDREvaluator>
ProbabilityAwarePDBHeuristicFactory::create_heuristic(
    const SharedProbabilisticTask& task)
{
    const utils::Timer construction_timer;

    const utils::Timer generator_timer;
    auto pattern_collection_info = generator_->generate(task);
    const double generator_time = generator_timer();

    std::vector<Pattern> patterns = pattern_collection_info.get_patterns();

    auto pdbs = std::move(pattern_collection_info.get_pdbs());
    auto subcollections =
        std::move(pattern_collection_info.get_subcollections());
    auto subcollection_finder =
        pattern_collection_info.get_subcollection_finder();

    double dominance_pruning_time = 0.0;

    if (max_time_dominance_pruning_ > 0.0) {
        const auto& variables = get_variables(task);

        const utils::Timer timer;

        ::pdbs::prune_dominated_cliques(
            patterns,
            pdbs,
            subcollections,
            static_cast<int>(variables.size()),
            max_time_dominance_pruning_,
            log_);

        dominance_pruning_time = timer();
    }

    if (log_.is_at_least_normal()) {
        // Gather statistics.
        const double construction_time = construction_timer();

        size_t largest_pattern = 0;
        size_t variables = 0;
        size_t abstract_states = 0;

        for (const auto& pdb : pdbs) {
            size_t vars = pdb->get_pattern().size();
            largest_pattern = std::max(largest_pattern, vars);
            variables += vars;
            abstract_states += pdb->num_states();
        }

        size_t total_subcollections_size = 0;

        for (const auto& subcollection : subcollections) {
            total_subcollections_size += subcollection.size();
        }

        const double avg_variables =
            static_cast<double>(variables) / static_cast<double>(pdbs.size());
        const double avg_abstract_states =
            static_cast<double>(abstract_states) /
            static_cast<double>(pdbs.size());

        const double avg_subcollection_size =
            static_cast<double>(total_subcollections_size) /
            static_cast<double>(subcollections.size());

        log_ << "\n"
             << "Pattern Databases Statistics:\n"
             << "  Total number of PDBs: " << pdbs.size() << "\n"
             << "  Total number of variables: " << variables << "\n"
             << "  Total number of abstract states: " << abstract_states << "\n"
             << "  Average number of variables per PDB: " << avg_variables
             << "\n"
             << "  Average number of abstract states per PDB: "
             << avg_abstract_states << "\n"

             << "  Largest pattern size: " << largest_pattern << "\n"

             << "  Total number of subcollections: " << subcollections.size()
             << "\n"
             << "  Total number of subcollection PDBs: "
             << total_subcollections_size << "\n"
             << "  Average size of subcollection PDBs: "
             << avg_subcollection_size << "\n"

             << "  Generator time: " << generator_time << "s\n"
             << "  Dominance pruning time: " << dominance_pruning_time << "s\n"
             << "  Total construction time: " << construction_time << "s\n";
    }

    const auto& term_costs = get_termination_costs(task);

    return std::make_unique<ProbabilityAwarePDBHeuristic>(
        std::move(pdbs),
        std::move(subcollections),
        std::move(subcollection_finder),
        term_costs.get_non_goal_termination_cost());
}

} // namespace probfd::heuristics
