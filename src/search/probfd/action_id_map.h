#ifndef MDPS_ACTION_ID_MAP_H
#define MDPS_ACTION_ID_MAP_H

#include "engine_interfaces/action_id_map.h"
#include "probabilistic_operator.h"

#include <cassert>

namespace probfd {
namespace engine_interfaces {

/**
 * @brief The specialiation of ActionIDMap for ProbabilisticOperator.
 *
 * @see ActionIDMap
 */
template <>
class ActionIDMap<const ProbabilisticOperator*> {
public:
    explicit ActionIDMap(const std::vector<ProbabilisticOperator>& operators);
    explicit ActionIDMap();

    ActionID get_action_id(
        const StateID& state_id,
        const ProbabilisticOperator* const& op);

    const ProbabilisticOperator*
    get_action(const StateID& state_id, const ActionID& action_id);

private:
    const ProbabilisticOperator* first_;
#ifndef NDEBUG
    std::size_t num_operators_;
#endif
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __ACTION_ID_MAP_H__