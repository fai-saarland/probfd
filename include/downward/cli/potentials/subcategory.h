#ifndef DOWNWARD_CLI_POTENTIALS_SUBCATEGORY_H
#define DOWNWARD_CLI_POTENTIALS_SUBCATEGORY_H

namespace language::plugins {
class DocumentationTopic;
class Registry;
} // namespace language::plugins

namespace downward::cli::potentials {

language::plugins::DocumentationTopic&
add_potential_heuristics_subcategory(language::plugins::Registry& registry);

}

#endif