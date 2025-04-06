#include "downward/open_list_factory.h"

using namespace std;

namespace downward {

template <>
unique_ptr<StateOpenList> OpenListFactory::create_open_list()
{
    return create_state_open_list();
}

template <>
unique_ptr<EdgeOpenList> OpenListFactory::create_open_list()
{
    return create_edge_open_list();
}

} // namespace downward