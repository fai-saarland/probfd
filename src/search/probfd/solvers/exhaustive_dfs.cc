#include "../engines/exhaustive_dfs.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../heuristic_search_interfaceable.h"
#include "../new_state_handler.h"
#include "../progress_report.h"
#include "../state_evaluator.h"
#include "../successor_sort.h"
#include "mdp_solver.h"

namespace probabilistic {
namespace solvers {

class ExhaustiveDFSSolver : public MDPSolver {
public:
    using Engine = engines::exhaustive_dfs::ExhaustiveDepthFirstSearch<
        GlobalState,
        const ProbabilisticOperator*,
        std::false_type>;

    using Engine2 = engines::exhaustive_dfs::ExhaustiveDepthFirstSearch<
        GlobalState,
        const ProbabilisticOperator*,
        std::true_type>;

    explicit ExhaustiveDFSSolver(const options::Options& opts)
        : MDPSolver(opts)
        , new_state_handler_(new NewGlobalStateHandlerList(
              opts.get_list<std::shared_ptr<NewGlobalStateHandler>>(
                  "on_new_state")))
        , heuristic_(opts.get<std::shared_ptr<GlobalStateEvaluator>>("eval"))
        , dead_end_eval_(
              opts.contains("dead_end_eval")
                  ? opts.get<std::shared_ptr<GlobalStateEvaluator>>(
                        "dead_end_eval")
                  : nullptr)
        , successor_sort_(
              opts.contains("order")
                  ? opts.get<
                        std::shared_ptr<ProbabilisticOperatorSuccessorSorting>>(
                        "order")
                  : nullptr)
        , dual_bounds_(
              opts.contains("dual_bounds") && opts.get<bool>("dual_bounds"))
        , interval_comparison_(
              opts.contains("interval_comparison") &&
              opts.get<bool>("interval_comparison"))
        , reevaluate_(opts.get<bool>("reevaluate"))
        , notify_s0_(opts.get<bool>("initial_state_notification"))
        , path_updates_(engines::exhaustive_dfs::BacktrackingUpdateType(
              opts.get_enum("reverse_path_updates")))
        , only_propagate_when_changed_(
              opts.get<bool>("only_propagate_when_changed"))
    {
        if (successor_sort_ != nullptr) {
            successor_sort_->initialize(
                &connector_,
                this->get_state_id_map(),
                this->get_action_id_map());
        }
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "eval",
            "",
            "const");
        parser.add_list_option<std::shared_ptr<NewGlobalStateHandler>>(
            "on_new_state",
            "",
            "[]");
        parser.add_option<bool>("interval_comparison", "", "false");
        parser.add_option<bool>("dual_bounds", "", "false");
        parser.add_option<std::shared_ptr<GlobalStateEvaluator>>(
            "dead_end_eval",
            "",
            options::OptionParser::NONE);
        parser
            .add_option<std::shared_ptr<ProbabilisticOperatorSuccessorSorting>>(
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
        parser.add_enum_option(
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

    virtual engines::MDPEngineInterface<GlobalState>* create_engine() override
    {
        if (dual_bounds_) {
            return this->template engine_factory<Engine2>(
                &connector_,
                heuristic_.get(),
                dead_end_eval_.get(),
                reevaluate_,
                notify_s0_,
                successor_sort_.get(),
                path_updates_,
                only_propagate_when_changed_,
                new_state_handler_.get(),
                &progress_);
        } else {
            return this->template engine_factory<Engine>(
                &connector_,
                heuristic_.get(),
                dead_end_eval_.get(),
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
    HeuristicSearchConnector connector_;

    std::shared_ptr<NewGlobalStateHandlerList> new_state_handler_;
    std::shared_ptr<GlobalStateEvaluator> heuristic_;
    std::shared_ptr<GlobalStateEvaluator> dead_end_eval_;
    std::shared_ptr<ProbabilisticOperatorSuccessorSorting> successor_sort_;

    const bool dual_bounds_;
    const bool interval_comparison_;
    const bool reevaluate_;
    const bool notify_s0_;
    const engines::exhaustive_dfs::BacktrackingUpdateType path_updates_;
    const bool only_propagate_when_changed_;
};

static Plugin<SolverInterface> _plugin(
    "exhaustive_dfs",
    options::parse<SolverInterface, ExhaustiveDFSSolver>);

} // namespace solvers
} // namespace probabilistic
