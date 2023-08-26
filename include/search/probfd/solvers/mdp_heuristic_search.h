#ifndef PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/policy_picker.h"
#include "probfd/algorithms/successor_sampler.h"

#include "probfd/algorithms/fret.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/solvers/bisimulation_heuristic_search_algorithm.h"

#include "downward/plugins/plugin.h"

#include <memory>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace solvers {

class MDPHeuristicSearchBase : public MDPSolver {
protected:
    const bool dual_bounds_;
    const bool interval_comparison_;

public:
    explicit MDPHeuristicSearchBase(const plugins::Options& opts);

    static void add_options_to_feature(plugins::Feature& feature);

    std::string get_algorithm_name() const override;

    virtual std::string get_heuristic_search_name() const = 0;
};

template <bool Bisimulation, bool Fret>
class MDPHeuristicSearch;

template <>
class MDPHeuristicSearch<false, false> : public MDPHeuristicSearchBase {
    std::shared_ptr<FDRPolicyPicker> tiebreaker_;

public:
    explicit MDPHeuristicSearch(const plugins::Options& opts);

    static void add_options_to_feature(plugins::Feature& feature);

    void print_additional_statistics() const override;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    create_heuristic_search_algorithm(Args&&... args)
    {
        if (dual_bounds_) {
            using HeuristicSearchType = HS<State, OperatorID, true>;
            return algorithm_factory<HeuristicSearchType>(
                tiebreaker_,
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...);
        } else {
            using HeuristicSearchType = HS<State, OperatorID, false>;
            return algorithm_factory<HeuristicSearchType>(
                tiebreaker_,
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...);
        }
    }
};

template <>
class MDPHeuristicSearch<false, true> : public MDPHeuristicSearchBase {
    using QState = quotients::QuotientState<State, OperatorID>;
    using QAction = quotients::QuotientAction<OperatorID>;

    std::shared_ptr<algorithms::PolicyPicker<QState, QAction>> tiebreaker_;

    const bool fret_on_policy_;

public:
    explicit MDPHeuristicSearch(const plugins::Options& opts);

    static void add_options_to_feature(plugins::Feature& feature);

    void print_additional_statistics() const override;

    std::string get_algorithm_name() const override
    {
        std::ostringstream out;
        out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
            << this->get_heuristic_search_name() << ")";
        return out.str();
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    create_heuristic_search_algorithm(Args&&... args)
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

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    create_quotient_heuristic_search_algorithm(Args&&... args)
    {
        if (dual_bounds_) {
            return std::make_unique<HS<State, OperatorID, true>>(
                tiebreaker_,
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...);
        } else {
            return std::make_unique<HS<State, OperatorID, false>>(
                tiebreaker_,
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...);
        }
    }

private:
    template <
        template <typename, typename, bool>
        class Fret,
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    create_heuristic_search_algorithm_wrapper(Args&&... args)
    {
        return std::make_unique<Fret<State, OperatorID, Interval>>(
            &progress_,
            std::make_shared<HS<QState, QAction, Interval>>(
                tiebreaker_,
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...));
    }
};

template <>
class MDPHeuristicSearch<true, false> : public MDPHeuristicSearchBase {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;

    std::shared_ptr<algorithms::PolicyPicker<QState, QAction>> tiebreaker_;

public:
    explicit MDPHeuristicSearch(const plugins::Options& opts);

    static void add_options_to_feature(plugins::Feature& feature);

    void print_additional_statistics() const override;

    std::string get_algorithm_name() const override
    {
        return get_heuristic_search_name() + "(bisimulation)";
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    create_heuristic_search_algorithm(Args&&... args)
    {
        if (dual_bounds_) {
            return BisimulationBasedHeuristicSearchAlgorithm::create<HS, true>(
                this->task,
                this->task_cost_function,
                this->get_heuristic_search_name(),
                this->progress_,
                this->interval_comparison_,
                this->tiebreaker_,
                std::forward<Args>(args)...);
        } else {
            return BisimulationBasedHeuristicSearchAlgorithm::create<HS, false>(
                this->task,
                this->task_cost_function,
                this->get_heuristic_search_name(),
                this->progress_,
                this->interval_comparison_,
                this->tiebreaker_,
                std::forward<Args>(args)...);
        }
    }
};

template <>
class MDPHeuristicSearch<true, true> : public MDPHeuristicSearchBase {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQState = quotients::QuotientState<QState, QAction>;
    using QQAction = quotients::QuotientAction<QAction>;

    std::shared_ptr<algorithms::PolicyPicker<QQState, QQAction>> tiebreaker_;

    const bool fret_on_policy_;

public:
    explicit MDPHeuristicSearch(const plugins::Options& opts);

    static void add_options_to_feature(plugins::Feature& feature);

    void print_additional_statistics() const override;

    std::string get_algorithm_name() const override
    {
        std::ostringstream out;
        out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
            << this->get_heuristic_search_name() << "(bisimulation)"
            << ")";
        return out.str();
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    create_heuristic_search_algorithm(Args&&... args)
    {
        if (this->dual_bounds_) {
            if (this->fret_on_policy_) {
                return this
                    ->template heuristic_search_algorithm_factory_wrapper<
                        algorithms::fret::FRETPi,
                        true,
                        HS>(std::forward<Args>(args)...);
            } else {
                return this
                    ->template heuristic_search_algorithm_factory_wrapper<
                        algorithms::fret::FRETV,
                        true,
                        HS>(std::forward<Args>(args)...);
            }
        } else {
            if (this->fret_on_policy_) {
                return this
                    ->template heuristic_search_algorithm_factory_wrapper<
                        algorithms::fret::FRETPi,
                        false,
                        HS>(std::forward<Args>(args)...);
            } else {
                return this
                    ->template heuristic_search_algorithm_factory_wrapper<
                        algorithms::fret::FRETV,
                        false,
                        HS>(std::forward<Args>(args)...);
            }
        }
    }

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    create_quotient_heuristic_search_algorithm(Args&&... args)
    {
        if (dual_bounds_) {
            return BisimulationBasedHeuristicSearchAlgorithm::create<HS, true>(
                this->task,
                this->task_cost_function,
                this->get_heuristic_search_name(),
                this->progress_,
                this->interval_comparison_,
                this->tiebreaker_,
                std::forward<Args>(args)...);
        } else {
            return BisimulationBasedHeuristicSearchAlgorithm::create<HS, false>(
                this->task,
                this->task_cost_function,
                this->get_heuristic_search_name(),
                this->progress_,
                this->interval_comparison_,
                this->tiebreaker_,
                std::forward<Args>(args)...);
        }
    }

private:
    template <
        template <typename, typename, bool>
        class Fret,
        bool Interval,
        template <typename, typename, bool>
        class HS,
        typename... Args>
    std::unique_ptr<FDRMDPAlgorithm>
    heuristic_search_algorithm_factory_wrapper(Args&&... args)
    {
        return BisimulationBasedHeuristicSearchAlgorithm::
            create<Fret, HS, Interval>(
                this->task,
                this->task_cost_function,
                this->get_heuristic_search_name(),
                this->progress_,
                this->interval_comparison_,
                this->tiebreaker_,
                std::forward<Args>(args)...);
    }
};

template <template <bool> class SolverClass, bool Bisimulation>
class MDPHeuristicSearchSolverFeature
    : public plugins::TypedFeature<SolverInterface, SolverClass<Bisimulation>> {
public:
    MDPHeuristicSearchSolverFeature(const std::string& name)
        : MDPHeuristicSearchSolverFeature::TypedFeature(name)
    {
        MDPHeuristicSearchBase::add_options_to_feature(*this);
    }

    std::shared_ptr<SolverClass<Bisimulation>>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return std::make_shared<SolverClass<Bisimulation>>(options);
    }
};

template <template <bool, bool> class SolverClass, bool Bisimulation, bool Fret>
class MDPFRETHeuristicSearchSolverFeature
    : public plugins::
          TypedFeature<SolverInterface, SolverClass<Bisimulation, Fret>> {
public:
    MDPFRETHeuristicSearchSolverFeature(const std::string& name)
        : MDPFRETHeuristicSearchSolverFeature::TypedFeature(name)
    {
        MDPHeuristicSearch<Bisimulation, Fret>::add_options_to_feature(*this);

        if constexpr (Fret) {
            this->add_option<bool>("fret_on_policy", "", "true");
        }
    }

    std::shared_ptr<SolverClass<Bisimulation, Fret>>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return std::make_shared<SolverClass<Bisimulation, Fret>>(options);
    }
};

} // namespace solvers
} // namespace probfd

#endif // __MDP_HEURISTIC_SEARCH_H__
