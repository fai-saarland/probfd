#ifndef DOWNWARD_HEURISTICS_ZERO_ONE_PDB_HEURISTIC_FEATURE_H
#define DOWNWARD_HEURISTICS_ZERO_ONE_PDB_HEURISTIC_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::heuristics {

language::parser::InternalFunctionDefinitionBase&
add_zero_one_pdbs_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif