#include "downward/cli/pdbs/subcategory.h"

#include "language/ast/compilation_context.h"

#include "language/documentation/topic.h"

using namespace language::parser;
using namespace language::documentation;

namespace {

class PDBGroupPlugin : public DocumentationTopic {
public:
    PDBGroupPlugin()
        : DocumentationTopic("heuristics_pdb")
    {
        document_title("Pattern Database Heuristics");
    }
};

} // namespace

namespace downward::cli::pdbs {

DocumentationTopic& add_pdb_heuristic_subcategory(CompilationContext& registry)
{
    return registry.insert_documentation_topic<PDBGroupPlugin>();
}

} // namespace downward::cli::pdbs
