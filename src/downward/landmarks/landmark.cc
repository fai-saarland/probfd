#include "downward/landmarks/landmark.h"

using namespace std;

namespace downward::landmarks {
bool Landmark::is_true_in_state(const State& state) const
{
    if (disjunctive) {
        for (const FactPair& fact : facts) {
            if (state[fact.var].get_value() == fact.value) {
                return true;
            }
        }
        return false;
    } else {
        // conjunctive or simple
        for (const FactPair& fact : facts) {
            if (state[fact.var].get_value() != fact.value) {
                return false;
            }
        }
        return true;
    }
}
} // namespace landmarks
