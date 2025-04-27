#ifndef OPERATOR_SPACE_H
#define OPERATOR_SPACE_H

#include "downward/fact_pair.h"
#include "downward/operator_id.h"
#include "downward/proxy_collection.h"

#include <string>

namespace downward {
struct FactPair;
}

namespace downward {

class OperatorPreconditionsProxy;
class PartialOperatorProxy;

class OperatorSpace
    : public ProxyCollectionTag
    , public std::ranges::view_interface<OperatorSpace> {
public:
    virtual ~OperatorSpace() = default;

    virtual std::string get_operator_name(int index) const = 0;
    virtual int get_num_operators() const = 0;
    virtual int get_num_operator_preconditions(int index) const = 0;
    virtual FactPair
    get_operator_precondition(int op_index, int fact_index) const = 0;

    std::size_t size() const { return this->get_num_operators(); }

    PartialOperatorProxy operator[](std::size_t index) const;
    PartialOperatorProxy operator[](OperatorID id) const;
};

class OperatorPreconditionsProxy
    : public ProxyCollectionTag
    , public std::ranges::view_interface<OperatorPreconditionsProxy> {
protected:
    const OperatorSpace* op_space;
    int op_index;

public:
    OperatorPreconditionsProxy(const OperatorSpace& op_space, int op_index)
        : op_space(&op_space)
        , op_index(op_index)
    {
    }

    std::size_t size() const
    {
        return op_space->get_num_operator_preconditions(op_index);
    }

    FactPair operator[](std::size_t fact_index) const
    {
        assert(fact_index < size());
        return op_space->get_operator_precondition(op_index, fact_index);
    }
};

class PartialOperatorProxy {
    const OperatorSpace* op_space;
    int index;

public:
    PartialOperatorProxy(const OperatorSpace& task, int index)
        : op_space(&task)
        , index(index)
    {
    }

    OperatorPreconditionsProxy get_preconditions() const
    {
        return OperatorPreconditionsProxy(*op_space, index);
    }

    std::string get_name() const { return op_space->get_operator_name(index); }

    int get_id() const { return index; }

    friend bool operator==(
        const PartialOperatorProxy& left,
        const PartialOperatorProxy& right)
    {
        assert(left.op_space == right.op_space);
        return left.index == right.index;
    }
};

inline PartialOperatorProxy OperatorSpace::operator[](std::size_t index) const
{
    assert(index < size());
    return PartialOperatorProxy(*this, index);
}

inline PartialOperatorProxy OperatorSpace::operator[](OperatorID id) const
{
    return (*this)[id.get_index()];
}

} // namespace downward

#endif