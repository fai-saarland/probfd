#ifndef DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_COMBO_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_COMBO_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::pdbs {

language::parser::InternalFunctionDefinitionBase&
add_pattern_collection_generator_combo_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif