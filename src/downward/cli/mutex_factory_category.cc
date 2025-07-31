#include "downward/cli/plugins/plugin.h"

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

MutexFactoryCategoryPlugin _category_plugin;

} // namespace
