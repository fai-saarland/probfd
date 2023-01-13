#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_FINDER_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_FINDER_FACTORY_H

#include <memory>
#include <string>

namespace probfd {
class ProbabilisticTask;

namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename>
class FlawFindingStrategy;

template <typename PDBType>
class FlawFindingStrategyFactory {
public:
    virtual ~FlawFindingStrategyFactory() = default;

    virtual std::unique_ptr<FlawFindingStrategy<PDBType>>
    create_flaw_finder(const ProbabilisticTask* task) = 0;

    virtual std::string get_name() const = 0;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif