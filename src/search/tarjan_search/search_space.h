#pragma once

#include "../algorithms/segmented_vector.h"
#include "../global_state.h"
#include "../state_id.h"

#include <algorithm>
#include <vector>

class GlobalOperator;

namespace tarjan_search {

template<typename Info, typename Node>
class SearchSpaceBase {
public:
    SearchSpaceBase();
    virtual ~SearchSpaceBase() = default;

    Node operator[](const GlobalState& state);
    Node operator[](const StateID& state_id);
    void trace_path(Node node, std::vector<const GlobalOperator*>& path);

private:
    segmented_vector::DynamicSegmentedVector<Info> data_;
};

template<typename Info, typename Node>
SearchSpaceBase<Info, Node>::SearchSpaceBase()
{
}

template<typename Info, typename Node>
Node
SearchSpaceBase<Info, Node>::operator[](const GlobalState& state)
{
    return Node(&data_[state.get_id().hash()], state.get_id());
}

template<typename Info, typename Node>
Node
SearchSpaceBase<Info, Node>::operator[](const StateID& state_id)
{
    return Node(&data_[state_id.hash()], state_id);
}

template<typename Info, typename Node>
void
SearchSpaceBase<Info, Node>::trace_path(
    Node node,
    std::vector<const GlobalOperator*>& path)
{
    if (node.get_operator() != nullptr) {
        path.push_back(node.get_operator());
        trace_path(operator[](node.get_parent_state_id()), path);
    } else {
        std::reverse(path.begin(), path.end());
    }
}

} // namespace tarjan_search
