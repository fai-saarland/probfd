#include "downward/cli/mutexes/mutex_factory_category.h"

#include "language/plugins/registry.h"

#include "downward/mutex_information.h"
#include "downward/task_dependent_factory.h"

#include <filesystem>

using namespace std;

namespace downward::cli::mutexes {

void add_mutex_factory_category(language::plugins::Namespace& nspace)
{
    nspace
        .insert_shared_type_declaration<TaskDependentFactory<MutexInformation>>(
            "MutexFactory",
            "A mutex factory computes mutually exclusive facts for a given "
            "planning task.");
}

} // namespace downward::cli::mutexes
