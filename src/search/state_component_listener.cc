#include "state_component_listener.h"

#include "plugin.h"

namespace state_component {

StateComponentListener::StateComponentListener()
{
    timer_.stop();
    timer_.reset();
    initialized_ = false;
}

void
StateComponentListener::initialize()
{
}

void
StateComponentListener::print_statistics() const
{
    std::cout << "  Heuristic refinement: " << timer_ << std::endl;
    statistics();
}

bool
StateComponentListener::requires_recognized_neighbors_implicitly() const
{
    return requires_recognized_neighbors_explicitly();
}

bool
StateComponentListener::requires_recognized_neighbors_explicitly() const
{
    return false;
}

const utils::Timer&
StateComponentListener::get_timer() const
{
    return timer_;
}

StateComponentListener::Status
StateComponentListener::notify_new_component(
    StateComponent& component,
    StateComponent& recognized_neighbors)
{
    if (!initialized_) {
        initialized_ = true;
        initialize();
    }
    timer_.resume();
    auto res = on_new_component(component, recognized_neighbors);
    timer_.stop();
    return res;
}

StateComponentListener::Status
StateComponentListener::notify_new_component(
    StateComponent&& component,
    StateComponent&& recognized_neighbors)
{
    return notify_new_component(component, recognized_neighbors);
}

} // namespace state_component

static PluginTypePlugin<state_component::StateComponentListener>
    _plugin_type("StateComponentListener", "");

