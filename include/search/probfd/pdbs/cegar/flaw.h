#ifndef PROBFD_PDBS_CEGAR_FLAW_H
#define PROBFD_PDBS_CEGAR_FLAW_H

namespace probfd {
namespace pdbs {
namespace cegar {

struct Flaw {
    int variable;
    bool is_precondition;
};

} // namespace cegar
} // namespace pdbs
} // namespace probfd

#endif // PROBFD_PDBS_CEGAR_FLAW_H
