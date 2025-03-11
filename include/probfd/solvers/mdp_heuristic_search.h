#ifndef PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/fret.h"
#include "probfd/algorithms/policy_picker.h"

#include "probfd/solvers/bisimulation_heuristic_search_algorithm.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

class State;
class OperatorID;

namespace probfd::quotients {
template <typename, typename>
struct QuotientState;
template <typename>
struct QuotientAction;
} // namespace probfd::quotients

namespace probfd::bisimulation {
enum class QuotientState;
} // namespace probfd::bisimulation

namespace probfd::solvers {

template <bool Bisimulation, bool Fret>
using StateType = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        probfd::quotients::
            QuotientState<probfd::bisimulation::QuotientState, OperatorID>,
        probfd::bisimulation::QuotientState>,
    std::conditional_t<
        Fret,
        probfd::quotients::QuotientState<State, OperatorID>,
        State>>;

template <bool Bisimulation, bool Fret>
using ActionType = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        probfd::quotients::QuotientAction<OperatorID>,
        OperatorID>,
    std::conditional_t<
        Fret,
        probfd::quotients::QuotientAction<OperatorID>,
        OperatorID>>;

template <bool Bisimulation, bool Fret>
class MDPHeuristicSearchBase : public MDPSolver {
protected:
    using PolicyPicker = algorithms::PolicyPicker<
        StateType<Bisimulation, Fret>,
        ActionType<Bisimulation, Fret>>;

    const value_t convergence_epsilon_;
    const bool dual_bounds_;
    const std::shared_ptr<PolicyPicker> tiebreaker_;

public:
    MDPHeuristicSearchBase(
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy,
        utils::Verbosity verbosity,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled);

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> create_quotient_heuristic_search_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&,
        Args&&... args)
    {
        if (dual_bounds_) {
            return std::make_unique<HS<State, OperatorID, true>>(
                convergence_epsilon_,
                tiebreaker_,
                std::forward<Args>(args)...);
        } else {
            return std::make_unique<HS<State, OperatorID, false>>(
                convergence_epsilon_,
                tiebreaker_,
                std::forward<Args>(args)...);
        }
    }

    virtual std::string get_heuristic_search_name() const = 0;
};

template <
    typename R,
    template <bool...> typename T,
    bool... b,
    typename... Args>
std::unique_ptr<R>
construct(std::tuple<Args...> args, bool f, std::same_as<bool> auto... flags);

template <
    typename R,
    template <bool...> typename T,
    bool... b,
    typename... Args>
std::unique_ptr<R> construct(std::tuple<Args...> args);

template <
    typename R,
    template <bool...> typename T,
    bool... b,
    typename... Args>
std::unique_ptr<R>
construct(std::tuple<Args...> args, bool f, std::same_as<bool> auto... flags)
{
    if (f) {
        return construct<R, T, b..., true>(std::move(args), flags...);
    } else {
        return construct<R, T, b..., false>(std::move(args), flags...);
    }
}

template <
    typename R,
    template <bool...> typename T,
    bool... b,
    typename... Args>
std::unique_ptr<R> construct(std::tuple<Args...> args)
{
    return std::apply(
        [](Args... args) {
            return std::make_unique<T<b...>>(std::forward<Args>(args)...);
        },
        std::move(args));
}

template <bool Bisimulation, bool Fret>
class MDPHeuristicSearch;

template <>
class MDPHeuristicSearch<false, false>
    : public MDPHeuristicSearchBase<false, false> {
    template <template <typename, typename, bool> typename T>
    struct AlgTypeHelper {
        template <bool dual>
        using type_template = T<State, OperatorID, dual>;
    };

public:
    MDPHeuristicSearch(
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy,
        utils::Verbosity verbosity,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled);

    std::string get_algorithm_name() const override;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> create_heuristic_search_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&,
        Args&&... args)
    {
        return construct<
            MDPAlgorithm<State, OperatorID>,
            AlgTypeHelper<HS>::template type_template>(
            std::forward_as_tuple(
                convergence_epsilon_,
                tiebreaker_,
                std::forward<Args>(args)...),
            dual_bounds_);
    }
};

template <>
class MDPHeuristicSearch<false, true>
    : public MDPHeuristicSearchBase<false, true> {
    using QState = quotients::QuotientState<State, OperatorID>;
    using QAction = quotients::QuotientAction<OperatorID>;

    const bool fret_on_policy_;

    template <template <typename, typename, bool> typename T>
    struct AlgTypeHelper {
        template <bool dual, bool fret_on_policy>
        using type_template = std::conditional_t<
            fret_on_policy,
            algorithms::fret::FRETPi<T<QState, QAction, dual>>,
            algorithms::fret::FRETV<T<QState, QAction, dual>>>;
    };

public:
    MDPHeuristicSearch(
        bool fret_on_policy,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy,
        utils::Verbosity verbosity,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled);

    std::string get_algorithm_name() const override;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> create_heuristic_search_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&,
        Args&&... args)
    {
        return construct<
            FDRMDPAlgorithm,
            AlgTypeHelper<HS>::template type_template>(
            std::forward_as_tuple(
                convergence_epsilon_,
                tiebreaker_,
                std::forward<Args>(args)...),
            this->dual_bounds_,
            this->fret_on_policy_);
    }
};

template <>
class MDPHeuristicSearch<true, false>
    : public MDPHeuristicSearchBase<true, false> {
    using QState = bisimulation::QuotientState;
    using QAction = OperatorID;

    template <template <typename, typename, bool> typename T>
    struct AlgTypeHelper {
        template <bool dual>
        using type_template = T<QState, QAction, dual>;
    };

public:
    MDPHeuristicSearch(
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy,
        utils::Verbosity verbosity,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled);

    std::string get_algorithm_name() const override;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> create_heuristic_search_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        Args&&... args)
    {
        return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
            task,
            task_cost_function,
            this->get_heuristic_search_name(),
            construct<
                MDPAlgorithm<QState, QAction>,
                AlgTypeHelper<HS>::template type_template>(
                std::forward_as_tuple(
                    this->convergence_epsilon_,
                    this->tiebreaker_,
                    std::forward<Args>(args)...),
                dual_bounds_));
    }
};

template <>
class MDPHeuristicSearch<true, true>
    : public MDPHeuristicSearchBase<true, true> {
    using QState = bisimulation::QuotientState;
    using QAction = OperatorID;

    using QQState = quotients::QuotientState<QState, QAction>;
    using QQAction = quotients::QuotientAction<QAction>;

    template <template <typename, typename, bool> typename T>
    struct AlgTypeHelper {
        template <bool dual, bool fret_on_policy>
        using type_template = std::conditional_t<
            fret_on_policy,
            algorithms::fret::FRETPi<T<QQState, QQAction, dual>>,
            algorithms::fret::FRETV<T<QQState, QQAction, dual>>>;
    };

    const bool fret_on_policy_;

public:
    MDPHeuristicSearch(
        bool fret_on_policy,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy,
        utils::Verbosity verbosity,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled);

    std::string get_algorithm_name() const override;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> create_heuristic_search_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        Args&&... args)
    {
        return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
            task,
            task_cost_function,
            this->get_heuristic_search_name(),
            construct<
                MDPAlgorithm<QState, QAction>,
                AlgTypeHelper<HS>::template type_template>(
                std::forward_as_tuple(
                    this->convergence_epsilon_,
                    this->tiebreaker_,
                    std::forward<Args>(args)...),
                dual_bounds_,
                fret_on_policy_));
    }
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H
