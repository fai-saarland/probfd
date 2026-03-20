#ifndef PROBFD_MERGE_AND_SHRINK_FACTORED_MAPPING_H
#define PROBFD_MERGE_AND_SHRINK_FACTORED_MAPPING_H

#include "probfd/task_utils/tuple_enumerator_fwd.h"

#include <memory>
#include <vector>

namespace downward {
class State;
}

namespace downward::utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {

/**
 * @brief Represents a factored mapping
 * \f$\sigma : S \rightharpoonup S'\f$
 * between factored states
 * \f$S = \prod_{i = 0}^{n-1} S_i\f$
 * with local state sets \f$S_i = \{ 0, 1, \dots, m_i - 1 \}\f$
 * and a set of abstract states, represented by an index set
 * \f$S' = \{ 0, 1, \dots, m - 1 \}\f$.
 */
class FactoredMapping {
protected:
    std::vector<int> lookup_table;

    explicit FactoredMapping(int table_size);

public:
    virtual ~FactoredMapping() = default;

    /**
     * @brief Chains an abstraction mapping
     * \f$\alpha : S' \rightharpoonup S''\f$
     * with this factored mapping, transforming it into the mapping
     * \f$\alpha \circ \sigma : S \rightharpoonup S''\f$.
     *
     * @param alpha The abstraction mapping is represented as a list of size
     * at least \f$m\f$ specifying a new abstract state index for each previous
     * abstract state index.
     * If a state is not in the domain of the abstraction mapping, it shall be
     * mapped to the special value PRUNED_STATE.
     */
    void apply_abstraction(const std::vector<int>& alpha);

    /// Returns the abstract state $\f$\sigma(s)\f$ for a factored state
    /// \f$s\f$.
    /// If \f$s \notin \sigma\f$, the special value PRUNED_STATE is returned.
    virtual int get_abstract_state(const downward::State& state) const = 0;

    /// Returns true iff the represented function is total.
    virtual bool is_total() const = 0;

    /// Prints a textual representation of the factored mapping to a log.
    virtual void dump(downward::utils::LogProxy& log) const = 0;
};

/**
 * @brief Constructs a projection factored mapping representing the factored
 * mapping
 * \f[
 *     \pi_{i} : \prod_{i = 0}^{n - 1} S_i \rightharpoonup S_i,
 *     s \mapsto s(i)
 * \f]
 * for the specified index \f$i \in \{ 0, ..., n - 1 \}\f$.
 *
 * @param index The index \f$i \in \{ 0, ..., n - 1 \}\f$.
 * @param num_local_states The cardinality of the index set \f$S_i\f$.
 */
std::unique_ptr<FactoredMapping>
create_projection_fm(int index, int num_local_states);

/**
 * @brief Constructs the merge factored mapping
 * \f[
 *     \textrm{MergeFM}_{\sigma_1, \sigma_2, f} :
 *         \prod_{i = 0}^{n - 1} S_i \rightharpoonup
 *         \{ 0, \dots, |S'_1 \times S'_2| - 1 \},
 *     s \to f(\sigma_1(s), \sigma_2(s))
 * \f]
 * where \f$f : S'_1 \times S'_2 \to \{ 0, \dots, |S'_1 \times S'_2| - 1 \}\f$
 * is an indexing function and
 * \f$\sigma_{1} : \prod_{i = 0}^{n - 1} S_i \pto S'_1\f$
 * and
 * \f$\sigma_{2} : \prod_{i = 0}^{n - 1} S_i \pto S'_2\f$
 * are factored mappings.
 *
 * @param enumerator The indexing function \f$f\f$
 * @param left_child The factored mapping \f$\sigma_1\f$
 * @param right_child The factored mapping \f$\sigma_2\f$
 */
std::unique_ptr<FactoredMapping> create_merge_fm(
    enumeration::PairEnumerator enumerator,
    std::unique_ptr<FactoredMapping> left_child,
    std::unique_ptr<FactoredMapping> right_child);

} // namespace probfd::merge_and_shrink

#endif
