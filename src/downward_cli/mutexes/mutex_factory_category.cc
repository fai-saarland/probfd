#include "downward_cli/mutexes/mutex_factory_category.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/mutex_information.h"
#include "downward/task_dependent_factory.h"

#include <filesystem>

using namespace std;

using namespace language;

namespace downward::cli::mutexes {

void add_mutex_factory_category(plugins::RawRegistry& raw_registry)
{
    auto& category =
        raw_registry
            .insert_category_plugin<TaskDependentFactory<MutexInformation>>(
                "MutexFactory");

    category.document_synopsis(
        "A mutex factory computes mutually exclusive facts for a given "
        "planning task.");
}

} // namespace downward::cli::mutexes
