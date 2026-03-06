#include "downward/cli/merge_and_shrink/merge_scoring_function_category.h"

#include "downward/merge_and_shrink/merge_scoring_function.h"

#include "language/ast/internal_type_declaration.h"

using namespace downward::merge_and_shrink;

using namespace language::parser;

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<MergeScoringFunction>(
        nspace,
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
