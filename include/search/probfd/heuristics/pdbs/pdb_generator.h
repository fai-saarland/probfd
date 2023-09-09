#ifndef PROBFD_HEURISTICS_PDBS_PDB_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_PDB_GENERATOR_H

#include "probfd/heuristics/pdbs/types.h"

#include "downward/utils/logging.h"

#include <memory>

namespace probfd {
class ProbabilisticTask;
namespace heuristics {
namespace pdbs {

class ProbabilityAwarePatternDatabase;

class PDBGenerator {
protected:
    mutable utils::LogProxy log;

public:
    explicit PDBGenerator(const plugins::Options& opts);
    explicit PDBGenerator(const utils::LogProxy& log);
    virtual ~PDBGenerator() = default;

    virtual std::unique_ptr<ProbabilityAwarePatternDatabase>
    generate(const std::shared_ptr<ProbabilisticTask>& task) = 0;
};

extern void add_pattern_generator_options_to_feature(plugins::Feature& feature);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_GENERATOR_H
