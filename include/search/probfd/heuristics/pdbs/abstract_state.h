#ifndef MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_H
#define MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_H

#include "utils/hash.h"

#include <ostream>

namespace probfd {
namespace heuristics {
namespace pdbs {

struct AbstractState {
    long long int id;
    explicit AbstractState(long long int id)
        : id(id)
    {
    }

    bool operator==(const AbstractState& x) const { return id == x.id; }
    bool operator<(const AbstractState& c) const { return id < c.id; }
    bool operator>(const AbstractState& c) const { return id > c.id; }

    AbstractState operator+(const AbstractState& x) const
    {
        return AbstractState(x.id + id);
    }

    AbstractState operator-(const AbstractState& x) const
    {
        return AbstractState(id - x.id);
    }

    AbstractState& operator+=(const AbstractState& x)
    {
        id += x.id;
        return *this;
    }

    AbstractState& operator-=(const AbstractState& x)
    {
        id -= x.id;
        return *this;
    }
};

extern std::ostream& operator<<(std::ostream& out, const AbstractState& s);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

namespace std {
template <>
struct hash<probfd::heuristics::pdbs::AbstractState> {
    size_t
    operator()(const probfd::heuristics::pdbs::AbstractState& state) const
    {
        return hash<long long int>()(state.id);
    }
};
} // namespace std

namespace utils {
void feed(HashState& h, const probfd::heuristics::pdbs::AbstractState& s);
} // namespace utils

#endif // __ABSTRACT_STATE_H__