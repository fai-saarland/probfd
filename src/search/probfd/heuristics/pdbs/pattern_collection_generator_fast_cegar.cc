#include "probfd/heuristics/pdbs/pattern_collection_generator_fast_cegar.h"

#include "probfd/heuristics/pdbs/pattern_collection_generator_cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"
#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "option_parser.h"
#include "plugin.h"

#include "utils/collections.h"
#include "utils/countdown_timer.h"
#include "utils/hash.h"
#include "utils/logging.h"
#include "utils/rng.h"
#include "utils/rng_options.h"

#include <vector>

using namespace std;
using utils::Verbosity;

namespace probfd {
namespace heuristics {
namespace pdbs {

using namespace cegar;

template <typename PDBType>
PatternCollectionGeneratorFastCegar<
    PDBType>::PatternCollectionGeneratorFastCegar(const options::Options& opts)
    : subcollection_finder_factory(
          opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
              "subcollection_finder_factory"))
    , flaw_strategy(opts.get<std::shared_ptr<FlawFindingStrategy<PDBType>>>(
          "flaw_strategy"))
    , single_generator_max_refinements(opts.get<int>("max_refinements"))
    , single_generator_max_pdb_size(opts.get<int>("max_pdb_size"))
    , single_generator_max_collection_size(opts.get<int>("max_collection_size"))
    , single_generator_wildcard_policies(opts.get<bool>("wildcard"))
    , single_generator_treat_goal_violations_differently(
          opts.get<bool>("treat_goal_violations_differently"))
    , single_generator_max_time(opts.get<double>("max_time"))
    , single_generator_verbosity(opts.get<Verbosity>("verbosity"))
    , initial_random_seed(opts.get<int>("initial_random_seed"))
    , total_collection_max_size(opts.get<int>("total_collection_max_size"))
    , stagnation_limit(opts.get<double>("stagnation_limit"))
    , blacklist_trigger_time(opts.get<double>("blacklist_trigger_time"))
    , blacklist_on_stagnation(opts.get<bool>("blacklist_on_stagnation"))
    , total_time_limit(opts.get<double>("total_time_limit"))
{
}

template <typename PDBType>
PatternCollectionInformation<PDBType>
PatternCollectionGeneratorFastCegar<PDBType>::generate(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    const ProbabilisticTaskProxy task_proxy(*task);
    const VariablesProxy variables = task_proxy.get_variables();
    const GoalsProxy task_goals = task_proxy.get_goals();

    cout << "Fast CEGAR: generating patterns" << endl;

    utils::CountdownTimer timer(total_time_limit);
    shared_ptr<PatternCollection> union_patterns =
        make_shared<PatternCollection>();
    shared_ptr<PPDBCollection<PDBType>> union_pdbs =
        make_shared<PPDBCollection<PDBType>>();
    utils::HashSet<Pattern> pattern_set; // for checking if a pattern is
                                         // already in collection

    const size_t nvars = variables.size();
    utils::RandomNumberGenerator rng(initial_random_seed);

    vector<int> goals;
    for (const FactProxy fact : task_goals) {
        goals.push_back(fact.get_variable().get_id());
    }
    rng.shuffle(goals);

    bool can_generate = true;
    bool stagnation = false;
    // blacklisting can be forced after a period of stagnation
    // (see stagnation_limit)
    bool force_blacklisting = false;
    double stagnation_start = 0;
    int num_iterations = 0;
    int goal_index = 0;
    const bool single_generator_ignore_goal_violations = true;
    const InitialCollectionType single_generator_initial =
        InitialCollectionType::GIVEN_GOAL;
    int collection_size = 0;
    while (can_generate) {
        // we start blacklisting once a certain amount of time has passed
        // or if blacklisting was forced due to stagnation
        int blacklist_size = 0;
        if (force_blacklisting || timer.get_elapsed_time() / total_time_limit >
                                      blacklist_trigger_time) {
            blacklist_size = static_cast<int>(nvars * rng.random());
            force_blacklisting = true;
        }

        int remaining_collection_size =
            total_collection_max_size - collection_size;
        double remaining_time = total_time_limit - timer.get_elapsed_time();

        PatternCollectionGeneratorCegar<PDBType> generator(
            make_shared<utils::RandomNumberGenerator>(
                initial_random_seed + num_iterations),
            subcollection_finder_factory,
            flaw_strategy,
            single_generator_wildcard_policies,
            single_generator_max_refinements,
            single_generator_max_pdb_size,
            min(remaining_collection_size,
                single_generator_max_collection_size),
            single_generator_ignore_goal_violations,
            single_generator_treat_goal_violations_differently,
            blacklist_size,
            single_generator_initial,
            goals[goal_index],
            single_generator_verbosity,
            min(remaining_time, single_generator_max_time));

        auto collection_info = generator.generate(task);
        auto pattern_collection = collection_info.get_patterns();
        auto pdb_collection = collection_info.get_pdbs();

        if (pdb_collection->size() > 1) {
            cerr << "A generator computed more than one pattern" << endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }

        // TODO: this is not very efficient since each pattern is stored twice.
        // Needs some optimization
        const Pattern& pattern = pattern_collection->front();
        if (pattern_set.insert(pattern).second) {
            // new pattern detected, so no stagnation
            stagnation = false;

            // decrease size limit
            shared_ptr<PDBType>& pdb = pdb_collection->front();
            collection_size += pdb->num_states();
            if (total_collection_max_size - collection_size <= 0) {
                // This happens because a single CEGAR run can violate the
                // imposed size limit if already the given goal variable is
                // too large.
                cout << "Fast CEGAR: Total collection size limit reached."
                     << endl;
                can_generate = false;
            }

            union_patterns->push_back(std::move(pattern));
            union_pdbs->push_back(std::move(pdb));
        } else {
            // no new pattern could be generated during this iteration
            // --> stagnation
            if (!stagnation) {
                stagnation = true;
                stagnation_start = timer.get_elapsed_time();
            }
        }
        // cout << "current collection size: " << collection_size << endl;

        // if stagnation has been going on for too long, then the
        // further behavior depends on the value of blacklist_on_stagnation
        const auto stag_time = timer.get_elapsed_time() - stagnation_start;
        if (stagnation && stag_time > stagnation_limit) {
            if (!blacklist_on_stagnation || force_blacklisting) {
                if (!blacklist_on_stagnation) {
                    // stagnation has been going on for too long and we are not
                    // allowed to force blacklisting, so nothing can be done.
                    cout << "Fast CEGAR: Stagnation limit reached. "
                         << "Stopping generation." << endl;
                } else {
                    // stagnation in spite of blacklisting
                    cout << "Fast CEGAR: Stagnation limit reached again. "
                         << "Stopping generation." << endl;
                }
                can_generate = false;
            } else {
                // We want to blacklist on stagnation but have not started
                // doing so yet.
                cout << "Fast CEGAR: Stagnation limit reached. "
                     << "Forcing global blacklisting." << endl;
                force_blacklisting = true;
                stagnation = false;
            }
        }

        if (timer.is_expired()) {
            cout << "Fast CEGAR: time limit reached" << endl;
            can_generate = false;
        }

        ++num_iterations;
        ++goal_index;
        goal_index = goal_index % goals.size();
        assert(utils::in_bounds(goal_index, goals));
    }

    cout << "Fast CEGAR: computation time: " << timer.get_elapsed_time()
         << endl;
    cout << "Fast CEGAR: number of iterations: " << num_iterations << endl;
    cout << "Fast CEGAR: average time per generator: "
         << timer.get_elapsed_time() / static_cast<double>(num_iterations + 1)
         << endl;
    cout << "Fast CEGAR: final collection: " << *union_patterns << endl;
    cout << "Fast CEGAR: final collection number of patterns: "
         << union_patterns->size() << endl;
    cout << "Fast CEGAR: final collection summed PDB size: " << collection_size
         << endl;

    std::shared_ptr<SubCollectionFinder> subcollection_finder =
        subcollection_finder_factory->create_subcollection_finder(task_proxy);

    PatternCollectionInformation<PDBType> result(
        task_proxy,
        union_patterns,
        subcollection_finder);
    result.set_pdbs(union_pdbs);
    return result;
}

template <typename PDBType>
static shared_ptr<PatternCollectionGenerator<PDBType>>
_parse(options::OptionParser& parser)
{
    parser.add_option<int>(
        "initial_random_seed",
        "seed for the random number generator(s) of the cegar generators",
        "10");
    parser.add_option<int>(
        "total_collection_max_size",
        "max. number of entries in the final collection",
        "infinity");
    parser.add_option<double>(
        "total_time_limit",
        "time budget for PDB collection generation",
        "25.0");
    parser.add_option<double>(
        "stagnation_limit",
        "max. time the algorithm waits for the introduction of a new pattern. "
        "Execution finishes prematurely if no new, unique pattern "
        "could be added to the collection during this time.",
        "5.0");
    parser.add_option<double>(
        "blacklist_trigger_time",
        "time given as percentage of overall time_limit, "
        "after which blacklisting (for diversification) is enabled. "
        "E.g. blacklist_trigger_time=0.5 will trigger blacklisting "
        "once half of the total time has passed.",
        "1.0",
        Bounds("0.0", "1.0"));
    parser.add_option<bool>(
        "blacklist_on_stagnation",
        "whether the algorithm forces blacklisting to start early if "
        "stagnation_limit is crossed (instead of aborting). "
        "The algorithm still aborts if stagnation_limit is "
        "reached for the second time.",
        "true");

    add_pattern_collection_generator_cegar_options_to_parser<PDBType>(parser);

    Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }

    return make_shared<PatternCollectionGeneratorFastCegar<PDBType>>(opts);
}

static Plugin<PatternCollectionGenerator<MaxProbPatternDatabase>>
    _plugin_mp("fast_cegar_mp", _parse<MaxProbPatternDatabase>);
static Plugin<PatternCollectionGenerator<SSPPatternDatabase>>
    _plugin_ec("fast_cegar_ec", _parse<SSPPatternDatabase>);

template class PatternCollectionGeneratorFastCegar<MaxProbPatternDatabase>;
template class PatternCollectionGeneratorFastCegar<SSPPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd