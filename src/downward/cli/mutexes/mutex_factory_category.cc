#include "downward/cli/mutexes/mutex_factory_category.h"

#include "downward/mutex_information.h"
#include "downward/task_dependent_factory.h"

#include "language/ast/internal_type_declaration.h"

using namespace std;

namespace downward::cli::mutexes {

void add_mutex_factory_category(language::parser::NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<TaskDependentFactory<MutexInformation>>(
        nspace,
        "MutexFactory",
        "A mutex factory computes mutually exclusive facts for a given "
        "planning task.");
}

} // namespace downward::cli::mutexes
