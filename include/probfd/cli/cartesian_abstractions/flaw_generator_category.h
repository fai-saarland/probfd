#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_FLAW_GENERATOR_CATEGORY_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_FLAW_GENERATOR_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::cartesian_abstractions {

void add_flaw_generator_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif