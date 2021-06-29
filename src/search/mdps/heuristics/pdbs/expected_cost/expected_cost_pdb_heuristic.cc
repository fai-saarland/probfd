#include "expected_cost_pdb_heuristic.h"

#include "additive_ecpdbs.h"

#include "pattern_selection/pattern_collection_information.h"

#include "../maxprob/orthogonality.h"
#include "../utils.h"

#include "../../../../globals.h"
#include "../../../../operator_cost.h"
#include "../../../../option_parser.h"
#include "../../../../pdbs/dominance_pruning.h"
#include "../../../../plugin.h"
#include "../../../../utils/countdown_timer.h"
#include "../../../globals.h"
#include "../../../logging.h"
#include "../../../analysis_objectives/expected_cost_objective.h"
#include "../../../../algorithms/max_cliques.h"

#include <cassert>
#include <string>
#include <iomanip>
#include <numeric>

namespace probabilistic {
namespace pdbs {

using namespace pattern_selection;

void ExpectedCostPDBHeuristic::Statistics::
print(std::ostream& out) const {
    out << "\nExpected-Cost Pattern Databases Statistics:\n"
        << "  Number of PDBs: " << num_patterns << "\n"
        << "  Total number of abstract states: " << total_states << "\n"
        << "  Dominance pruning time: " << dominance_pruning_time << "s\n"
        << "  Total construction time: " << construction_time << "s"
        << std::endl;
}

AdditiveExpectedCostPDBs
ExpectedCostPDBHeuristic::
get_additive_ecpdbs_from_options(
    std::shared_ptr<PatternCollectionGenerator> generator,
    double max_time_dominance_pruning)
{
    utils::Timer construction_timer;

    auto pattern_collection_info = generator->generate(NORMAL);
    std::shared_ptr patterns = pattern_collection_info.get_patterns();
    
    std::shared_ptr pdbs = pattern_collection_info.get_pdbs();
    std::shared_ptr cliques = pattern_collection_info.get_pattern_cliques();

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

        statistics_.dominance_pruning_time = timer();
    }

    statistics_.num_patterns = pdbs->size();
    statistics_.num_additive_subcollections = cliques->size();

    auto add = [](size_t a, auto b) { return a + b->num_states(); };
    statistics_.total_states =
        std::accumulate(pdbs->begin(), pdbs->end(), 0, add);
    statistics_.construction_time = construction_timer();

    generator_report = generator->get_report();

    return AdditiveExpectedCostPDBs(pdbs, cliques);
}

ExpectedCostPDBHeuristic::ExpectedCostPDBHeuristic(const options::Options& opts)
    : ExpectedCostPDBHeuristic(
        opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
        opts.get<double>("max_time_dominance_pruning"))
{
}

ExpectedCostPDBHeuristic::ExpectedCostPDBHeuristic(
    std::shared_ptr<PatternCollectionGenerator> generator,
    double max_time_dominance_pruning)
    : additive_ecpds(
        get_additive_ecpdbs_from_options(
            generator,
            max_time_dominance_pruning))
{
}

void
ExpectedCostPDBHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<PatternCollectionGenerator>>(
        "patterns", "", "det_adapter(generator=systematic(pattern_max_size=2))");
    parser.add_option<double>("max_time_dominance_pruning", "", "0.0");
}

EvaluationResult
ExpectedCostPDBHeuristic::evaluate(const GlobalState& state)
{
    return additive_ecpds.evaluate(state);
}

void ExpectedCostPDBHeuristic::print_statistics() const {
    statistics_.print(logging::out);

    if (generator_report) {
        generator_report->print(logging::out);
    }
}

static Plugin<GlobalStateEvaluator> _plugin(
    "ecpdb",
    options::parse<GlobalStateEvaluator, ExpectedCostPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
