#include "probfd/heuristics/pdbs/probabilistic_pdb_heuristic.h"

#include "probfd/heuristics/pdbs/pattern_selection/pattern_collection_information.h"

#include "probfd/heuristics/pdbs/utils.h"
#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include "probfd/utils/logging.h"

#include "pdbs/dominance_pruning.h"

#include "utils/countdown_timer.h"
#include "utils/logging.h"

#include "option_parser.h"
#include "plugin.h"

#include <cassert>
#include <iomanip>
#include <numeric>
#include <string>

namespace probfd {
namespace heuristics {
namespace pdbs {

using namespace pattern_selection;

template <typename PDBType>
void ProbabilisticPDBHeuristic<PDBType>::Statistics::print_construction_info(
    std::ostream& out) const
{
    const double avg_variables = (double)variables / pdbs;
    const double avg_abstract_states = (double)abstract_states / pdbs;

    const double avg_subcollection_size =
        (double)total_subcollections_size / num_subcollections;

    constexpr auto prefix = std::is_same_v<PDBType, MaxProbProjection>
                                ? "MaxProb"
                                : "Expected Cost";

    out << "\n"
        << prefix << " Pattern Databases Statistics:\n"
        << "  Total number of PDBs: " << pdbs << "\n"
        << "  Total number of variables: " << variables << "\n"
        << "  Total number of abstract states: " << abstract_states << "\n"

        << "  Average number of variables per PDB: " << avg_variables << "\n"
        << "  Average number of abstract states per PDB: "
        << avg_abstract_states << "\n"

        << "  Largest pattern size: " << largest_pattern << "\n"

        << "  Total number of subcollections: " << num_subcollections << "\n"
        << "  Total number of subcollection PDBs: " << total_subcollections_size
        << "\n"
        << "  Average size of subcollection PDBs: " << avg_subcollection_size
        << "\n"

        << "  Generator time: " << generator_time << "s\n"
        << "  Dominance pruning time: " << dominance_pruning_time << "s\n"
        << "  Total construction time: " << construction_time << "s\n";
}

template <typename PDBType>
void ProbabilisticPDBHeuristic<PDBType>::Statistics::print(
    std::ostream& out) const
{
    print_construction_info(out);
}

template <typename PDBType>
ProbabilisticPDBHeuristic<PDBType>::ProbabilisticPDBHeuristic(
    const options::Options& opts)
    : ProbabilisticPDBHeuristic(
          opts.get<std::shared_ptr<ProbabilisticTask>>("transform"),
          opts.get<std::shared_ptr<PatternCollectionGenerator<PDBType>>>(
              "patterns"),
          opts.get<double>("max_time_dominance_pruning"))
{
}

template <typename PDBType>
ProbabilisticPDBHeuristic<PDBType>::ProbabilisticPDBHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<PatternCollectionGenerator<PDBType>> generator,
    double max_time_dominance_pruning)
    : TaskDependentHeuristic(task)
{
    utils::Timer construction_timer;

    utils::Timer generator_timer;
    auto pattern_collection_info = generator->generate(task);
    const double generator_time = generator_timer();

    this->patterns = pattern_collection_info.get_patterns();
    this->pdbs = pattern_collection_info.get_pdbs();
    this->subcollections = pattern_collection_info.get_subcollections();

    double dominance_pruning_time = 0.0;

    if (max_time_dominance_pruning > 0.0) {
        utils::Timer timer;

        ::pdbs::prune_dominated_cliques(
            *patterns,
            *pdbs,
            *subcollections,
            task_proxy.get_variables().size(),
            max_time_dominance_pruning,
            utils::g_log);

        dominance_pruning_time = timer();
    }

    // Gather statistics.
    const double construction_time = construction_timer();

    statistics_.generator_time = generator_time;
    statistics_.dominance_pruning_time = dominance_pruning_time;
    statistics_.construction_time = construction_time;

    statistics_.pdbs = pdbs->size();

    for (auto pdb : *pdbs) {
        size_t vars = pdb->get_pattern().size();
        statistics_.largest_pattern =
            std::max(statistics_.largest_pattern, vars);
        statistics_.variables += vars;
        statistics_.abstract_states += pdb->num_states();
    }

    statistics_.num_subcollections = subcollections->size();

    for (auto subcollection : *subcollections) {
        statistics_.total_subcollections_size += subcollection.size();
    }

    statistics_.print_construction_info(logging::out);

    this->generator_report = generator->get_report();
}

template <typename PDBType>
EvaluationResult
ProbabilisticPDBHeuristic<PDBType>::evaluate(const State& state) const
{
    return pdbs::evaluate<PDBType>(*pdbs, *subcollections, state);
}

template <typename PDBType>
void ProbabilisticPDBHeuristic<PDBType>::print_statistics() const
{
    if (generator_report) {
        generator_report->print(logging::out);
    }

    statistics_.print(logging::out);
}

template <>
void ProbabilisticPDBHeuristic<ExpCostProjection>::add_options_to_parser(
    options::OptionParser& parser)
{
    TaskDependentHeuristic::add_options_to_parser(parser);
    parser.add_option<
        std::shared_ptr<PatternCollectionGenerator<ExpCostProjection>>>(
        "patterns",
        "",
        "det_adapter_ec(generator=systematic(pattern_max_size=2))");
    parser.add_option<double>("max_time_dominance_pruning", "", "0.0");
}

template <>
void ProbabilisticPDBHeuristic<MaxProbProjection>::add_options_to_parser(
    options::OptionParser& parser)
{
    TaskDependentHeuristic::add_options_to_parser(parser);
    parser.add_option<
        std::shared_ptr<PatternCollectionGenerator<MaxProbProjection>>>(
        "patterns",
        "",
        "det_adapter_mp(generator=systematic(pattern_max_size=2))");
    parser.add_option<double>("max_time_dominance_pruning", "", "0.0");
}

template class ProbabilisticPDBHeuristic<ExpCostProjection>;
template class ProbabilisticPDBHeuristic<MaxProbProjection>;

static Plugin<TaskStateEvaluator> _plugin_ec(
    "ecpdb",
    options::parse<TaskStateEvaluator, ExpCostPDBHeuristic>);

static Plugin<TaskStateEvaluator> _plugin_mp(
    "maxprob_pdb",
    options::parse<TaskStateEvaluator, MaxProbPDBHeuristic>);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
