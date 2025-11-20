#include "probfd/cli/heuristics/probability_aware_pdb_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/probability_aware_pdb_heuristic.h"

using namespace downward;
using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace downward::cli::plugins;

namespace {
class ProbabilityAwarePDBHeuristicFactoryFeature
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          std::shared_ptr<PatternCollectionGenerator>,
          utils::FSeconds,
          utils::Verbosity> {
public:
    ProbabilityAwarePDBHeuristicFactoryFeature()
        : TypedFeature(
              "ppdbs",
              &ProbabilityAwarePDBHeuristicFactoryFeature::func)
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

        make_optional_argument_with_default(
            0,
            "patterns",
            "classical_generator(generator=systematic(pattern_max_size=2))",
            "The pattern generation method");
        make_optional_argument_with_default(
            1,
            "max_time_dominance_pruning",
            "0.0s",
            "The maximum time spent pruning dominated patterns");
        add_task_dependent_heuristic_options_to_feature(*this, 2);
    }

    static std::shared_ptr<TaskHeuristicFactory> func(
        std::shared_ptr<PatternCollectionGenerator> generator,
        utils::FSeconds max_time_dominance_pruning,
        utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<ProbabilityAwarePDBHeuristicFactory>(
            std::move(generator),
            max_time_dominance_pruning,
            verbosity);
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_pdb_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<ProbabilityAwarePDBHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
