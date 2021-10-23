#include "pattern_collection_generator_cegar.h"

#include "abstract_solution_data.h"

#include "../../expcost_projection.h"
#include "../../maxprob_projection.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

#include "../../../../../utils/collections.h"
#include "../../../../../utils/countdown_timer.h"
#include "../../../../../utils/logging.h"
#include "../../../../../utils/math.h"
#include "../../../../../utils/rng.h"
#include "../../../../../utils/rng_options.h"
#include "../../../../../utils/system.h"

#include "../../../../../algorithms/priority_queues.h"

#include "../../../../../successor_generator.h"

#include <stack>

using namespace std;
using utils::Verbosity;

namespace std {
template <>
struct hash<probabilistic::pdbs::pattern_selection::ExplicitGState> {
    size_t
    operator()(const probabilistic::pdbs::pattern_selection::ExplicitGState&
                   state) const
    {
        return state.get_hash();
    }
};
} // namespace std

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

namespace {
static const std::string token = "CEGAR_PDBs: ";
}

template <typename PDBType>
PatternCollectionGeneratorCegar<PDBType>::PatternCollectionGeneratorCegar(
    std::shared_ptr<SubCollectionFinder> subcollection_finder,
    const shared_ptr<utils::RandomNumberGenerator>& arg_rng,
    FlawFinderEnum flaw_finder_val,
    int arg_max_refinements,
    int arg_max_pdb_size,
    int arg_max_collection_size,
    bool arg_ignore_goal_violations,
    bool treat_goal_violations_differently,
    bool arg_local_blacklisting,
    int global_blacklist_size,
    InitialCollectionType arg_initial,
    int given_goal,
    Verbosity verbosity,
    double arg_max_time)
    : subcollection_finder(subcollection_finder)
    , rng(arg_rng)
    , max_refinements(arg_max_refinements)
    , max_pdb_size(arg_max_pdb_size)
    , max_collection_size(arg_max_collection_size)
    , ignore_goal_violations(arg_ignore_goal_violations)
    , treat_goal_violations_differently(treat_goal_violations_differently)
    , local_blacklisting(arg_local_blacklisting)
    , global_blacklist_size(global_blacklist_size)
    , initial(arg_initial)
    , given_goal(given_goal)
    , verbosity(verbosity)
    , max_time(arg_max_time)
    , collection_size(0)
    , concrete_solution_index(-1)
{
    switch (flaw_finder_val) {
    case FlawFinderEnum::PGBFS:
        flaw_strategy.reset(new ProbabilityGBFS(*this));
        break;
    case FlawFinderEnum::BFS: flaw_strategy.reset(new BFS(*this)); break;
    case FlawFinderEnum::SAMPLING: flaw_strategy.reset(new SamplingStrategy(*this)); break;
    }

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

        cout << token << "flaw strategy: ";

        switch (flaw_finder_val) {
        case FlawFinderEnum::PGBFS:
            cout << "probability greedy best-first search" << endl;
            break;
        case FlawFinderEnum::BFS: cout << "breadth-first search" << endl; break;
        case FlawFinderEnum::SAMPLING: cout << "successor sampling" << endl; break;
        }

        cout << token << "max refinements: " << max_refinements << endl;
        cout << token << "max pdb size: " << max_pdb_size << endl;
        cout << token << "max collection size: " << max_collection_size << endl;
        cout << token << "ignore goal violations: " << ignore_goal_violations
             << endl;
        cout << token << "treat goal violations like regular ones: "
             << treat_goal_violations_differently << endl;
        cout << token << "local blacklisting: " << local_blacklisting << endl;
        cout << token << "global blacklist size: " << global_blacklist_size
             << endl;
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
          opts.get<std::shared_ptr<SubCollectionFinder>>("subcollection_finder"),
          utils::parse_rng_from_options(opts),
          static_cast<FlawFinderEnum>(opts.get_enum("flaw_strategy")),
          opts.get<int>("max_refinements"),
          opts.get<int>("max_pdb_size"),
          opts.get<int>("max_collection_size"),
          opts.get<bool>("ignore_goal_violations"),
          opts.get<bool>("treat_goal_violations_differently"),
          opts.get<bool>("local_blacklisting"),
          opts.get<int>("global_blacklist_size"),
          static_cast<InitialCollectionType>(opts.get_enum("initial")),
          opts.get<int>("given_goal"),
          static_cast<Verbosity>(opts.get_enum("verbosity")),
          opts.get<double>("max_time"))
{
}

template <typename PDBType>
PatternCollectionGeneratorCegar<PDBType>::~PatternCollectionGeneratorCegar() =
    default;

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::print_collection() const
{
    cout << "[";

    for (size_t i = 0; i < solutions.size(); ++i) {
        const auto& sol = solutions[i];
        if (sol) {
            cout << sol->get_pattern();
            if (i != solutions.size() - 1) {
                cout << ", ";
            }
        }
    }

    cout << "]" << endl;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::
generate_trivial_solution_collection()
{
    assert(!remaining_goals.empty());

    switch (initial) {
    case InitialCollectionType::GIVEN_GOAL: {
        assert(given_goal != -1);
        update_goals(given_goal);
        add_pattern_for_var(given_goal);
        break;
    }
    case InitialCollectionType::RANDOM_GOAL: {
        int var = remaining_goals.back();
        remaining_goals.pop_back();
        add_pattern_for_var(var);
        break;
    }
    case InitialCollectionType::ALL_GOALS: {
        while (!remaining_goals.empty()) {
            int var = remaining_goals.back();
            remaining_goals.pop_back();
            add_pattern_for_var(var);
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
bool PatternCollectionGeneratorCegar<PDBType>::termination_conditions_met(
    const utils::CountdownTimer& timer,
    int refinement_counter) const
{
    if (time_limit_reached(timer)) {
        return true;
    }

    if (refinement_counter == max_refinements) {
        if (verbosity >= Verbosity::NORMAL) {
            cout << token << "maximum allowed number of refinements reached."
                 << endl;
        }

        return true;
    }

    return false;
}

template <typename PDBType>
PatternCollectionGeneratorCegar<PDBType>::FlawFindingStrategy::
    FlawFindingStrategy(PatternCollectionGeneratorCegar<PDBType>& base)
    : base(base)
{
}

template <typename PDBType>
PatternCollectionGeneratorCegar<PDBType>::ProbabilityGBFS::ProbabilityGBFS(
    PatternCollectionGeneratorCegar<PDBType>& base)
    : FlawFindingStrategy(base)
{
}

template <typename PDBType>
PatternCollectionGeneratorCegar<PDBType>::BFS::BFS(
    PatternCollectionGeneratorCegar<PDBType>& base)
    : FlawFindingStrategy(base)
{
}

template <typename PDBType>
PatternCollectionGeneratorCegar<PDBType>::SamplingStrategy::SamplingStrategy(
    PatternCollectionGeneratorCegar<PDBType>& base)
    : FlawFindingStrategy(base)
{
}

template <typename PDBType>
FlawList
PatternCollectionGeneratorCegar<PDBType>::ProbabilityGBFS::apply_policy(
    int solution_index,
    const ExplicitGState& init)
{
    AbstractSolutionData<PDBType>& solution = *this->base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    FlawList flaws;
    priority_queues::HeapQueue<value_type::value_t, ExplicitGState> pq;
    pq.push(1.0, init);

    std::unordered_map<ExplicitGState, value_type::value_t> probabilities;

    while (!pq.empty()) {
        const auto& [priority, current] = pq.pop();

        // TODO remove this once we have a real priority queue...
        if (priority < probabilities[current]) {
            continue;
        }

        const AbstractState abs = pdb.get_abstract_state(current.values);
        const AbstractOperator* abs_op = policy.get_operator_if_present(abs);

        // We reached an abstract goal, check if the concrete state is a
        // goal
        if (!abs_op) {
            if (!current.is_goal()) {
                if (this->base.verbosity >= Verbosity::VERBOSE) {
                    cout << token << "Policy of pattern "
                         << solution.get_pattern()
                         << "failed with goal violation." << std::endl;
                }

                if (!this->base.ignore_goal_violations) {
                    // Collect all non-satisfied goal variables that are
                    // still available.
                    for (const auto& [goal_var, goal_value] : g_goal) {
                        if (current[goal_var] != goal_value &&
                            !utils::contains(this->base.global_blacklist, goal_var) &&
                            utils::contains(this->base.remaining_goals, goal_var)) {
                            flaws.emplace_back(true, solution_index, goal_var);
                        }
                    }

                    return flaws;
                }

                if (this->base.verbosity >= Verbosity::VERBOSE) {
                    cout << "We ignore goal violations, thus we continue."
                         << endl;
                }
            }

            continue;
        }

        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperator& op = g_operators[original_id];

        // Check whether all preconditions are fulfilled
        for (const auto& [pre_var, pre_val] : op.get_preconditions()) {
            // We ignore blacklisted variables
            const bool is_blacklist_var =
                utils::contains(this->base.global_blacklist, pre_var);

            if (is_blacklist_var || solution.is_blacklisted(pre_var)) {
                assert(
                    !solution.is_blacklisted(pre_var) ||
                    this->base.local_blacklisting);
                continue;
            }

            if (current[pre_var] != pre_val) {
                flaws.emplace_back(false, solution_index, pre_var);
            }
        }

        if (!flaws.empty()) {
            if (this->base.verbosity >= Verbosity::VERBOSE) {
                cout << token << "Policy of pattern "
                        << solution.get_pattern()
                        << "failed with precondition violation."
                        << std::endl;
            }

            return flaws;
        }

        // Generate the successors and add them to the queue
        for (const auto& [det_op, prob] : op) {
            const value_type::value_t succ_prob = priority * prob;
            auto successor = current.get_successor(*det_op);

            auto [it, inserted] = probabilities.emplace(successor, succ_prob);

            if (!inserted) {
                if (succ_prob <= it->second) {
                    continue;
                }

                it->second = succ_prob;
            }

            pq.push(succ_prob, std::move(successor));
        }
    }

    return flaws;
}

template <typename PDBType>
FlawList PatternCollectionGeneratorCegar<PDBType>::BFS::apply_policy(
    int solution_index,
    const ExplicitGState& init)
{
    AbstractSolutionData<PDBType>& solution = *this->base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    FlawList flaws;
    ExplicitGState current(init);

    std::deque<ExplicitGState> open;
    open.push_back(init);
    std::unordered_set<ExplicitGState> closed;
    closed.insert(init.values);

    while (!open.empty()) {
        ExplicitGState current = open.front();
        open.pop_front();

        const AbstractState abs = pdb.get_abstract_state(current.values);
        const AbstractOperator* abs_op = policy.get_operator_if_present(abs);

        // We reached an abstract goal, check if the concrete state is a goal
        if (!abs_op) {
            if (!current.is_goal()) {
                if (this->base.verbosity >= Verbosity::VERBOSE) {
                    cout << token << "Policy of pattern "
                         << solution.get_pattern()
                         << "failed with goal violation." << std::endl;
                }

                if (!this->base.ignore_goal_violations) {
                    // Collect all non-satisfied goal variables that are still
                    // available.
                    for (const auto& [goal_var, goal_value] : g_goal) {
                        if (current[goal_var] != goal_value &&
                            !utils::contains(this->base.global_blacklist, goal_var) &&
                            utils::contains(this->base.remaining_goals, goal_var)) {
                            flaws.emplace_back(true, solution_index, goal_var);
                        }
                    }

                    return flaws;
                }

                if (this->base.verbosity >= Verbosity::VERBOSE) {
                    cout << "We ignore goal violations, thus we continue."
                         << endl;
                }
            }

            continue;
        }

        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperator& op = g_operators[original_id];

        // Check whether all preconditions are fulfilled
        for (const auto& [pre_var, pre_val] : op.get_preconditions()) {
            // We ignore blacklisted variables
            const bool is_blacklist_var =
                utils::contains(this->base.global_blacklist, pre_var);

            if (is_blacklist_var || solution.is_blacklisted(pre_var)) {
                assert(
                    !solution.is_blacklisted(pre_var) ||
                    this->base.local_blacklisting);
                continue;
            }

            if (current[pre_var] != pre_val) {
                flaws.emplace_back(false, solution_index, pre_var);
            }
        }

        if (!flaws.empty()) {
            if (this->base.verbosity >= Verbosity::VERBOSE) {
                cout << token << "Policy of pattern "
                        << solution.get_pattern()
                        << "failed with precondition violation."
                        << std::endl;
            }

            return flaws;
        }

        // Generate the successors and add them to the open list
        for (const auto& outcome : op) {
            ExplicitGState successor = current.get_successor(*outcome.op);

            if (!utils::contains(closed, successor)) {
                closed.insert(successor);
                open.push_back(successor);
            }
        }
    }

    if (this->base.verbosity >= Verbosity::VERBOSE) {
        cout << token << "Policy of pattern " << solution.get_pattern()
             << " successfully executed.";
    }

    if (this->base.global_blacklist.empty() && solution.get_blacklist().empty() &&
        !this->base.ignore_goal_violations) {
        if (this->base.verbosity >= Verbosity::VERBOSE) {
            cout << "There are no blacklisted variables and there were no goal "
                    "violations, hence the concrete task is solved."
                 << endl;
        }

        this->base.concrete_solution_index = solution_index;
    } else {
        if (this->base.verbosity >= Verbosity::VERBOSE) {
            cout << "Since there are blacklisted variables, the policy "
                    "is not guaranteed to work in the concrete state "
                    "space. Marking this solution as solved."
                 << endl;
        }

        solution.mark_as_solved();
    }

    return flaws;
}

template <typename PDBType>
FlawList
PatternCollectionGeneratorCegar<PDBType>::SamplingStrategy::apply_policy(
    int solution_index,
    const ExplicitGState& init)
{
    AbstractSolutionData<PDBType>& solution =
        *this->base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    FlawList flaws;
    std::stack<ExplicitGState> stk;
    stk.push(init);

    struct ExplorationInfo {
        Distribution<ExplicitGState> successors;
    };

    std::unordered_map<ExplicitGState, ExplorationInfo> einfos;

    while (!stk.empty()) {
        const ExplicitGState& current = stk.top();

        auto p = einfos.try_emplace(current);
        ExplorationInfo& einfo = p.first->second;

        // Already explored
        if (!p.second) {
            stk.pop();
            continue;
        }

        // Check flaws, generate successors
        const AbstractState abs = pdb.get_abstract_state(current.values);
        const AbstractOperator* abs_op = policy.get_operator_if_present(abs);

        // We reached an abstract goal, check if the concrete state is a
        // goal
        if (!abs_op) {
            if (!current.is_goal()) {
                if (this->base.verbosity >= Verbosity::VERBOSE) {
                    cout << token << "Policy of pattern "
                         << solution.get_pattern()
                         << "failed with goal violation." << std::endl;
                }

                if (!this->base.ignore_goal_violations) {
                    // Collect all non-satisfied goal variables that are
                    // still available.
                    for (const auto& [goal_var, goal_value] : g_goal) {
                        if (current[goal_var] != goal_value &&
                            !utils::contains(this->base.global_blacklist, goal_var) &&
                            utils::contains(this->base.remaining_goals, goal_var)) {
                            flaws.emplace_back(true, solution_index, goal_var);
                        }
                    }

                    return flaws;
                }

                if (this->base.verbosity >= Verbosity::VERBOSE) {
                    cout << "We ignore goal violations, thus we continue."
                         << endl;
                }
            }

            stk.pop();
            continue;
        }

        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperator& op = g_operators[original_id];

        // Check whether precondition flaws occur
        for (const auto& [pre_var, pre_val] : op.get_preconditions()) {
            // We ignore blacklisted variables
            const bool is_blacklist_var =
                utils::contains(this->base.global_blacklist, pre_var);

            if (is_blacklist_var || solution.is_blacklisted(pre_var)) {
                assert(
                    !solution.is_blacklisted(pre_var) ||
                    this->base.local_blacklisting);
                continue;
            }

            if (current[pre_var] != pre_val) {
                flaws.emplace_back(false, solution_index, pre_var);
            }
        }

        // Flaws occured.
        if (!flaws.empty()) {
            if (this->base.verbosity >= Verbosity::VERBOSE) {
                cout << token << "Policy of pattern "
                        << solution.get_pattern()
                        << "failed with precondition violation."
                        << std::endl;
            }

            return flaws;
        }

        // Generate the successors
        for (const auto& [det_op, prob] : op) {
            einfo.successors.add_unique(current.get_successor(*det_op), prob);
        }

        // Sample successor and recurse
        auto it = einfo.successors.sample(*this->base.rng);
        stk.push(std::move(it->first));
        einfo.successors.erase(it);
    }

    assert (flaws.empty());
    return flaws;
}

template <typename PDBType>
FlawList PatternCollectionGeneratorCegar<PDBType>::apply_policy(
    int solution_index,
    const ExplicitGState& init)
{
    return flaw_strategy->apply_policy(solution_index, init);
}

template <typename PDBType>
FlawList PatternCollectionGeneratorCegar<PDBType>::get_flaws()
{
    FlawList flaws;
    ExplicitGState concrete_init(g_initial_state_data);

    const int num_solutions = static_cast<int>(solutions.size());
    for (int sol_idx = 0; sol_idx < num_solutions; ++sol_idx) {
        auto& sol = solutions[sol_idx];

        if (!sol || sol->is_solved()) {
            continue;
        }

        // AbstractSolutionData<PDBType>& solution = *solutions[sol_idx];

        // abort here if no abstract solution could be found
        if (!sol->solution_exists()) {
            cout << token << "Problem unsolvable" << endl;
            utils::exit_with(utils::ExitCode::SEARCH_UNSOLVABLE);
        }

        // find out if and why the abstract solution
        // would not work for the concrete task.
        // We always start with the initial state.
        FlawList new_flaws = apply_policy(sol_idx, concrete_init);

        if (concrete_solution_index != -1) {
            // We solved the concrete task. Return empty flaws to signal
            // terminating.
            assert(concrete_solution_index == sol_idx);
            assert(new_flaws.empty());
            flaws.clear();
            return flaws;
        }

        for (Flaw& flaw : new_flaws) {
            flaws.push_back(move(flaw));
        }
    }

    return flaws;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::update_goals(int added_var)
{
    /*
      Only call this method if added_var is definitely added to some
      pattern. It removes the variable from remaining_goals if it is
      contained there.
    */
    auto result =
        find(remaining_goals.begin(), remaining_goals.end(), added_var);
    if (result != remaining_goals.end()) {
        remaining_goals.erase(result);
    }
}

template <typename PDBType>
bool PatternCollectionGeneratorCegar<PDBType>::can_add_singleton_pattern(
    int var) const
{
    int pdb_size = g_variable_domain[var];
    return pdb_size <= max_pdb_size &&
           collection_size <= max_collection_size - pdb_size;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::add_pattern_for_var(int var)
{
    auto& sol = solutions.emplace_back(
        new AbstractSolutionData<PDBType>({var}, {}));
    solution_lookup[var] = solutions.size() - 1;
    collection_size += sol->get_pdb().num_states();
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::handle_goal_violation(
    const Flaw& flaw)
{
    int var = flaw.variable;
    assert(!utils::contains_key(solution_lookup, var));

    if (verbosity >= Verbosity::VERBOSE) {
        cout << token << "introducing goal variable " << var << endl;
    }

    // check for the edge case where the single-variable pattern
    // causes the collection to grow larger than the allowed limit
    if (can_add_singleton_pattern(var)) {
        update_goals(var);
        add_pattern_for_var(var);
    } else {
        if (verbosity >= Verbosity::VERBOSE) {
            cout << token
                 << "Can't add variable because it is too large to "
                    "fit either the pdb max size limit or the "
                    "collection size limit. Blacklisting..."
                 << endl;
        }

        global_blacklist.insert(var);
    }
}

template <typename PDBType>
bool PatternCollectionGeneratorCegar<PDBType>::can_merge_patterns(
    int index1,
    int index2) const
{
    int pdb_size1 = solutions[index1]->get_pdb().num_states();
    int pdb_size2 = solutions[index2]->get_pdb().num_states();

    if (!utils::is_product_within_limit(pdb_size1, pdb_size2, max_pdb_size)) {
        return false;
    }

    int added_size = pdb_size1 * pdb_size2 - pdb_size1 - pdb_size2;
    return collection_size + added_size <= max_collection_size;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::merge_patterns(
    int index1,
    int index2)
{
    // Merge pattern at index2 into pattern at index2
    AbstractSolutionData<PDBType>& solution1 = *solutions[index1];
    AbstractSolutionData<PDBType>& solution2 = *solutions[index2];

    const PDBType& pdb1 = solution1.get_pdb();
    const PDBType& pdb2 = solution2.get_pdb();

    // update look-up table
    for (int var : solution2.get_pattern()) {
        solution_lookup[var] = index1;
    }

    // compute merged local blacklist
    const set<int>& blacklist1 = solution1.get_blacklist();
    const set<int>& blacklist2 = solution2.get_blacklist();
    set<int> new_blacklist(blacklist1);
    new_blacklist.insert(blacklist2.begin(), blacklist2.end());

    // store old pdb sizes
    int pdb_size1 = pdb1.num_states();
    int pdb_size2 = pdb2.num_states();

    // compute merge solution
    unique_ptr<AbstractSolutionData<PDBType>> merged(
        new AbstractSolutionData<PDBType>(pdb1, pdb2, new_blacklist));

    // update collection size
    collection_size -= pdb_size1;
    collection_size -= pdb_size2;
    collection_size += merged->get_pdb().num_states();

    // clean-up
    solutions[index1] = std::move(merged);
    solutions[index2] = nullptr;
}

template <typename PDBType>
bool PatternCollectionGeneratorCegar<PDBType>::can_add_variable_to_pattern(
    int index,
    int var) const
{
    int pdb_size = solutions[index]->get_pdb().num_states();
    int domain_size = g_variable_domain[var];

    if (!utils::is_product_within_limit(pdb_size, domain_size, max_pdb_size)) {
        return false;
    }

    int added_size = pdb_size * domain_size - pdb_size;
    return collection_size + added_size <= max_collection_size;
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::add_variable_to_pattern(
    int index,
    int var)
{
    AbstractSolutionData<PDBType>& solution = *solutions[index];

    // compute new solution
    unique_ptr<AbstractSolutionData<PDBType>> new_solution(
        new AbstractSolutionData<PDBType>(
            solution.get_pdb(),
            var,
            solution.get_blacklist()));

    // update collection size
    collection_size -= solution.get_pdb().num_states();
    collection_size += new_solution->get_pdb().num_states();

    // update look-up table and possibly remaining_goals, clean-up
    solution_lookup[var] = index;
    update_goals(var);
    solutions[index] = move(new_solution);
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::handle_precondition_violation(
    const Flaw& flaw)
{
    int sol_index = flaw.solution_index;
    int var = flaw.variable;
    bool added_var = false;

    const auto it = solution_lookup.find(var);

    if (it != solution_lookup.end()) {
        // var is already in another pattern of the collection
        int other_index = it->second;
        assert(other_index != sol_index);
        assert(solutions[other_index] != nullptr);

        if (verbosity >= Verbosity::VERBOSE) {
            cout << token << "var" << var << " is already in pattern "
                 << solutions[other_index]->get_pattern() << endl;
        }

        if (can_merge_patterns(sol_index, other_index)) {
            if (verbosity >= Verbosity::VERBOSE) {
                cout << token << "merge the two patterns" << endl;
            }

            merge_patterns(sol_index, other_index);
            added_var = true;
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

        if (can_add_variable_to_pattern(sol_index, var)) {
            if (verbosity >= Verbosity::VERBOSE) {
                cout << token << "add it to the pattern" << endl;
            }

            add_variable_to_pattern(sol_index, var);
            added_var = true;
        }
    }

    if (!added_var) {
        if (verbosity >= Verbosity::VERBOSE) {
            cout << token
                 << "Could not add var/merge patterns due to size "
                    "limits. Blacklisting ";
        }

        if (local_blacklisting) {
            solutions[sol_index]->blacklist_variable(var);

            if (verbosity >= Verbosity::VERBOSE) {
                cout << "locally." << endl;
            }
        } else {
            global_blacklist.insert(var);

            if (verbosity >= Verbosity::VERBOSE) {
                cout << "globally." << endl;
            }
        }
    }
}

template <typename PDBType>
void PatternCollectionGeneratorCegar<PDBType>::refine(
    const FlawList& flaws)
{
    assert(!flaws.empty());

    // pick a random flaw
    int random_flaw_index = (*rng)(flaws.size());
    const Flaw& flaw = flaws[random_flaw_index];

    if (verbosity >= Verbosity::VERBOSE) {
        cout << token << "chosen flaw: pattern "
             << solutions[flaw.solution_index]->get_pattern();
    }

    if (treat_goal_violations_differently && flaw.is_goal_violation) {
        if (verbosity >= Verbosity::VERBOSE) {
            cout << " with a goal violation on " << flaw.variable << endl;
        }

        handle_goal_violation(flaw);
    } else {
        if (verbosity >= Verbosity::VERBOSE) {
            cout << " with a violated precondition on " << flaw.variable
                 << endl;
        }

        handle_precondition_violation(flaw);
    }
}

template <typename PDBType>
PatternCollectionInformation<PDBType>
PatternCollectionGeneratorCegar<PDBType>::generate(OperatorCost)
{
    utils::CountdownTimer timer(max_time);

    if (given_goal != -1 &&
        given_goal >= static_cast<int>(g_variable_domain.size())) {
        cerr << "Goal variable out of range of task's variables" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    // save all goals in random order for refinement later
    bool found_given_goal = false;
    for (auto& [goal_var, _] : g_goal) {
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

    if (global_blacklist_size) {
        int num_vars = g_variable_domain.size();
        vector<int> nongoals;
        nongoals.reserve(num_vars - remaining_goals.size());
        for (int var_id = 0; var_id < num_vars; ++var_id) {
            if (find(remaining_goals.begin(), remaining_goals.end(), var_id) ==
                remaining_goals.end()) {
                nongoals.push_back(var_id);
            }
        }
        rng->shuffle(nongoals);

        // Select a random subset of non goals.
        const auto m =
            min(static_cast<size_t>(global_blacklist_size), nongoals.size());

        for (size_t i = 0; i < m; ++i) {
            int var_id = nongoals[i];

            if (verbosity >= Verbosity::VERBOSE) {
                cout << token << "blacklisting var" << var_id << endl;
            }

            global_blacklist.insert(var_id);
        }
    }

    // Start with a solution of the trivial abstraction
    generate_trivial_solution_collection();

    // main loop of the algorithm
    int refinement_counter = 0;
    while (!termination_conditions_met(timer, refinement_counter)) {
        if (verbosity >= Verbosity::VERBOSE) {
            cout << "iteration #" << refinement_counter + 1 << endl;
        }

        // vector of solution indices and flaws associated with said solutions
        FlawList flaws = get_flaws();

        if (flaws.empty()) {
            if (concrete_solution_index != -1) {
                auto& sol = solutions[concrete_solution_index];

                assert(
                    global_blacklist.empty() && sol->get_blacklist().empty());

                if (verbosity >= Verbosity::VERBOSE) {
                    cout << token
                         << "Task solved during computation of abstract"
                         << "policies." << endl;
                    // sol->print_plan(cout);
                    // cout << token
                    //     << "length of plan: " << sol->get_plan().size()
                    //     << " step(s)." << endl;
                    cout << token
                         << "Cost of policy: " << sol->get_policy_cost()
                         << endl;
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
        refine(flaws);

        ++refinement_counter;

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

    if (concrete_solution_index != -1) {
        unique_ptr<PDBType> pdb =
            solutions[concrete_solution_index]->steal_pdb();
        patterns->push_back(pdb->get_pattern());
        pdbs->emplace_back(std::move(pdb));
    } else {
        for (const auto& sol : solutions) {
            if (sol) {
                unique_ptr<PDBType> pdb = sol->steal_pdb();
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

    PatternCollectionInformation<PDBType> pattern_collection_information(
        patterns, subcollection_finder);
    pattern_collection_information.set_pdbs(pdbs);
    return pattern_collection_information;
}

void add_pattern_collection_generator_cegar_options_to_parser(
    options::OptionParser& parser)
{
    utils::add_verbosity_option_to_parser(parser);

    std::vector<std::string> flaw_finder_options;
    flaw_finder_options.emplace_back("pgbfs");
    flaw_finder_options.emplace_back("bfs");
    flaw_finder_options.emplace_back("sampling");
    parser.add_enum_option(
        "flaw_strategy",
        flaw_finder_options,
        "strategy used to find flaws in a policy",
        "pgbfs");
    parser.add_option<int>(
        "max_refinements",
        "maximum allowed number of refinements",
        "infinity",
        Bounds("0", "infinity"));
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
    parser.add_option<bool>(
        "local_blacklisting",
        "if a variable is too large to be added to a pattern, forbid "
        "it only for that pattern",
        "true");
    parser.add_option<bool>(
        "ignore_goal_violations",
        "ignore goal violations and consequently generate a single pattern",
        "false");
    parser.add_option<int>(
        "global_blacklist_size",
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
    parser.add_enum_option(
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
    parser.add_option<bool>(
        "treat_goal_violations_differently",
        "If true, violated goal variables will be introduced as a separate "
        "pattern. Otherwise, they will be treated like precondition variables, "
        "thus added to the pattern in question or merging two patterns if "
        "already in the collection.",
        "true");
    parser.add_option<std::shared_ptr<SubCollectionFinder>>(
        "subcollection_finder",
        "The subcollection finder.",
        "finder_max_orthogonality()");
}

template <typename PDBType>
static shared_ptr<PatternCollectionGenerator<PDBType>>
_parse(options::OptionParser& parser)
{
    add_pattern_collection_generator_cegar_options_to_parser(parser);
    utils::add_rng_options(parser);

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<PatternCollectionGeneratorCegar<PDBType>>(opts);
}

static Plugin<PatternCollectionGenerator<MaxProbProjection>>
    _plugin_maxprob("cegar_maxprob_pdbs", _parse<MaxProbProjection>);
static Plugin<PatternCollectionGenerator<ExpCostProjection>>
    _plugin_expcost("cegar_ecpdbs", _parse<ExpCostProjection>);

template class PatternCollectionGeneratorCegar<MaxProbProjection>;
template class PatternCollectionGeneratorCegar<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic