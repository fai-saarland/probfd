#ifndef MDPS_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define MDPS_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/engine_interfaces/heuristic_search_connector.h"
#include "probfd/engine_interfaces/new_state_handler.h"
#include "probfd/engine_interfaces/transition_sampler.h"
#include "probfd/engine_interfaces/policy_picker.h"

#include "probfd/engines/fret.h"

#include "probfd/quotient_system/engine_interfaces.h"
#include "probfd/quotient_system/heuristic_search_interface.h"
#include "probfd/quotient_system/quotient_system.h"

#include "probfd/quotient_system.h"

#include "option_parser.h"
#include "state_space_interface_wrappers.h"

#include <memory>

namespace probfd {
namespace solvers {

template <typename Bisimulation, typename Fret>
class MDPHeuristicSearch;

class MDPHeuristicSearchBase : public MDPSolver {
    friend class MDPHeuristicSearch<std::false_type, std::true_type>;
    friend class MDPHeuristicSearch<std::false_type, std::false_type>;
    friend class MDPHeuristicSearch<std::true_type, std::true_type>;
    friend class MDPHeuristicSearch<std::true_type, std::false_type>;

public:
    explicit MDPHeuristicSearchBase(const options::Options& opts);

    static void add_options_to_parser(options::OptionParser& parser);

    virtual std::string get_engine_name() const override;
    virtual std::string get_heuristic_search_name() const = 0;
    virtual void print_additional_statistics() const override;

protected:
    engine_interfaces::HeuristicSearchConnector connector_;

    std::shared_ptr<TaskPolicyPicker> policy_tiebreaker_;
    std::shared_ptr<TaskNewStateHandler> new_state_handler_;
    std::shared_ptr<TaskStateEvaluator> heuristic_;

    const bool dual_bounds_;
    const bool interval_comparison_;
    const bool stable_policy_;
};

template <typename Bisimulation>
class MDPHeuristicSearch<Bisimulation, std::false_type>
    : public MDPHeuristicSearchBase {
public:
    using MDPHeuristicSearchBase::add_options_to_parser;
    using MDPHeuristicSearchBase::MDPHeuristicSearchBase;

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngine<State, OperatorID>*
    heuristic_search_engine_factory(Args&&... args)
    {
        if (dual_bounds_) {
            using HeuristicSearchType = HS<State, OperatorID, std::true_type>;
            return engine_factory<HeuristicSearchType>(
                policy_tiebreaker_.get(),
                new_state_handler_.get(),
                heuristic_.get(),
                &connector_,
                &progress_,
                interval_comparison_,
                stable_policy_,
                std::forward<Args>(args)...);
        } else {
            using HeuristicSearchType = HS<State, OperatorID, std::false_type>;
            return engine_factory<HeuristicSearchType>(
                policy_tiebreaker_.get(),
                new_state_handler_.get(),
                heuristic_.get(),
                &connector_,
                &progress_,
                interval_comparison_,
                stable_policy_,
                std::forward<Args>(args)...);
        }
    }

protected:
    template <typename T>
    using WrappedType =
        typename Wrapper<std::false_type, std::false_type, T>::type;

    template <typename T>
    T wrap(T t) const
    {
        return t;
    }

    template <typename T>
    T unwrap(T t) const
    {
        return t;
    }
};

template <typename Bisimulation>
class MDPHeuristicSearch<Bisimulation, std::true_type>
    : public MDPHeuristicSearchBase {
    using QAction = quotient_system::QuotientAction<OperatorID>;

public:
    explicit MDPHeuristicSearch(const options::Options& opts)
        : MDPHeuristicSearchBase(opts)
        , quotient_(new quotient_system::QuotientSystem<OperatorID>(
              this->get_action_id_map(),
              this->get_transition_generator()))
        , q_reward_(new quotient_system::DefaultQuotientRewardFunction<
                    State,
                    OperatorID>(quotient_.get(), this->get_reward_function()))
        , q_action_id_map_(
              new engine_interfaces::ActionIDMap<QAction>(quotient_.get()))
        , q_transition_gen_(new engine_interfaces::TransitionGenerator<QAction>(
              quotient_.get()))
        , q_policy_tiebreaker_(
              this->policy_tiebreaker_ != nullptr
                  ? new engine_interfaces::PolicyPicker<QAction>(
                        quotient_.get(),
                        this->policy_tiebreaker_)
                  : nullptr)
        , fret_on_policy_(
              opts.contains("fret_on_policy") &&
              opts.get<bool>("fret_on_policy"))
        , opts_(opts)
        , engine_(nullptr)
    {
    }

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngine<State, OperatorID>*
    heuristic_search_engine_factory(Args... args)
    {
        if (this->dual_bounds_) {
            if (this->fret_on_policy_) {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETPi,
                    std::true_type,
                    HS>(args...);
            } else {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETV,
                    std::true_type,
                    HS>(args...);
            }
        } else {
            if (this->fret_on_policy_) {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETPi,
                    std::false_type,
                    HS>(args...);
            } else {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETV,
                    std::false_type,
                    HS>(args...);
            }
        }
    }

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngine<State, QAction>*
    quotient_heuristic_search_factory(Args... args)
    {
        if (dual_bounds_) {
            return new HS<State, QAction, std::true_type>(
                this->get_state_id_map(),
                q_action_id_map_.get(),
                q_reward_.get(),
                q_transition_gen_.get(),
                q_policy_tiebreaker_.get(),
                new_state_handler_.get(),
                heuristic_.get(),
                &connector_,
                &progress_,
                interval_comparison_,
                stable_policy_,
                this->quotient_.get(),
                args...);
        } else {
            return new HS<State, QAction, std::false_type>(
                this->get_state_id_map(),
                q_action_id_map_.get(),
                q_reward_.get(),
                q_transition_gen_.get(),
                q_policy_tiebreaker_.get(),
                new_state_handler_.get(),
                heuristic_.get(),
                &connector_,
                &progress_,
                interval_comparison_,
                stable_policy_,
                this->quotient_.get(),
                args...);
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
    using WrappedType =
        typename Wrapper<std::false_type, std::true_type, T>::type;

    template <typename T>
    typename Wrapper<std::false_type, std::true_type, T>::type wrap(T t) const
    {
        return Wrapper<std::false_type, std::true_type, T>()(
            quotient_.get(),
            t);
    }

    template <typename T>
    typename Unwrapper<std::false_type, std::true_type, T>::type
    unwrap(T t) const
    {
        return Unwrapper<std::false_type, std::true_type, T>()(t);
    }

    quotient_system::QuotientSystem<OperatorID>* get_quotient_system() const
    {
        return quotient_.get();
    }

private:
    template <
        template <typename, typename, typename>
        class HS,
        typename Bounds,
        typename... Args>
    engines::fret::HeuristicSearchEngine<State, QAction, Bounds>*
    quotient_heuristic_search_factory_wrapper(Args... args)
    {
        return new HS<State, QAction, Bounds>(
            this->get_state_id_map(),
            q_action_id_map_.get(),
            q_reward_.get(),
            q_transition_gen_.get(),
            q_policy_tiebreaker_.get(),
            new_state_handler_.get(),
            heuristic_.get(),
            &connector_,
            &progress_,
            interval_comparison_,
            stable_policy_,
            args...);
    }

    template <
        template <typename, typename, typename>
        class Fret,
        typename Bounds,
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngine<State, OperatorID>*
    heuristic_search_engine_factory_wrapper(Args... args)
    {
        using FretVariant = Fret<State, OperatorID, Bounds>;
        engines::fret::HeuristicSearchEngine<State, QAction, Bounds>* engine =
            this->template quotient_heuristic_search_factory_wrapper<
                HS,
                Bounds>(args...);
        engine_ = std::unique_ptr<engines::MDPEngine<State, QAction>>(engine);
        return new FretVariant(
            this->get_state_id_map(),
            this->get_action_id_map(),
            this->get_reward_function(),
            this->get_transition_generator(),
            quotient_.get(),
            &progress_,
            engine);
    }

    std::unique_ptr<quotient_system::QuotientSystem<OperatorID>> quotient_;

    std::unique_ptr<engine_interfaces::RewardFunction<State, QAction>>
        q_reward_;
    std::unique_ptr<engine_interfaces::ActionIDMap<QAction>> q_action_id_map_;
    std::unique_ptr<engine_interfaces::TransitionGenerator<QAction>>
        q_transition_gen_;

    std::unique_ptr<engine_interfaces::PolicyPicker<QAction>>
        q_policy_tiebreaker_;

    const bool fret_on_policy_;

    options::Options opts_; // keep copy

    std::unique_ptr<engines::MDPEngine<State, QAction>> engine_;
};

struct NoAdditionalOptions {
    void operator()(options::OptionParser&) const {}
};

struct NoOptionsPostprocessing {
    void operator()(options::Options&) const {}
};

template <
    template <typename>
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

    if (!parser.dry_run()) {
        if (opts.get<bool>("bisimulation")) {
            return std::make_shared<SolverClass<std::true_type>>(opts);
        } else {
            return std::make_shared<SolverClass<std::false_type>>(opts);
        }
    }

    return nullptr;
}

template <
    template <typename, typename>
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

    if (!parser.dry_run()) {
        const int fret_type = opts.get<int>("fret");
        if (opts.get<bool>("bisimulation")) {
            if (fret_type == 0) {
                return std::make_shared<
                    SolverClass<std::true_type, std::false_type>>(opts);
            } else {
                opts.set<bool>("fret_on_policy", fret_type == 1);
                return std::make_shared<
                    SolverClass<std::true_type, std::true_type>>(opts);
            }
        } else {
            if (fret_type == 0) {
                return std::make_shared<
                    SolverClass<std::false_type, std::false_type>>(opts);
            } else {
                opts.set<bool>("fret_on_policy", fret_type == 1);
                return std::make_shared<
                    SolverClass<std::false_type, std::true_type>>(opts);
            }
        }
    }

    return nullptr;
}

} // namespace solvers
} // namespace probfd

#include "bisimulation_heuristic_search.h"

#endif // __MDP_HEURISTIC_SEARCH_H__
