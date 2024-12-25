#include "downward/cli/plugins/plugin.h"

using namespace downward::cli::plugins;

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
