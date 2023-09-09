#ifndef PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_H

#include "probfd/heuristics/pdbs/pattern_collection_generator.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <memory>

namespace probfd {
class ProbabilisticTask;
namespace heuristics {
namespace pdbs {

class PDBCollectionInformation;

class PDBCollectionGenerator {
protected:
    mutable utils::LogProxy log;

public:
    explicit PDBCollectionGenerator(const plugins::Options& opts);
    explicit PDBCollectionGenerator(utils::LogProxy log);

    virtual ~PDBCollectionGenerator() = default;

    virtual PDBCollectionInformation generate(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) = 0;
};

extern void
add_pdb_collection_generator_options_to_feature(plugins::Feature& feature);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_H
