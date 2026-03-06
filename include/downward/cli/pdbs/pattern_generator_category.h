#ifndef DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_CATEGORY_H
#define DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::pdbs {

void add_pattern_generator_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif