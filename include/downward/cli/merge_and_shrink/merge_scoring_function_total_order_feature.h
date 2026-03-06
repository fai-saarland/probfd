#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_FEATURE_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_total_order_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::merge_and_shrink

#endif