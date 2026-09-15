#include "probfd_cli/merge_and_shrink/merge_strategy_factory_sccs_selector.h"

#include "probfd_cli/merge_and_shrink/merge_strategy_factory_sccs_options.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/utils/logging_options.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_sccs_selector.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_strategy_sccs_selector.h"
#include "probfd/merge_and_shrink/merge_tree_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

#include "probfd_cli/merge_and_shrink/merge_strategy_factory_options.h"

using namespace std;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

using namespace language;
using namespace language::plugins;

namespace {
class MergeStrategyFactorySCCsSelectorFeature
    : public SharedTypedFeature<MergeStrategyFactory> {
public:
    MergeStrategyFactorySCCsSelectorFeature()
        : TypedFeature("pmerge_sccs_selector")
    {
        document_title("Merge strategy SCCs-Selector");
        document_synopsis(
            "This merge strategy implements the algorithm described in the "
            "paper " +
            utils::format_conference_reference(
                {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
                "An Analysis of Merge Strategies for Merge-and-Shrink "
                "Heuristics",
                "https://ai.dmi.unibas.ch/papers/sievers-et-al-icaps2016.pdf",
                "Proceedings of the 26th International Conference on Planning "
                "and "
                "Scheduling (ICAPS 2016)",
                "2358-2366",
                "AAAI Press",
                "2016") +
            "In a nutshell, it computes the maximal SCCs of the causal graph, "
            "obtaining a partitioning of the task's variables. Every such "
            "partition is then merged individually, using the specified "
            "fallback "
            "merge strategy, considering the SCCs in a configurable order. "
            "Afterwards, all resulting composite abstractions are merged to "
            "form "
            "the final abstraction, again using the specified fallback merge "
            "strategy and the configurable order of the SCCs.");

        add_merge_strategy_options_to_feature(*this);
        add_merge_strategy_sccs_options_to_feature(*this);
        add_option<shared_ptr<MergeSelectorFactory>>(
            "merge_selector",
            "the fallback merge strategy to use if a stateless strategy should "
            "be used.",
            ArgumentInfo::NO_DEFAULT);
    }

    shared_ptr<MergeStrategyFactory>
    create_component(const Options& options, const Context& context)
        const override
    {
        return make_shared_from_arg_tuples<MergeStrategyFactorySCCsSelector>(
            downward::cli::utils::get_log_arguments_from_options(
                context,
                options),
            get_merge_strategy_sccs_args_from_options(context, options),
            options.get<shared_ptr<MergeSelectorFactory>>(
                context,
                "merge_selector"));
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_sccs_selector_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<MergeStrategyFactorySCCsSelectorFeature>();
}

} // namespace probfd::cli::merge_and_shrink
