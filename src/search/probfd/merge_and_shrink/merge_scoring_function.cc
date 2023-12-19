#include "probfd/merge_and_shrink/merge_scoring_function.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/logging.h"

#include <iostream>

using namespace std;

namespace probfd::merge_and_shrink {

void MergeScoringFunction::dump_options(utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Merge scoring function:" << endl;
        log << "Name: " << name() << endl;
        dump_function_specific_options(log);
    }
}

static class MergeScoringFunctionCategoryPlugin
    : public plugins::TypedCategoryPlugin<MergeScoringFunction> {
public:
    MergeScoringFunctionCategoryPlugin()
        : TypedCategoryPlugin("pMergeScoringFunction")
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

} // namespace probfd::merge_and_shrink