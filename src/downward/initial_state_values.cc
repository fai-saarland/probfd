#include "downward/initial_state_values.h"

#include "downward/state.h"

namespace downward {

State InitialStateValues::get_initial_state() const
{
    return State(this->get_initial_state_values());
}

}