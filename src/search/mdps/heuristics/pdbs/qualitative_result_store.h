#pragma once

#include "abstract_state.h"

#include <unordered_set>

namespace probabilistic {
namespace pdbs {

class ProbabilisticProjection;

class QualitativeResultStore {
public:
    struct assignable_bool_t {
    public:
        assignable_bool_t& operator=(const bool value);
        operator bool();

    private:
        friend class QualitativeResultStore;
        assignable_bool_t(
            const AbstractState& s,
            QualitativeResultStore* store);
        const AbstractState state_;
        QualitativeResultStore* ref_;
    };
    QualitativeResultStore();
    QualitativeResultStore(QualitativeResultStore&&) = default;
    virtual ~QualitativeResultStore() = default;

    /*virtual*/ void negate_all();
    /*virtual*/ void clear();
    /*virtual*/ void set(const AbstractState& s, bool val);
    /*virtual*/ bool get(const AbstractState& s);

    assignable_bool_t operator[](const AbstractState& s);

private:
    bool is_negated_;
    std::unordered_set<int> states_;
};

} // namespace pdbs
} // namespace probabilistic
