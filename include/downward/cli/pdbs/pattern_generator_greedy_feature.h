#ifndef DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_GREEDY_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_GREEDY_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::pdbs {

language::parser::InternalFunctionDefinitionBase&
add_pattern_generator_greedy_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif