#include "probfd_cli/heuristics/probability_aware_pdb_heuristic.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd_cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/probability_aware_pdb_heuristic.h"

#include "downward_cli/utils/logging_options.h"

using namespace downward;
using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language;
using namespace language::plugins;

namespace {
class ProbabilityAwarePDBHeuristicFactoryFeature
    : public TypedFeature<TaskHeuristicFactory> {
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

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<ProbabilityAwarePDBHeuristicFactory>(
            opts.get<std::shared_ptr<PatternCollectionGenerator>>(
                context,
                "patterns"),
            opts.get<utils::Duration>(context, "max_time_dominance_pruning"),
            downward::cli::utils::get_log_arguments_from_options(
                context,
                opts));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_pdb_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<ProbabilityAwarePDBHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
