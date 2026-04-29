#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_H

#include "probfd/probabilistic_task.h"

#include <memory>
#include <string>

namespace downward::utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class MergeScoringFunction;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeScoringFunctionFactory {
public:
    virtual ~MergeScoringFunctionFactory() = default;

    virtual std::unique_ptr<MergeScoringFunction>
    create(SharedProbabilisticTask task) = 0;

    void dump_options(downward::utils::LogProxy& log) const;

private:
    virtual std::string name() const = 0;

    virtual void
    dump_function_specific_options(downward::utils::LogProxy&) const
    {
    }
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_H
