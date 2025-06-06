#include "downward/cli/plugins/plugin.h"

#include "downward/operator_counting/constraint_generator.h"

using namespace std;
using namespace downward::operator_counting;

using namespace downward::cli::plugins;

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
