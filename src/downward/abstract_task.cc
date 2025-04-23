#include "downward/abstract_task.h"

#include "downward/task_utils/causal_graph.h"

namespace downward {

const causal_graph::CausalGraph& AbstractTask::get_causal_graph() const
{
    return causal_graph::get_causal_graph(this);
}

}
