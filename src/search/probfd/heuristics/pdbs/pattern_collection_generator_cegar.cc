#include "probfd/heuristics/pdbs/pattern_collection_generator_cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"
#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

#include "probfd/tasks/root_task.h"

#include "probfd/cost_model.h"
#include "probfd/task_proxy.h"

#include "utils/collections.h"
#include "utils/countdown_timer.h"
#include "utils/logging.h"
#include "utils/math.h"
#include "utils/rng.h"
#include "utils/rng_options.h"
#include "utils/system.h"

#include "algorithms/priority_queues.h"

#include "option_parser.h"
#include "plugin.h"

#include "task_utils/successor_generator.h"

#include <stack>

using namespace std;
using utils::Verbosity;

namespace probfd {
namespace heuristics {
namespace pdbs {

using namespace cegar;

namespace {
static const std::string token = "CEGAR_PDBs: ";
} // namespace

template <typename PDBType>
PDBInfo<PDBType>::PDBInfo(
    const ProbabilisticTaskProxy& task_proxy,
    StateRankingFunction ranking_function,
    TaskCostFunction& task_cost_function,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard)
    : state_space(task_proxy, ranking_function, task_cost_function, !wildcard)
    , cost_function(task_proxy, ranking_function, &task_cost_function)
    , initial_state(ranking_function.rank(task_proxy.get_initial_state()))
    , pdb(new PDBType(
          state_space,
          std::move(ranking_function),
          cost_function,
          initial_state))
    , policy(pdb->compute_optimal_abstract_policy(
          state_space,
          cost_function,
          initial_state,
          rng,
          wildcard))
{
}

template <typename PDBType>
PDBInfo<PDBType>::PDBInfo(
    const ProbabilisticTaskProxy& task_proxy,
    StateRankingFunction ranking_function,
    TaskCostFunction& task_cost_function,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const PDBType& previous,
    int add_var,
    bool wildcard)
    : state_space(task_proxy, ranking_function, task_cost_function, !wildcard)
    , cost_function(task_proxy, ranking_function, &task_cost_function)
    , initial_state(ranking_function.rank(task_proxy.get_initial_state()))
    , pdb(new PDBType(
          state_space,
          std::move(ranking_function),
          cost_function,
          initial_state,
          previous,
          add_var))
    , policy(pdb->compute_optimal_abstract_policy(
          state_space,
          cost_function,
          initial_state,
          rng,
          wildcard))
{
}

template <typename PDBType>
PDBInfo<PDBType>::PDBInfo(
    const ProbabilisticTaskProxy& task_proxy,
    StateRankingFunction ranking_function,
    TaskCostFunction& task_cost_function,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const PDBType& left,
    const PDBType& right,
    bool wildcard)
    : state_space(task_proxy, ranking_function, task_cost_function, !wildcard)
    , cost_function(task_proxy, ranking_function, &task_cost_function)
    , initial_state(ranking_function.rank(task_proxy.get_initial_state()))
    , pdb(new PDBType(
          state_space,
          std::move(ranking_function),
          cost_function,
          initial_state,
          left,
          right))
    , policy(pdb->compute_optimal_abstract_policy(
          state_space,
          cost_function,
          initial_state,
          rng,
          wildcard))
{
}

template <typename PDBType>
const Pattern& PDBInfo<PDBType>::get_pattern() const
{
    return pdb->get_pattern();
}

template <typename PDBType>
const PDBType& PDBInfo<PDBType>::get_pdb() const
{
    assert(pdb);
    return *pdb;
}

template <typename PDBType>
std::unique_ptr<PDBType> PDBInfo<PDBType>::steal_pdb()
{
    return std::move(pdb);
}

template <typename PDBType>
const AbstractPolicy& PDBInfo<PDBType>::get_policy() const
{
    return *policy;
}

template <typename PDBType>
value_t PDBInfo<PDBType>::get_policy_cost(const State& state) const
{
    return pdb->lookup(state);
}

template <typename PDBType>
bool PDBInfo<PDBType>::is_solved() const
{
    return solved;
}

template <typename PDBType>
void PDBInfo<PDBType>::mark_as_solved()
{
    solved = true;
}

template <typename PDBType>
bool PDBInfo<PDBType>::solution_exists() const
{
    return !pdb->is_dead_end(initial_state);
}

template <typename PDBType>
bool PDBInfo<PDBType>::is_goal(StateRank rank) const
{
    return cost_function.is_goal(rank);
}

// Instantiations
template class PDBInfo<MaxProbPatternDatabase>;
template class PDBInfo<SSPPatternDatabase>;

template <typename PDBType>
PatternCollectionGeneratorCegar<PDBType>::PatternCollectionGeneratorCegar(
    const shared_ptr<utils::RandomNumberGenerator>& arg_rng,
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory,
    std::shared_ptr<FlawFindingStrategy<PDBType>> flaw_strategy,
    bool wildcard,
    int arg_max_pdb_size,
    int arg_max_collection_size,
    int blacklist_size,
    InitialCollectionType arg_initial,
    int given_goal,
    Verbosity verbosity,
    double arg_max_time)
    : rng(arg_rng)
    , subcollection_finder_factory(subcollection_finder_factory)
    , flaw_strategy(flaw_strategy)
    , wildcard(wildcard)
    , max_pdb_size(arg_max_pdb_size)
    , max_collection_size(arg_max_collection_size)
    , blacklist_size(blacklist_size)
    , initial(arg_initial)
    , given_goal(given_goal)
    , verbosity(verbosity)
    , max_time(arg_max_time)
    , collection_size(0)
{
    if (initial == InitialCollectionType::GIVEN_GOAL && given_goal == -1) {
        cerr << "Initial collection type 'given goal', but no goal specified"
             << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    if (initial != InitialCollectionType::GIVEN_GOAL && given_goal != -1) {
        cerr << "Goal given, but initial collection type is not set to use it"
             << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    if (verbosity >= Verbosity::NORMAL) {
        cout << token << "options: " << endl;

        cout << token << "flaw strategy: " << flaw_strategy->get_name() << endl;
        cout << token << "max pdb size: " << max_pdb_size << endl;
        cout << token << "max collection size: " << max_collection_size << endl;
        cout << token << "global blacklist size: " << blacklist_size << endl;
        cout << token << "initial collection type: ";

        switch (initial) {
        case InitialCollectionType::GIVEN_GOAL:
            cout << "given goal" << endl;
            break;
        case InitialCollectionType::RANDOM_GOAL:
            cout << "random goal" << endl;
            break;
        case InitialCollectionType::ALL_GOALS:
            cout << "all goals" << endl;
            break;
        }

        cout << token << "given goal: " << given_goal << endl;
        cout << token << "Verbosity: ";

        switch (verbosity) {
        default:
        case Verbosity::NORMAL: cout << "normal"; break;
        case Verbosity::VERBOSE: cout << "verbose"; break;
        case Verbosity::DEBUG: cout << "debug"; break;
        }

        cout << endl;
        cout << token << "max time: " << max_time << endl;
    }

    if (verbosity >= Verbosity::NORMAL) {
        cout << endl;
    }
}

template <typename PDBType>
PatternCollectionGeneratorCegar<PDBType>::PatternCollectionGeneratorCegar(
    const options::Options& opts)
    : PatternCollectionGeneratorCegar(
          utils::parse_rng_from_options(opts),
          opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
              "subcollection_finder_factory"),
          opts.get<std::shared_ptr<FlawFindingStrategy<PDBType>>>(
              "flaw_strategy"),
          opts.get<bool>("wildcard"),
          opts.get<int>("max_pdb_size"),
          opts.get<int>("max_collection_size"),
          opts.get<int>("blacklist_size"),
          opts.get<InitialCollectionType>("initial"),
          opts.get<int>("given_goal"),
          opts.get<Verbosity>("verbosity"),
          opts.get<double>("max_time"))
{
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::print_collection() const
{
    cout << "[";

    for (size_t i = 0; i < pdb_infos.size(); ++i) {
        const auto& info = pdb_infos[i];
        if (info) {
            cout << info->get_pattern();
            if (i != pdb_infos.size() - 1) {
                cout << ", ";
            }
        }
    }

    cout << "]" << endl;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::
    generate_trivial_solution_collection(
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function)
{
    assert(!remaining_goals.empty());

    switch (initial) {
    case InitialCollectionType::GIVEN_GOAL: {
        assert(given_goal != -1);
        add_pattern_for_var(task_proxy, task_cost_function, given_goal);
        break;
    }
    case InitialCollectionType::RANDOM_GOAL: {
        int var = remaining_goals.back();
        remaining_goals.pop_back();
        add_pattern_for_var(task_proxy, task_cost_function, var);
        break;
    }
    case InitialCollectionType::ALL_GOALS: {
        while (!remaining_goals.empty()) {
            int var = remaining_goals.back();
            remaining_goals.pop_back();
            add_pattern_for_var(task_proxy, task_cost_function, var);
        }

        break;
    }
    }

    if (verbosity >= Verbosity::NORMAL) {
        cout << token << "initial collection: ";
        print_collection();

        if (verbosity >= Verbosity::VERBOSE) {
            cout << endl;
        }
    }
}

template <typename PDBType>
bool PatternCollectionGeneratorCegar<PDBType>::time_limit_reached(
    const utils::CountdownTimer& timer) const
{
    if (timer.is_expired()) {
        if (verbosity >= Verbosity::NORMAL) {
            cout << token << "time limit reached" << endl;
        }

        return true;
    }

    return false;
}

template <typename PDBType>
int PatternCollectionGeneratorCegar<PDBType>::get_flaws(
    const ProbabilisticTaskProxy& task_proxy,
    std::vector<Flaw>& flaws)
{
    const int num_pdb_infos = static_cast<int>(pdb_infos.size());
    for (int idx = 0; idx < num_pdb_infos; ++idx) {
        auto& info = pdb_infos[idx];

        if (!info || info->is_solved()) {
            continue;
        }

        // abort here if no abstract solution could be found
        if (!info->solution_exists()) {
            cout << token << "Problem unsolvable" << endl;
            utils::exit_with(utils::ExitCode::SEARCH_UNSOLVABLE);
        }

        // find out if and why the abstract solution
        // would not work for the concrete task.
        // We always start with the initial state.
        const size_t num_flaws_before = flaws.size();
        const bool executable =
            flaw_strategy->apply_policy(*this, task_proxy, idx, flaws);

        // Check for new flaws
        if (flaws.size() == num_flaws_before) {
            // Check if policy is executable modulo blacklisting.
            // Even if there are no flaws, there might be goal violations
            // that did not make it into the flaw list.
            if (executable && blacklisted_variables.empty()) {
                /*
                 * If there are no flaws, this does not guarantee that the
                 * plan is valid in the concrete state space because we might
                 * have ignored variables that have been blacklisted. Hence the
                 * tests for empty blacklists.
                 */
                flaws.clear();
                return idx;
            }

            info->mark_as_solved();
        }
    }

    return -1;
}

template <typename PDBType>
bool PatternCollectionGeneratorCegar<PDBType>::can_add_singleton_pattern(
    const VariablesProxy& variables,
    int var) const
{
    const int pdb_size = variables[var].get_domain_size();
    return pdb_size <= max_pdb_size &&
           collection_size <= max_collection_size - pdb_size;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::add_pattern_for_var(
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction& task_cost_function,
    int var)
{
    auto& info = pdb_infos.emplace_back(new PDBInfo<PDBType>(
        task_proxy,
        StateRankingFunction(task_proxy, {var}),
        task_cost_function,
        rng,
        wildcard));
    variable_to_collection_index[var] = pdb_infos.size() - 1;
    collection_size += info->get_pdb().num_states();
}

template <typename PDBType>
bool PatternCollectionGeneratorCegar<PDBType>::can_merge_patterns(
    int index1,
    int index2) const
{
    int pdb_size1 = pdb_infos[index1]->get_pdb().num_states();
    int pdb_size2 = pdb_infos[index2]->get_pdb().num_states();

    if (!utils::is_product_within_limit(pdb_size1, pdb_size2, max_pdb_size)) {
        return false;
    }

    int added_size = pdb_size1 * pdb_size2 - pdb_size1 - pdb_size2;
    return collection_size + added_size <= max_collection_size;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::merge_patterns(
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction& task_cost_function,
    int index1,
    int index2)
{
    // Merge pattern at index2 into pattern at index2
    PDBInfo<PDBType>& solution1 = *pdb_infos[index1];
    PDBInfo<PDBType>& solution2 = *pdb_infos[index2];

    const PDBType& pdb1 = solution1.get_pdb();
    const PDBType& pdb2 = solution2.get_pdb();

    // update look-up table
    for (int var : solution2.get_pattern()) {
        variable_to_collection_index[var] = index1;
    }

    // store old pdb sizes
    int pdb_size1 = pdb1.num_states();
    int pdb_size2 = pdb2.num_states();

    // compute merge solution
    unique_ptr<PDBInfo<PDBType>> merged(new PDBInfo<PDBType>(
        task_proxy,
        StateRankingFunction(
            task_proxy,
            utils::merge_sorted(pdb1.get_pattern(), pdb2.get_pattern())),
        task_cost_function,
        rng,
        pdb1,
        pdb2,
        wildcard));

    // update collection size
    collection_size -= pdb_size1;
    collection_size -= pdb_size2;
    collection_size += merged->get_pdb().num_states();

    // clean-up
    pdb_infos[index1] = std::move(merged);
    pdb_infos[index2] = nullptr;
}

template <typename PDBType>
bool PatternCollectionGeneratorCegar<PDBType>::can_add_variable_to_pattern(
    const VariablesProxy& variables,
    int index,
    int var) const
{
    int pdb_size = pdb_infos[index]->get_pdb().num_states();
    int domain_size = variables[var].get_domain_size();

    if (!utils::is_product_within_limit(pdb_size, domain_size, max_pdb_size)) {
        return false;
    }

    int added_size = pdb_size * domain_size - pdb_size;
    return collection_size + added_size <= max_collection_size;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::add_variable_to_pattern(
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction& task_cost_function,
    int index,
    int var)
{
    PDBInfo<PDBType>& info = *pdb_infos[index];

    auto pdb = info.get_pdb();

    // compute new solution
    std::unique_ptr<PDBInfo<PDBType>> new_info(new PDBInfo<PDBType>(
        task_proxy,
        StateRankingFunction(task_proxy, utils::insert(pdb.get_pattern(), var)),
        task_cost_function,
        rng,
        pdb,
        var,
        wildcard));

    // update collection size
    collection_size -= pdb.num_states();
    collection_size += new_info->get_pdb().num_states();

    // update look-up table
    variable_to_collection_index[var] = index;
    pdb_infos[index] = std::move(new_info);
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::refine(
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction& task_cost_function,
    const VariablesProxy& variables,
    const std::vector<Flaw>& flaws)
{
    assert(!flaws.empty());

    // pick a random flaw
    int random_flaw_index = rng->random(flaws.size());
    const Flaw& flaw = flaws[random_flaw_index];

    if (verbosity >= Verbosity::VERBOSE) {
        cout << token << "chosen flaw: pattern "
             << pdb_infos[flaw.solution_index]->get_pattern();
    }

    if (verbosity >= Verbosity::VERBOSE) {
        cout << " with a violated precondition on " << flaw.variable << endl;
    }

    int sol_index = flaw.solution_index;
    int var = flaw.variable;

    const auto it = variable_to_collection_index.find(var);

    if (it != variable_to_collection_index.end()) {
        // var is already in another pattern of the collection
        int other_index = it->second;
        assert(other_index != sol_index);
        assert(pdb_infos[other_index] != nullptr);

        if (verbosity >= Verbosity::VERBOSE) {
            cout << token << "var" << var << " is already in pattern "
                 << pdb_infos[other_index]->get_pattern() << endl;
        }

        if (can_merge_patterns(sol_index, other_index)) {
            if (verbosity >= Verbosity::VERBOSE) {
                cout << token << "merge the two patterns" << endl;
            }

            merge_patterns(
                task_proxy,
                task_cost_function,
                sol_index,
                other_index);
            return;
        }
    } else {
        // var is not yet in the collection
        // Note on precondition violations: var may be a goal variable but
        // nevertheless is added to the pattern causing the flaw and not to
        // a single new pattern.
        if (verbosity >= Verbosity::VERBOSE) {
            cout << token << "var" << var << " is not in the collection yet"
                 << endl;
        }

        if (can_add_variable_to_pattern(variables, sol_index, var)) {
            if (verbosity >= Verbosity::VERBOSE) {
                cout << token << "add it to the pattern" << endl;
            }

            add_variable_to_pattern(
                task_proxy,
                task_cost_function,
                sol_index,
                var);
            return;
        }
    }

    if (verbosity >= Verbosity::VERBOSE) {
        cout << token
             << "Could not add var/merge patterns due to size limits. "
                "Blacklisting...";
    }

    blacklisted_variables.insert(var);
}

template <typename PDBType>
PatternCollectionInformation<PDBType>
PatternCollectionGeneratorCegar<PDBType>::generate(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    utils::CountdownTimer timer(max_time);

    const ProbabilisticTaskProxy task_proxy(*task);
    TaskCostFunction* task_cost_function(g_cost_model->get_cost_function());
    const VariablesProxy variables = task_proxy.get_variables();
    const GoalsProxy goals = task_proxy.get_goals();

    if (given_goal != -1 && given_goal >= static_cast<int>(goals.size())) {
        cerr << "Goal variable out of range of task's variables" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    // save all goals in random order for refinement later
    bool found_given_goal = false;
    for (FactProxy fact : goals) {
        const int goal_var = fact.get_variable().get_id();
        remaining_goals.push_back(goal_var);
        if (given_goal != -1 && goal_var == given_goal) {
            found_given_goal = true;
        }
    }

    if (given_goal != -1 && !found_given_goal) {
        cerr << " Given goal variable is not a goal variable" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    rng->shuffle(remaining_goals);

    if (blacklist_size > 0) {
        const int num_vars = variables.size();
        vector<int> nongoals;
        nongoals.reserve(num_vars - remaining_goals.size());
        for (int var_id = 0; var_id < num_vars; ++var_id) {
            if (!utils::contains(remaining_goals, var_id)) {
                nongoals.push_back(var_id);
            }
        }
        rng->shuffle(nongoals);

        // Select a random subset of non goals.
        const auto m =
            min(static_cast<size_t>(blacklist_size), nongoals.size());

        for (size_t i = 0; i < m; ++i) {
            int var_id = nongoals[i];

            if (verbosity >= Verbosity::VERBOSE) {
                cout << token << "blacklisting var" << var_id << endl;
            }

            blacklisted_variables.insert(var_id);
        }
    }

    // Start with a solution of the trivial abstraction
    generate_trivial_solution_collection(task_proxy, *task_cost_function);

    const State initial_state = task_proxy.get_initial_state();
    initial_state.unpack();

    std::vector<Flaw> flaws;
    int solution_index;

    // main loop of the algorithm
    int refinement_counter = 1;
    while (!time_limit_reached(timer)) {
        if (verbosity >= Verbosity::VERBOSE) {
            cout << "iteration #" << refinement_counter << endl;
        }

        solution_index = get_flaws(task_proxy, flaws);

        if (flaws.empty()) {
            if (solution_index != -1) {
                const auto& info = pdb_infos[solution_index];

                assert(blacklisted_variables.empty());

                if (verbosity >= Verbosity::VERBOSE) {
                    cout << token
                         << "Task solved during computation of abstract"
                         << "policies." << endl;
                    cout << token << "Cost of policy: "
                         << info->get_policy_cost(initial_state) << endl;
                }
            } else {
                if (verbosity >= Verbosity::VERBOSE) {
                    cout << token << "Flaw list empty."
                         << "No further refinements possible." << endl;
                }
            }

            break;
        }

        if (time_limit_reached(timer)) {
            break;
        }

        // if there was a flaw, then refine the abstraction
        // such that said flaw does not occur again
        refine(task_proxy, *task_cost_function, variables, flaws);

        ++refinement_counter;
        flaws.clear();

        if (verbosity >= Verbosity::VERBOSE) {
            cout << token << "current collection size: " << collection_size
                 << endl;
            cout << token << "current collection: ";
            print_collection();
        }

        if (verbosity >= Verbosity::VERBOSE) {
            cout << endl;
        }
    }

    if (verbosity >= Verbosity::NORMAL) {
        cout << endl;
    }

    auto patterns = std::make_shared<PatternCollection>();
    auto pdbs = std::make_shared<PPDBCollection<PDBType>>();

    if (solution_index != -1) {
        unique_ptr<PDBType> pdb = pdb_infos[solution_index]->steal_pdb();
        patterns->push_back(pdb->get_pattern());
        pdbs->emplace_back(std::move(pdb));
    } else {
        for (const auto& info : pdb_infos) {
            if (info) {
                unique_ptr<PDBType> pdb = info->steal_pdb();
                patterns->push_back(pdb->get_pattern());
                pdbs->emplace_back(std::move(pdb));
            }
        }
    }

    if (verbosity >= Verbosity::NORMAL) {
        cout << token << "computation time: " << timer.get_elapsed_time()
             << endl;
        cout << token << "number of iterations: " << refinement_counter << endl;
        cout << token << "final collection: " << *patterns << endl << endl;
        cout << token
             << "final collection number of patterns: " << patterns->size()
             << endl;
        cout << token
             << "final collection summed PDB sizes: " << collection_size
             << endl;
    }

    std::shared_ptr<SubCollectionFinder> subcollection_finder =
        subcollection_finder_factory->create_subcollection_finder(task_proxy);

    PatternCollectionInformation<PDBType> pattern_collection_information(
        task_proxy,
        task_cost_function,
        patterns,
        subcollection_finder);
    pattern_collection_information.set_pdbs(pdbs);
    return pattern_collection_information;
}

template <typename PDBType>
void add_flaw_finder_options_to_parser(options::OptionParser& parser);

template <>
void add_flaw_finder_options_to_parser<SSPPatternDatabase>(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<FlawFindingStrategy<SSPPatternDatabase>>>(
        "flaw_strategy",
        "strategy used to find flaws in a policy",
        "pucs_flaw_finder_ec()");
}

template <>
void add_flaw_finder_options_to_parser<MaxProbPatternDatabase>(
    options::OptionParser& parser)
{
    parser.add_option<
        std::shared_ptr<FlawFindingStrategy<MaxProbPatternDatabase>>>(
        "flaw_strategy",
        "strategy used to find flaws in a policy",
        "pucs_flaw_finder_mp()");
}

template <typename PDBType>
void add_pattern_collection_generator_cegar_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_log_options_to_parser(parser);

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
    parser.add_option<int>(
        "blacklist_size",
        "Number of randomly selected non-goal variables that are globally "
        "blacklisted, which means excluded from being added to the pattern "
        "collection. 0 means no global blacklisting happens, infinity "
        "means to always exclude all non-goal variables.",
        "0",
        Bounds("0", "infinity"));
    std::vector<std::string> initial_collection_options;
    initial_collection_options.emplace_back("GIVEN_GOAL");
    initial_collection_options.emplace_back("RANDOM_GOAL");
    initial_collection_options.emplace_back("ALL_GOALS");
    parser.add_enum_option<InitialCollectionType>(
        "initial",
        initial_collection_options,
        "initial collection for refinement",
        "ALL_GOALS");
    parser.add_option<int>(
        "given_goal",
        "a goal variable to be used as the initial collection",
        "-1");
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
        "finder_max_orthogonality_factory()");

    add_flaw_finder_options_to_parser<PDBType>(parser);
}

template void add_pattern_collection_generator_cegar_options_to_parser<
    MaxProbPatternDatabase>(options::OptionParser& parser);
template void
add_pattern_collection_generator_cegar_options_to_parser<SSPPatternDatabase>(
    options::OptionParser& parser);

template <typename PDBType>
static shared_ptr<PatternCollectionGenerator<PDBType>>
_parse(options::OptionParser& parser)
{
    add_pattern_collection_generator_cegar_options_to_parser<PDBType>(parser);
    utils::add_rng_options(parser);

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<PatternCollectionGeneratorCegar<PDBType>>(opts);
}

static Plugin<PatternCollectionGenerator<MaxProbPatternDatabase>>
    _plugin_maxprob("cegar_maxprob_pdbs", _parse<MaxProbPatternDatabase>);
static Plugin<PatternCollectionGenerator<SSPPatternDatabase>>
    _plugin_expcost("cegar_ecpdbs", _parse<SSPPatternDatabase>);

template class PatternCollectionGeneratorCegar<MaxProbPatternDatabase>;
template class PatternCollectionGeneratorCegar<SSPPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd