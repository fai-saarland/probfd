#include "downward/abstract_task.h"

#include <iostream>

using namespace std;

namespace downward {
const FactPair FactPair::no_fact = FactPair(-1, -1);

ostream& operator<<(ostream& os, const FactPair& fact_pair)
{
    os << fact_pair.var << "=" << fact_pair.value;
    return os;
}
} // namespace downward