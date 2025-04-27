#ifndef CLASSICAL_OPERATOR_SPACE_H
#define CLASSICAL_OPERATOR_SPACE_H

#include "downward/fact_pair.h"
#include "downward/operator_space.h"
#include "downward/proxy_collection.h"

#include <string>

namespace downward {
struct FactPair;
class State;
} // namespace downward

namespace downward {

class AxiomEvaluator;
class OperatorProxy;
class OperatorEffectsProxy;
class OperatorEffectConditionsProxy;

class ClassicalOperatorSpace
    : public OperatorSpace
    , public std::ranges::view_interface<ClassicalOperatorSpace> {
    using OperatorSpace::size;
    using OperatorSpace::operator[];

public:
    virtual int get_num_operator_effects(int op_index) const = 0;
    virtual int
    get_num_operator_effect_conditions(int op_index, int eff_index) const = 0;
    virtual FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const = 0;
    virtual FactPair get_operator_effect(int op_index, int eff_index) const = 0;

    std::size_t size() const { return this->get_num_operators(); }

    OperatorProxy operator[](std::size_t index) const;
    OperatorProxy operator[](OperatorID id) const;
};

class OperatorEffectConditionsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<OperatorEffectConditionsProxy> {
    const ClassicalOperatorSpace* op_space;
    int op_index;
    int eff_index;

public:
    OperatorEffectConditionsProxy(
        const ClassicalOperatorSpace& op_space,
        int op_index,
        int eff_index)
        : op_space(&op_space)
        , op_index(op_index)
        , eff_index(eff_index)
    {
    }

    std::size_t size() const
    {
        return op_space->get_num_operator_effect_conditions(
            op_index,
            eff_index);
    }

    FactPair operator[](std::size_t index) const
    {
        assert(index < size());
        return op_space->get_operator_effect_condition(
            op_index,
            eff_index,
            index);
    }
};

class OperatorEffectProxy {
    const ClassicalOperatorSpace* op_space;
    int op_index;
    int eff_index;

public:
    OperatorEffectProxy(
        const ClassicalOperatorSpace& op_space,
        int op_index,
        int eff_index)
        : op_space(&op_space)
        , op_index(op_index)
        , eff_index(eff_index)
    {
    }

    OperatorEffectConditionsProxy get_conditions() const
    {
        return OperatorEffectConditionsProxy(*op_space, op_index, eff_index);
    }

    FactPair get_fact() const
    {
        return op_space->get_operator_effect(op_index, eff_index);
    }
};

class OperatorEffectsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<OperatorEffectsProxy> {
    const ClassicalOperatorSpace* op_space;
    int op_index;

public:
    OperatorEffectsProxy(const ClassicalOperatorSpace& op_space, int op_index)
        : op_space(&op_space)
        , op_index(op_index)
    {
    }

    std::size_t size() const
    {
        return op_space->get_num_operator_effects(op_index);
    }

    OperatorEffectProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return OperatorEffectProxy(*op_space, op_index, eff_index);
    }
};

class OperatorProxy {
    const ClassicalOperatorSpace* op_space;
    int index;

public:
    OperatorProxy(const ClassicalOperatorSpace& op_space, int index)
        : op_space(&op_space)
        , index(index)
    {
    }

    operator PartialOperatorProxy() const
    {
        return PartialOperatorProxy(*op_space, index);
    }

    OperatorPreconditionsProxy get_preconditions() const
    {
        return OperatorPreconditionsProxy(*op_space, index);
    }

    std::string get_name() const { return op_space->get_operator_name(index); }

    int get_id() const { return index; }

    OperatorEffectsProxy get_effects() const
    {
        return OperatorEffectsProxy(*op_space, index);
    }

    State get_unregistered_successor(
        const State& state,
        AxiomEvaluator& axiom_evaluator) const;

    friend bool
    operator==(const OperatorProxy& left, const OperatorProxy& right)
    {
        assert(left.op_space == right.op_space);
        return left.index == right.index;
    }
};

inline OperatorProxy ClassicalOperatorSpace::operator[](std::size_t index) const
{
    assert(index < size());
    return OperatorProxy(*this, index);
}

inline OperatorProxy ClassicalOperatorSpace::operator[](OperatorID id) const
{
    return (*this)[id.get_index()];
}

} // namespace downward

#endif