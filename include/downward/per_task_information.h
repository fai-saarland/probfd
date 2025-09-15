#ifndef PER_TASK_INFORMATION_H
#define PER_TASK_INFORMATION_H

#include "downward/algorithms/int_hash_set.h"
#include "downward/algorithms/subscriber.h"

#include "downward/abstract_task.h"
#include "downward/tuple_utils.h"

#include "downward/utils/hash.h"

#include <functional>
#include <memory>

namespace downward {
/*
  A PerTaskInformation<T> acts like a HashMap<TaskID, T>
  with two main differences:
  (1) If an entry is accessed that does not exist yet, it is created using a
      factory function that is passed to the PerTaskInformation in its
      constructor.
  (2) If a task is destroyed, its associated data in all PerTaskInformation
      objects is automatically destroyed as well.

*/

template <typename Derived, typename Component, std::size_t Idx>
class PerComponentInformationBase : public subscriber::Subscriber<Component> {
protected:
    void notify_service_destroyed(const Component* task) override
    {
        uintptr_t addr = reinterpret_cast<uintptr_t>(task);

        auto& entries = static_cast<Derived&>(*this).entries;

        for (auto it = entries.begin(); it != entries.end();) {
            if (std::get<Idx>(it->first) == addr) {
                it = entries.erase(it);
            } else {
                ++it;
            }
        }
    }
};

namespace detail {
template <typename T, typename... List>
struct index_of_helper {};

template <typename T, typename R, typename... List>
    requires(std::is_same_v<T, R>)
struct index_of_helper<T, R, List...>
    : std::integral_constant<std::size_t, 0> {};

template <typename T, typename R, typename... List>
    requires(
        !std::is_same_v<T, R> &&
        requires { index_of_helper<T, List...>::value; })
struct index_of_helper<T, R, List...>
    : std::integral_constant<
          std::size_t,
          1 + index_of_helper<T, List...>::value> {};
}

template <typename T, typename... List>
    requires requires { detail::index_of_helper<T, List...>::value; }
constexpr std::size_t index_of = detail::index_of_helper<T, List...>::value;

template <class Entry, typename... DependentComponents>
class PerComponentInformation
    : public PerComponentInformationBase<
          PerComponentInformation<Entry, DependentComponents...>,
          DependentComponents,
          index_of<DependentComponents, DependentComponents...>>... {
    template <typename D, typename E, std::size_t Idx>
    friend class PerComponentInformationBase;

    using KeyType = std::array<uintptr_t, sizeof...(DependentComponents)>;

    /*
      EntryConstructor is the type of a function that can create objects for
      a given task if the PerTaskInformation is accessed for a task that has no
      associated entry yet. It should return a unique_ptr to the newly created
      object.
    */
    using EntryConstructor =
        std::function<std::unique_ptr<Entry>(const DependentComponents&...)>;
    EntryConstructor entry_constructor;
    utils::HashMap<KeyType, std::unique_ptr<Entry>> entries;

public:
    /*
      If no entry_constructor is passed to the PerTaskInformation explicitly,
      we assume the class Entry has a constructor that takes a single
      PlanningTaskProxy parameter.
    */
    PerComponentInformation()
        : entry_constructor([](const DependentComponents&... components) {
            return std::make_unique<Entry>(components...);
        })
    {
    }

    explicit PerComponentInformation(EntryConstructor entry_constructor)
        : entry_constructor(entry_constructor)
    {
    }

    Entry&
    operator[](const std::tuple<const DependentComponents&...>& components)
    {
        static_assert(
            (std::derived_from<
                 DependentComponents,
                 subscriber::SubscriberService<DependentComponents>> &&
             ...),
            "All components must derive from SubscriberService<T>!");

        KeyType id = to_array(
            std::apply(
                [](auto&&... t) {
                    return std::make_tuple(reinterpret_cast<uintptr_t>(&t)...);
                },
                components));

        const auto& it = entries.find(id);
        if (it == entries.end()) {
            entries[id] = std::apply(entry_constructor, components);
            map_tuple(components, [&](const auto& component) {
                component.subscribe(this);
            });
        }
        return *entries[id];
    }

    Entry& operator[](const DependentComponents&... components)
    {
        static_assert(
            (std::derived_from<
                 DependentComponents,
                 subscriber::SubscriberService<DependentComponents>> &&
             ...),
            "All components must derive from SubscriberService<T>!");

        KeyType id{reinterpret_cast<uintptr_t>(&components)...};

        const auto& it = entries.find(id);
        if (it == entries.end()) {
            entries[id] = entry_constructor(components...);
            (components.subscribe(this), ...);
        }
        return *entries[id];
    }
};

template <typename T>
using PerTaskInformation = PerComponentInformation<
    T,
    VariableSpace,
    AxiomSpace,
    OperatorSpace,
    GoalFactList,
    InitialStateValues,
    OperatorCostFunction<int>>;

} // namespace downward

#endif // PER_TASK_INFORMATION_H
