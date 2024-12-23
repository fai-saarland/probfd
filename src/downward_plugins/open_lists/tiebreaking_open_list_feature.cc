#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/open_list_options.h"

#include "downward/open_lists/tiebreaking_open_list.h"

#include "downward/utils/memory.h"

using namespace std;
using namespace tiebreaking_open_list;

using namespace downward_plugins::plugins;

using downward_plugins::add_open_list_options_to_feature;
using downward_plugins::get_open_list_arguments_from_options;

namespace {

class TieBreakingOpenListFeature
    : public TypedFeature<OpenListFactory, TieBreakingOpenListFactory> {
public:
    TieBreakingOpenListFeature()
        : TypedFeature("tiebreaking")
    {
        document_title("Tie-breaking open list");
        document_synopsis("");

        add_list_option<shared_ptr<Evaluator>>("evals", "evaluators");
        add_option<bool>(
            "unsafe_pruning",
            "allow unsafe pruning when the main evaluator regards a state a "
            "dead end",
            "true");
        add_open_list_options_to_feature(*this);
    }

    virtual shared_ptr<TieBreakingOpenListFactory>
    create_component(const Options& opts, const utils::Context& context)
        const override
    {
        verify_list_non_empty<shared_ptr<Evaluator>>(context, opts, "evals");
        return make_shared_from_arg_tuples<TieBreakingOpenListFactory>(
            opts.get_list<shared_ptr<Evaluator>>("evals"),
            opts.get<bool>("unsafe_pruning"),
            get_open_list_arguments_from_options(opts));
    }
};

FeaturePlugin<TieBreakingOpenListFeature> _plugin;

} // namespace
