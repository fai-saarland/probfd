#ifndef AXIOMS_H
#define AXIOMS_H

#include "downward/axiom_evaluator.h"

namespace downward {

template <class Entry>
class PerTaskInformation;

extern PerTaskInformation<AxiomEvaluator> g_axiom_evaluators;

}

#endif
