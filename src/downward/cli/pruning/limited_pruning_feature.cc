#include "downward/cli/pruning/limited_pruning_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/pruning/limited_pruning.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::limited_pruning;
using namespace downward::utils;

using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase& add_limited_pruning_strategy_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "limited_pruning",
        &downward::cli::plugins::construct_shared<
            downward::PruningMethod,
            LimitedPruning,
            const std::shared_ptr<downward::PruningMethod>&,
            double,
            int,
            Verbosity>);
    f.document_title("Limited pruning");
    f.document_synopsis(
        "Limited pruning applies another pruning method and switches it "
        "off "
        "after a fixed number of expansions if the pruning ratio is below "
        "a "
        "given value. The pruning ratio is the sum of all pruned operators "
        "divided by the sum of all operators before pruning, considering "
        "all "
        "previous expansions.");
    f.document_note(
        "Example",
        "To use atom centric stubborn sets and limit them, use\n"
        "{{{\npruning=limited_pruning(pruning=atom_centric_stubborn_sets(),"
        "min_required_pruning_ratio=0.2,expansions_before_checking_pruning_"
        "ratio=1000)\n}}}\n"
        "in an eager search such as astar.");

    f.make_required_argument(
        0,
        "pruning",
        "the underlying pruning method to be applied");
    f.make_optional_argument_with_default(
        1,
        "min_required_pruning_ratio",
        "0.2",
        "disable pruning if the pruning ratio is lower than this value "
        "after 'expansions_before_checking_pruning_ratio' expansions");
    f.make_optional_argument_with_default(
        2,
        "expansions_before_checking_pruning_ratio",
        "1000",
        "number of expansions before deciding whether to disable pruning");
    add_pruning_options_to_feature(f, 3);

    return f;
}

}

namespace downward::cli::pruning {

void add_limited_pruning_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_limited_pruning_strategy_namespace(n);
}

} // namespace downward::cli::pruning
