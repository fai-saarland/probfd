#ifndef PROBFD_CLI_PDBS_TRIVIAL_FINDER_FACTORY_H
#define PROBFD_CLI_PDBS_TRIVIAL_FINDER_FACTORY_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace probfd::cli::pdbs {

language::plugins::InternalFunctionDefinitionBase&
add_trivial_finder_factory_feature(language::plugins::Namespace& nspace);

} // namespace probfd::cli::pdbs

#endif