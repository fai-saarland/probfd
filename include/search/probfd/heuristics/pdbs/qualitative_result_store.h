#ifndef MDPS_HEURISTICS_PDBS_QUALITATIVE_RESULT_STORE_H
#define MDPS_HEURISTICS_PDBS_QUALITATIVE_RESULT_STORE_H

#include "probfd/heuristics/pdbs/abstract_state.h"

#include "probfd/types.h"

#include "utils/iterators.h"

#include <unordered_set>

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProbabilisticProjection;

class QualitativeResultStore {
public:
    struct assignable_bool_t {
    public:
        assignable_bool_t& operator=(const bool value);
        operator bool() const;

    private:
        friend class QualitativeResultStore;
        assignable_bool_t(
            const AbstractState& s,
            QualitativeResultStore* store);
        const AbstractState state_;
        QualitativeResultStore* ref_;
    };

    QualitativeResultStore() = default;
    QualitativeResultStore(
        bool is_negated,
        std::unordered_set<AbstractState> states);

    void negate_all();
    void clear();
    void set(const AbstractState& s, bool val);
    bool get(const AbstractState& s) const;

    assignable_bool_t operator[](const AbstractState& s);
    bool operator[](const AbstractState& s) const;

    assignable_bool_t operator[](int s);
    bool operator[](int s) const;

    std::unordered_set<AbstractState>& get_storage();
    const std::unordered_set<AbstractState>& get_storage() const;

private:
    bool is_negated_ = false;
    std::unordered_set<AbstractState> states_;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

namespace utils {
template <>
class set_output_iterator<probfd::heuristics::pdbs::QualitativeResultStore> {
    probfd::heuristics::pdbs::QualitativeResultStore& store;

public:
    set_output_iterator(probfd::heuristics::pdbs::QualitativeResultStore& store)
        : store(store)
    {
    }

    decltype(auto) operator=(const ::probfd::StateID& id)
    {
        return store[id.id] = true;
    }

    set_output_iterator& operator++() { return *this; }

    set_output_iterator operator++(int) { return *this; }

    set_output_iterator& operator*() { return *this; }
};

bool contains(
    probfd::heuristics::pdbs::QualitativeResultStore& store,
    probfd::heuristics::pdbs::AbstractState s);

} // namespace utils

#endif