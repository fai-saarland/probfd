#ifndef PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/engine_interfaces/new_state_handler.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/engines/fret.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/heuristic_search_interface.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/quotient_system.h"

#include "probfd/solvers/bisimulation_heuristic_search_engine.h"
#include "probfd/solvers/state_space_interface_wrappers.h"

#include "option_parser.h"

#include <memory>

namespace probfd {
namespace solvers {

class MDPHeuristicSearchBase : public MDPSolver {
protected:
    std::shared_ptr<TaskEvaluator> heuristic_;
    std::shared_ptr<TaskPolicyPicker> policy_tiebreaker_;
    std::shared_ptr<TaskNewStateHandler> new_state_handler_;

    const bool dual_bounds_;
    const bool interval_comparison_;
    const bool stable_policy_;

public:
    explicit MDPHeuristicSearchBase(const options::Options& opts);

    static void add_options_to_parser(options::OptionParser& parser);

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

    using MDPHeuristicSearchBase::add_options_to_parser;

    template <template <typename, typename, bool> class HS, typename... Args>
    engines::MDPEngine<State, OperatorID>*
    create_heuristic_search_engine(Args&&... args)
    {
        if (dual_bounds_) {
            using HeuristicSearchType = HS<State, OperatorID, true>;
            return engine_factory<HeuristicSearchType>(
                heuristic_.get(),
                policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                stable_policy_,
                std::forward<Args>(args)...);
        } else {
            using HeuristicSearchType = HS<State, OperatorID, false>;
            return engine_factory<HeuristicSearchType>(
                heuristic_.get(),
                policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                stable_policy_,
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

    quotients::QuotientSystem<OperatorID> quotient_;

    engine_interfaces::ActionIDMap<QAction> q_action_id_map_;
    engine_interfaces::TransitionGenerator<QAction> q_transition_gen_;

    std::shared_ptr<engine_interfaces::CostFunction<State, QAction>> q_cost_;
    std::shared_ptr<engine_interfaces::PolicyPicker<QAction>>
        q_policy_tiebreaker_;

    const bool fret_on_policy_;

public:
    explicit MDPHeuristicSearch(const options::Options& opts)
        : MDPHeuristicSearchBase(opts)
        , quotient_(&this->action_id_map_, &this->transition_generator_)
        , q_action_id_map_(&quotient_)
        , q_transition_gen_(&quotient_)
        , q_cost_(new quotients::DefaultQuotientCostFunction<State, OperatorID>(
              &quotient_,
              this->cost_function_))
        , q_policy_tiebreaker_(
              this->policy_tiebreaker_ != nullptr
                  ? new quotients::RepresentativePolicyPicker(
                        &quotient_,
                        this->policy_tiebreaker_)
                  : nullptr)
        , fret_on_policy_(
              opts.contains("fret_on_policy") &&
              opts.get<bool>("fret_on_policy"))
    {
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    engines::MDPEngine<State, OperatorID>*
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
    engines::MDPEngine<State, QAction>*
    create_quotient_heuristic_search_engine(Args&&... args)
    {
        if (dual_bounds_) {
            return new HS<State, QAction, true>(
                &this->state_id_map_,
                &q_action_id_map_,
                &q_transition_gen_,
                q_cost_.get(),
                heuristic_.get(),
                q_policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                stable_policy_,
                &this->quotient_,
                std::forward<Args>(args)...);
        } else {
            return new HS<State, QAction, false>(
                &this->state_id_map_,
                &q_action_id_map_,
                &q_transition_gen_,
                q_cost_.get(),
                heuristic_.get(),
                q_policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                stable_policy_,
                &this->quotient_,
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
        return Wrapper<false, true, T>()(&quotient_, t);
    }

private:
    template <
        template <typename, typename, bool>
        class Fret,
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    engines::MDPEngine<State, OperatorID>*
    create_heuristic_search_engine_wrapper(Args&&... args)
    {
        std::shared_ptr<HS<State, QAction, Interval>> engine(
            new HS<State, QAction, Interval>(
                &this->state_id_map_,
                &q_action_id_map_,
                &q_transition_gen_,
                q_cost_.get(),
                heuristic_.get(),
                q_policy_tiebreaker_.get(),
                new_state_handler_.get(),
                &progress_,
                interval_comparison_,
                stable_policy_,
                std::forward<Args>(args)...));
        return new Fret<State, OperatorID, Interval>(
            &this->state_id_map_,
            &this->action_id_map_,
            &this->transition_generator_,
            this->cost_function_,
            &quotient_,
            &progress_,
            std::move(engine));
    }
};

template <>
class MDPHeuristicSearch<true, false> : public MDPHeuristicSearchBase {
public:
    using MDPHeuristicSearchBase::add_options_to_parser;
    using MDPHeuristicSearchBase::MDPHeuristicSearchBase;

    virtual std::string get_engine_name() const override
    {
        return get_heuristic_search_name() + "(bisimulation)";
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    engines::MDPEngineInterface<State>*
    create_heuristic_search_engine(Args&&... args)
    {
        if (dual_bounds_) {
            return BisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, true>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
                    std::forward<Args>(args)...);
        } else {
            return BisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, false>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
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
    explicit MDPHeuristicSearch(const options::Options& opts)
        : MDPHeuristicSearchBase(opts)
        , fret_on_policy_(opts.get<bool>("fret_on_policy"))
    {
    }

    using MDPHeuristicSearchBase::add_options_to_parser;

    template <template <typename, typename, bool> class HS, typename... Args>
    engines::MDPEngineInterface<State>*
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
    engines::MDPEngineInterface<State>*
    create_quotient_heuristic_search_engine(Args&&... args)
    {
        if (dual_bounds_) {
            return QBisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, true>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
                    std::forward<Args>(args)...);
        } else {
            return QBisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, false>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
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
    engines::MDPEngineInterface<State>*
    heuristic_search_engine_factory_wrapper(Args&&... args)
    {
        return QBisimulationBasedHeuristicSearchEngine::
            template Constructor<Fret, HS, Interval>(
                this->get_heuristic_search_name(),
                this->progress_,
                this->interval_comparison_,
                this->stable_policy_,
                std::forward<Args>(args)...);
    }
};

struct NoAdditionalOptions {
    void operator()(options::OptionParser&) const {}
};

struct NoOptionsPostprocessing {
    void operator()(options::Options&) const {}
};

template <
    template <bool>
    class SolverClass,
    typename AddOptions = NoAdditionalOptions,
    typename OptionsPostprocessing = NoOptionsPostprocessing>
std::shared_ptr<SolverInterface>
parse_mdp_heuristic_search_solver(options::OptionParser& parser)
{
    MDPHeuristicSearchBase::add_options_to_parser(parser);

    parser.add_option<bool>("bisimulation", "", "false");

    AddOptions()(parser);

    options::Options opts = parser.parse();

    OptionsPostprocessing()(opts);

    if (parser.dry_run()) {
        return nullptr;
    }

    if (opts.get<bool>("bisimulation")) {
        return std::make_shared<SolverClass<true>>(opts);
    } else {
        return std::make_shared<SolverClass<false>>(opts);
    }
}

template <
    template <bool, bool>
    class SolverClass,
    typename AddOptions = NoAdditionalOptions,
    typename OptionsPostprocessing = NoOptionsPostprocessing>
std::shared_ptr<SolverInterface>
parse_mdp_heuristic_search_solver(options::OptionParser& parser)
{
    MDPHeuristicSearchBase::add_options_to_parser(parser);

    std::vector<std::string> fret_types({"disabled", "policy", "value"});
    parser.add_enum_option<int>("fret", fret_types, "", "disabled");

    parser.add_option<bool>("bisimulation", "", "false");

    AddOptions()(parser);

    options::Options opts = parser.parse();

    OptionsPostprocessing()(opts);

    if (parser.dry_run()) {
        return nullptr;
    }

    const int fret_type = opts.get<int>("fret");
    if (opts.get<bool>("bisimulation")) {
        if (fret_type == 0) {
            return std::make_shared<SolverClass<true, false>>(opts);
        } else {
            opts.set<bool>("fret_on_policy", fret_type == 1);
            return std::make_shared<SolverClass<true, true>>(opts);
        }
    } else {
        if (fret_type == 0) {
            return std::make_shared<SolverClass<false, false>>(opts);
        } else {
            opts.set<bool>("fret_on_policy", fret_type == 1);
            return std::make_shared<SolverClass<false, true>>(opts);
        }
    }
}

} // namespace solvers
} // namespace probfd

#endif // __MDP_HEURISTIC_SEARCH_H__
