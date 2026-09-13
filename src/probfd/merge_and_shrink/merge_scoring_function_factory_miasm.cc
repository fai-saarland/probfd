#include "probfd/merge_and_shrink/merge_scoring_function_factory_miasm.h"

#include "probfd/merge_and_shrink/merge_scoring_function_miasm.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_scoring_function_miasm_utils.h"
#include "probfd/merge_and_shrink/shrink_strategy.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/probabilistic_task.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeScoringFunctionFactoryMIASM::MergeScoringFunctionFactoryMIASM(
    bool use_caching,
    std::shared_ptr<ShrinkStrategy> shrink_strategy,
    int max_states,
    int max_states_before_merge,
    int shrink_threshold_before_merge)
    : use_caching(use_caching)
    , shrink_strategy(std::move(shrink_strategy))
    , max_states(max_states)
    , max_states_before_merge(max_states_before_merge)
    , shrink_threshold_before_merge(shrink_threshold_before_merge)
{
}

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionFactoryMIASM::compute_scoring_function(
    const FactoredTransitionSystem& fts)
{
    return std::make_unique<MergeScoringFunctionMIASM>(
        fts,
        use_caching,
        shrink_strategy,
        max_states,
        max_states_before_merge,
        shrink_threshold_before_merge);
}

void MergeScoringFunctionFactoryMIASM::dump_function_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log.println("Use caching: {}", use_caching ? "yes" : "no");
    }
}

string MergeScoringFunctionFactoryMIASM::name() const
{
    return "miasm";
}

} // namespace probfd::merge_and_shrink