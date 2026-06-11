#include "downward_cli/merge_and_shrink/merge_scoring_function_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/merge_and_shrink/merge_scoring_function.h"

using namespace downward::merge_and_shrink;

using namespace language::plugins;

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_category(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<MergeScoringFunction>(
        "MergeScoringFunction");
    category.document_synopsis(
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
