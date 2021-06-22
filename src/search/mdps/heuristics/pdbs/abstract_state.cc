#include "abstract_state.h"

namespace probabilistic {
namespace pdbs {

std::ostream& operator<<(std::ostream& out, const AbstractState& s)
{
    return out << "PDBState(" << s.id << ")";
}

} // namespace pdbs
} // namespace probabilistic

namespace utils {

void feed(HashState& h, const probabilistic::pdbs::AbstractState& s)
{
    feed(h, s.id);
}

} // namespace utils
