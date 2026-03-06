#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::cartesian_abstractions {

void add_subtask_generator_category(language::parser::NamespaceLevelDeclarationList& nspace);

void add_subtask_generator_features(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::cartesian_abstractions

#endif