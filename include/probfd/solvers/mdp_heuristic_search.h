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

template <bool Bisimulation, bool Fret>
class MDPHeuristicSearch;

template <>
class MDPHeuristicSearch<false, false>
    : public MDPHeuristicSearchBase<false, false> {
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
        if (dual_bounds_) {
            using HeuristicSearchType = HS<State, OperatorID, true>;
            return std::make_unique<HeuristicSearchType>(
                convergence_epsilon_,
                tiebreaker_,
                std::forward<Args>(args)...);
        } else {
            using HeuristicSearchType = HS<State, OperatorID, false>;
            return std::make_unique<HeuristicSearchType>(
                convergence_epsilon_,
                tiebreaker_,
                std::forward<Args>(args)...);
        }
    }
};

template <>
class MDPHeuristicSearch<false, true>
    : public MDPHeuristicSearchBase<false, true> {
    using QState = quotients::QuotientState<State, OperatorID>;
    using QAction = quotients::QuotientAction<OperatorID>;

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
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&,
        Args&&... args)
    {
        if (this->dual_bounds_) {
            if (this->fret_on_policy_) {
                return this->template create_heuristic_search_algorithm_wrapper<
                    algorithms::fret::FRETPi,
                    HS,
                    true>(std::forward<Args>(args)...);
            } else {
                return this->template create_heuristic_search_algorithm_wrapper<
                    algorithms::fret::FRETV,
                    HS,
                    true>(std::forward<Args>(args)...);
            }
        } else {
            if (this->fret_on_policy_) {
                return this->template create_heuristic_search_algorithm_wrapper<
                    algorithms::fret::FRETPi,
                    HS,
                    false>(std::forward<Args>(args)...);
            } else {
                return this->template create_heuristic_search_algorithm_wrapper<
                    algorithms::fret::FRETV,
                    HS,
                    false>(std::forward<Args>(args)...);
            }
        }
    }

private:
    template <
        template <typename> class Fret,
        template <typename, typename, bool> class HS,
        bool Interval,
        typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    create_heuristic_search_algorithm_wrapper(Args&&... args)
    {
        return std::make_unique<
            Fret<HS<QState, QAction, Interval>>>(
            convergence_epsilon_,
            tiebreaker_,
            std::forward<Args>(args)...);
    }
};

template <>
class MDPHeuristicSearch<true, false>
    : public MDPHeuristicSearchBase<true, false> {
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
        using QState = bisimulation::QuotientState;
        using QAction = OperatorID;

        if (dual_bounds_) {
            return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
                task,
                task_cost_function,
                this->get_heuristic_search_name(),
                std::make_shared<HS<QState, QAction, true>>(
                    this->convergence_epsilon_,
                    this->tiebreaker_,
                    std::forward<Args>(args)...));
        } else {
            return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
                task,
                task_cost_function,
                this->get_heuristic_search_name(),
                std::make_shared<HS<QState, QAction, false>>(
                    this->convergence_epsilon_,
                    this->tiebreaker_,
                    std::forward<Args>(args)...));
        }
    }
};

template <>
class MDPHeuristicSearch<true, true>
    : public MDPHeuristicSearchBase<true, true> {
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
        if (this->dual_bounds_) {
            if (this->fret_on_policy_) {
                return this
                    ->template heuristic_search_algorithm_factory_wrapper<
                        algorithms::fret::FRETPi,
                        true,
                        HS>(
                        task,
                        task_cost_function,
                        std::forward<Args>(args)...);
            } else {
                return this
                    ->template heuristic_search_algorithm_factory_wrapper<
                        algorithms::fret::FRETV,
                        true,
                        HS>(
                        task,
                        task_cost_function,
                        std::forward<Args>(args)...);
            }
        } else {
            if (this->fret_on_policy_) {
                return this
                    ->template heuristic_search_algorithm_factory_wrapper<
                        algorithms::fret::FRETPi,
                        false,
                        HS>(
                        task,
                        task_cost_function,
                        std::forward<Args>(args)...);
            } else {
                return this
                    ->template heuristic_search_algorithm_factory_wrapper<
                        algorithms::fret::FRETV,
                        false,
                        HS>(
                        task,
                        task_cost_function,
                        std::forward<Args>(args)...);
            }
        }
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> create_quotient_heuristic_search_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        Args&&... args)
    {
        using QState = bisimulation::QuotientState;
        using QAction = OperatorID;

        if (dual_bounds_) {
            return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
                task,
                task_cost_function,
                this->get_heuristic_search_name(),
                std::make_shared<HS<QState, QAction, true>>(
                    this->convergence_epsilon_,
                    this->tiebreaker_,
                    std::forward<Args>(args)...));
        } else {
            return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
                task,
                task_cost_function,
                this->get_heuristic_search_name(),
                std::make_shared<HS<QState, QAction, false>>(
                    this->convergence_epsilon_,
                    this->tiebreaker_,
                    std::forward<Args>(args)...));
        }
    }

private:
    template <
        template <typename> class Fret,
        bool Interval,
        template <typename, typename, bool> class HS,
        typename... Args>
    std::unique_ptr<FDRMDPAlgorithm> heuristic_search_algorithm_factory_wrapper(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        Args&&... args)
    {
        using QState = bisimulation::QuotientState;
        using QAction = OperatorID;

        using QQState = quotients::QuotientState<QState, QAction>;
        using QQAction = quotients::QuotientAction<QAction>;

        return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
            task,
            task_cost_function,
            this->get_heuristic_search_name(),
            std::make_shared<
                Fret<HS<QQState, QQAction, Interval>>>(
                this->convergence_epsilon_,
                this->tiebreaker_,
                std::forward<Args>(args)...));
    }
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H
