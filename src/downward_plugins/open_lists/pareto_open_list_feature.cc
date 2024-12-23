#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/open_list_options.h"

#include "downward_plugins/utils/rng_options.h"

#include "downward/open_lists/pareto_open_list.h"

#include "downward/utils/memory.h"

using namespace std;
using namespace pareto_open_list;

using namespace downward_plugins::plugins;

using downward_plugins::add_open_list_options_to_feature;
using downward_plugins::get_open_list_arguments_from_options;

using downward_plugins::utils::add_rng_options_to_feature;
using downward_plugins::utils::get_rng_arguments_from_options;

namespace {

class ParetoOpenListFeature
    : public TypedFeature<OpenListFactory, ParetoOpenListFactory> {
public:
    ParetoOpenListFeature()
        : TypedFeature("pareto")
    {
        document_title("Pareto open list");
        document_synopsis(
            "Selects one of the Pareto-optimal (regarding the sub-evaluators) "
            "entries for removal.");

        add_list_option<shared_ptr<Evaluator>>("evals", "evaluators");
        add_option<bool>(
            "state_uniform_selection",
            "When removing an entry, we select a non-dominated bucket "
            "and return its oldest entry. If this option is false, we select "
            "uniformly from the non-dominated buckets; if the option is true, "
            "we weight the buckets with the number of entries.",
            "false");
        add_rng_options_to_feature(*this);
        add_open_list_options_to_feature(*this);
    }

    virtual shared_ptr<ParetoOpenListFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<ParetoOpenListFactory>(
            opts.get_list<shared_ptr<Evaluator>>("evals"),
            opts.get<bool>("state_uniform_selection"),
            get_rng_arguments_from_options(opts),
            get_open_list_arguments_from_options(opts));
    }
};

FeaturePlugin<ParetoOpenListFeature> _plugin;

} // namespace
