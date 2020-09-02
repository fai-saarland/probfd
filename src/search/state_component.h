#pragma once

#include "../global_state.h"

namespace state_component {

template<typename Value>
struct Component {
    virtual ~Component() = default;
    virtual Value current() = 0;
    virtual void next(unsigned i = 1) = 0;
    virtual bool end() = 0;
    virtual void reset() = 0;
};

template<typename Value>
struct SingletonComponent : public Component<Value> {
private:
    bool ended;
    Value state;

public:
    explicit SingletonComponent(Value state);
    virtual Value current() override;
    virtual void next(unsigned) override;
    virtual void reset() override;
    virtual bool end() override;
};

template<typename Value, typename Iterator>
struct ComponentIterator : public Component<Value> {
private:
    Iterator begin_;
    Iterator current_;
    Iterator end_;

public:
    explicit ComponentIterator(Iterator i, Iterator end);
    virtual Value current() override;
    virtual void next(unsigned i = 1) override;
    virtual void reset() override;
    virtual bool end() override;
};

using StateComponent = Component<GlobalState>;
template<typename Iterator>
using StateComponentIterator = ComponentIterator<GlobalState, Iterator>;

using SuccessorComponent = Component<std::pair<int, GlobalState>>;
template<typename Iterator>
using SuccessorComponentIterator =
    ComponentIterator<std::pair<int, GlobalState>, Iterator>;

template<typename Value>
SingletonComponent<Value>::SingletonComponent(Value state)
    : ended(false)
    , state(state)
{
}

template<typename Value>
Value
SingletonComponent<Value>::current()
{
    return state;
}

template<typename Value>
void
SingletonComponent<Value>::next(unsigned)
{
    ended = true;
}

template<typename Value>
void
SingletonComponent<Value>::reset()
{
    ended = false;
}

template<typename Value>
bool
SingletonComponent<Value>::end()
{
    return ended;
}

template<typename Value, typename Iterator>
ComponentIterator<Value, Iterator>::ComponentIterator(Iterator i, Iterator end)
    : begin_(i)
    , current_(i)
    , end_(end)
{
}

template<typename Value, typename Iterator>
Value
ComponentIterator<Value, Iterator>::current()
{
    return *current_;
}

template<typename Value, typename Iterator>
void
ComponentIterator<Value, Iterator>::next(unsigned i)
{
    while (i-- > 0) {
        current_++;
    }
}

template<typename Value, typename Iterator>
void
ComponentIterator<Value, Iterator>::reset()
{
    current_ = begin_;
}

template<typename Value, typename Iterator>
bool
ComponentIterator<Value, Iterator>::end()
{
    return current_ == end_;
}

} // namespace state_components
