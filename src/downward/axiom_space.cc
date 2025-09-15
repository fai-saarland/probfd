#include "downward/axiom_space.h"

#include "downward/variable_space.h"

namespace downward {

bool AxiomSpace::is_derived(VariableProxy var) const
{
    return is_derived(var.get_id());
}

int AxiomSpace::get_axiom_layer(VariableProxy var) const
{
    return get_axiom_layer(var.get_id());
}

int AxiomSpace::get_default_axiom_value(VariableProxy var) const
{
    return get_default_axiom_value(var.get_id());
}

}