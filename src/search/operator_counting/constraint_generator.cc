#include "operator_counting/constraint_generator.h"

#include "plugin.h"

using namespace std;

namespace operator_counting {
void ConstraintGenerator::initialize_constraints(OperatorCost, vector<lp::LPConstraint> &, double) {
}

static PluginTypePlugin<ConstraintGenerator> _type_plugin(
    "ConstraintGenerator",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");
}
