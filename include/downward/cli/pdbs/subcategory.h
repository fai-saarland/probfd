#ifndef DOWNWARD_CLI_PDBS_SUBCATEGORY_H
#define DOWNWARD_CLI_PDBS_SUBCATEGORY_H

namespace language::plugins {
class DocumentationTopic;
class Registry;
} // namespace language::plugins

namespace downward::cli::pdbs {

language::plugins::DocumentationTopic&
add_pdb_heuristic_subcategory(language::plugins::Registry& registry);

}

#endif