#include "downward/cli/search_algorithms/iterated_search_feature.h"
#include "downward/cli/search_algorithms/search_algorithm_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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
    utils::Duration max_time;
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
        utils::Duration max_time,
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
    : public TypedFeature<TaskDependentFactory<SearchAlgorithm>> {
public:
    IteratedSearchFeature()
        : TypedFeature("iterated")
    {
        document_title("Iterated search");
        document_synopsis("");

        add_list_option<shared_ptr<SearchAlgorithm>>(
            "algorithm_configs",
            "list of search algorithms for each phase",
            "");
        add_option<bool>(
            "pass_bound",
            "use the bound of iterated search as a bound for its component "
            "search algorithms, unless these already have a lower bound set. "
            "The iterated search bound is tightened whenever a component finds "
            "a cheaper plan.",
            "true");
        add_option<bool>("repeat_last", "repeat last phase of search", "false");
        add_option<bool>(
            "continue_on_fail",
            "continue search after no solution found",
            "false");
        add_option<bool>(
            "continue_on_solve",
            "continue search after solution found",
            "true");
        add_search_algorithm_options_to_feature(*this, "iterated");

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
    }

    shared_ptr<TaskDependentFactory<SearchAlgorithm>>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        verify_list_non_empty<
            std::shared_ptr<TaskDependentFactory<SearchAlgorithm>>>(
            context,
            options,
            "algorithm_configs");

        return make_shared<IteratedSearchFactory>(
            options.get<OperatorCost>("cost_type"),
            options.get<int>("bound"),
            options.get<utils::Duration>("max_time"),
            options.get_unparsed_config(),
            options.get<utils::Verbosity>("verbosity"),
            options.get_list<
                std::shared_ptr<TaskDependentFactory<SearchAlgorithm>>>(
                "algorithm_configs"),
            options.get<bool>("pass_bound"),
            options.get<bool>("repeat_last"),
            options.get<bool>("continue_on_fail"),
            options.get<bool>("continue_on_solve"));
    }
};
} // namespace

namespace downward::cli::search_algorithms {

void add_iterated_search_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<IteratedSearchFeature>();
}

} // namespace downward::cli::search_algorithms
