#ifndef DOWNWARD_CLI_UTILS_VERBOSITY_ENUM_H
#define DOWNWARD_CLI_UTILS_VERBOSITY_ENUM_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::utils {

void add_verbosity_enum(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif