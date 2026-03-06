#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_CATEGORY_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::merge_and_shrink {

void add_merge_scoring_function_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif