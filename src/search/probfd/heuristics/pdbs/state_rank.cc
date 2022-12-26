#include "probfd/heuristics/pdbs/state_rank.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::ostream& operator<<(std::ostream& out, const StateRank& s)
{
    return out << "StateRank(" << s.id << ")";
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

namespace utils {
using namespace probfd::heuristics::pdbs;
void feed(HashState& h, const StateRank& s)
{
    feed(h, s.id);
}

} // namespace utils
