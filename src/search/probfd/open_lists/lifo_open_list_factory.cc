#include "probfd/open_lists/lifo_open_list_factory.h"
#include "probfd/open_lists/lifo_open_list.h"

namespace probfd {
namespace open_lists {

std::shared_ptr<engine_interfaces::OpenList<OperatorID>>
LifoOpenListFactory::create_open_list(
    engine_interfaces::HeuristicSearchConnector*,
    engine_interfaces::StateIDMap<State>*,
    engine_interfaces::ActionIDMap<OperatorID>*)
{
    return std::make_shared<LifoOpenList<OperatorID>>();
}

} // namespace open_lists
} // namespace probfd