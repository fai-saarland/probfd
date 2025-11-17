#include "downward/cli/search_algorithms/iterated_search_feature.h"
#include "downward/cli/search_algorithms/search_algorithm_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/search_algorithms/iterated_search.h"

#include "downward/search_algorithm.h"
#include "downward/task_dependent_factory.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;
using namespace downward::iterated_search;

using namespace downward::cli::plugins;

using downward::cli::add_search_algorithm_options_to_feature;

namespace {
class IteratedSearchFactory : public TaskDependentFactory<SearchAlgorithm> {
    OperatorCost cost_type;
    int bound;
    utils::FSeconds max_time;
    std::string description;
    utils::Verbosity verbosity;
    std::vector<std::shared_ptr<TaskDependentFactory<SearchAlgorithm>>>
        algorithm_configs;
    bool pass_bound;
    bool repeat_last;
    bool continue_on_fail;
    bool continue_on_solve;

public:
    explicit IteratedSearchFactory(
        OperatorCost cost_type,
        int bound,
        utils::FSeconds max_time,
        std::string description,
        utils::Verbosity verbosity,
        std::vector<std::shared_ptr<TaskDependentFactory<SearchAlgorithm>>>
            algorithm_configs,
        bool pass_bound,
        bool repeat_last,
        bool continue_on_fail,
        bool continue_on_solve)
        : cost_type(cost_type)
        , bound(bound)
        , max_time(max_time)
        , description(std::move(description))
        , verbosity(verbosity)
        , algorithm_configs(std::move(algorithm_configs))
        , pass_bound(pass_bound)
        , repeat_last(repeat_last)
        , continue_on_fail(continue_on_fail)
        , continue_on_solve(continue_on_solve)
    {
    }

    unique_ptr<SearchAlgorithm>
    create_object(const SharedAbstractTask& task) override
    {
        return std::make_unique<IteratedSearch>(
            task,
            cost_type,
            bound,
            max_time,
            description,
            verbosity,
            algorithm_configs,
            pass_bound,
            repeat_last,
            continue_on_fail,
            continue_on_solve);
    }
};

class IteratedSearchFeature
    : public SharedTypedFeature<
          TaskDependentFactory<SearchAlgorithm>,
          OperatorCost,
          int,
          utils::FSeconds,
          std::string,
          utils::Verbosity,
          std::vector<std::shared_ptr<TaskDependentFactory<SearchAlgorithm>>>,
          bool,
          bool,
          bool,
          bool> {
public:
    IteratedSearchFeature()
        : TypedFeature("iterated", &IteratedSearchFeature::func)
    {
        document_title("Iterated search");
        document_synopsis("");
        document_note(
            "Note 1",
            "We don't cache heuristic values between search iterations at"
            " the moment. If you perform a LAMA-style iterative search,"
            " heuristic values and other per-state information will be computed"
            " multiple times.");
        document_note(
            "Note 2",
            "The configuration\n```\n"
            "--search \"iterated([lazy_wastar([ipdb()],w=10), "
            "lazy_wastar([ipdb()],w=5), lazy_wastar([ipdb()],w=3), "
            "lazy_wastar([ipdb()],w=2), lazy_wastar([ipdb()],w=1)])\"\n"
            "```\nwould perform the preprocessing phase of the ipdb heuristic "
            "5 times (once before each iteration).\n\n"
            "To avoid this, use heuristic predefinition, which avoids "
            "duplicate preprocessing, as follows:\n```\n"
            "\"let(h,ipdb(),iterated([lazy_wastar([h],w=10), "
            "lazy_wastar([h],w=5), lazy_wastar([h],w=3), lazy_wastar([h],w=2), "
            "lazy_wastar([h],w=1)]))\"\n"
            "```");

        make_required_argument(
            0,
            "algorithm_configs",
            "list of search algorithms for each phase");
        make_optional_argument_with_default(
            1,
            "pass_bound",
            "true",
            "use the bound of iterated search as a bound for its component "
            "search algorithms, unless these already have a lower bound set. "
            "The iterated search bound is tightened whenever a component finds "
            "a cheaper plan.");
        make_optional_argument_with_default(
            2,
            "repeat_last",
            "false",
            "repeat last phase of search");
        make_optional_argument_with_default(
            3,
            "continue_on_fail",
            "false",
            "continue search after no solution found");
        make_optional_argument_with_default(
            4,
            "continue_on_solve",
            "true",
            "continue search after solution found");
        add_search_algorithm_options_to_feature(*this, "iterated", 5);
    }

    static shared_ptr<TaskDependentFactory<SearchAlgorithm>> func(
        const utils::Context& context,
        OperatorCost cost_type,
        int bound,
        utils::FSeconds max_time,
        std::string description,
        utils::Verbosity verbosity,
        std::vector<std::shared_ptr<TaskDependentFactory<SearchAlgorithm>>>
            algorithm_configs,
        bool pass_bound,
        bool repeat_last,
        bool continue_on_fail,
        bool continue_on_solve)
    {
        if (algorithm_configs.empty()) {
            context.error("List of algorithms may not be empty.");
        }

        return make_shared<IteratedSearchFactory>(
            cost_type,
            bound,
            max_time,
            std::move(description),
            verbosity,
            std::move(algorithm_configs),
            pass_bound,
            repeat_last,
            continue_on_fail,
            continue_on_solve);
    }
};
} // namespace

namespace downward::cli::search_algorithms {

void add_iterated_search_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<IteratedSearchFeature>();
}

} // namespace downward::cli::search_algorithms
