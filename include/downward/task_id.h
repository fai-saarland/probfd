#ifndef TASK_ID_H
#define TASK_ID_H

#include "downward/abstract_task.h"

#include "downward/utils/hash.h"

#include <array>
#include <cstdint>
#include <functional>

namespace downward {

class TaskID;

namespace utils {
void feed(HashState& hash_state, TaskID id);
}

/*
  A TaskID uniquely identifies a task (for unordered_maps and comparison)
  without publicly exposing the internal AbstractTask pointer.
*/
class TaskID {
    const std::array<std::uintptr_t, std::tuple_size_v<PlanningTaskTuple>>
        value;

public:
    explicit TaskID(const PlanningTaskTuple& task)
        : value(to_array(
              std::apply(
                  [](auto&&... t) {
                      return std::make_tuple(
                          reinterpret_cast<uintptr_t>(&t)...);
                  },
                  task)))
    {
    }

    friend bool operator==(const TaskID& left, const TaskID& right)
    {
        return left.value == right.value;
    }

    friend void utils::feed(utils::HashState& hash_state, TaskID id);
};
} // namespace downward

inline void downward::utils::feed(HashState& hash_state, TaskID id)
{
    feed(hash_state, id.value);
}

#endif
