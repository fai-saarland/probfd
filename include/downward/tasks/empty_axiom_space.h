#ifndef EMPTY_AXIOM_SPACE_H
#define EMPTY_AXIOM_SPACE_H

#include "downward/axiom_space.h"

namespace downward::tasks {

class EmptyAxiomSpace : public AxiomSpace {
public:
    int get_variable_axiom_layer(int) const override { return -1; }

    int get_variable_default_axiom_value(int) const override { return -1; }

    int get_num_axioms() const override { return 0; }

    std::string get_axiom_name(int) const override { abort(); }

    int get_num_axiom_preconditions(int) const override { abort(); }

    downward::FactPair get_axiom_precondition(int, int) const override
    {
        abort();
    }

    int get_num_axiom_effects(int) const override { abort(); }

    int get_num_axiom_effect_conditions(int, int) const override { abort(); }

    downward::FactPair get_axiom_effect_condition(int, int, int) const override
    {
        abort();
    }

    downward::FactPair get_axiom_effect(int, int) const override { abort(); }
};

} // namespace downward::tasks

#endif // EMPTY_AXIOM_SPACE_H
