#include "probfd/cli/heuristics/probability_aware_pdb_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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
    : public TypedFeature<
          TaskHeuristicFactory,
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

        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "",
            "classical_generator(generator=systematic(pattern_max_size=2))");
        add_option<double>("max_time_dominance_pruning", "", "0.0");
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<ProbabilityAwarePDBHeuristicFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<ProbabilityAwarePDBHeuristicFactory>(
            opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
            opts.get<utils::Duration>("max_time_dominance_pruning"),
            get_task_dependent_heuristic_arguments_from_options(opts));
    }
};
}

namespace probfd::cli::heuristics {

void add_pdb_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<ProbabilityAwarePDBHeuristicFactoryFeature>();
}

} // namespace
