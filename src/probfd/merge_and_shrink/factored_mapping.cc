#include "probfd/merge_and_shrink/factored_mapping.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/types.h"

#include "probfd/task_utils/tuple_enumerator.h"

#include "downward/state.h"

#include "downward/utils/logging.h"

#include <algorithm>
#include <cassert>
#include <numeric>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

FactoredMapping::FactoredMapping(int table_size)
    : lookup_table(table_size)
{ std::iota(lookup_table.begin(), lookup_table.end(), 0); }

void FactoredMapping::apply_abstraction(const vector<int>& alpha)
{
    for (int& entry : lookup_table) {
        if (entry != PRUNED_STATE) { entry = alpha[entry]; }
    }
}

/**
 * @brief Represents an atomic factored mapping
 * \f[
 *     \textrm{AtomicFM}_{i, \alpha} : S \rightharpoonup S',
 *     s \mapsto \alpha(s(i))
 * \f]
 * between factored states
 * \f$S = \prod_{i \in \indexset} S_i\f$
 * and abstract states
 * \f$S_i\f$
 * for a specified index
 * \f$i \in I\f$
 * and abstraction mapping
 * \f$\alpha : S_i \to S'\f$.
 *
 * All sets are assumed to be sets of consecutive integers.
 */
class FactoredMappingAtomic final : public FactoredMapping {
    const int var_id;

public:
    /// Constructs an atomic factored mapping with \p var_id as
    /// the index \f$i\f$, the identity function as the abstraction mapping and
    /// \p num_local_states as the cardinality of \f$S_i\f$.
    FactoredMappingAtomic(int var_id, int num_local_states);

    int get_abstract_state(const State& state) const override;

    bool is_total() const override;

    void dump(utils::LogProxy& log) const override;
};

/**
 * @brief Represents a merge factored mapping
 * \f[
 *     \textrm{MergeFM}_{\sigma_1, \sigma_2, \alpha} :
 *         S \rightharpoonup S',
 *     s \to \alpha(\sigma_1(s), \sigma_2(s))
 * \f]
 * between factored states
 * \f$S = \prod_{i \in \indexset} S_i\f$
 * and abstract states
 * \f$S'_1 \times S'_2\f$
 * for two given factored mappings
 * \f$\sigma_{1} : S \pto S'_1\f$
 * and
 * \f$\sigma_{2} : S \pto S'_2\f$
 * and an abstraction mapping
 * \f$\alpha : S'_1 \times S'_2 \rightharpoonup S'\f$.
 *
 * All sets are assumed to be sets of consecutive integers.
 */
class FactoredMappingMerge final : public FactoredMapping {
    enumeration::PairEnumerator enumerator;
    std::unique_ptr<FactoredMapping> left_child;
    std::unique_ptr<FactoredMapping> right_child;

public:
    /// Constructs a merge factored mapping for the children factored mappings
    /// \p left_child and \p right_child with co-domain cardinalities
    /// as specified by \p prod .
    FactoredMappingMerge(
        enumeration::PairEnumerator linearization,
        std::unique_ptr<FactoredMapping> left_child,
        std::unique_ptr<FactoredMapping> right_child);

    int get_abstract_state(const State& state) const override;

    bool is_total() const override;

    void dump(utils::LogProxy& log) const override;
};

FactoredMappingAtomic::FactoredMappingAtomic(int var_id, int num_local_states)
    : FactoredMapping(num_local_states)
    , var_id(var_id)
{
}

int FactoredMappingAtomic::get_abstract_state(const State& state) const
{ return lookup_table[state[var_id]]; }

bool FactoredMappingAtomic::is_total() const
{ return !std::ranges::contains(lookup_table, PRUNED_STATE); }

void FactoredMappingAtomic::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log.println("abstract state lookup table (leaf): {}", lookup_table);
    }
}

FactoredMappingMerge::FactoredMappingMerge(
    enumeration::PairEnumerator linearization,
    unique_ptr<FactoredMapping> left_child,
    unique_ptr<FactoredMapping> right_child)
    : FactoredMapping(linearization.get_product_size())
    , enumerator(std::move(linearization))
    , left_child(std::move(left_child))
    , right_child(std::move(right_child))
{
}

int FactoredMappingMerge::get_abstract_state(const State& state) const
{
    const int state1 = left_child->get_abstract_state(state);
    const int state2 = right_child->get_abstract_state(state);
    if (state1 == PRUNED_STATE || state2 == PRUNED_STATE) return PRUNED_STATE;
    const int merged_state = enumerator.to_index(state1, state2);
    return lookup_table[merged_state];
}

bool FactoredMappingMerge::is_total() const
{
    return !std::ranges::contains(lookup_table, PRUNED_STATE) &&
           left_child->is_total() && right_child->is_total();
}

void FactoredMappingMerge::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log.println("abstract state lookup table (merge): ");

        auto it = lookup_table.begin();
        const auto end = lookup_table.end();

        while (it != end) {
            auto sub_end = it + enumerator.get_size<0>();
            assert(it != sub_end);
            log.println("{}", std::ranges::subrange(it, sub_end));
            it = sub_end;
        }

        log.println("left child:");
        left_child->dump(log);
        log.println("right child:");
        right_child->dump(log);
    }
}

std::unique_ptr<FactoredMapping>
create_projection_fm(int index, int num_local_states)
{ return std::make_unique<FactoredMappingAtomic>(index, num_local_states); }

std::unique_ptr<FactoredMapping> create_merge_fm(
    enumeration::PairEnumerator enumerator,
    std::unique_ptr<FactoredMapping> left_child,
    std::unique_ptr<FactoredMapping> right_child)
{
    return std::make_unique<FactoredMappingMerge>(
        enumerator,
        std::move(left_child),
        std::move(right_child));
}

} // namespace probfd::merge_and_shrink