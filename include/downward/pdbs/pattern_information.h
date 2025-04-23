#ifndef PDBS_PATTERN_INFORMATION_H
#define PDBS_PATTERN_INFORMATION_H

#include "downward/pdbs/types.h"

#include "downward/state.h"

#include <memory>

namespace downward {
class AbstractTask;
}

namespace downward::utils {
class LogProxy;
}

namespace downward::pdbs {
/*
  This class is a wrapper for a pair of a pattern and the corresponding PDB.
  It always contains a pattern and can contain the computed PDB. If the latter
  is not set, it is computed on demand.
  Ownership of the information is shared between the creators of this class
  (usually PatternGenerators), the class itself, and its users (consumers of
  patterns like heuristics).

  TODO: consider using this class not for shared ownership but for actual
  ownership transfer, from the generator to the user.
*/
class PatternInformation {
    const AbstractTask& task;
    Pattern pattern;
    std::shared_ptr<PatternDatabase> pdb;

    void create_pdb_if_missing();

    bool information_is_valid() const;

public:
    PatternInformation(
        const AbstractTask& task,
        Pattern pattern,
        utils::LogProxy& log);

    void set_pdb(const std::shared_ptr<PatternDatabase>& pdb);

    const AbstractTask& get_task() const { return task; }

    const Pattern& get_pattern() const;
    std::shared_ptr<PatternDatabase> get_pdb();
};
} // namespace pdbs

#endif
