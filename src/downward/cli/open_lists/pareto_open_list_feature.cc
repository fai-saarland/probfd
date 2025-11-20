#include "downward/cli/open_lists/pareto_open_list_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/pareto_open_list.h"

#include "downward/utils/memory.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pareto_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
template <typename T>
class ParetoOpenListFeature
    : public SharedTypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          const std::vector<std::shared_ptr<
              downward::TaskDependentFactory<downward::Evaluator>>>&,
          bool,
          int,
          bool> {
public:
    ParetoOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : ParetoOpenListFeature::TypedFeature(
              "state_pareto",
              &ParetoOpenListFeature::func)
    {
        this->document_title("Pareto state open list");
        this->document_synopsis(
            "Selects one of the Pareto-optimal (regarding the sub-evaluators) "
            "entries for removal.");

        this->make_required_argument(0, "evals", "evaluators");
        this->make_optional_argument_with_default(
            1,
            "state_uniform_selection",
            "false",
            "When removing an entry, we select a non-dominated bucket "
            "and return its oldest entry. If this option is false, we select "
            "uniformly from the non-dominated buckets; if the option is true, "
            "we weight the buckets with the number of entries.");
        const auto n = add_rng_options_to_feature(*this, 2);
        add_open_list_options_to_feature(*this, n + 2);
    }

    ParetoOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : ParetoOpenListFeature::TypedFeature(
              "edge_pareto",
              &ParetoOpenListFeature::func)
    {
        this->document_title("Pareto edge open list");
        this->document_synopsis(
            "Selects one of the Pareto-optimal (regarding the sub-evaluators) "
            "entries for removal.");

        this->make_required_argument(0, "evals", "evaluators");
        this->make_optional_argument_with_default(
            1,
            "state_uniform_selection",
            "false",
            "When removing an entry, we select a non-dominated bucket "
            "and return its oldest entry. If this option is false, we select "
            "uniformly from the non-dominated buckets; if the option is true, "
            "we weight the buckets with the number of entries.");
        const auto n = add_rng_options_to_feature(*this, 2);
        add_open_list_options_to_feature(*this, n + 2);
    }

    static shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>
    func(
        const std::vector<std::shared_ptr<
            downward::TaskDependentFactory<downward::Evaluator>>>&
            eval_factories,
        bool state_uniform_selection,
        int random_seed,
        bool pref_only)
    {
        return make_shared<ParetoOpenListFactory<T>>(
            eval_factories,
            state_uniform_selection,
            random_seed,
            pref_only);
    }
};
} // namespace

namespace downward::cli::open_lists {

void add_pareto_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<
        ParetoOpenListFeature<downward::StateOpenListEntry>>();
    n.insert_feature_plugin<
        ParetoOpenListFeature<downward::EdgeOpenListEntry>>();
}

} // namespace downward::cli::open_lists
