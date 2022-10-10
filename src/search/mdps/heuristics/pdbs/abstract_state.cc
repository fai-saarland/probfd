#include "abstract_state.h"

namespace probabilistic {
namespace heuristics {
namespace pdbs {

std::ostream& operator<<(std::ostream& out, const AbstractState& s)
{
    return out << "PDBState(" << s.id << ")";
}

} // namespace pdbs
} // namespace heuristics
} // namespace probabilistic

namespace utils {
using namespace probabilistic::heuristics::pdbs;
void feed(HashState& h, const AbstractState& s)
{
    feed(h, s.id);
}

} // namespace utils
