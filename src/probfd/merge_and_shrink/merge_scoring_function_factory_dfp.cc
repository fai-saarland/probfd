#include "probfd/merge_and_shrink/merge_scoring_function_factory_dfp.h"

#include "probfd/merge_and_shrink/merge_scoring_function_dfp.h"

#include <cassert>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionFactoryDFP::compute_scoring_function(
    const ProbabilisticTaskTuple&)
{
    return std::make_unique<MergeScoringFunctionDFP>();
}

string MergeScoringFunctionFactoryDFP::name() const
{
    return "dfp";
}

} // namespace probfd::merge_and_shrink