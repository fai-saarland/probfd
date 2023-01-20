#include "probfd/solvers/mdp_solver.h"

#include "probfd/analysis_objectives/analysis_objective.h"

#include "probfd/engines/exhaustive_dfs.h"

#include "probfd/engine_interfaces/new_state_handler.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/successor_sorter.h"

#include "probfd/successor_sorter/task_successor_sorter_factory.h"

#include "probfd/progress_report.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {

using namespace engine_interfaces;

using namespace engines::exhaustive_dfs;

class ExhaustiveDFSSolver : public MDPSolver {
public:
    using Engine = ExhaustiveDepthFirstSearch<State, OperatorID, false>;
    using Engine2 = ExhaustiveDepthFirstSearch<State, OperatorID, true>;

    explicit ExhaustiveDFSSolver(const options::Options& opts)
        : MDPSolver(opts)
        , reward_bound_(g_analysis_objective->reward_bound())
        , new_state_handler_(new TaskNewStateHandlerList(
              opts.get_list<std::shared_ptr<TaskNewStateHandler>>(
                  "on_new_state")))
        , heuristic_(opts.get<std::shared_ptr<TaskStateEvaluator>>("eval"))
        , successor_sort_(
              opts.contains("order")
                  ? opts.get<std::shared_ptr<TaskSuccessorSorterFactory>>(
                            "order")
                        ->create_successor_sorter(
                            this->get_state_id_map(),
                            this->get_action_id_map())
                  : nullptr)
        , dual_bounds_(
              opts.contains("dual_bounds") && opts.get<bool>("dual_bounds"))
        , interval_comparison_(
              opts.contains("interval_comparison") &&
              opts.get<bool>("interval_comparison"))
        , reevaluate_(opts.get<bool>("reevaluate"))
        , notify_s0_(opts.get<bool>("initial_state_notification"))
        , path_updates_(BacktrackingUpdateType(
              opts.get<BacktrackingUpdateType>("reverse_path_updates")))
        , only_propagate_when_changed_(
              opts.get<bool>("only_propagate_when_changed"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<TaskStateEvaluator>>(
            "eval",
            "",
            "const");
        parser.add_list_option<std::shared_ptr<TaskNewStateHandler>>(
            "on_new_state",
            "",
            "[]");
        parser.add_option<bool>("interval_comparison", "", "false");
        parser.add_option<bool>("dual_bounds", "", "false");
        parser.add_option<std::shared_ptr<TaskSuccessorSorterFactory>>(
            "order",
            "",
            options::OptionParser::NONE);
        parser.add_option<bool>("reevaluate", "", "true");
        parser.add_option<bool>("initial_state_notification", "", "false");
        std::vector<std::string> t(
            {"disabled",
             "cheap",
             "updates_along_trace",
             "updates_along_stack"});
        parser.add_enum_option<BacktrackingUpdateType>(
            "reverse_path_updates",
            t,
            "",
            "updates_along_trace");
        parser.add_option<bool>("only_propagate_when_changed", "", "true");
        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override
    {
        return "exhaustive_dfs";
    }

    virtual engines::MDPEngineInterface<State>* create_engine() override
    {
        if (dual_bounds_) {
            return this->template engine_factory<Engine2>(
                reward_bound_,
                heuristic_.get(),
                reevaluate_,
                notify_s0_,
                successor_sort_.get(),
                path_updates_,
                only_propagate_when_changed_,
                new_state_handler_.get(),
                &progress_);
        } else {
            return this->template engine_factory<Engine>(
                reward_bound_,
                heuristic_.get(),
                reevaluate_,
                notify_s0_,
                successor_sort_.get(),
                path_updates_,
                only_propagate_when_changed_,
                new_state_handler_.get(),
                &progress_);
        }
    }

private:
    const value_utils::IntervalValue reward_bound_;

    std::shared_ptr<TaskNewStateHandlerList> new_state_handler_;
    std::shared_ptr<TaskStateEvaluator> heuristic_;
    std::shared_ptr<TaskSuccessorSorter> successor_sort_;

    const bool dual_bounds_;
    const bool interval_comparison_;
    const bool reevaluate_;
    const bool notify_s0_;
    const BacktrackingUpdateType path_updates_;
    const bool only_propagate_when_changed_;
};

static Plugin<SolverInterface> _plugin(
    "exhaustive_dfs",
    options::parse<SolverInterface, ExhaustiveDFSSolver>);

} // namespace solvers
} // namespace probfd
