#include "downward/cli/plugins/plugin.h"

#include "downward/cli/open_list_options.h"

#include "downward/open_lists/tiebreaking_open_list.h"

#include "downward/utils/memory.h"

using namespace std;
using namespace downward::utils;
using namespace downward::tiebreaking_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;
using downward::cli::get_open_list_arguments_from_options;

namespace {

class TieBreakingOpenListFeature
    : public TypedFeature<downward::OpenListFactory, TieBreakingOpenListFactory> {
public:
    TieBreakingOpenListFeature()
        : TypedFeature("tiebreaking")
    {
        document_title("Tie-breaking open list");
        document_synopsis("");

        add_list_option<shared_ptr<downward::Evaluator>>("evals", "evaluators");
        add_option<bool>(
            "unsafe_pruning",
            "allow unsafe pruning when the main evaluator regards a state a "
            "dead end",
            "true");
        add_open_list_options_to_feature(*this);
    }

    virtual shared_ptr<TieBreakingOpenListFactory>
    create_component(const Options& opts, const Context& context)
        const override
    {
        verify_list_non_empty<shared_ptr<downward::Evaluator>>(context, opts, "evals");
        return make_shared_from_arg_tuples<TieBreakingOpenListFactory>(
            opts.get_list<shared_ptr<downward::Evaluator>>("evals"),
            opts.get<bool>("unsafe_pruning"),
            get_open_list_arguments_from_options(opts));
    }
};

FeaturePlugin<TieBreakingOpenListFeature> _plugin;

} // namespace
