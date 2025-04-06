#ifndef PER_TASK_INFORMATION_H
#define PER_TASK_INFORMATION_H

#include "downward/task_proxy.h"

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
template <class Entry>
class PerTaskInformation : public subscriber::Subscriber<PlanningTask> {
    /*
      EntryConstructor is the type of a function that can create objects for
      a given task if the PerTaskInformation is accessed for a task that has no
      associated entry yet. It receives a TaskProxy instead of the AbstractTask
      because AbstractTask is an internal implementation detail as far as other
      classes are concerned. It should return a unique_ptr to the newly created
      object.
    */
    using EntryConstructor =
        std::function<std::unique_ptr<Entry>(const PlanningTaskProxy&)>;
    EntryConstructor entry_constructor;
    utils::HashMap<TaskID, std::unique_ptr<Entry>> entries;

public:
    /*
      If no entry_constructor is passed to the PerTaskInformation explicitly,
      we assume the class Entry has a constructor that takes a single
      PlanningTaskProxy parameter.
    */
    PerTaskInformation()
        : entry_constructor([](const PlanningTaskProxy& task_proxy) {
            return std::make_unique<Entry>(task_proxy);
        })
    {
    }

    explicit PerTaskInformation(EntryConstructor entry_constructor)
        : entry_constructor(entry_constructor)
    {
    }

    Entry& operator[](const PlanningTaskProxy& task_proxy)
    {
        TaskID id = task_proxy.get_id();
        const auto& it = entries.find(id);
        if (it == entries.end()) {
            entries[id] = entry_constructor(task_proxy);
            task_proxy.subscribe_to_task_destruction(this);
        }
        return *entries[id];
    }

    virtual void notify_service_destroyed(const PlanningTask* task) override
    {
        TaskID id = PlanningTaskProxy(*task).get_id();
        entries.erase(id);
    }
};
}

#endif
