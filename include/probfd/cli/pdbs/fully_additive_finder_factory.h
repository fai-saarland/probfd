#ifndef PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_FULLY_ADDITIVE_FINDER_FACTORY_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::pdbs {

language::plugins::InternalFunctionDefinitionBase&
add_fully_additive_finder_factory_feature(language::plugins::Namespace& nspace);

}

#endif