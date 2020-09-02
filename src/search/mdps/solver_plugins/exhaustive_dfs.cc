#include "../algorithms/exhaustive_dfs.h"

#include "../../heuristic.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../heuristics/dead_end_pruning.h"
#include "../open_lists/open_list.h"
#include "../state_listener.h"
#include "mdp_solver.h"

#include <memory>
#include <string>

namespace probabilistic {
namespace solvers {

class ExhaustiveDepthFirstSearch : public MDPSolver {
public:
    explicit ExhaustiveDepthFirstSearch(const options::Options& opts)
        : MDPSolver(opts)
        , dual_(opts.get<bool>("dual"))
        , reevaluate_(opts.get<bool>("reevaluate"))
        , notify_s0_(opts.get<bool>("initial_state_notification"))
        , path_updates_(algorithms::exhaustive_dfs::PathUpdates(
              opts.get_enum("reverse_path_updates")))
        , eval_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , dead_end_eval_(
              opts.contains("dead_end_eval")
                  ? std::make_shared<dead_end_pruning::DeadEndPruningHeuristic>(
                      value_type::zero,
                      value_type::zero,
                      opts.get<std::shared_ptr<Heuristic>>("dead_end_eval"))
                  : nullptr)
        , listener_(GlobalStateComponentListener::from_options(opts))
        , open_list_(opts.get<std::shared_ptr<open_lists::GlobalStateOpenList>>(
              "open_list"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval", "", "const");
        parser.add_option<std::shared_ptr<Heuristic>>(
            "dead_end_eval", "", options::OptionParser::NONE);
        parser.add_option<bool>("dual", "", "false");
        parser.add_option<bool>("reevaluate", "", "true");
        parser.add_option<bool>("initial_state_notification", "", "false");
        parser.add_option<std::shared_ptr<open_lists::GlobalStateOpenList>>(
            "open_list", "", "fifo");
        std::vector<std::string> t({ "disabled", "changes", "complete" });
        parser.add_enum_option("reverse_path_updates", t, "", "changes");
        GlobalStateComponentListener::add_options_to_parser(parser);
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string name() const override { return "exhaustive_dfs"; }

    virtual IMDPEngine<State>* create_engine(
        ProgressReport* report,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t val,
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
                val,
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
                val,
                state_reward,
                transition_reward);
        }
    }

    template<typename B>
    IMDPEngine<State>* create_engine(
        const B&,
        ProgressReport* r,
        algorithms::StateIDMap<State>* state_id_map,
        algorithms::ApplicableActionsGenerator<State, Action>* aops_generator,
        algorithms::TransitionGenerator<State, Action>* transition_generator,
        value_type::value_t dead_end_value,
        value_type::value_t,
        algorithms::StateEvaluationFunction<State>* state_reward,
        algorithms::TransitionRewardFunction<State, Action>* transition_reward)
    {
        return new algorithms::exhaustive_dfs::
            ExhaustiveDepthFirstSearch<State, Action, B>(
                eval_.get(),
                dead_end_eval_.get(),
                listener_.get(),
                reevaluate_,
                notify_s0_,
                open_list_.get(),
                path_updates_,
                r,
                state_id_map,
                aops_generator,
                transition_generator,
                state_reward,
                transition_reward,
                this->handler_.get(),
                dead_end_value);
    }


    virtual void print_additional_statistics(std::ostream& out) const override
    {
        if (listener_) {
            listener_->get_statistics().print(out);
        }
    }

private:
    const bool dual_;
    const bool reevaluate_;
    const bool notify_s0_;
    const algorithms::exhaustive_dfs::PathUpdates path_updates_;
    std::shared_ptr<GlobalStateEvaluator> eval_;
    std::shared_ptr<GlobalStateEvaluator> dead_end_eval_;
    std::shared_ptr<GlobalStateComponentListener> listener_;
    std::shared_ptr<open_lists::GlobalStateOpenList> open_list_;
};

static Plugin<SolverInterface> _plugin(
    "exhaustive_dfs",
    options::parse<SolverInterface, ExhaustiveDepthFirstSearch>);

} // namespace solvers
} // namespace probabilistic
