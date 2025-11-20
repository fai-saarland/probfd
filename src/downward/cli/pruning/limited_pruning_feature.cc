#include "downward/cli/pruning/limited_pruning_feature.h"

#include "downward/cli/pruning/pruning_method_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/pruning/limited_pruning.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::limited_pruning;
using namespace downward::utils;

using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {
class LimitedPruningFeature
    : public SharedTypedFeature<
          downward::PruningMethod,
          const std::shared_ptr<downward::PruningMethod>&,
          double,
          int,
          downward::utils::Verbosity> {
public:
    LimitedPruningFeature()
        : TypedFeature("limited_pruning", &LimitedPruningFeature::func)
    {
        document_title("Limited pruning");
        document_synopsis(
            "Limited pruning applies another pruning method and switches it "
            "off "
            "after a fixed number of expansions if the pruning ratio is below "
            "a "
            "given value. The pruning ratio is the sum of all pruned operators "
            "divided by the sum of all operators before pruning, considering "
            "all "
            "previous expansions.");
        document_note(
            "Example",
            "To use atom centric stubborn sets and limit them, use\n"
            "{{{\npruning=limited_pruning(pruning=atom_centric_stubborn_sets(),"
            "min_required_pruning_ratio=0.2,expansions_before_checking_pruning_"
            "ratio=1000)\n}}}\n"
            "in an eager search such as astar.");

        make_required_argument(
            0,
            "pruning",
            "the underlying pruning method to be applied");
        make_optional_argument_with_default(
            1,
            "min_required_pruning_ratio",
            "0.2",
            "disable pruning if the pruning ratio is lower than this value "
            "after 'expansions_before_checking_pruning_ratio' expansions");
        make_optional_argument_with_default(
            2,
            "expansions_before_checking_pruning_ratio",
            "1000",
            "number of expansions before deciding whether to disable pruning");
        add_pruning_options_to_feature(*this, 3);
    }

    static shared_ptr<downward::PruningMethod> func(
        const std::shared_ptr<downward::PruningMethod>& pruning,
        double min_required_pruning_ratio,
        int expansions_before_checking_pruning_ratio,
        downward::utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<LimitedPruning>(
            pruning,
            min_required_pruning_ratio,
            expansions_before_checking_pruning_ratio,
            verbosity);
    }
};
} // namespace

namespace downward::cli::pruning {

void add_limited_pruning_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LimitedPruningFeature>();
}

} // namespace downward::cli::pruning
