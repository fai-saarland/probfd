#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::cartesian_abstractions {

void add_split_selector_category(language::parser::NamespaceLevelDeclarationList& nspace);

void add_split_selector_features(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::cartesian_abstractions

#endif