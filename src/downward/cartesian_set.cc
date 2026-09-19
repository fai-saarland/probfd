#include "downward/cartesian_set.h"

#include "downward/state.h"

#include <sstream>

using namespace std;

namespace downward {

void CartesianSet::add(int var, int value)
{
    domain_subsets[var].set(value);
}

void CartesianSet::remove(int var, int value)
{
    domain_subsets[var].reset(value);
}

void CartesianSet::set_single_value(int var, int value)
{
    remove_all(var);
    add(var, value);
}

void CartesianSet::add_all(int var)
{
    domain_subsets[var].set();
}

void CartesianSet::remove_all(int var)
{
    domain_subsets[var].reset();
}

int CartesianSet::count(int var) const
{
    return domain_subsets[var].count();
}

bool CartesianSet::intersects(const CartesianSet& other, int var) const
{
    return dynamic_bitset::intersects(
        domain_subsets[var],
        other.domain_subsets[var]);
}

bool CartesianSet::contains(const State& state) const
{
    return contains_facts(state | as_fact_pair_set, *this);
}

bool is_superset_of(const CartesianSet& lhs, const CartesianSet& rhs)
{
    int num_vars = lhs.domain_subsets.size();
    for (int var = 0; var < num_vars; ++var) {
        if (!dynamic_bitset::is_subset_of(
                rhs.domain_subsets[var],
                lhs.domain_subsets[var]))
            return false;
    }
    return true;
}

ostream& operator<<(ostream& os, const CartesianSet& cartesian_set)
{
    int num_vars = cartesian_set.domain_subsets.size();
    string var_sep;
    os << "<";
    for (int var = 0; var < num_vars; ++var) {
        const Bitset& domain = cartesian_set.domain_subsets[var];
        vector<int> values;
        for (size_t value = 0; value < domain.size(); ++value) {
            if (domain[value]) values.push_back(value);
        }
        assert(!values.empty());
        if (values.size() < domain.size()) {
            os << var_sep << var << "={";
            string value_sep;
            for (int value : values) {
                os << value_sep << value;
                value_sep = ",";
            }
            os << "}";
            var_sep = ",";
        }
    }
    return os << ">";
}
} // namespace downward