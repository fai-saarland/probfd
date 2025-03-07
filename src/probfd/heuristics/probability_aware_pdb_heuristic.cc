#include "probfd/heuristics/probability_aware_pdb_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/cost_function.h"
#include "probfd/task_evaluator_factory.h"

#include "downward/pdbs/dominance_pruning.h"

#include "downward/utils/logging.h"

#include <algorithm>

using namespace probfd::pdbs;

namespace probfd::heuristics {

ProbabilityAwarePDBHeuristic::ProbabilityAwarePDBHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    std::shared_ptr<PatternCollectionGenerator> generator,
    double max_time_dominance_pruning,
    utils::LogProxy log)
    : TaskDependentHeuristic(std::move(task), std::move(log))
    , termination_cost_(task_cost_function->get_non_goal_termination_cost())
{
    utils::Timer construction_timer;

    utils::Timer generator_timer;
    auto pattern_collection_info =
        generator->generate(task_, task_cost_function);
    const double generator_time = generator_timer();

    std::shared_ptr<std::vector<pdbs::Pattern>> patterns =
        pattern_collection_info.get_patterns();

    this->pdbs_ = pattern_collection_info.get_pdbs();
    this->subcollections_ = pattern_collection_info.get_subcollections();
    this->subcollection_finder_ =
        pattern_collection_info.get_subcollection_finder();

    double dominance_pruning_time = 0.0;

    if (max_time_dominance_pruning > 0.0) {
        utils::Timer timer;

        ::pdbs::prune_dominated_cliques(
            *patterns,
            *pdbs_,
            *subcollections_,
            static_cast<int>(task_proxy_.get_variables().size()),
            max_time_dominance_pruning,
            utils::g_log);

        dominance_pruning_time = timer();
    }

    if (log_.is_at_least_normal()) {
        // Gather statistics.
        const double construction_time = construction_timer();

        size_t largest_pattern = 0;
        size_t variables = 0;
        size_t abstract_states = 0;

        for (const auto& pdb : *pdbs_) {
            size_t vars = pdb->get_pattern().size();
            largest_pattern = std::max(largest_pattern, vars);
            variables += vars;
            abstract_states += pdb->num_states();
        }

        size_t total_subcollections_size = 0;

        for (const auto& subcollection : *subcollections_) {
            total_subcollections_size += subcollection.size();
        }

        const double avg_variables =
            static_cast<double>(variables) / static_cast<double>(pdbs_->size());
        const double avg_abstract_states =
            static_cast<double>(abstract_states) /
            static_cast<double>(pdbs_->size());

        const double avg_subcollection_size =
            static_cast<double>(total_subcollections_size) /
            static_cast<double>(subcollections_->size());

        log_ << "\n"
             << "Pattern Databases Statistics:\n"
             << "  Total number of PDBs: " << pdbs_->size() << "\n"
             << "  Total number of variables: " << variables << "\n"
             << "  Total number of abstract states: " << abstract_states << "\n"
             << "  Average number of variables per PDB: " << avg_variables
             << "\n"
             << "  Average number of abstract states per PDB: "
             << avg_abstract_states << "\n"

             << "  Largest pattern size: " << largest_pattern << "\n"

             << "  Total number of subcollections: " << subcollections_->size()
             << "\n"
             << "  Total number of subcollection PDBs: "
             << total_subcollections_size << "\n"
             << "  Average size of subcollection PDBs: "
             << avg_subcollection_size << "\n"

             << "  Generator time: " << generator_time << "s\n"
             << "  Dominance pruning time: " << dominance_pruning_time << "s\n"
             << "  Total construction time: " << construction_time << "s\n";
    }
}

value_t ProbabilityAwarePDBHeuristic::evaluate(const State& state) const
{
    return subcollection_finder_
        ->evaluate(*pdbs_, *subcollections_, state, termination_cost_);
}

ProbabilityAwarePDBHeuristicFactory::ProbabilityAwarePDBHeuristicFactory(
    std::shared_ptr<PatternCollectionGenerator> patterns,
    double max_time_dominance_pruning,
    utils::Verbosity verbosity)
    : patterns_(std::move(patterns))
    , max_time_dominance_pruning_(max_time_dominance_pruning)
    , verbosity_(verbosity)
{
}

std::unique_ptr<FDREvaluator>
ProbabilityAwarePDBHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function)
{
    return std::make_unique<ProbabilityAwarePDBHeuristic>(
        task,
        task_cost_function,
        patterns_,
        max_time_dominance_pruning_,
        utils::get_log_for_verbosity(verbosity_));
}

} // namespace probfd::heuristics
