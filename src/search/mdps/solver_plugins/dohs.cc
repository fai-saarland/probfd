#include "../../option_parser.h"
#include "../../plugin.h"
#include "../algorithms/depth_oriented_heuristic_search.h"
#include "../policy_chooser.h"
#include "../state_listener.h"
#include "mdp_solver.h"

#include <memory>
#include <sstream>
#include <string>

namespace probabilistic {
namespace solvers {

class DepthOrientedHeuristicSearch : public MDPSolver {
public:
    explicit DepthOrientedHeuristicSearch(const options::Options& opts)
        : MDPSolver(opts)
        , dual_(opts.get<bool>("dual"))
        , interval_comparison_(opts.get<bool>("interval_comparison"))
        , forward_updates_(opts.get<bool>("fwup"))
        , backward_updates_(
              algorithms::depth_oriented_search::BacktrackingUpdateType(
                  opts.get_enum("bwup")))
        , cutoff_inconsistent_(opts.get<bool>("cutoff_inconsistent"))
        , terminate_inconsistent_(opts.get<bool>("terminate_inconsistent"))
        , value_iteration_(opts.get<bool>("vi"))
        , expand_tip_states_(opts.get<bool>("expand_tip"))
        , dead_end_listener_(GlobalStateComponentListener::from_options(opts))
        , level_(get_dead_end_level(opts))
        , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , policy_(opts.get<std::shared_ptr<PolicyChooser>>("policy"))
    {
        parameter_validity_check();
        if (opts.contains("name")) {
            name_ = opts.get<std::string>("name");
        } else {
            std::ostringstream name;
            name << "dohs(";
            name << "fw=" << forward_updates_;
            name << ", bw=";
            switch (backward_updates_) {
            case (algorithms::depth_oriented_search::BacktrackingUpdateType::
                      Disabled):
                name << "disabled";
                break;
            case (algorithms::depth_oriented_search::BacktrackingUpdateType::
                      OnDemand):
                name << "ondemand";
                break;
            case (algorithms::depth_oriented_search::BacktrackingUpdateType::
                      Single):
                name << "single";
                break;
            case (algorithms::depth_oriented_search::BacktrackingUpdateType::
                      UntilConvergence):
                name << "convergence";
                break;
            default:
                assert(false);
                break;
            }
            name << ", expand_tip=" << expand_tip_states_;
            name << ", expand_inconsistent=" << (!cutoff_inconsistent_);
            name << ", terminate_inconsistent=" << terminate_inconsistent_;
            name << ", termination=" << (value_iteration_ ? "vi" : "label");
            name << ")";
            name_ = name.str();
        }
    }

    static void add_core_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", "const");
        parser.add_option<std::shared_ptr<PolicyChooser>>(
            "policy", "", "arbitrary_tiebreaker");
        parser.add_option<bool>("dual", "", "false");
        parser.add_option<bool>("interval_comparison", "", "false");
        GlobalStateComponentListener::add_options_to_parser(parser);
        add_dead_end_level_option(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        DepthOrientedHeuristicSearch::add_core_options_to_parser(parser);
        parser.add_option<bool>("fwup", "", "true");
        {
            std::vector<std::string> bwups(
                { "disabled", "ondemand", "single", "convergence" });
            parser.add_enum_option("bwup", bwups, "", "ondemand");
        }
        parser.add_option<bool>("cutoff_inconsistent", "", "true");
        parser.add_option<bool>("terminate_inconsistent", "", "false");
        parser.add_option<bool>("vi", "", "false");
        parser.add_option<bool>("expand_tip", "", "true");
    }

    virtual std::string name() const override { return name_; }

    void parameter_validity_check() const
    {
        bool valid = true;
        std::ostringstream error_msg;
        if (!forward_updates_) {
            if (cutoff_inconsistent_) {
                error_msg << "cutoff_inconsistent requires forward updates!"
                          << std::endl;
                valid = false;
            }
            if (backward_updates_
                == algorithms::depth_oriented_search::BacktrackingUpdateType::
                    OnDemand) {
                error_msg
                    << "ondemand backward updates required forward updates!"
                    << std::endl;
                valid = false;
            }
            if (!value_iteration_
                && backward_updates_
                    == algorithms::depth_oriented_search::
                        BacktrackingUpdateType::Disabled) {
                error_msg << "either value_iteration, forward_updates, or "
                             "backward_updates must be enabled!"
                          << std::endl;
                valid = false;
            }
            if (expand_tip_states_
                && backward_updates_
                    == algorithms::depth_oriented_search::
                        BacktrackingUpdateType::OnDemand) {
                error_msg
                    << "ondemand backward updates require forward updates or "
                       "expand_tip=true!"
                    << std::endl;
                valid = false;
            }
        }
        if (terminate_inconsistent_ && expand_tip_states_
            && !cutoff_inconsistent_) {
            error_msg
                << "greedy exploration requires either expand_tip=false or "
                   "cutoff_inconsistent=true"
                << std::endl;
            valid = false;
        }
        if (!valid) {
            std::cerr
                << "depth-oriented heuristic search has been misconfigured!"
                << std::endl;
            std::cerr << error_msg.str();
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t gase,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
        override
    {
        if (dual_) {
            return create_engine(
                std::true_type(),
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                dead_end_value,
                gase,
                state_reward,
                transition_reward);
        } else {
            return create_engine(
                std::false_type(),
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                dead_end_value,
                gase,
                state_reward,
                transition_reward);
        }
    }

    template<typename B2>
    IMDPEngine<State>* create_engine(
        const B2&,
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
    {
        return new algorithms::depth_oriented_search::
            DepthOrientedHeuristicSearch<State, Action, B2>(
                forward_updates_,
                backward_updates_,
                cutoff_inconsistent_,
                terminate_inconsistent_,
                value_iteration_,
                expand_tip_states_,
                GlobalStateComponentListener::create_state_listener(
                    dead_end_listener_, aops_generator, transition_generator),
                level_,
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                state_reward,
                transition_reward,
                eval_.get(),
                policy_.get(),
                dead_end_value,
                this->handler_.get(),
                interval_comparison_);
    }

    virtual IMDPEngine<QuotientSystem::QState>* create_engine(
        QuotientSystemFunctionFactory& factory,
        QuotientSystem* sys,
        ProgressReport* report,
        algorithms::StateIDMap<QuotientSystem::QState>* state_id_map,
        algorithms::ApplicableActionsGenerator<
            QuotientSystem::QState,
            QuotientSystem::QAction>* aops_generator,
        algorithms::TransitionGenerator<
            QuotientSystem::QState,
            QuotientSystem::QAction>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t asdasd,
        algorithms::StateEvaluationFunction<QuotientSystem::QState>*
            state_reward,
        algorithms::TransitionRewardFunction<
            QuotientSystem::QState,
            QuotientSystem::QAction>* transition_reward) override
    {
        if (dual_) {
            return create_engine(
                std::true_type(),
                factory,
                sys,
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                dead_end_value,
                asdasd,
                state_reward,
                transition_reward);
        } else {
            return create_engine(
                std::false_type(),
                factory,
                sys,
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                dead_end_value,
                asdasd,
                state_reward,
                transition_reward);
        }
    }

    template<typename B2>
    IMDPEngine<QuotientSystem::QState>* create_engine(
        const B2&,
        QuotientSystemFunctionFactory& factory,
        QuotientSystem*,
        ProgressReport* report,
        algorithms::StateIDMap<QuotientSystem::QState>* state_id_map,
        algorithms::ApplicableActionsGenerator<
            QuotientSystem::QState,
            QuotientSystem::QAction>* aops_generator,
        algorithms::TransitionGenerator<
            QuotientSystem::QState,
            QuotientSystem::QAction>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t,
        algorithms::StateEvaluationFunction<QuotientSystem::QState>*
            state_reward,
        algorithms::TransitionRewardFunction<
            QuotientSystem::QState,
            QuotientSystem::QAction>* transition_reward)
    {
        return new algorithms::depth_oriented_search::
            DepthOrientedHeuristicSearchQ<QState, QAction, B2>(
                forward_updates_,
                backward_updates_,
                cutoff_inconsistent_,
                terminate_inconsistent_,
                value_iteration_,
                expand_tip_states_,
                factory.create(
                    GlobalStateComponentListener::create_state_listener(
                        dead_end_listener_,
                        aops_generator->base(),
                        transition_generator->base())),
                level_,
                report,
                state_id_map,
                aops_generator,
                transition_generator,
                state_reward,
                transition_reward,
                factory.create(eval_.get()),
                policy_.get(),
                dead_end_value,
                factory.create(this->handler_.get()),
                interval_comparison_);
    }

    virtual bool maintains_dual_bounds() const override { return dual_; }

    virtual void print_additional_statistics(std::ostream& out) const override
    {
        if (dead_end_listener_) {
            dead_end_listener_->get_statistics().print(out);
        }
    }

private:
    std::string name_;

    const bool dual_;
    const bool interval_comparison_;

    const bool forward_updates_;
    const algorithms::depth_oriented_search::BacktrackingUpdateType
        backward_updates_;
    const bool cutoff_inconsistent_;
    const bool terminate_inconsistent_;
    const bool value_iteration_;
    const bool expand_tip_states_;

    std::shared_ptr<GlobalStateComponentListener> dead_end_listener_;
    algorithms::DeadEndIdentificationLevel level_;
    std::shared_ptr<GlobalStateEvaluator> eval_;
    std::shared_ptr<PolicyChooser> policy_;
};

static std::shared_ptr<SolverInterface>
_parse_lao(options::OptionParser& parser)
{
    DepthOrientedHeuristicSearch::add_core_options_to_parser(parser);
    options::Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }

    opts.set<std::string>("name", "lao");
    opts.set<bool>("fwup", false);
    opts.set<int>("bwup", 3);
    opts.set<bool>("cutoff_inconsistent", false);
    opts.set<bool>("terminate_inconsistent", true);
    opts.set<bool>("expand_tip", false);
    opts.set<bool>("vi", true);

    return std::make_shared<DepthOrientedHeuristicSearch>(opts);
}

static std::shared_ptr<SolverInterface>
_parse_ilao(options::OptionParser& parser)
{
    DepthOrientedHeuristicSearch::add_core_options_to_parser(parser);
    options::Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }

    opts.set<std::string>("name", "ilao");
    opts.set<bool>("fwup", false);
    opts.set<int>("bwup", 2);
    opts.set<bool>("cutoff_inconsistent", false);
    opts.set<bool>("terminate_inconsistent", false);
    opts.set<bool>("expand_tip", false);
    opts.set<bool>("vi", true);

    return std::make_shared<DepthOrientedHeuristicSearch>(opts);
}

static std::shared_ptr<SolverInterface>
_parse_iilao(options::OptionParser& parser)
{
    DepthOrientedHeuristicSearch::add_core_options_to_parser(parser);
    options::Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }

    opts.set<std::string>("name", "iilao");
    opts.set<bool>("fwup", false);
    opts.set<int>("bwup", 2);
    opts.set<bool>("cutoff_inconsistent", false);
    opts.set<bool>("terminate_inconsistent", false);
    opts.set<bool>("expand_tip", false);
    opts.set<bool>("vi", false);

    return std::make_shared<DepthOrientedHeuristicSearch>(opts);
}

static std::shared_ptr<SolverInterface>
_parse_hdp(options::OptionParser& parser)
{
    DepthOrientedHeuristicSearch::add_core_options_to_parser(parser);
    options::Options opts = parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }

    opts.set<std::string>("name", "hdp");
    opts.set<bool>("fwup", true);
    opts.set<int>("bwup", 1);
    opts.set<bool>("cutoff_inconsistent", true);
    opts.set<bool>("terminate_inconsistent", false);
    opts.set<bool>("vi", false);
    opts.set<bool>("expand_tip", false);

    return std::make_shared<DepthOrientedHeuristicSearch>(opts);
}

static Plugin<SolverInterface> _plugin_lao("lao", _parse_lao);
static Plugin<SolverInterface> _plugin_ilao("ilao", _parse_ilao);
static Plugin<SolverInterface> _plugin_iilao("iilao", _parse_iilao);
static Plugin<SolverInterface> _plugin_hdp("hdp", _parse_hdp);

static Plugin<SolverInterface> _plugin(
    "dohs",
    options::parse<SolverInterface, DepthOrientedHeuristicSearch>);

} // namespace solvers
} // namespace probabilistic

