#include "downward_plugins/plugins/plugin.h"

using namespace downward_plugins::plugins;

namespace {

class EvaluatorGroupPlugin : public SubcategoryPlugin {
public:
    EvaluatorGroupPlugin()
        : SubcategoryPlugin("evaluators_basic")
    {
        document_title("Basic Evaluators");
    }
} _subcategory_plugin;

} // namespace
