#include "probfd/heuristics/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "probfd/heuristics/pdbs/cegar/cegar.h"

#include "utils/logging.h"

#include "option_parser.h"
#include "plugin.h"

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

using namespace cegar;

PatternCollectionGeneratorMultipleCegar::
    PatternCollectionGeneratorMultipleCegar(options::Options& opts)
    : PatternCollectionGeneratorMultiple(opts, "CEGAR")
    , use_wildcard_plans(opts.get<bool>("use_wildcard_plans"))
    , subcollection_finder_factory(
          opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
              "subcollection_finder_factory"))
    , flaw_strategy(
          opts.get<std::shared_ptr<FlawFindingStrategy>>("flaw_strategy"))
{
}

PatternInformation PatternCollectionGeneratorMultipleCegar::compute_pattern(
    int max_pdb_size,
    double max_time,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const shared_ptr<ProbabilisticTask>& task,
    TaskCostFunction* task_cost_function,
    const FactPair& goal,
    unordered_set<int>&& blacklisted_variables)
{
    CEGAR cegar(
        log,
        rng,
        subcollection_finder_factory,
        flaw_strategy,
        use_wildcard_plans,
        max_pdb_size,
        max_pdb_size,
        max_time,
        {goal.var},
        std::move(blacklisted_variables));
    PatternCollectionInformation collection_info = cegar.generate(task);
    shared_ptr<PatternCollection> new_patterns = collection_info.get_patterns();
    if (new_patterns->size() > 1) {
        cerr << "CEGAR limited to one goal computed more than one pattern"
             << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    Pattern& pattern = new_patterns->front();
    shared_ptr<PPDBCollection> new_pdbs = collection_info.get_pdbs();
    auto pdb = new_pdbs->front();
    PatternInformation result(
        ProbabilisticTaskProxy(*task),
        task_cost_function,
        std::move(pattern));
    result.set_pdb(pdb);
    return result;
}

static shared_ptr<PatternCollectionGenerator>
_parse(options::OptionParser& parser)
{
    add_multiple_options_to_parser(parser);
    add_cegar_wildcard_option_to_parser(parser);

    parser.add_option<std::shared_ptr<SubCollectionFinderFactory>>(
        "subcollection_finder_factory",
        "The subcollection finder factory.",
        "finder_trivial_factory()");
    parser.add_option<std::shared_ptr<cegar::FlawFindingStrategy>>(
        "flaw_strategy",
        "strategy used to find flaws in a policy",
        "pucs_flaw_finder()");

    Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }

    return make_shared<PatternCollectionGeneratorMultipleCegar>(opts);
}

static Plugin<PatternCollectionGenerator>
    _plugin("ppdbs_multiple_cegar", _parse);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd