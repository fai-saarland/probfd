#include "downward/cli/merge_and_shrink/merge_scoring_function_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/merge_and_shrink/merge_scoring_function.h"

using namespace downward::merge_and_shrink;

using namespace language::plugins;

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<MergeScoringFunction>(
        "MergeScoringFunction",
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

} // namespace downward::cli::merge_and_shrink
