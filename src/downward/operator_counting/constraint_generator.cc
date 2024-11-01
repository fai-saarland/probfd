#include "downward/operator_counting/constraint_generator.h"

#include "downward/plugins/plugin.h"

using namespace std;

namespace operator_counting {
void ConstraintGenerator::initialize_constraints(
    const shared_ptr<AbstractTask>&,
    lp::LinearProgram&)
{
}

static class ConstraintGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<ConstraintGenerator> {
public:
    ConstraintGeneratorCategoryPlugin()
        : TypedCategoryPlugin("ConstraintGenerator")
    {
        // TODO: Replace empty string by synopsis for the wiki page.
        // document_synopsis("");
    }
} _category_plugin;
} // namespace operator_counting
