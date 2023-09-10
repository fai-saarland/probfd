#include "probfd/heuristics/pdbs/probability_aware_pdb_heuristic.h"

#include "probfd/heuristics/pdbs/dominance_pruning.h"
#include "probfd/heuristics/pdbs/pdb_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "probfd/cost_function.h"
#include "probfd/task_evaluator_factory.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"

#include "downward/plugins/plugin.h"

#include <cassert>
#include <iomanip>
#include <numeric>
#include <string>

namespace probfd {
namespace heuristics {
namespace pdbs {

ProbabilityAwarePDBHeuristic::ProbabilityAwarePDBHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    std::shared_ptr<PDBCollectionGenerator> generator,
    double max_time_dominance_pruning,
    utils::LogProxy log)
    : TaskDependentHeuristic(task, log)
    , termination_cost(task_cost_function->get_non_goal_termination_cost())
{
    utils::Timer construction_timer;

    auto pdb_collection_info = generator->generate(task, task_cost_function);

    pdbs = std::move(pdb_collection_info.get_pdbs());
    pdb_combinator = std::move(pdb_collection_info.get_pdb_combinator());

    if (log.is_at_least_normal()) {
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

        const double avg_variables = (double)variables / pdbs.size();
        const double avg_abstract_states =
            (double)abstract_states / pdbs.size();

        log << "\n"
            << "Probability-aware Pattern Databases Heuristic Statistics:\n"
            << "Construction time: " << construction_time << "s\n"
            << "Total number of PDBs: " << pdbs.size() << "\n"
            << "Total number of variables: " << variables << "\n"
            << "Total number of abstract states: " << abstract_states << "\n"
            << "Average number of variables per PDB: " << avg_variables << "\n"
            << "Average number of abstract states per PDB: "
            << avg_abstract_states << "\n"
            << "Largest pattern size: " << largest_pattern << "\n";
        pdb_combinator->print_statistics(log);
        log << std::endl;
    }
}

value_t ProbabilityAwarePDBHeuristic::evaluate(const State& state) const
{
    return pdb_combinator->evaluate(pdbs, state, termination_cost);
}

namespace {

class ProbabilityAwarePDBHeuristicFactory : public TaskEvaluatorFactory {
    const std::shared_ptr<PDBCollectionGenerator> generator_;
    const double time_dominance_pruning_;
    const utils::LogProxy log_;

public:
    explicit ProbabilityAwarePDBHeuristicFactory(const plugins::Options& opts);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

ProbabilityAwarePDBHeuristicFactory::ProbabilityAwarePDBHeuristicFactory(
    const plugins::Options& opts)
    : generator_(opts.get<std::shared_ptr<PDBCollectionGenerator>>("patterns"))
    , time_dominance_pruning_(opts.get<double>("max_time_dominance_pruning"))
    , log_(utils::get_log_from_options(opts))
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
        generator_,
        time_dominance_pruning_,
        log_);
}

class ProbabilityAwarePDBHeuristicFactoryFeature
    : public plugins::TypedFeature<
          TaskEvaluatorFactory,
          ProbabilityAwarePDBHeuristicFactory> {
public:
    ProbabilityAwarePDBHeuristicFactoryFeature()
        : TypedFeature("ppdbs")
    {
        document_title("Probability-aware Pattern database heuristic");
        document_synopsis(
            "An admissible heuristic obtained by combining multiple projection "
            "heuristics.");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");

        TaskDependentHeuristic::add_options_to_feature(*this);
        add_option<std::shared_ptr<PDBCollectionGenerator>>(
            "patterns",
            "",
            "classical_generator(generator=systematic(pattern_max_size=2))");
        add_option<double>("max_time_dominance_pruning", "", "0.0");
    }
};

static plugins::FeaturePlugin<ProbabilityAwarePDBHeuristicFactoryFeature>
    _plugin;
} // namespace

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
