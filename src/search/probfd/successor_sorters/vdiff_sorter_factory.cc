#include "probfd/successor_sorters/vdiff_sorter_factory.h"
#include "probfd/successor_sorters/vdiff_sorter.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace successor_sorters {

VDiffSorterFactory::VDiffSorterFactory(const options::Options& opts)
    : favor_large_gaps_(opts.get<bool>("prefer_large_gaps") ? -1 : 1)
{
}

void VDiffSorterFactory::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("prefer_large_gaps", "", "false");
}

std::shared_ptr<engine_interfaces::SuccessorSorter<OperatorID>>
VDiffSorterFactory::create_successor_sorter(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::make_unique<VDiffSorter>(favor_large_gaps_);
}

} // namespace successor_sorting
} // namespace probfd
