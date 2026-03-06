#ifndef DOWNWARD_CLI_POTENTIALS_SUBCATEGORY_H
#define DOWNWARD_CLI_POTENTIALS_SUBCATEGORY_H

namespace language::parser {
class CompilationContext;
} // namespace language::parser

namespace language::documentation {
class DocumentationTopic;
} // namespace language::documentation

namespace downward::cli::potentials {

language::documentation::DocumentationTopic&
add_potential_heuristics_subcategory(
    language::parser::CompilationContext& registry);

}

#endif