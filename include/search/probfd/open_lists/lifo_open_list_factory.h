#ifndef PROBFD_OPEN_LISTS_LIFO_OPEN_LIST_FACTORY_H
#define PROBFD_OPEN_LISTS_LIFO_OPEN_LIST_FACTORY_H

#include "probfd/open_lists/task_open_list_factory.h"

namespace probfd {
namespace open_lists {

class LifoOpenListFactory : public FDROpenListFactory {
public:
    ~LifoOpenListFactory() override = default;

    std::shared_ptr<FDROpenList> create_open_list(FDRMDP* state_space) override;
};

} // namespace open_lists
} // namespace probfd

#endif // __LIFO_OPEN_LIST_H__