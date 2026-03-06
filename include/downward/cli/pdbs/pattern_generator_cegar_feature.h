#ifndef DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_CEGAR_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_CEGAR_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::pdbs {

language::parser::InternalFunctionDefinitionBase&
add_pattern_generator_cegar_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif