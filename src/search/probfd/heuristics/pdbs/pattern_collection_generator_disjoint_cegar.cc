#include "probfd/heuristics/pdbs/pattern_collection_generator_disjoint_cegar.h"

#include "probfd/heuristics/pdbs/cegar/cegar.h"

#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/cost_model.h"

#include "probfd/task_proxy.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "option_parser.h"
#include "plugin.h"

#include <stack>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

using namespace cegar;

PatternCollectionGeneratorDisjointCegar::
    PatternCollectionGeneratorDisjointCegar(const options::Options& opts)
    : PatternCollectionGenerator(opts)
    , wildcard(opts.get<bool>("wildcard"))
    , max_pdb_size(opts.get<int>("max_pdb_size"))
    , max_collection_size(opts.get<int>("max_collection_size"))
    , max_time(opts.get<double>("max_time"))
    , rng(utils::parse_rng_from_options(opts))
    , subcollection_finder_factory(
          opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
              "subcollection_finder_factory"))
    , flaw_strategy(
          opts.get<std::shared_ptr<FlawFindingStrategy>>("flaw_strategy"))
{
}

PatternCollectionInformation PatternCollectionGeneratorDisjointCegar::generate(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    // Store the set of goals in random order.
    ProbabilisticTaskProxy task_proxy(*task);
    TaskCostFunction* task_cost_function = g_cost_model->get_cost_function();
    vector<int> goals = get_goals_in_random_order(task_proxy, *rng);

    CEGAR cegar(
        log,
        rng,
        flaw_strategy,
        wildcard,
        max_pdb_size,
        max_collection_size,
        max_time,
        std::move(goals));

    std::shared_ptr pdbs =
        cegar.generate_pdbs(task_proxy, *task_cost_function).second;

    auto patterns = std::make_shared<PatternCollection>();

    for (const auto& pdb : *pdbs) {
        patterns->push_back(pdb->get_pattern());
    }

    std::shared_ptr<SubCollectionFinder> subcollection_finder =
        subcollection_finder_factory->create_subcollection_finder(task_proxy);

    PatternCollectionInformation pattern_collection_information(
        task_proxy,
        task_cost_function,
        patterns,
        subcollection_finder);
    pattern_collection_information.set_pdbs(pdbs);
    return pattern_collection_information;
}

void add_pattern_collection_generator_cegar_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<bool>(
        "wildcard",
        "whether to compute a wildcard policy",
        "false");
    parser.add_option<int>(
        "max_pdb_size",
        "maximum allowed number of states in a pdb (not applied to initial "
        "goal variable pattern(s))",
        "1000000",
        Bounds("1", "infinity"));
    parser.add_option<int>(
        "max_collection_size",
        "limit for the total number of PDB entries across all PDBs (not "
        "applied to initial goal variable pattern(s))",
        "infinity",
        Bounds("1", "infinity"));
    parser.add_option<double>(
        "max_time",
        "maximum time in seconds for CEGAR pattern generation. "
        "This includes the creation of the initial PDB collection"
        " as well as the creation of the correlation matrix.",
        "infinity",
        Bounds("0.0", "infinity"));
    parser.add_option<std::shared_ptr<SubCollectionFinderFactory>>(
        "subcollection_finder_factory",
        "The subcollection finder factory.",
        "finder_trivial_factory()");
    parser.add_option<std::shared_ptr<FlawFindingStrategy>>(
        "flaw_strategy",
        "strategy used to find flaws in a policy",
        "pucs_flaw_finder()");

    add_generator_options_to_parser(parser);
}

static shared_ptr<PatternCollectionGenerator>
_parse(options::OptionParser& parser)
{
    add_pattern_collection_generator_cegar_options_to_parser(parser);
    utils::add_rng_options(parser);

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<PatternCollectionGeneratorDisjointCegar>(opts);
}

static Plugin<PatternCollectionGenerator>
    _plugin("ppdbs_disjoint_cegar", _parse);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd