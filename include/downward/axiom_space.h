#ifndef AXIOM_SPACE_H
#define AXIOM_SPACE_H

#include "downward/algorithms/subscriber.h"

#include "downward/fact_pair.h"
#include "downward/proxy_collection.h"

#include <string>

namespace downward {

class AxiomProxy;
class VariableProxy;
class AxiomPreconditionsProxy;
class AxiomEffectConditionsProxy;
class AxiomEffectsProxy;

class AxiomSpace
    : public ProxyCollectionTag
    , public subscriber::SubscriberService<AxiomSpace>
    , public std::ranges::view_interface<AxiomSpace> {
public:
    virtual int get_variable_axiom_layer(int var) const = 0;
    virtual int get_variable_default_axiom_value(int var) const = 0;
    virtual int get_num_axioms() const = 0;
    virtual std::string get_axiom_name(int index) const = 0;
    virtual int get_num_axiom_preconditions(int index) const = 0;
    virtual FactPair
    get_axiom_precondition(int op_index, int fact_index) const = 0;
    virtual int get_num_axiom_effects(int op_index) const = 0;
    virtual int
    get_num_axiom_effect_conditions(int op_index, int eff_index) const = 0;
    virtual FactPair
    get_axiom_effect_condition(int op_index, int eff_index, int cond_index)
        const = 0;
    virtual FactPair get_axiom_effect(int op_index, int eff_index) const = 0;

    bool is_derived(int var) const
    {
        int axiom_layer = this->get_variable_axiom_layer(var);
        return axiom_layer != -1;
    }

    int get_axiom_layer(int var) const
    {
        int axiom_layer = this->get_variable_axiom_layer(var);
        /*
          This should only be called for derived variables.
          Non-derived variables have axiom_layer == -1.
          Use var.is_derived() to check.
        */
        assert(axiom_layer >= 0);
        return axiom_layer;
    }

    int get_default_axiom_value(int var) const
    {
        assert(is_derived(var));
        return this->get_variable_default_axiom_value(var);
    }

    bool is_derived(VariableProxy var) const;
    int get_axiom_layer(VariableProxy var) const;
    int get_default_axiom_value(VariableProxy var) const;

    AxiomProxy operator[](std::size_t index) const;

    std::size_t size() const { return this->get_num_axioms(); }
};

class AxiomPreconditionsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<AxiomPreconditionsProxy> {
    const AxiomSpace* axiom_space;
    int axiom_index;

public:
    AxiomPreconditionsProxy(const AxiomSpace& axiom_space, int axiom_index)
        : axiom_space(&axiom_space)
        , axiom_index(axiom_index)
    {
    }

    std::size_t size() const
    {
        return axiom_space->get_num_axiom_preconditions(axiom_index);
    }

    FactPair operator[](std::size_t fact_index) const
    {
        assert(fact_index < size());
        return axiom_space->get_axiom_precondition(axiom_index, fact_index);
    }
};

class AxiomEffectConditionsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<AxiomEffectConditionsProxy> {
    const AxiomSpace* axiom_space;
    int axiom_index;
    int eff_index;

public:
    AxiomEffectConditionsProxy(
        const AxiomSpace& axiom_space,
        int axiom_index,
        int eff_index)
        : axiom_space(&axiom_space)
        , axiom_index(axiom_index)
        , eff_index(eff_index)
    {
    }

    std::size_t size() const
    {
        return axiom_space->get_num_axiom_effect_conditions(
            axiom_index,
            eff_index);
    }

    FactPair operator[](std::size_t index) const
    {
        assert(index < size());
        return axiom_space->get_axiom_effect_condition(
            axiom_index,
            eff_index,
            index);
    }
};

class AxiomEffectProxy {
    const AxiomSpace* axiom_space;
    int axiom_index;
    int eff_index;

public:
    AxiomEffectProxy(
        const AxiomSpace& axiom_space,
        int axiom_index,
        int eff_index)
        : axiom_space(&axiom_space)
        , axiom_index(axiom_index)
        , eff_index(eff_index)
    {
    }

    AxiomEffectConditionsProxy get_conditions() const
    {
        return AxiomEffectConditionsProxy(*axiom_space, axiom_index, eff_index);
    }

    FactPair get_fact() const
    {
        return axiom_space->get_axiom_effect(axiom_index, eff_index);
    }
};

class AxiomEffectsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<AxiomEffectsProxy> {
    const AxiomSpace* axiom_space;
    int op_index;

public:
    AxiomEffectsProxy(const AxiomSpace& axiom_space, int op_index)
        : axiom_space(&axiom_space)
        , op_index(op_index)
    {
    }

    std::size_t size() const
    {
        return axiom_space->get_num_axiom_effects(op_index);
    }

    AxiomEffectProxy operator[](std::size_t eff_index) const
    {
        assert(eff_index < size());
        return AxiomEffectProxy(*axiom_space, op_index, eff_index);
    }
};

class AxiomProxy {
    const AxiomSpace* axiom_space;
    int index;

public:
    AxiomProxy(const AxiomSpace& axiom_space, int index)
        : axiom_space(&axiom_space)
        , index(index)
    {
    }

    AxiomPreconditionsProxy get_preconditions() const
    {
        return AxiomPreconditionsProxy(*axiom_space, index);
    }

    AxiomEffectsProxy get_effects() const
    {
        return AxiomEffectsProxy(*axiom_space, index);
    }

    std::string get_name() const { return axiom_space->get_axiom_name(index); }

    int get_id() const { return index; }

    friend bool operator==(const AxiomProxy& left, const AxiomProxy& right)
    {
        assert(left.axiom_space == right.axiom_space);
        return left.index == right.index;
    }
};

inline AxiomProxy AxiomSpace::operator[](std::size_t index) const
{
    assert(index < size());
    return AxiomProxy(*this, index);
}

} // namespace downward

#endif
