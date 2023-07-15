#ifndef PROBFD_ENGINE_INTERFACES_NEW_STATE_OBSERVER_H
#define PROBFD_ENGINE_INTERFACES_NEW_STATE_OBSERVER_H

#include "probfd/type_traits.h"

#include <memory>
#include <vector>

namespace probfd {
namespace engine_interfaces {

/**
 * @brief An observer interface notified of newly encountered states during
 * search.
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
class NewStateObserver {
public:
    virtual ~NewStateObserver() = default;

    /**
     * @brief Called when a new state is encountered.
     */
    virtual void notify_state(param_type<State>) {}

    /**
     * @brief Called when a new goal state is encountered.
     */
    virtual void notify_goal(param_type<State>) {}

    /**
     * @brief Called when a new dead end state is encountered.
     */
    virtual void notify_dead(param_type<State>) {}
};

/**
 * @brief Implements a list of NewStateObservers which are notified in order.
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
class NewStateObserverList : public NewStateObserver<State> {
    std::vector<std::shared_ptr<NewStateObserver<State>>> handlers_;

public:
    explicit NewStateObserverList(
        std::vector<std::shared_ptr<NewStateObserver<State>>> handlers)
        : handlers_(std::move(handlers))
    {
    }

    virtual ~NewStateObserverList() = default;

    virtual void notify_state(param_type<State> s) override
    {
        for (auto& handler : handlers_) {
            handler->notify_state(s);
        }
    }

    virtual void notify_goal(param_type<State> s) override
    {
        for (auto& handler : handlers_) {
            handler->notify_goal(s);
        }
    }

    virtual void notify_dead(param_type<State> s) override
    {
        for (auto& handler : handlers_) {
            handler->notify_dead(s);
        }
    }
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __NEW_STATE_HANDLER_H__