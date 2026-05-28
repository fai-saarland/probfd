#include "downward/cli/open_lists/tiebreaking_open_list_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/open_lists/tiebreaking_open_list.h"

#include "downward/utils/memory.h"

using namespace std;
using namespace downward::utils;
using namespace downward::tiebreaking_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;
using downward::cli::get_open_list_arguments_from_options;

namespace {
template <typename T>
class TieBreakingOpenListFeature
    : public SharedTypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>> {
public:
    TieBreakingOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : TieBreakingOpenListFeature::SharedTypedFeature("state_tiebreaking")
    {
        this->document_title("Tie-breaking state open list");
        this->document_synopsis("");

        this->template add_list_option<shared_ptr<downward::Evaluator>>(
            "evals",
            "evaluators");
        this->template add_option<bool>(
            "unsafe_pruning",
            "allow unsafe pruning when the main evaluator regards a state a "
            "dead end",
            "true");
        add_open_list_options_to_feature(*this);
    }

    TieBreakingOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : TieBreakingOpenListFeature::SharedTypedFeature("edge_tiebreaking")
    {
        this->document_title("Tie-breaking edge open list");
        this->document_synopsis("");

        this->template add_list_option<shared_ptr<downward::Evaluator>>(
            "evals",
            "evaluators");
        this->template add_option<bool>(
            "unsafe_pruning",
            "allow unsafe pruning when the main evaluator regards a state a "
            "dead end",
            "true");
        add_open_list_options_to_feature(*this);
    }

    virtual shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>
    create_component(const Options& opts, const Context& context) const override
    {
        verify_list_non_empty<shared_ptr<downward::Evaluator>>(
            context,
            opts,
            "evals");
        return make_shared_from_arg_tuples<TieBreakingOpenListFactory<T>>(
            opts.get_list<shared_ptr<downward::Evaluator>>("evals"),
            opts.get<bool>("unsafe_pruning"),
            get_open_list_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::open_lists {

void add_tiebreaking_open_list_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<
        TieBreakingOpenListFeature<downward::StateOpenListEntry>>();
    raw_registry.insert_feature_plugin<
        TieBreakingOpenListFeature<downward::EdgeOpenListEntry>>();
}

} // namespace downward::cli::open_lists
