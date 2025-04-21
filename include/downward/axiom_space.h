#ifndef AXIOM_SPACE_H
#define AXIOM_SPACE_H

#include "downward/fact_pair.h"

#include <string>

namespace downward {

class AxiomSpace {
public:
    virtual ~AxiomSpace() = default;

    virtual int get_variable_axiom_layer(int var) const = 0;
    virtual int get_variable_default_axiom_value(int var) const = 0;
    virtual int get_num_axioms() const = 0;
    virtual std::string get_axiom_name(int index) const = 0;
    virtual int get_num_axiom_preconditions(int index) const = 0;
    virtual FactPair
    get_axiom_precondition(int op_index, int fact_index) const = 0;
    virtual int get_num_axiom_effects(int op_index) const = 0;
    virtual int
    get_num_axiom_effect_conditions(int op_index, int eff_index) const = 0;
    virtual FactPair
    get_axiom_effect_condition(int op_index, int eff_index, int cond_index)
        const = 0;
    virtual FactPair get_axiom_effect(int op_index, int eff_index) const = 0;
};

} // namespace downward

#endif
