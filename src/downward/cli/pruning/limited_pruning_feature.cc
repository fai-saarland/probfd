#include "downward/cli/plugins/plugin.h"

#include "downward/cli/pruning_method_options.h"

#include "downward/pruning/limited_pruning.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace limited_pruning;

using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {

class LimitedPruningFeature
    : public TypedFeature<PruningMethod, LimitedPruning> {
public:
    LimitedPruningFeature()
        : TypedFeature("limited_pruning")
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

        add_option<shared_ptr<PruningMethod>>(
            "pruning",
            "the underlying pruning method to be applied");
        add_option<double>(
            "min_required_pruning_ratio",
            "disable pruning if the pruning ratio is lower than this value "
            "after"
            " 'expansions_before_checking_pruning_ratio' expansions",
            "0.2",
            Bounds("0.0", "1.0"));
        add_option<int>(
            "expansions_before_checking_pruning_ratio",
            "number of expansions before deciding whether to disable pruning",
            "1000",
            Bounds("0", "infinity"));
        add_pruning_options_to_feature(*this);

        document_note(
            "Example",
            "To use atom centric stubborn sets and limit them, use\n"
            "{{{\npruning=limited_pruning(pruning=atom_centric_stubborn_sets(),"
            "min_required_pruning_ratio=0.2,expansions_before_checking_pruning_"
            "ratio=1000)\n}}}\n"
            "in an eager search such as astar.");
    }

    virtual shared_ptr<LimitedPruning>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<LimitedPruning>(
            opts.get<shared_ptr<PruningMethod>>("pruning"),
            opts.get<double>("min_required_pruning_ratio"),
            opts.get<int>("expansions_before_checking_pruning_ratio"),
            get_pruning_arguments_from_options(opts));
    }
};

FeaturePlugin<LimitedPruningFeature> _plugin;

} // namespace
