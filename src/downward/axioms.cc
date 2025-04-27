#include "downward/axioms.h"

#include "downward/axiom_evaluator.h"

#include "downward/per_task_information.h"

using namespace std;

namespace downward {

PerComponentInformation<AxiomEvaluator, VariableSpace, AxiomSpace>
    g_axiom_evaluators;

} // namespace downward