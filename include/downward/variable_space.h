#ifndef VARIABLE_SPACE_H
#define VARIABLE_SPACE_H

#include "downward/fact_pair.h"

#include "downward/proxy_collection.h"

#include <string>

namespace downward {

class VariableProxy;
class FactProxy;
class FactsProxy;

class VariableSpace
    : public ProxyCollectionTag
    , public std::ranges::view_interface<VariableSpace> {
public:
    virtual ~VariableSpace() = default;

    virtual int get_num_variables() const = 0;
    virtual int get_variable_domain_size(int var) const = 0;

    virtual std::string get_variable_name(int var) const = 0;
    virtual std::string get_fact_name(const FactPair& fact) const = 0;

    std::size_t size() const { return this->get_num_variables(); }

    VariableProxy operator[](std::size_t index) const;

    FactsProxy get_facts() const;

    FactProxy get_fact_proxy(FactPair fact) const;
};

class FactProxy {
    const VariableSpace* var_space;
    FactPair fact;

public:
    FactProxy(const VariableSpace& var_space, int var_id, int value);
    FactProxy(const VariableSpace& var_space, const FactPair& fact);

    ~FactProxy() = default;

    VariableProxy get_variable() const;

    int get_value() const { return fact.value; }

    FactPair get_pair() const { return fact; }

    std::string get_name() const { return var_space->get_fact_name(fact); }

    friend bool operator==(const FactProxy& left, const FactProxy& right)
    {
        assert(left.var_space == right.var_space);
        return left.fact == right.fact;
    }
};

/*
  Proxy class for the collection of all facts of a var_space.

  We don't implement size() because it would not be constant-time.

  FactsProxy supports iteration, e.g. for range-based for loops. This
  iterates over all facts in order of increasing variable ID, and in
  order of increasing value for each variable.
*/
class FactsProxy {
    class FactsProxyIterator {
        const VariableSpace* var_space;
        int var_id;
        int value;

    public:
        FactsProxyIterator(
            const VariableSpace& var_space,
            int var_id,
            int value)
            : var_space(&var_space)
            , var_id(var_id)
            , value(value)
        {
        }

        FactProxy operator*() const
        {
            return FactProxy(*var_space, var_id, value);
        }

        FactsProxyIterator& operator++()
        {
            assert(var_id < var_space->get_num_variables());
            int num_facts = var_space->get_variable_domain_size(var_id);
            assert(value < num_facts);
            ++value;
            if (value == num_facts) {
                ++var_id;
                value = 0;
            }
            return *this;
        }

        friend bool operator==(
            const FactsProxyIterator& left,
            const FactsProxyIterator& right)
        {
            assert(left.var_space == right.var_space);
            return left.var_id == right.var_id && left.value == right.value;
        }
    };

    const VariableSpace* var_space;

public:
    explicit FactsProxy(const VariableSpace& var_space)
        : var_space(&var_space)
    {
    }

    FactsProxyIterator begin() const
    {
        return FactsProxyIterator(*var_space, 0, 0);
    }

    FactsProxyIterator end() const
    {
        return FactsProxyIterator(
            *var_space,
            var_space->get_num_variables(),
            0);
    }
};

class VariableProxy {
    const VariableSpace* var_space;
    int id;

public:
    VariableProxy(const VariableSpace& var_space, int id)
        : var_space(&var_space)
        , id(id)
    {
    }

    int get_id() const { return id; }

    std::string get_name() const { return var_space->get_variable_name(id); }

    int get_domain_size() const
    {
        return var_space->get_variable_domain_size(id);
    }

    FactProxy get_fact(int index) const
    {
        assert(index < get_domain_size());
        return FactProxy(*var_space, id, index);
    }

    friend bool
    operator==(const VariableProxy& left, const VariableProxy& right)
    {
        assert(left.var_space == right.var_space);
        return left.id == right.id;
    }
};

inline VariableProxy VariableSpace::operator[](std::size_t index) const
{
    assert(index < size());
    return VariableProxy(*this, index);
}

inline FactsProxy VariableSpace::get_facts() const
{
    return FactsProxy(*this);
}

inline FactProxy::FactProxy(
    const VariableSpace& var_space,
    const FactPair& fact)
    : var_space(&var_space)
    , fact(fact)
{
    assert(fact.var >= 0 && fact.var < var_space.get_num_variables());
    assert(fact.value >= 0 && fact.value < get_variable().get_domain_size());
}

inline FactProxy::FactProxy(
    const VariableSpace& var_space,
    int var_id,
    int value)
    : FactProxy(var_space, FactPair(var_id, value))
{
}

inline VariableProxy FactProxy::get_variable() const
{
    return VariableProxy(*var_space, fact.var);
}

inline FactProxy VariableSpace::get_fact_proxy(FactPair fact) const
{
    return FactProxy(*this, fact);
}

} // namespace downward

#endif
