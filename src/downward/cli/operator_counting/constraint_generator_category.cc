#include "downward/cli/operator_counting/constraint_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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
};
}

namespace downward::cli::operator_counting {

void add_constraint_generator_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<ConstraintGeneratorCategoryPlugin>();
}

} // namespace
