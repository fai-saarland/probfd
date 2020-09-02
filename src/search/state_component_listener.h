#pragma once

#include "state_component.h"
#include "utils/timer.h"

#include <memory>

namespace state_component {

class StateComponentListener {
public:
    enum class Status {
        KeepListening,
        Mark,
        StopListening,
    };
    StateComponentListener();
    virtual ~StateComponentListener() = default;

    Status notify_new_component(
        StateComponent& component,
        StateComponent& recognized_neighbors);

    Status notify_new_component(
        StateComponent&& component,
        StateComponent&& recognized_neighbors);

    virtual bool requires_recognized_neighbors_implicitly() const;
    virtual bool requires_recognized_neighbors_explicitly() const;

    void print_statistics() const;

    const utils::Timer& get_timer() const;

protected:
    virtual Status on_new_component(StateComponent&, StateComponent&) = 0;

    virtual void initialize();
    virtual void statistics() const { }

private:
    bool initialized_;
    utils::Timer timer_;
};

} // namespace state_component
