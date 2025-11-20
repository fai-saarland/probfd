#include "probfd/cli/heuristics/gzocp_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/gzocp_heuristic.h"

using namespace downward;
using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
class GZOCPHeuristicFactoryFeature
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          std::shared_ptr<PatternCollectionGenerator>,
          GZOCPHeuristicFactory::OrderingStrategy,
          int> {
public:
    GZOCPHeuristicFactoryFeature()
        : TypedFeature("gzocp_heuristic", &GZOCPHeuristicFactoryFeature::func)
    {
        document_title("Greedy Zero-One Operator Cost Partitioning Heuristic");
        document_synopsis(
            "This heuristic computes a greedy cost-partitioning estimate over "
            "a set of projections. The cost-partitioning assigns the full "
            "operator cost to the first projection in the sequence that is "
            "affected by it (induces a non-self-loop) and assigns a cost of "
            "zero for this operator for all subsequent projections.");

        make_optional_argument_with_default(
            0,
            "patterns",
            "classical_generator(generator=systematic(pattern_max_size=2))",
            "The pattern generation algorithm to construct the projections.");

        make_optional_argument_with_default(
            1,
            "order",
            "random",
            "The order in which the projections are considered.");

        add_rng_options_to_feature(*this, 2);
    }

    static std::shared_ptr<TaskHeuristicFactory> func(
        std::shared_ptr<PatternCollectionGenerator>
            pattern_collection_generator,
        GZOCPHeuristicFactory::OrderingStrategy ordering,
        int random_seed)
    {
        return make_shared_from_arg_tuples<GZOCPHeuristicFactory>(
            std::move(pattern_collection_generator),
            ordering,
            random_seed);
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_gzocp_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<GZOCPHeuristicFactory::OrderingStrategy>(
        {{"random", "the order is random"},
         {"size_asc", "orders the PDBs by increasing size"},
         {"size_desc", "orders the PDBs by decreasing size"},
         {"inherit",
          "inherits the order from the underlying pattern generation "
          "algorithm"}});

    n.insert_feature_plugin<GZOCPHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
