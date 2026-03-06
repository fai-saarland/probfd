#ifndef DOWNWARD_CLI_PDBS_SUBCATEGORY_H
#define DOWNWARD_CLI_PDBS_SUBCATEGORY_H

namespace language::parser {
class CompilationContext;
} // namespace language::parser

namespace language::documentation {
class DocumentationTopic;
} // namespace language::parser

namespace downward::cli::pdbs {

language::documentation::DocumentationTopic&
add_pdb_heuristic_subcategory(language::parser::CompilationContext& registry);

}

#endif