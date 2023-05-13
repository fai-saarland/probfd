#ifndef PROBFD_HEURISTICS_PDBS_SATURATED_COST_PARTITIONING_GZOCP_HEURISTIC_H
#define PROBFD_HEURISTICS_PDBS_SATURATED_COST_PARTITIONING_GZOCP_HEURISTIC_H

#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/types.h"

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "probfd/heuristics/pdbs/pattern_generator.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class GZOCPHeuristic : public TaskDependentHeuristic {
    enum OrderingStrategy { RANDOM, SIZE_ASC, SIZE_DESC, INHERIT };

    std::vector<ProbabilisticPatternDatabase> pdbs;
    OrderingStrategy ordering;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit GZOCPHeuristic(const options::Options& opts);

    explicit GZOCPHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log,
        std::shared_ptr<PatternCollectionGenerator>
            generator,
        OrderingStrategy order,
        std::shared_ptr<utils::RandomNumberGenerator> rng);

    void print_statistics() const override
    {
        // TODO
    }

protected:
    EvaluationResult evaluate(const State& state) const override;

public:
    static void add_options_to_parser(options::OptionParser& parser);
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif