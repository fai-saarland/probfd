#include "downward/cli/utils/verbosity_enum.h"

#include "downward/utils/logging.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"

using namespace std;

using downward::utils::Verbosity;

namespace downward::cli::utils {

void add_verbosity_enum(language::parser::NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<Verbosity>(
        nspace,
        "Verbosity",
        {{"silent", "only the most basic output"},
         {"normal", "relevant information to monitor progress"},
         {"verbose", "full output"},
         {"debug", "like verbose with additional debug output"}});
}

} // namespace downward::cli::utils
