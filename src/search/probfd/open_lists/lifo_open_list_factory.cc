#include "probfd/open_lists/lifo_open_list_factory.h"
#include "probfd/open_lists/lifo_open_list.h"

#include "downward/operator_id.h"

namespace probfd {
namespace open_lists {

std::shared_ptr<FDROpenList> LifoOpenListFactory::create_open_list(FDRMDP*)
{
    return std::make_shared<LifoOpenList<OperatorID>>();
}

} // namespace open_lists
} // namespace probfd