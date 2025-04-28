
#ifndef PROBFD_UTILS_OBSERVER_H
#define PROBFD_UTILS_OBSERVER_H

#include "probfd/concepts.h"

#include <functional>
#include <type_traits>
#include <vector>

namespace probfd {

template <typename Event>
using EventCallback = std::function<void(const Event&)>;

/// This concept is satisfied by types that implement handler functions
/// for the specific events.
template <typename T, typename... Events>
concept EventHandlerFor =
    requires(T& t, const Events&... event) { (t.handleEvent(event), ...); };

template <typename... Events>
class Observable {
protected:
    template <typename Event>
    using ObserverList = std::vector<EventCallback<Event>>;

private:
    std::tuple<ObserverList<Events>...> observers;

protected:
    template <downward::MemberOf<Events...> Event>
    void notifyEvent(const Event& event)
    {
        for (auto& observer : std::get<ObserverList<Event>>(this->observers)) {
            observer(event);
        }
    }

    template <typename... ForEvents>
        requires(downward::MemberOf<ForEvents, Events...> && ...)
    void notifyEvents(const ForEvents&... events)
    {
        (this->notifyEvent(events), ...);
    }

public:
    template <typename T>
        requires(EventHandlerFor<T, Events> || ...)
    void registerObserverForSupported(T& observer)
    {
        auto f = [this, &observer]<typename E>() {
            if constexpr (EventHandlerFor<T, E>) {
                this->template registerObserver<E>(observer);
            }
        };

        (f.template operator()<Events>(), ...);
    }

    template <typename... ForEvents>
        requires(downward::MemberOf<ForEvents, Events...> && ...)
    void registerObserver(EventHandlerFor<ForEvents...> auto& observer)
    {
        (std::get<ObserverList<ForEvents>>(this->observers)
             .emplace_back([&observer](const ForEvents& event) {
                 observer.handleEvent(event);
             }),
         ...);
    }

    template <typename... ForEvents, typename F>
        requires(
            (downward::MemberOf<ForEvents, Events...> && ...) &&
            !EventHandlerFor<F, ForEvents...> &&
            (std::constructible_from<EventCallback<ForEvents>, F &&> && ...))
    void registerObserver(F&& f)
    {
        (std::get<ObserverList<ForEvents>>(this->observers)
             .emplace_back(std::forward<F>(f)),
         ...);
    }

    template <downward::MemberOf<Events...> Event>
    void registerObserver(const EventCallback<Event>& observer)
    {
        std::get<ObserverList<Event>>(this->observers).push_back(observer);
    }

    template <downward::MemberOf<Events...> Event>
    void registerObserver(EventCallback<Event>&& observer)
    {
        std::get<ObserverList<Event>>(this->observers)
            .push_back(std::move(observer));
    }
};

} // namespace probfd

#endif // PROBFD_UTILS_OBSERVER_H
