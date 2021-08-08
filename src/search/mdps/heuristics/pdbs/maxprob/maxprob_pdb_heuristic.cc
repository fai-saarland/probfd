#include "maxprob_pdb_heuristic.h"

#include "pattern_selection/pattern_collection_information.h"

#include "../orthogonality.h"
#include "../utils.h"

#include "../../../../algorithms/max_cliques.h"
#include "../../../../globals.h"
#include "../../../../operator_cost.h"
#include "../../../../option_parser.h"
#include "../../../../pdbs/dominance_pruning.h"
#include "../../../../plugin.h"
#include "../../../../utils/countdown_timer.h"
#include "../../../../utils/logging.h"
#include "../../../analysis_objectives/expected_cost_objective.h"
#include "../../../globals.h"
#include "../../../logging.h"

#include <cassert>
#include <iomanip>
#include <numeric>
#include <string>

namespace probabilistic {
namespace pdbs {
namespace maxprob {

using namespace pattern_selection;

// Default Statictics
struct MaxProbPDBHeuristic::Statistics {
    Statistics(
        double generator_time,
        double dominance_pruning_time,
        double construction_time,
        const MaxProbPDBCollection& pdbs,
        const std::vector<PatternClique>& additive_subcollections)
        : generator_time(generator_time)
        , dominance_pruning_time(dominance_pruning_time)
        , construction_time(construction_time)
    {
        this->pdbs = pdbs.size();

        for (auto pdb : pdbs) {
            size_t vars = pdb->get_pattern().size();
            largest_pattern = std::max(largest_pattern, vars);
            variables += vars;
            abstract_states += pdb->num_states();
        }

        subcollections = additive_subcollections.size();

        for (auto subcollection : additive_subcollections) {
            total_subcollections_size += subcollection.size();
        }
    }

    double generator_time = 0.0;
    double dominance_pruning_time = 0.0;
    double construction_time = 0.0;

    size_t pdbs = 0;
    size_t variables = 0;
    size_t abstract_states = 0;
    size_t largest_pattern = 0;

    size_t subcollections = 0;
    size_t total_subcollections_size = 0;

    // Run-time statistics (expensive)
    // TODO

    void print_construction_info(std::ostream& out) const
    {
        const double avg_variables = (double)variables / pdbs;
        const double avg_abstract_states = (double)abstract_states / pdbs;

        const double avg_subcollection_size =
            (double)total_subcollections_size / subcollections;

        out << "\nMaxProb Pattern Databases Statistics:\n"
            << "  Total number of PDBs: " << pdbs << "\n"
            << "  Total number of variables: " << variables << "\n"
            << "  Total number of abstract states: " << abstract_states << "\n"

            << "  Average number of variables per PDB: " << avg_variables
            << "\n"
            << "  Average number of abstract states per PDB: "
            << avg_abstract_states << "\n"

            << "  Largest pattern size: " << largest_pattern << "\n"

            << "  Total number of multiplicative subcollections: "
            << subcollections << "\n"
            << "  Total number of subcollections PDBs: "
            << total_subcollections_size << "\n"
            << "  Average size of subcollection PDB: " << avg_subcollection_size
            << "\n"

            << "  Generator time: " << generator_time << "s\n"
            << "  Dominance pruning time: " << dominance_pruning_time << "s\n"
            << "  Total construction time: " << construction_time << "s\n";
    }

    void print(std::ostream& out) const { print_construction_info(out); }
};

MultiplicativeMaxProbPDBs
MaxProbPDBHeuristic::get_multiplicative_mppdbs_from_options(
    std::shared_ptr<PatternCollectionGenerator> generator,
    double max_time_dominance_pruning)
{
    utils::Timer construction_timer;

    utils::Timer generator_timer;
    auto pattern_collection_info = generator->generate(NORMAL);
    const double generator_time = generator_timer();

    std::shared_ptr patterns = pattern_collection_info.get_patterns();

    std::shared_ptr pdbs = pattern_collection_info.get_pdbs();
    std::shared_ptr cliques = pattern_collection_info.get_pattern_cliques();

    double dominance_pruning_time = 0.0;

    if (max_time_dominance_pruning > 0.0) {
        utils::Timer timer;

        int num_variables = g_variable_domain.size();
        /*
          NOTE: Dominance pruning could also be computed without having access
          to the PDBs, but since we want to delete patterns, we also want to
          update the list of corresponding PDBs so they are synchronized.

          In the long term, we plan to have patterns and their PDBs live
          together, in which case we would only need to pass their container
          and the pattern cliques.
        */
        ::pdbs::prune_dominated_cliques(
            *patterns,
            *pdbs,
            *cliques,
            num_variables,
            max_time_dominance_pruning);

        dominance_pruning_time = timer();
    }

    // Gather statistics.
    const double construction_time = construction_timer();

    this->statistics_.reset(new Statistics(
        generator_time,
        dominance_pruning_time,
        construction_time,
        *pdbs,
        *cliques));

    this->statistics_->print_construction_info(logging::out);

    this->generator_report = generator->get_report();

    return MultiplicativeMaxProbPDBs(pdbs, cliques);
}

MaxProbPDBHeuristic::MaxProbPDBHeuristic(const options::Options& opts)
    : MaxProbPDBHeuristic(
          opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
          opts.get<double>("max_time_dominance_pruning"))
{
}

MaxProbPDBHeuristic::MaxProbPDBHeuristic(
    std::shared_ptr<PatternCollectionGenerator> generator,
    double max_time_dominance_pruning)
    : multiplicative_mppdbs(get_multiplicative_mppdbs_from_options(
          generator,
          max_time_dominance_pruning))
{
}

MaxProbPDBHeuristic::~MaxProbPDBHeuristic() = default;

void MaxProbPDBHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<PatternCollectionGenerator>>(
        "patterns",
        "",
        "det_adapter(generator=systematic(pattern_max_size=2))");
    parser.add_option<double>("max_time_dominance_pruning", "", "0.0");
}

EvaluationResult MaxProbPDBHeuristic::evaluate(const GlobalState& state)
{
    return multiplicative_mppdbs.evaluate(state);
}

void MaxProbPDBHeuristic::print_statistics() const
{
    if (generator_report) {
        generator_report->print(logging::out);
    }

    statistics_->print(logging::out);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "maxprob_pdb",
    options::parse<GlobalStateEvaluator, MaxProbPDBHeuristic>);

} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic
