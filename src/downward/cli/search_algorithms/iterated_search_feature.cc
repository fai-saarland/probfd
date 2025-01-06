#include "downward/cli/plugins/plugin.h"

#include "downward/cli/search_algorithm_options.h"

#include "downward/search_algorithms/iterated_search.h"

#include "downward/search_algorithm_factory.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace iterated_search;

using namespace downward::cli::plugins;

using downward::cli::add_search_algorithm_options_to_feature;

namespace {

class IteratedSearchFeature
    : public TypedFeature<SearchAlgorithm, IteratedSearch> {
public:
    IteratedSearchFeature()
        : TypedFeature("iterated")
    {
        document_title("Iterated search");
        document_synopsis("");

        add_list_option<shared_ptr<SearchAlgorithm>>(
            "algorithm_configs",
            "list of search algorithms for each phase",
            "",
            true);
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

    virtual shared_ptr<IteratedSearch>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        verify_list_non_empty<std::shared_ptr<SearchAlgorithmFactory>>(
            context,
            options,
            "algorithm_configs");

        return make_shared<IteratedSearch>(
            options.get<OperatorCost>("cost_type"),
            options.get<int>("bound"),
            options.get<double>("max_time"),
            options.get_unparsed_config(),
            options.get<utils::Verbosity>("verbosity"),
            options.get_list<std::shared_ptr<SearchAlgorithmFactory>>(
                "algorithm_configs"),
            options.get<bool>("pass_bound"),
            options.get<bool>("repeat_last"),
            options.get<bool>("continue_on_fail"),
            options.get<bool>("continue_on_solve"));
    }
};

FeaturePlugin<IteratedSearchFeature> _plugin;

} // namespace
