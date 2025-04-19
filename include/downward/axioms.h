#ifndef AXIOMS_H
#define AXIOMS_H

#include "downward/axiom_evaluator.h"
#include "downward/per_task_information.h"

namespace downward {

extern PerTaskInformation<AxiomEvaluator> g_axiom_evaluators;

}

#endif
