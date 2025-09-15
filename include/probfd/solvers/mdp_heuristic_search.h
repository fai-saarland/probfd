#ifndef PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/fret.h"
#include "probfd/algorithms/policy_picker.h"

#include "probfd/solvers/bisimulation_heuristic_search_algorithm.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace downward {
class State;
class OperatorID;
} // namespace downward

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

template <
    bool Bisimulation,
    bool Fret,
    typename State = downward::State,
    typename Action = downward::OperatorID>
using StateType = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        probfd::quotients::
            QuotientState<probfd::bisimulation::QuotientState, Action>,
        probfd::bisimulation::QuotientState>,
    std::conditional_t<
        Fret,
        probfd::quotients::QuotientState<State, Action>,
        State>>;

template <
    bool Bisimulation,
    bool Fret,
    typename State = downward::State,
    typename Action = downward::OperatorID>
using ActionType = std::conditional_t<
    Bisimulation,
    std::conditional_t<Fret, probfd::quotients::QuotientAction<Action>, Action>,
    std::
        conditional_t<Fret, probfd::quotients::QuotientAction<Action>, Action>>;

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

template <
    bool Bisimulation,
    bool Fret,
    typename State = downward::State,
    typename Action = downward::OperatorID>
class MDPHeuristicSearchBase : public StatisticalMDPAlgorithmFactory {
protected:
    using PolicyPicker = algorithms::PolicyPicker<
        StateType<Bisimulation, Fret, State, Action>,
        ActionType<Bisimulation, Fret, State, Action>>;

    const value_t convergence_epsilon_;
    const bool dual_bounds_;
    const std::shared_ptr<PolicyPicker> tiebreaker_;

    template <template <typename, typename, bool> typename T>
    struct AlgTypeHelper {
        template <bool dual>
        using type_template = T<State, Action, dual>;
    };

public:
    MDPHeuristicSearchBase(
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy);

    template <template <typename, typename, bool> class S, typename... Args>
    std::unique_ptr<MDPAlgorithm<State, Action>> create_search_algorithm(
        const SharedProbabilisticTask&,
        Args&&... args)
    {
        return construct<
            MDPAlgorithm<State, Action>,
            AlgTypeHelper<S>::template type_template>(
            std::forward_as_tuple(
                convergence_epsilon_,
                tiebreaker_,
                std::forward<Args>(args)...),
            dual_bounds_);
    }

    virtual std::string get_heuristic_search_name() const = 0;
};

template <
    bool Bisimulation,
    bool Fret,
    typename State = downward::State,
    typename Action = downward::OperatorID>
class MDPHeuristicSearch;

template <typename State, typename Action>
class MDPHeuristicSearch<false, false, State, Action>
    : public MDPHeuristicSearchBase<false, false, State, Action> {
    template <template <typename, typename, bool> typename T>
    struct AlgTypeHelper {
        template <bool dual>
        using type_template = T<State, Action, dual>;
    };

protected:
    using PolicyPicker =
        typename MDPHeuristicSearch::MDPHeuristicSearchBase::PolicyPicker;

public:
    MDPHeuristicSearch(
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy);

    std::string get_algorithm_name() const override;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<MDPAlgorithm<State, Action>>
    create_heuristic_search_algorithm(
        const SharedProbabilisticTask&,
        Args&&... args)
    {
        return construct<
            MDPAlgorithm<State, downward::OperatorID>,
            AlgTypeHelper<HS>::template type_template>(
            std::forward_as_tuple(
                this->convergence_epsilon_,
                this->tiebreaker_,
                std::forward<Args>(args)...),
            this->dual_bounds_);
    }
};

template <typename State, typename Action>
class MDPHeuristicSearch<false, true, State, Action>
    : public MDPHeuristicSearchBase<false, true, State, Action> {
    template <template <typename, typename, bool> typename T>
    struct AlgTypeHelper {
        using QState = quotients::QuotientState<State, Action>;
        using QAction = quotients::QuotientAction<Action>;

        template <bool dual, bool fret_on_policy>
        using type_template = std::conditional_t<
            fret_on_policy,
            algorithms::fret::FRETPi<T<QState, QAction, dual>>,
            algorithms::fret::FRETV<T<QState, QAction, dual>>>;
    };

protected:
    using PolicyPicker =
        typename MDPHeuristicSearch::MDPHeuristicSearchBase::PolicyPicker;

    const bool fret_on_policy_;

public:
    MDPHeuristicSearch(
        bool fret_on_policy,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy);

    std::string get_algorithm_name() const override;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<MDPAlgorithm<State, Action>>
    create_heuristic_search_algorithm(
        const SharedProbabilisticTask&,
        Args&&... args)
    {
        return construct<
            MDPAlgorithm<State, Action>,
            AlgTypeHelper<HS>::template type_template>(
            std::forward_as_tuple(
                this->convergence_epsilon_,
                this->tiebreaker_,
                std::forward<Args>(args)...),
            this->dual_bounds_,
            this->fret_on_policy_);
    }
};

template <bool Fret>
class MDPHeuristicSearch<true, Fret, downward::State, downward::OperatorID>
    : public MDPHeuristicSearch<
          false,
          Fret,
          bisimulation::QuotientState,
          downward::OperatorID> {
    using Base = MDPHeuristicSearch<
        false,
        Fret,
        bisimulation::QuotientState,
        downward::OperatorID>;

    using QState = bisimulation::QuotientState;
    using QAction = downward::OperatorID;

    using PolicyPicker = typename Base::PolicyPicker;

public:
    using Base::Base;

    std::string get_algorithm_name() const override;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> create_heuristic_search_algorithm(
        const SharedProbabilisticTask& task,
        Args&&... args)
    {
        return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
            task,
            this->get_heuristic_search_name(),
            Base::template create_heuristic_search_algorithm<HS>(
                task,
                std::forward<Args>(args)...));
    }
};

} // namespace probfd::solvers

#include "probfd/solvers/mdp_heuristic_search_impl.h"

#endif // PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H
