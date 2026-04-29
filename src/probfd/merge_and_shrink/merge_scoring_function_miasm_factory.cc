#include "probfd/merge_and_shrink/merge_scoring_function_miasm_factory.h"

#include "downward/utils/logging.h"
#include "probfd/merge_and_shrink/merge_scoring_function_miasm.h"

namespace probfd::merge_and_shrink {

MergeScoringFunctionMIASMFactory::MergeScoringFunctionMIASMFactory(
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
MergeScoringFunctionMIASMFactory::create(SharedProbabilisticTask task)
{
    return std::make_unique<MergeScoringFunctionMIASM>(
        downward::to_refs(task),
        use_caching,
        shrink_strategy,
        max_states,
        max_states_before_merge,
        shrink_threshold_before_merge);
}

std::string MergeScoringFunctionMIASMFactory::name() const
{ return "miasm"; }

void MergeScoringFunctionMIASMFactory::dump_function_specific_options(
    downward::utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log.println("Use caching: {}", use_caching ? "yes" : "no");
    }
}

} // namespace probfd::merge_and_shrink