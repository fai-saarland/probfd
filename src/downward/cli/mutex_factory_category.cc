#include "downward/cli/plugins/plugin.h"

#include "downward/mutexes.h"

#include <filesystem>

using namespace std;

using namespace downward;
using namespace downward::cli;
using namespace downward::cli::plugins;

namespace {

class MutexFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<MutexFactory> {
public:
    MutexFactoryCategoryPlugin()
        : TypedCategoryPlugin("MutexFactory")
    {
        document_synopsis(
            "A mutex factory computes mutually exclusive facts for a given "
            "planning task.");
        allow_variable_binding();
    }
};

MutexFactoryCategoryPlugin _category_plugin;

} // namespace
