#include "probfd/probabilistic_task.h"

#include "probfd/task_utils/causal_graph.h"

namespace probfd {

const causal_graph::ProbabilisticCausalGraph&
ProbabilisticTask::get_causal_graph() const
{
    return causal_graph::get_causal_graph(this);
}

}
