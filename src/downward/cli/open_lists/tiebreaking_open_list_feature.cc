#include "downward/cli/open_lists/tiebreaking_open_list_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/open_lists/tiebreaking_open_list.h"

using namespace std;
using namespace downward::utils;
using namespace downward::tiebreaking_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;

namespace {
template <typename T>
class TieBreakingOpenListFeature
    : public SharedTypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          const std::vector<std::shared_ptr<
              downward::TaskDependentFactory<downward::Evaluator>>>&,
          bool,
          bool> {
public:
    TieBreakingOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : TieBreakingOpenListFeature::TypedFeature(
              "state_tiebreaking",
              &TieBreakingOpenListFeature::func)
    {
        this->document_title("Tie-breaking state open list");
        this->document_synopsis("");

        this->make_required_argument(0, "evals", "evaluators");
        this->make_optional_argument_with_default(
            1,
            "unsafe_pruning",
            "true",
            "allow unsafe pruning when the main evaluator regards a state a "
            "dead end");
        add_open_list_options_to_feature(*this, 2);
    }

    TieBreakingOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : TieBreakingOpenListFeature::TypedFeature(
              "edge_tiebreaking",
              &TieBreakingOpenListFeature::func)
    {
        this->document_title("Tie-breaking edge open list");
        this->document_synopsis("");

        this->make_required_argument(0, "evals", "evaluators");
        this->make_optional_argument_with_default(
            1,
            "unsafe_pruning",
            "true",
            "allow unsafe pruning when the main evaluator regards a state a "
            "dead end");
        add_open_list_options_to_feature(*this, 2);
    }

    static shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>
    func(
        const Context& context,
        const std::vector<std::shared_ptr<
            downward::TaskDependentFactory<downward::Evaluator>>>& factories,
        bool unsafe_pruning,
        bool pref_only)
    {
        if (factories.empty()) {
            context.error("List of evaluators may not be empty.");
        }

        return make_shared_from_arg_tuples<TieBreakingOpenListFactory<T>>(
            std::move(factories),
            unsafe_pruning,
            pref_only);
    }
};
} // namespace

namespace downward::cli::open_lists {

void add_tiebreaking_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<
        TieBreakingOpenListFeature<downward::StateOpenListEntry>>();
    n.insert_feature_plugin<
        TieBreakingOpenListFeature<downward::EdgeOpenListEntry>>();
}

} // namespace downward::cli::open_lists
