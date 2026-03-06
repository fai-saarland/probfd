#ifndef DOWNWARD_SUBTASK_GENERATORS_FEATURES_H
#define DOWNWARD_SUBTASK_GENERATORS_FEATURES_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::cartesian_abstractions {

void add_subtask_generators_features(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::cartesian_abstractions

#endif