#ifndef PROBFD_MERGE_AND_SHRINK_FACTORED_MAPPING_H
#define PROBFD_MERGE_AND_SHRINK_FACTORED_MAPPING_H

#include "probfd/value_type.h"

#include <vector>

class State;

namespace utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {

class FactoredMappingMerge;

class FactoredMapping {
protected:
    std::vector<int> lookup_table;

public:
    explicit FactoredMapping(int domain_size);
    virtual ~FactoredMapping() = default;

    int get_num_abstract_states() const;

    void apply_abstraction(const std::vector<int>& abstraction_mapping);

    /// Returns the abstract state this state is mapped to, or PRUNED_STATE if
    /// the state is not in the domain of the mapping.
    virtual int get_abstract_state(const State& state) const = 0;

    /* Return true iff the represented function is total, i.e., does not map
       to PRUNED_STATE. */
    virtual bool is_total() const = 0;

    virtual void dump(utils::LogProxy& log) const = 0;

private:
    friend class FactoredMappingMerge;
    void scale(int scale);
};

class FactoredMappingAtomic : public FactoredMapping {
    const int var_id;

public:
    FactoredMappingAtomic(int var_id, int domain_size);

    int get_abstract_state(const State& state) const override;
    bool is_total() const override;
    void dump(utils::LogProxy& log) const override;
};

class FactoredMappingMerge : public FactoredMapping {
    std::unique_ptr<FactoredMapping> left_child;
    std::unique_ptr<FactoredMapping> right_child;

public:
    FactoredMappingMerge(
        std::unique_ptr<FactoredMapping> left_child,
        std::unique_ptr<FactoredMapping> right_child);

    int get_abstract_state(const State& state) const override;
    bool is_total() const override;
    void dump(utils::LogProxy& log) const override;

    std::pair<int, int> get_children_states(int state) const;
};

} // namespace probfd::merge_and_shrink

#endif
