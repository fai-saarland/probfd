#ifndef PER_TASK_INFORMATION_H
#define PER_TASK_INFORMATION_H

#include "downward/abstract_task.h"
#include "downward/tuple_utils.h"

#include "downward/algorithms/subscriber.h"

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
template <class Entry, typename... DependentComponents>
class PerComponentInformation {
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
        KeyType id = to_array(
            std::apply(
                [](auto&&... t) {
                    return std::make_tuple(reinterpret_cast<uintptr_t>(&t)...);
                },
                components));

        const auto& it = entries.find(id);
        if (it == entries.end()) {
            entries[id] = std::apply(entry_constructor, components);
            // TODO subscribe
        }
        return *entries[id];
    }

    Entry& operator[](const DependentComponents&... components)
    {
        KeyType id{reinterpret_cast<uintptr_t>(&components)...};

        const auto& it = entries.find(id);
        if (it == entries.end()) {
            entries[id] = entry_constructor(components...);
            // TODO subscribe
        }
        return *entries[id];
    }

    /*
    virtual void notify_service_destroyed(const PlanningTask* task) override
    {
        TaskID id = task->get_id();
        entries.erase(id);
    }
    */
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
