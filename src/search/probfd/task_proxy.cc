
#include "probfd/task_proxy.h"

#include "probfd/task_utils/causal_graph.h"

namespace probfd {
const causal_graph::ProbabilisticCausalGraph&
ProbabilisticTaskProxy::get_causal_graph() const
{
    return causal_graph::get_causal_graph(
        static_cast<const ProbabilisticTask*>(task));
}
} // namespace probfd