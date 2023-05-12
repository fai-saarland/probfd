#include "probfd/solvers/mdp_solver.h"

#include "probfd/cost_model.h"

#include "probfd/engines/exhaustive_dfs.h"

#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/transition_sorter.h"

#include "probfd/transition_sorters/task_transition_sorter_factory.h"

#include "probfd/progress_report.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

using namespace engines::exhaustive_dfs;

class ExhaustiveDFSSolver : public MDPSolver {
    const Interval cost_bound_;

    std::shared_ptr<TaskEvaluator> heuristic_;
    std::shared_ptr<TaskNewStateObserverList> new_state_handler_;
    std::shared_ptr<TaskTransitionSorter> transition_sort_;

    const bool dual_bounds_;
    const bool interval_comparison_;
    const bool reevaluate_;
    const bool notify_s0_;
    const bool path_updates_;
    const bool only_propagate_when_changed_;

public:
    explicit ExhaustiveDFSSolver(const options::Options& opts)
        : MDPSolver(opts)
        , cost_bound_(g_cost_model->optimal_value_bound())
        , heuristic_(opts.get<std::shared_ptr<TaskEvaluator>>("eval"))
        , new_state_handler_(new TaskNewStateObserverList(
              opts.get_list<std::shared_ptr<TaskNewStateObserver>>(
                  "on_new_state")))
        , transition_sort_(
              opts.contains("order")
                  ? opts.get<std::shared_ptr<TaskTransitionSorterFactory>>(
                            "order")
                        ->create_transition_sorter(this->state_space_.get())
                  : nullptr)
        , dual_bounds_(
              opts.contains("dual_bounds") && opts.get<bool>("dual_bounds"))
        , interval_comparison_(
              opts.contains("interval_comparison") &&
              opts.get<bool>("interval_comparison"))
        , reevaluate_(opts.get<bool>("reevaluate"))
        , notify_s0_(opts.get<bool>("initial_state_notification"))
        , path_updates_(opts.get<bool>("reverse_path_updates"))
        , only_propagate_when_changed_(
              opts.get<bool>("only_propagate_when_changed"))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<TaskEvaluator>>(
            "eval",
            "",
            "blind_eval");
        parser.add_list_option<std::shared_ptr<TaskNewStateObserver>>(
            "on_new_state",
            "",
            "[]");
        parser.add_option<std::shared_ptr<TaskTransitionSorterFactory>>(
            "order",
            "",
            options::OptionParser::NONE);

        parser.add_option<bool>("interval_comparison", "", "false");
        parser.add_option<bool>("dual_bounds", "", "false");
        parser.add_option<bool>("reevaluate", "", "true");
        parser.add_option<bool>("initial_state_notification", "", "false");
        parser.add_option<bool>("reverse_path_updates", "", "true");
        parser.add_option<bool>("only_propagate_when_changed", "", "true");

        MDPSolver::add_options_to_parser(parser);
    }

    virtual std::string get_engine_name() const override
    {
        return "exhaustive_dfs";
    }

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
    {
        using Engine = ExhaustiveDepthFirstSearch<State, OperatorID, false>;
        using Engine2 = ExhaustiveDepthFirstSearch<State, OperatorID, true>;

        if (dual_bounds_) {
            return this->template engine_factory<Engine2>(
                heuristic_.get(),
                new_state_handler_.get(),
                transition_sort_.get(),
                cost_bound_,
                reevaluate_,
                notify_s0_,
                path_updates_,
                only_propagate_when_changed_,
                &progress_);
        } else {
            return this->template engine_factory<Engine>(
                heuristic_.get(),
                new_state_handler_.get(),
                transition_sort_.get(),
                cost_bound_,
                reevaluate_,
                notify_s0_,
                path_updates_,
                only_propagate_when_changed_,
                &progress_);
        }
    }
};

static Plugin<SolverInterface> _plugin(
    "exhaustive_dfs",
    options::parse<SolverInterface, ExhaustiveDFSSolver>);

} // namespace
} // namespace solvers
} // namespace probfd
