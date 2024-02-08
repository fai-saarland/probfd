#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_REPRESENTATION_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_AND_SHRINK_REPRESENTATION_H

#include "probfd/value_type.h"

#include <memory>
#include <vector>

class State;

namespace utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class Distances;
class MergeAndShrinkDistanceRepresentation;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeAndShrinkRepresentation {
protected:
    int domain_size;
    std::vector<int> lookup_table;

public:
    explicit MergeAndShrinkRepresentation(int domain_size);
    virtual ~MergeAndShrinkRepresentation() = default;

    int get_domain_size() const;

    void scale(int scale);

    virtual void apply_abstraction_to_lookup_table(
        const std::vector<int>& abstraction_mapping) = 0;
    /*
      Return the value that state is mapped to. This is either an abstract
      state (if set_distances has not been called) or a distance (if it has).
      If the represented function is not total, the returned value is DEAD_END
      if the abstract state is PRUNED_STATE or if the (distance) value is
      INFINITE_VALUE.
    */
    virtual int get_abstract_state(const State& state) const = 0;
    /* Return true iff the represented function is total, i.e., does not map
       to PRUNED_STATE. */
    virtual bool is_total() const = 0;
    virtual void dump(utils::LogProxy& log) const = 0;

    virtual std::unique_ptr<MergeAndShrinkDistanceRepresentation>
    create_distance_representation(const Distances& distances) = 0;
};

class MergeAndShrinkRepresentationLeaf : public MergeAndShrinkRepresentation {
    const int var_id;

public:
    MergeAndShrinkRepresentationLeaf(int var_id, int domain_size);

    void apply_abstraction_to_lookup_table(
        const std::vector<int>& abstraction_mapping) override;
    int get_abstract_state(const State& state) const override;
    bool is_total() const override;
    void dump(utils::LogProxy& log) const override;

    std::unique_ptr<MergeAndShrinkDistanceRepresentation>
    create_distance_representation(const Distances& distances) override;
};

class MergeAndShrinkRepresentationMerge : public MergeAndShrinkRepresentation {
    std::unique_ptr<MergeAndShrinkRepresentation> left_child;
    std::unique_ptr<MergeAndShrinkRepresentation> right_child;

public:
    MergeAndShrinkRepresentationMerge(
        std::unique_ptr<MergeAndShrinkRepresentation> left_child,
        std::unique_ptr<MergeAndShrinkRepresentation> right_child);

    void apply_abstraction_to_lookup_table(
        const std::vector<int>& abstraction_mapping) override;
    int get_abstract_state(const State& state) const override;
    bool is_total() const override;
    void dump(utils::LogProxy& log) const override;

    std::unique_ptr<MergeAndShrinkDistanceRepresentation>
    create_distance_representation(const Distances& distances) override;
};

class MergeAndShrinkDistanceRepresentation {
protected:
    std::vector<value_t> lookup_table;

public:
    explicit MergeAndShrinkDistanceRepresentation(std::size_t table_size);
    virtual ~MergeAndShrinkDistanceRepresentation() = default;

    virtual value_t get_abstract_distance(const State& state) const = 0;
    virtual void dump(utils::LogProxy& log) const = 0;
};

class MergeAndShrinkDistanceRepresentationLeaf
    : public MergeAndShrinkDistanceRepresentation {
    int var_id;

public:
    MergeAndShrinkDistanceRepresentationLeaf(
        int var_id,
        std::vector<int>& state_lookup_table,
        const Distances& distances);

    value_t get_abstract_distance(const State& state) const override;
    void dump(utils::LogProxy& log) const override;
};

class MergeAndShrinkDistanceRepresentationMerge
    : public MergeAndShrinkDistanceRepresentation {
    std::unique_ptr<MergeAndShrinkRepresentation> left_child;
    std::unique_ptr<MergeAndShrinkRepresentation> right_child;

public:
    MergeAndShrinkDistanceRepresentationMerge(
        std::unique_ptr<MergeAndShrinkRepresentation> left_child,
        std::unique_ptr<MergeAndShrinkRepresentation> right_child,
        std::vector<int>& state_lookup_table,
        const Distances& distances);

    value_t get_abstract_distance(const State& state) const override;
    void dump(utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
