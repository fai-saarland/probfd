#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_SCCS_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_sccs_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif