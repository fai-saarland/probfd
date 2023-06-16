#include "probfd/heuristics/pdbs/probability_aware_pdb_heuristic.h"

#include "probfd/heuristics/pdbs/pattern_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "pdbs/dominance_pruning.h"

#include "utils/countdown_timer.h"
#include "utils/logging.h"

#include "plugins/plugin.h"

#include <cassert>
#include <iomanip>
#include <numeric>
#include <string>

namespace probfd {
namespace heuristics {
namespace pdbs {

ProbabilityAwarePDBHeuristic::ProbabilityAwarePDBHeuristic(
    const plugins::Options& opts)
    : ProbabilityAwarePDBHeuristic(
          opts.get<std::shared_ptr<ProbabilisticTask>>("transform"),
          opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
          opts.get<double>("max_time_dominance_pruning"),
          utils::get_log_from_options(opts))
{
}

ProbabilityAwarePDBHeuristic::ProbabilityAwarePDBHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<PatternCollectionGenerator> generator,
    double max_time_dominance_pruning,
    utils::LogProxy log)
    : TaskDependentHeuristic(task, log)
{
    utils::Timer construction_timer;

    utils::Timer generator_timer;
    auto pattern_collection_info = generator->generate(task);
    const double generator_time = generator_timer();

    this->patterns = pattern_collection_info.get_patterns();
    this->pdbs = pattern_collection_info.get_pdbs();
    this->subcollections = pattern_collection_info.get_subcollections();
    this->subcollection_finder =
        pattern_collection_info.get_subcollection_finder();

    double dominance_pruning_time = 0.0;

    if (max_time_dominance_pruning > 0.0) {
        utils::Timer timer;

        ::pdbs::prune_dominated_cliques(
            *patterns,
            *pdbs,
            *subcollections,
            task_proxy.get_variables().size(),
            max_time_dominance_pruning,
            utils::g_log);

        dominance_pruning_time = timer();
    }

    if (log.is_at_least_normal()) {
        // Gather statistics.
        const double construction_time = construction_timer();

        size_t largest_pattern = 0;
        size_t variables = 0;
        size_t abstract_states = 0;

        for (auto pdb : *pdbs) {
            size_t vars = pdb->get_pattern().size();
            largest_pattern = std::max(largest_pattern, vars);
            variables += vars;
            abstract_states += pdb->num_states();
        }

        size_t total_subcollections_size = 0;

        for (auto subcollection : *subcollections) {
            total_subcollections_size += subcollection.size();
        }

        const double avg_variables = (double)variables / pdbs->size();
        const double avg_abstract_states =
            (double)abstract_states / pdbs->size();

        const double avg_subcollection_size =
            (double)total_subcollections_size / subcollections->size();

        log << "\n"
            << "Pattern Databases Statistics:\n"
            << "  Total number of PDBs: " << pdbs->size() << "\n"
            << "  Total number of variables: " << variables << "\n"
            << "  Total number of abstract states: " << abstract_states << "\n"
            << "  Average number of variables per PDB: " << avg_variables
            << "\n"
            << "  Average number of abstract states per PDB: "
            << avg_abstract_states << "\n"

            << "  Largest pattern size: " << largest_pattern << "\n"

            << "  Total number of subcollections: " << subcollections->size()
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

EvaluationResult
ProbabilityAwarePDBHeuristic::evaluate(const State& state) const
{
    return subcollection_finder->evaluate(*pdbs, *subcollections, state);
}

class ProbabilityAwarePDBHeuristicFeature
    : public plugins::
          TypedFeature<TaskEvaluator, ProbabilityAwarePDBHeuristic> {
public:
    ProbabilityAwarePDBHeuristicFeature()
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
        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "",
            "classical_generator(generator=systematic(pattern_max_size=2))");
        add_option<double>("max_time_dominance_pruning", "", "0.0");
    }
};

static plugins::FeaturePlugin<ProbabilityAwarePDBHeuristicFeature> _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
