#ifndef AXIOMS_H
#define AXIOMS_H

namespace downward {
class VariableSpace;
class AxiomSpace;

class AxiomEvaluator;

template <class Entry, typename... Components>
class PerComponentInformation;
} // namespace downward

namespace downward {

extern PerComponentInformation<AxiomEvaluator, VariableSpace, AxiomSpace>
    g_axiom_evaluators;

} // namespace downward

#endif
