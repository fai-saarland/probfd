#include "downward/cli/merge_and_shrink/shrink_fh_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/merge_and_shrink/shrink_bucket_based_options.h"

#include "downward/merge_and_shrink/shrink_fh.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

using downward::cli::merge_and_shrink::add_shrink_bucket_options_to_feature;
using downward::cli::merge_and_shrink::get_shrink_bucket_arguments_from_options;

namespace {
class ShrinkFHFeature : public TypedFeature<ShrinkStrategy> {
public:
    ShrinkFHFeature()
        : TypedFeature("shrink_fh")
    {
        document_title("f-preserving shrink strategy");
        document_synopsis(
            "This shrink strategy implements the algorithm described in"
            " the paper:" +
            format_conference_reference(
                {"Malte Helmert", "Patrik Haslum", "Joerg Hoffmann"},
                "Flexible Abstraction Heuristics for Optimal Sequential "
                "Planning",
                "https://ai.dmi.unibas.ch/papers/helmert-et-al-icaps2007.pdf",
                "Proceedings of the Seventeenth International Conference on"
                " Automated Planning and Scheduling (ICAPS 2007)",
                "176-183",
                "AAAI Press",
                "2007"));

        add_option<ShrinkFH::HighLow>(
            "shrink_f",
            "in which direction the f based shrink priority is ordered",
            "high");
        add_option<ShrinkFH::HighLow>(
            "shrink_h",
            "in which direction the h based shrink priority is ordered",
            "low");
        add_shrink_bucket_options_to_feature(*this);

        document_note(
            "Note",
            "The strategy first partitions all states according to their "
            "combination of f- and g-values. These partitions are then sorted, "
            "first according to their f-value, then according to their h-value "
            "(increasing or decreasing, depending on the chosen options). "
            "States sorted last are shrinked together until reaching "
            "max_states.");

        document_note(
            "shrink_fh()",
            "Combine this with the merge-and-shrink option max_states=N (where "
            "N "
            "is a numerical parameter for which sensible values include 1000, "
            "10000, 50000, 100000 and 200000) and the linear merge startegy "
            "cg_goal_level to obtain the variant 'f-preserving shrinking of "
            "transition systems', called called HHH in the IJCAI 2011 paper, "
            "see "
            "bisimulation based shrink strategy. "
            "When we last ran experiments on interaction of shrink strategies "
            "with label reduction, this strategy performed best when used with "
            "label reduction before merging (and no label reduction before "
            "shrinking). "
            "We also recommend using full pruning with this shrink strategy, "
            "because both distances from the initial state and to the goal "
            "states "
            "must be computed anyway, and because the existence of only one "
            "dead state causes this shrink strategy to always use the "
            "map-based "
            "approach for partitioning states rather than the more efficient "
            "vector-based approach.");
    }

    virtual shared_ptr<ShrinkStrategy>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<ShrinkFH>(
            opts.get<ShrinkFH::HighLow>("shrink_f"),
            opts.get<ShrinkFH::HighLow>("shrink_h"),
            get_shrink_bucket_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_shrink_fh_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<ShrinkFHFeature>();

    raw_registry.insert_enum_plugin<ShrinkFH::HighLow>(
        {{"high", "prefer shrinking states with high value"},
         {"low", "prefer shrinking states with low value"}});
}

} // namespace downward::cli::merge_and_shrink
