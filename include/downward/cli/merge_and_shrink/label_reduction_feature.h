#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_LABEL_REDUCTION_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::merge_and_shrink {

void add_label_reduction_category(language::plugins::Namespace& nspace);

language::plugins::InternalFunctionDefinitionBase&
add_label_reduction_to_namespace(language::plugins::Namespace& nspace);

} // namespace downward::cli::merge_and_shrink

#endif