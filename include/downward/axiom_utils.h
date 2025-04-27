#ifndef AXIOM_UTILS_H
#define AXIOM_UTILS_H

#include "downward/proxy_collection.h"

#include "downward/axiom_space.h"
#include "downward/classical_operator_space.h"

#include <variant>

namespace downward {

class PreconditionsProxy;
class EffectConditionsProxy;
class EffectsProxy;

class PreconditionsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<PreconditionsProxy> {
    std::variant<AxiomPreconditionsProxy, OperatorPreconditionsProxy> proxy;

public:
    PreconditionsProxy(AxiomPreconditionsProxy proxy)
        : proxy(proxy)
    {
    }

    PreconditionsProxy(OperatorPreconditionsProxy proxy)
        : proxy(proxy)
    {
    }

    std::size_t size() const
    {
        return std::visit([](const auto& arg) { return arg.size(); }, proxy);
    }

    FactPair operator[](std::size_t fact_index) const
    {
        return std::visit(
            [fact_index](const auto& arg) {
                return arg.operator[](fact_index);
            },
            proxy);
    }
};

class EffectConditionsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<EffectConditionsProxy> {
    std::variant<AxiomEffectConditionsProxy, OperatorEffectConditionsProxy>
        proxy;

public:
    EffectConditionsProxy(AxiomEffectConditionsProxy proxy)
        : proxy(proxy)
    {
    }

    EffectConditionsProxy(OperatorEffectConditionsProxy proxy)
        : proxy(proxy)
    {
    }

    std::size_t size() const
    {
        return std::visit([](const auto& arg) { return arg.size(); }, proxy);
    }

    FactPair operator[](std::size_t fact_index) const
    {
        return std::visit(
            [fact_index](const auto& arg) {
                return arg.operator[](fact_index);
            },
            proxy);
    }
};

class EffectProxy {
    std::variant<AxiomEffectProxy, OperatorEffectProxy> proxy;

public:
    EffectProxy(AxiomEffectProxy proxy)
        : proxy(proxy)
    {
    }

    EffectProxy(OperatorEffectProxy proxy)
        : proxy(proxy)
    {
    }

    EffectConditionsProxy get_conditions() const
    {
        return std::visit(
            [](const auto& arg) {
                return EffectConditionsProxy(arg.get_conditions());
            },
            proxy);
    }

    FactPair get_fact() const
    {
        return std::visit(
            [](const auto& arg) { return arg.get_fact(); },
            proxy);
    }
};

class EffectsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<EffectsProxy> {
    std::variant<AxiomEffectsProxy, OperatorEffectsProxy> proxy;

public:
    EffectsProxy(AxiomEffectsProxy proxy)
        : proxy(proxy)
    {
    }

    EffectsProxy(OperatorEffectsProxy proxy)
        : proxy(proxy)
    {
    }

    std::size_t size() const
    {
        return std::visit([](const auto& arg) { return arg.size(); }, proxy);
    }

    EffectProxy operator[](std::size_t eff_index) const
    {
        return std::visit(
            [eff_index](const auto& arg) {
                return EffectProxy(arg.operator[](eff_index));
            },
            proxy);
    }
};

class AxiomOrOperatorProxy {
    std::variant<AxiomProxy, OperatorProxy> proxy;

public:
    AxiomOrOperatorProxy(AxiomProxy proxy)
        : proxy(proxy)
    {
    }

    AxiomOrOperatorProxy(OperatorProxy proxy)
        : proxy(proxy)
    {
    }

    operator AxiomProxy() const
    {
        assert(is_axiom());
        return std::get<AxiomProxy>(proxy);
    }

    operator OperatorProxy() const
    {
        assert(!is_axiom());
        return std::get<OperatorProxy>(proxy);
    }

    bool is_axiom() const { return proxy.index() == 0; }

    PreconditionsProxy get_preconditions() const
    {
        return std::visit(
            [](const auto& arg) {
                return PreconditionsProxy(arg.get_preconditions());
            },
            proxy);
    }

    EffectsProxy get_effects() const
    {
        return std::visit(
            [](const auto& arg) { return EffectsProxy(arg.get_effects()); },
            proxy);
    }

    std::string get_name() const
    {
        return std::visit(
            [](const auto& arg) { return arg.get_name(); },
            proxy);
    }

    int get_id() const
    {
        return std::visit([](const auto& arg) { return arg.get_id(); }, proxy);
    }

    friend bool operator==(
        const AxiomOrOperatorProxy& left,
        const AxiomOrOperatorProxy& right)
    {
        return left.proxy == right.proxy;
    }
};
} // namespace downward

#endif