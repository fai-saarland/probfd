#ifndef PROBFD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H

namespace language::parser {
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::merge_and_shrink {

void add_label_reduction_category(language::parser::NamespaceLevelDeclarationList& nspace);

void add_label_reduction_features(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::merge_and_shrink

#endif