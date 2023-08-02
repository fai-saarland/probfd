#include "probfd/open_lists/fifo_open_list_factory.h"
#include "probfd/open_lists/fifo_open_list.h"

#include "downward/operator_id.h"

namespace probfd {
namespace open_lists {

std::shared_ptr<TaskOpenList> FifoOpenListFactory::create_open_list(TaskMDP*)
{
    return std::make_shared<FifoOpenList<OperatorID>>();
}

} // namespace open_lists
} // namespace probfd