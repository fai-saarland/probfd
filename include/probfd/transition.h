#ifndef PROBFD_TRANSITION_H
#define PROBFD_TRANSITION_H

#include "probfd/distribution.h"
#include "probfd/types.h"

namespace probfd {

template <typename Action>
struct Transition {
    Action action;
    Distribution<StateID> successor_dist;
};

} // namespace probfd

#endif