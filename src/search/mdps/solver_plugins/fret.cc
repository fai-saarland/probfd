#include "../algorithms/fret.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../../utils/system.h"
#include "../algorithms/heuristic_search_base.h"
#include "../logging.h"
#include "../state_listener.h"
#include "mdp_solver.h"

#include <cassert>
#include <sstream>

namespace probabilistic {
namespace solvers {

class FRET : public MDPSolver {
public:
    FRET(const options::Options& opts)
        : MDPSolver(opts)
        , on_policy_(opts.get<bool>("on_policy"))
        , remove_self_loops_(opts.get<bool>("remove_self_loops"))
        , inner_(std::dynamic_pointer_cast<MDPSolver>(
              opts.get<std::shared_ptr<SolverInterface>>("inner")))
        , dead_end_listener_(GlobalStateComponentListener::from_options(opts))
    {
        if (inner_ == nullptr) {
            g_err << "FRET: Invalid inner MDP solver" << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<SolverInterface>>("inner");
        parser.add_option<bool>("on_policy", "", "true");
        parser.add_option<bool>("remove_self_loops", "", "false");
        GlobalStateComponentListener::add_options_to_parser(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string name() const override
    {
        std::ostringstream out;
        out << (on_policy_ ? "fret-pi" : "fret-v") << "(" << inner_->name()
            << ")";
        return out.str();
    }

    template<typename B2>
    IMDPEngine<State>* create_engine(
        const B2&,
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t minval,
        value_type::value_t maxval,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
    {
        QuotientSystem* qs = new QuotientSystem(
            remove_self_loops_,
            state_id_map,
            aops_generator,
            transition_generator);
        QuotientSystemFunctionFactory* qsf =
            new QuotientSystemFunctionFactory(qs);
        IMDPEngine<QState>* inner = inner_->create_engine(
            *qsf,
            qs,
            report,
            qsf->get_id_map(),
            qsf->get_aops_gen(),
            qsf->get_transition_gen(),
            minval,
            maxval,
            qsf->create(state_reward),
            qsf->create(transition_reward));
        algorithms::heuristic_search_base::HeuristicSearchBase<
            QState,
            QAction,
            B2,
            std::true_type>* hs =
            dynamic_cast<
                algorithms::heuristic_search_base::
                    HeuristicSearchBase<QState, QAction, B2, std::true_type>*>(
                inner);
        if (hs == nullptr) {
            g_err << inner_->name()
                  << " doesn't support being wrapped into FRET!" << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
        return new FRETEngine<B2>(
            on_policy_,
            report,
            qs,
            qsf,
            hs,
            qsf->create(GlobalStateComponentListener::create_state_listener(
                dead_end_listener_, aops_generator, transition_generator)));
    }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t minval,
        value_type::value_t maxval,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
        override
    {
        if (inner_->maintains_dual_bounds()) {
            return create_engine(
                std::true_type(),
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                minval,
                maxval,
                state_reward,
                transition_reward);
        } else {
            return create_engine(
                std::false_type(),
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                minval,
                maxval,
                state_reward,
                transition_reward);
        }
    }

    virtual bool maintains_dual_bounds() const override
    {
        return inner_->maintains_dual_bounds();
    }

    virtual void print_additional_statistics(std::ostream& out) const override
    {
        if (dead_end_listener_) {
            dead_end_listener_->get_statistics().print(out);
        }
        inner_->print_additional_statistics(out);
    }

private:
    const bool on_policy_;
    const bool remove_self_loops_;
    std::shared_ptr<MDPSolver> inner_;
    std::shared_ptr<GlobalStateComponentListener> dead_end_listener_;

    template<typename B2>
    class FRETEngine : public IMDPEngine<State> {
    public:
        FRETEngine(
            bool on_policy,
            ProgressReport* report,
            QuotientSystem* qs,
            QuotientSystemFunctionFactory* qsf,
            algorithms::heuristic_search_base::
                HeuristicSearchBase<QState, QAction, B2, std::true_type>* inner,
            algorithms::StateListener<QState, QAction>* listener)
            : on_policy_(on_policy)
            , report_(report)
            , qs_(qs)
            , qsf_(qsf)
            , inner_(inner)
            , listener_(listener)
        {
        }

        virtual ~FRETEngine()
        {
            delete (inner_);
            delete (qsf_);
            delete (qs_);
        }

        virtual AnalysisResult solve(const State& state) override
        {
            if (on_policy_) {
                algorithms::fret::FRETPi<State, Action, B2> fret_pi(
                    report_, qs_, inner_, listener_);
                return fret_pi.solve(state);
            } else {
                algorithms::fret::FRETV<State, Action, B2> fret_v(
                    report_, qs_, inner_, listener_);
                return fret_v.solve(state);
            }
        }

    private:
        const bool on_policy_;
        ProgressReport* report_;
        QuotientSystem* qs_;
        QuotientSystemFunctionFactory* qsf_;
        algorithms::heuristic_search_base::
            HeuristicSearchBase<QState, QAction, B2, std::true_type>* inner_;
        algorithms::StateListener<QState, QAction>* listener_;
    };
};

static Plugin<SolverInterface>
    _plugin("fret", options::parse<SolverInterface, FRET>);

} // namespace solvers
} // namespace probabilistic
