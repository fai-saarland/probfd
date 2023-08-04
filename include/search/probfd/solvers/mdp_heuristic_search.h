#ifndef PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/successor_sampler.h"

#include "probfd/engines/fret.h"

#include "probfd/quotients/heuristic_search_interface.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/solvers/bisimulation_heuristic_search_engine.h"
#include "probfd/solvers/state_space_interface_wrappers.h"

#include "downward/plugins/plugin.h"

#include <memory>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace solvers {

enum class FretMode { DISABLED, POLICY, VALUE };

class MDPHeuristicSearchBase : public MDPSolver {
protected:
    std::shared_ptr<TaskPolicyPicker> policy_tiebreaker_;
    std::shared_ptr<TaskNewStateObserver> new_state_handler_;

    const bool dual_bounds_;
    const bool interval_comparison_;

public:
    explicit MDPHeuristicSearchBase(const plugins::Options& opts);

    static void add_options_to_feature(plugins::Feature& feature);

    virtual std::string get_engine_name() const override;
    virtual std::string get_heuristic_search_name() const = 0;
    virtual void print_additional_statistics() const override;
};

template <bool Bisimulation, bool Fret>
class MDPHeuristicSearch;

template <>
class MDPHeuristicSearch<false, false> : public MDPHeuristicSearchBase {
public:
    using MDPHeuristicSearchBase::MDPHeuristicSearchBase;

    using MDPHeuristicSearchBase::add_options_to_feature;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<TaskMDPEngine>
    create_heuristic_search_engine(Args&&... args)
    {
        if (dual_bounds_) {
            using HeuristicSearchType = HS<State, OperatorID, true>;
            return engine_factory<HeuristicSearchType>(
                policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...);
        } else {
            using HeuristicSearchType = HS<State, OperatorID, false>;
            return engine_factory<HeuristicSearchType>(
                policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...);
        }
    }

protected:
    template <typename T>
    using WrappedType = typename Wrapper<false, false, T>::type;

    template <typename T>
    T wrap(T t)
    {
        return t;
    }
};

template <>
class MDPHeuristicSearch<false, true> : public MDPHeuristicSearchBase {
    using QAction = quotients::QuotientAction<OperatorID>;

    std::shared_ptr<engine_interfaces::PolicyPicker<State, QAction>>
        q_policy_tiebreaker_;

    const bool fret_on_policy_;

public:
    explicit MDPHeuristicSearch(const plugins::Options& opts)
        : MDPHeuristicSearchBase(opts)
        , q_policy_tiebreaker_(
              this->policy_tiebreaker_ != nullptr
                  ? new quotients::RepresentativePolicyPicker<State>(
                        this->policy_tiebreaker_)
                  : nullptr)
        , fret_on_policy_(opts.get<bool>("fret_on_policy", false))
    {
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<TaskMDPEngine>
    create_heuristic_search_engine(Args&&... args)
    {
        if (this->dual_bounds_) {
            if (this->fret_on_policy_) {
                return this->template create_heuristic_search_engine_wrapper<
                    engines::fret::FRETPi,
                    HS,
                    true>(std::forward<Args>(args)...);
            } else {
                return this->template create_heuristic_search_engine_wrapper<
                    engines::fret::FRETV,
                    HS,
                    true>(std::forward<Args>(args)...);
            }
        } else {
            if (this->fret_on_policy_) {
                return this->template create_heuristic_search_engine_wrapper<
                    engines::fret::FRETPi,
                    HS,
                    false>(std::forward<Args>(args)...);
            } else {
                return this->template create_heuristic_search_engine_wrapper<
                    engines::fret::FRETV,
                    HS,
                    false>(std::forward<Args>(args)...);
            }
        }
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<TaskMDPEngine>
    create_quotient_heuristic_search_engine(Args&&... args)
    {
        if (dual_bounds_) {
            return std::make_unique<HS<State, OperatorID, true>>(
                q_policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...);
        } else {
            return std::make_unique<HS<State, OperatorID, false>>(
                q_policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                std::forward<Args>(args)...);
        }
    }

    virtual std::string get_engine_name() const override
    {
        std::ostringstream out;
        out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
            << this->get_heuristic_search_name() << ")";
        return out.str();
    }

protected:
    template <typename T>
    using WrappedType = typename Wrapper<false, true, T>::type;

    template <typename T>
    typename Wrapper<false, true, T>::type wrap(T t)
    {
        return Wrapper<false, true, T>()(t);
    }

private:
    template <
        template <typename, typename, bool>
        class Fret,
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    std::unique_ptr<TaskMDPEngine>
    create_heuristic_search_engine_wrapper(Args&&... args)
    {
        std::shared_ptr engine = std::make_shared<HS<State, QAction, Interval>>(
            q_policy_tiebreaker_.get(),
            new_state_handler_.get(),
            &progress_,
            interval_comparison_,
            std::forward<Args>(args)...);
        return std::make_unique<Fret<State, OperatorID, Interval>>(
            &progress_,
            std::move(engine));
    }
};

template <>
class MDPHeuristicSearch<true, false> : public MDPHeuristicSearchBase {
public:
    using MDPHeuristicSearchBase::add_options_to_feature;
    using MDPHeuristicSearchBase::MDPHeuristicSearchBase;

    virtual std::string get_engine_name() const override
    {
        return get_heuristic_search_name() + "(bisimulation)";
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<TaskMDPEngine>
    create_heuristic_search_engine(Args&&... args)
    {
        if (dual_bounds_) {
            return BisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, true>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    std::forward<Args>(args)...);
        } else {
            return BisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, false>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    std::forward<Args>(args)...);
        }
    }

protected:
    template <typename T>
    using WrappedType = typename Wrapper<true, false, T>::type;

    template <typename T>
    WrappedType<T> wrap(T t) const
    {
        return Wrapper<true, false, T>()(t);
    }
};

template <>
class MDPHeuristicSearch<true, true> : public MDPHeuristicSearchBase {
    const bool fret_on_policy_;

public:
    explicit MDPHeuristicSearch(const plugins::Options& opts)
        : MDPHeuristicSearchBase(opts)
        , fret_on_policy_(opts.get<bool>("fret_on_policy"))
    {
    }

    using MDPHeuristicSearchBase::add_options_to_feature;

    template <template <typename, typename, bool> class HS, typename... Args>
    std::unique_ptr<TaskMDPEngine>
    create_heuristic_search_engine(Args&&... args)
    {
        if (this->dual_bounds_) {
            if (this->fret_on_policy_) {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETPi,
                    true,
                    HS>(std::forward<Args>(args)...);
            } else {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETV,
                    true,
                    HS>(std::forward<Args>(args)...);
            }
        } else {
            if (this->fret_on_policy_) {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETPi,
                    false,
                    HS>(std::forward<Args>(args)...);
            } else {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETV,
                    false,
                    HS>(std::forward<Args>(args)...);
            }
        }
    }

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    std::unique_ptr<TaskMDPEngine>
    create_quotient_heuristic_search_engine(Args&&... args)
    {
        if (dual_bounds_) {
            return QBisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, true>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    std::forward<Args>(args)...);
        } else {
            return QBisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, false>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    std::forward<Args>(args)...);
        }
    }

    virtual std::string get_engine_name() const override
    {
        std::ostringstream out;
        out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
            << this->get_heuristic_search_name() << "(bisimulation)"
            << ")";
        return out.str();
    }

protected:
    template <typename T>
    using WrappedType = typename Wrapper<true, true, T>::type;

    template <typename T>
    auto wrap(T t) const
    {
        return Wrapper<true, true, T>()(t);
    }

private:
    template <
        template <typename, typename, bool>
        class Fret,
        bool Interval,
        template <typename, typename, bool>
        class HS,
        typename... Args>
    std::unique_ptr<TaskMDPEngine>
    heuristic_search_engine_factory_wrapper(Args&&... args)
    {
        return QBisimulationBasedHeuristicSearchEngine::
            template Constructor<Fret, HS, Interval>(
                this->get_heuristic_search_name(),
                this->progress_,
                this->interval_comparison_,
                std::forward<Args>(args)...);
    }
};

template <template <bool> class SolverClass>
class MDPHeuristicSearchSolverFeature
    : public plugins::TypedFeature<SolverInterface, SolverInterface> {
public:
    MDPHeuristicSearchSolverFeature(const std::string& name)
        : TypedFeature(name)
    {
        MDPHeuristicSearchBase::add_options_to_feature(*this);
        add_option<bool>("bisimulation", "", "false");
    }

    std::shared_ptr<SolverInterface>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        if (options.get<bool>("bisimulation")) {
            return std::make_shared<SolverClass<true>>(options);
        } else {
            return std::make_shared<SolverClass<false>>(options);
        }
    }
};

template <template <bool, bool> class SolverClass>
class MDPFRETHeuristicSearchSolverFeature
    : public plugins::TypedFeature<SolverInterface, SolverInterface> {
public:
    MDPFRETHeuristicSearchSolverFeature(const std::string& name)
        : TypedFeature(name)
    {
        MDPHeuristicSearchBase::add_options_to_feature(*this);
        add_option<FretMode>("fret", "", "disabled");
        add_option<bool>("bisimulation", "", "false");
    }

    std::shared_ptr<SolverInterface>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        const int fret_type = options.get<int>("fret");
        if (options.get<bool>("bisimulation")) {
            if (fret_type == 0) {
                return std::make_shared<SolverClass<true, false>>(options);
            } else {
                plugins::Options options_copy(options);
                options_copy.set<bool>("fret_on_policy", fret_type == 1);
                return std::make_shared<SolverClass<true, true>>(options_copy);
            }
        } else {
            if (fret_type == 0) {
                return std::make_shared<SolverClass<false, false>>(options);
            } else {
                plugins::Options options_copy(options);
                options_copy.set<bool>("fret_on_policy", fret_type == 1);
                return std::make_shared<SolverClass<false, true>>(options_copy);
            }
        }
    }
};

} // namespace solvers
} // namespace probfd

#endif // __MDP_HEURISTIC_SEARCH_H__
