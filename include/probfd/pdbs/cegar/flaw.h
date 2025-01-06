#ifndef PROBFD_PDBS_CEGAR_FLAW_H
#define PROBFD_PDBS_CEGAR_FLAW_H

namespace probfd::pdbs::cegar {

struct Flaw {
    int variable;
    bool is_precondition;
};

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_FLAW_H
