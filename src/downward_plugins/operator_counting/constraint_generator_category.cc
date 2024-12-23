#include "downward_plugins/plugins/plugin.h"

#include "downward/operator_counting/constraint_generator.h"

using namespace std;
using namespace operator_counting;

using namespace downward_plugins::plugins;

namespace {

class ConstraintGeneratorCategoryPlugin
    : public TypedCategoryPlugin<ConstraintGenerator> {
public:
    ConstraintGeneratorCategoryPlugin()
        : TypedCategoryPlugin("ConstraintGenerator")
    {
        // TODO: Replace empty string by synopsis for the wiki page.
        // document_synopsis("");
    }
} _category_plugin;

} // namespace
