#include "expected_cost_pdb_heuristic.h"

#include "additive_ecpdbs.h"

#include "pattern_selection/pattern_collection_information.h"
#include "pattern_selection/pattern_generator.h"

#include "../maxprob/orthogonality.h"
#include "../utils.h"

#include "../../../../globals.h"
#include "../../../../operator_cost.h"
#include "../../../../option_parser.h"
#include "../../../../pdbs/dominance_pruning.h"
#include "../../../../plugin.h"
#include "../../../../utils/countdown_timer.h"
#include "../../../analysis_objective.h"
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

static AdditiveExpectedCostPDBs
get_additive_ecpdbs_from_options(const Options &opts) {
    std::shared_ptr pattern_generator =
        opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns");

    PatternCollectionInformation pattern_collection_info =
        pattern_generator->generate(NORMAL);

    std::shared_ptr patterns = pattern_collection_info.get_patterns();
    /*
      We compute PDBs and pattern cliques here (if they have not been
      computed before) so that their computation is not taken into account
      for dominance pruning time.
    */
    std::shared_ptr pdbs = pattern_collection_info.get_pdbs();
    std::shared_ptr pattern_cliques =
        pattern_collection_info.get_pattern_cliques();

    double max_time_dominance_pruning =
        opts.get<double>("max_time_dominance_pruning");

    if (max_time_dominance_pruning > 0.0) {
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
            *pattern_cliques,
            num_variables,
            max_time_dominance_pruning);
    }

    return AdditiveExpectedCostPDBs(pdbs, pattern_cliques);
}

ExpectedCostPDBHeuristic::ExpectedCostPDBHeuristic(const options::Options& opts)
    : additive_ecpds(get_additive_ecpdbs_from_options(opts))
{
}

void
ExpectedCostPDBHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<PatternCollectionGenerator>>(
        "patterns", "", "systematic_ec(pattern_max_size=2)");
    parser.add_option<double>("max_time_dominance_pruning", "", "0.0");
    //parser.add_option<double>("time_limit", "", "0");
    //parser.add_option<int>("max_states", "", "-1");
    //parser.add_option<bool>("dump_projections", "", "false");
    //parser.add_option<bool>("additive", "", "false");
}

EvaluationResult
ExpectedCostPDBHeuristic::evaluate(const GlobalState& state)
{
    return additive_ecpds.evaluate(state);
}

void ExpectedCostPDBHeuristic::print_statistics() const {
#if 0
    auto& out = logging::out;

    // Set fp output precision to 5 digits
    out << std::setprecision(4);

    out << "\nExpected-Cost Pattern Databases Statistics:" << std::endl;

    dump_init_statistics(out);

    out << "  Estimate calls: " << statistics_.num_estimates << std::endl;
    out << "  Average estimate: " << statistics_.average_estimate << std::endl;
    out << "  Lowest estimate: " << statistics_.lowest_estimate << std::endl;
    out << "  Highest estimate: " << statistics_.highest_estimate << std::endl;

    out << "  Pattern construction time: "
        << statistics_.pattern_construction_time << "s" << std::endl;
    out << "  Database construction time: "
        << statistics_.database_construction_time << "s" << std::endl;
    out << "  Clique construction time: "
        << statistics_.clique_computation_time << "s" << std::endl;
    out << "  Dominance pruning time: "
        << statistics_.dominance_pruning_time << "s" << std::endl;
    out << "  Total construction time: "
        << statistics_.construction_time << "s" << std::endl;

#ifdef ECPDB_MEASURE_EVALUATE
    out << "  Estimate time: " << statistics_.evaluate_time << "s" << std::endl;
#endif

    // Undo for later statistics
    out << std::setprecision(std::numeric_limits<double>::digits10 + 1);
#endif
}

static Plugin<GlobalStateEvaluator> _plugin(
    "ecpdb",
    options::parse<GlobalStateEvaluator, ExpectedCostPDBHeuristic>);

} // namespace pdbs
} // namespace probabilistic
