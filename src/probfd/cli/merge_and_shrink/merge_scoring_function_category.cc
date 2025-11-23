#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_scoring_function.h"

using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_scoring_function_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<MergeScoringFunction>(
        "PMergeScoringFunction",
        "This page describes various merge scoring functions. A scoring "
        "function, "
        "given a list of merge candidates and a factored transition "
        "system, "
        "computes a score for each candidate based on this information and "
        "potentially some chosen options. Minimal scores are considered "
        "best. "
        "Scoring functions are currently only used within the score based "
        "filtering merge selector.");
}

} // namespace probfd::cli::merge_and_shrink
