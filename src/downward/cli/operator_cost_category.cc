#include "downward/cli/operator_cost_category.h"

#include "downward/operator_cost.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"

using namespace std;

namespace downward::cli {

using namespace language::parser;

void add_operator_cost_category(NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<OperatorCost>(
        nspace,
        "OperatorCost",
        {{"normal", "all actions are accounted for with their real cost"},
         {"one", "all actions are accounted for as unit cost"},
         {"plusone",
          "all actions are accounted for as their real cost + 1 "
          "(except if all actions have original cost 1, "
          "in which case cost 1 is used). "
          "This is the behaviour known for the heuristics of the LAMA "
          "planner. "
          "This is intended to be used by the heuristics, not search "
          "algorithms, "
          "but is supported for both."}});
}

} // namespace downward::cli