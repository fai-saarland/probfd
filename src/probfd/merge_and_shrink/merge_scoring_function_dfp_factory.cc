#include "probfd/merge_and_shrink/merge_scoring_function_dfp_factory.h"

#include "probfd/merge_and_shrink/merge_scoring_function_dfp.h"

namespace probfd::merge_and_shrink {

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionDFPFactory::create(SharedProbabilisticTask)
{ return std::make_unique<MergeScoringFunctionDFP>(); }

std::string MergeScoringFunctionDFPFactory::name() const
{ return "dfp"; }

} // namespace probfd::merge_and_shrink