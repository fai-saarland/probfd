#include "downward/cli/plugins/plugin.h"

#include "downward/merge_and_shrink/merge_scoring_function.h"

using namespace std;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace {

class MergeScoringFunctionCategoryPlugin
    : public TypedCategoryPlugin<MergeScoringFunction> {
public:
    MergeScoringFunctionCategoryPlugin()
        : TypedCategoryPlugin("MergeScoringFunction")
    {
        document_synopsis(
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
} _category_plugin;

} // namespace
