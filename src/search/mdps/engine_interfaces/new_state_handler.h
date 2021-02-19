#pragma once

namespace probabilistic {

template<typename State>
struct NewStateHandler {
    using is_default_implementation = std::true_type;

    void touch(const State&) { }
    void touch_goal(const State&) { }
    void touch_dead_end(const State&) { }
};

} // namespace probabilistic
