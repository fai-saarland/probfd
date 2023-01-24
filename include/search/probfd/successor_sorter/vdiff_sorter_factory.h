#ifndef MDPS_SUCCESSOR_SORTING_VDIFF_SORTER_FACTORY_H
#define MDPS_SUCCESSOR_SORTING_VDIFF_SORTER_FACTORY_H

#include "probfd/successor_sorter/task_successor_sorter_factory.h"

#include "probfd/value_type.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

/// Namespace dedicated to policy tiebreaker implementations.
namespace successor_sorting {

class VDiffSorterFactory : public TaskSuccessorSorterFactory {
    const value_t favor_large_gaps_;

public:
    explicit VDiffSorterFactory(const options::Options&);
    ~VDiffSorterFactory() override = default;

    static void add_options_to_parser(options::OptionParser&);

    std::shared_ptr<engine_interfaces::SuccessorSorter<OperatorID>>
    create_successor_sorter(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) override;
};

} // namespace successor_sorting
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__