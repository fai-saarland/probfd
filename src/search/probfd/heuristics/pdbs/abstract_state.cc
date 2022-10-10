#include "abstract_state.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::ostream& operator<<(std::ostream& out, const AbstractState& s)
{
    return out << "PDBState(" << s.id << ")";
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

namespace utils {
using namespace probfd::heuristics::pdbs;
void feed(HashState& h, const AbstractState& s)
{
    feed(h, s.id);
}

} // namespace utils
