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

std::pair<
    std::shared_ptr<ProjectionStateSpace>,
    std::shared_ptr<ProbabilisticPatternDatabase>>
PatternCollectionGeneratorMultipleCegar::compute_pattern(
    int max_pdb_size,
    double max_time,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction& task_cost_function,
    const FactPair& goal,
    unordered_set<int>&& blacklisted_variables)
{
    CEGAR cegar(
        log,
        rng,
        flaw_strategy,
        use_wildcard_plans,
        max_pdb_size,
        max_pdb_size,
        max_time,
        {goal.var},
        std::move(blacklisted_variables));
    auto [state_spaces, pdbs] =
        cegar.generate_pdbs(task_proxy, task_cost_function);
    assert(state_spaces->size() == pdbs->size());
    if (pdbs->size() > 1) {
        cerr << "CEGAR limited to one goal computed more than one pattern"
             << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    return {std::move(state_spaces->front()), pdbs->front()};
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