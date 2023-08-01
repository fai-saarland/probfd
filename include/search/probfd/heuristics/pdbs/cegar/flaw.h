#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_H

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

struct Flaw {
    int variable;
    bool is_precondition;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif