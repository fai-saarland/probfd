#include "downward/cli/plugins/plugin.h"

#include "downward/cli/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/pareto_open_list.h"

#include "downward/utils/memory.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pareto_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;
using downward::cli::get_open_list_arguments_from_options;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

template <typename T>
class ParetoOpenListFeature
    : public TypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          ParetoOpenListFactory<T>> {
public:
    ParetoOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : ParetoOpenListFeature::TypedFeature("state_pareto")
    {
        this->document_title("Pareto state open list");
        this->document_synopsis(
            "Selects one of the Pareto-optimal (regarding the sub-evaluators) "
            "entries for removal.");

        this->template add_list_option<
            shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>>(
            "evals",
            "evaluators");
        this->template add_option<bool>(
            "state_uniform_selection",
            "When removing an entry, we select a non-dominated bucket "
            "and return its oldest entry. If this option is false, we select "
            "uniformly from the non-dominated buckets; if the option is true, "
            "we weight the buckets with the number of entries.",
            "false");
        add_rng_options_to_feature(*this);
        add_open_list_options_to_feature(*this);
    }

    ParetoOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : ParetoOpenListFeature::TypedFeature("edge_pareto")
    {
        this->document_title("Pareto edge open list");
        this->document_synopsis(
            "Selects one of the Pareto-optimal (regarding the sub-evaluators) "
            "entries for removal.");

        this->template add_list_option<
            shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>>(
            "evals",
            "evaluators");
        this->template add_option<bool>(
            "state_uniform_selection",
            "When removing an entry, we select a non-dominated bucket "
            "and return its oldest entry. If this option is false, we select "
            "uniformly from the non-dominated buckets; if the option is true, "
            "we weight the buckets with the number of entries.",
            "false");
        add_rng_options_to_feature(*this);
        add_open_list_options_to_feature(*this);
    }

    shared_ptr<ParetoOpenListFactory<T>>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<ParetoOpenListFactory<T>>(
            opts.get_list<shared_ptr<
                downward::TaskDependentFactory<downward::Evaluator>>>("evals"),
            opts.get<bool>("state_uniform_selection"),
            get_rng_arguments_from_options(opts),
            get_open_list_arguments_from_options(opts));
    }
};

FeaturePlugin<ParetoOpenListFeature<downward::StateOpenListEntry>> _plugin;
FeaturePlugin<ParetoOpenListFeature<downward::EdgeOpenListEntry>> _plugin2;

} // namespace
