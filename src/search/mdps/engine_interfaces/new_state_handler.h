#pragma once

namespace probabilistic {

/**
 * @brief Function object used as a callback when a new state is encountered
 * during search.
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
struct NewStateHandler {
    /// Tags the default implementation.
    using is_default_implementation = std::true_type;

    virtual ~NewStateHandler() = default;

    /**
     * Called when a new state is encountered.
     */
    virtual void touch(const State&) {}

    /**
     * Called when a new goal state is encountered.
     */
    virtual void touch_goal(const State&) {}

    /**
     * Called when a new dead end state is encountered.
     */
    virtual void touch_dead_end(const State&) {}
};

} // namespace probabilistic
