#ifndef PROBFD_TASK_UTILS_TUPLE_ENUMERATOR_FWD_H
#define PROBFD_TASK_UTILS_TUPLE_ENUMERATOR_FWD_H

#include <cstddef>

namespace probfd::enumeration {

template <std::size_t N>
    requires(N > 0)
class TupleEnumerator;

using PairEnumerator = TupleEnumerator<2>;

} // namespace probfd::enumeration

#endif // PROBFD_TASK_UTILS_TUPLE_ENUMERATOR_FWD_H
