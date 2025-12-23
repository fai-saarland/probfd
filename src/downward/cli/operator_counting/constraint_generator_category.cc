#include "downward/cli/operator_counting/constraint_generator_category.h"

#include "language/plugins/registry.h"

#include "downward/operator_counting/constraint_generator.h"

using namespace downward::operator_counting;

using namespace language::plugins;

namespace downward::cli::operator_counting {

void add_constraint_generator_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<ConstraintGenerator>(
        "ConstraintGenerator",
        "");
}

} // namespace downward::cli::operator_counting
