#include "downward/cli/lp/lp_solver_enum.h"

#include "downward/lp/lp_solver.h"

#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_enum_declaration.h"

using namespace std;
using namespace downward::lp;

namespace downward::cli::lp {

void add_lp_solver_enum(language::parser::NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<LPSolverType>(
        nspace,
        "LPSolverType",
        {{"cplex", "commercial solver by IBM"},
         {"soplex", "open source solver by ZIB"}});
}

} // namespace downward::cli::lp