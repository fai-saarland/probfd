#include "downward/cli/mutexes/mutex_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/mutex_information.h"
#include "downward/task_dependent_factory.h"

#include <filesystem>

using namespace std;

using namespace downward;
using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {
class MutexFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<
          TaskDependentFactory<MutexInformation>> {
public:
    MutexFactoryCategoryPlugin()
        : TypedCategoryPlugin("MutexFactory")
    {
        document_synopsis(
            "A mutex factory computes mutually exclusive facts for a given "
            "planning task.");
    }
};
}

namespace downward::cli::mutexes {

void add_mutex_factory_category(plugins::RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<MutexFactoryCategoryPlugin>();
}

} // namespace
