#include "downward/cli/operator_counting/constraint_generator_category.h"

#include "downward/operator_counting/constraint_generator.h"

#include "language/ast/internal_type_declaration.h"

using namespace downward::operator_counting;

using namespace language::parser;

namespace downward::cli::operator_counting {

void add_constraint_generator_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<ConstraintGenerator>(
        nspace,
        "ConstraintGenerator",
        "");
}

} // namespace downward::cli::operator_counting
