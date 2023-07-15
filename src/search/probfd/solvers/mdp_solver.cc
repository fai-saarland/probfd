#include "probfd/solvers/mdp_solver.h"

#include "probfd/caching_task_state_space.h"
#include "probfd/cost_model.h"

#include "downward/utils/timer.h"

#include "downward/heuristic.h"
#include "downward/operator_cost.h"

#include "probfd/tasks/root_task.h"

#include "downward/utils/exceptions.h"

#include "downward/plugins/plugin.h"

#include <iomanip>
#include <vector>

namespace probfd {
namespace solvers {

MDPSolver::MDPSolver(const plugins::Options& opts)
    : task(tasks::g_root_task)
    , task_proxy(*task)
    , state_space_(
          opts.get<bool>("cache")
              ? new CachingTaskStateSpace(
                    task,
                    opts.get_list<std::shared_ptr<Evaluator>>(
                        "path_dependent_evaluators"))
              : new InducedTaskStateSpace(
                    task,
                    opts.get_list<std::shared_ptr<Evaluator>>(
                        "path_dependent_evaluators")))
    , cost_function_(g_cost_model->get_cost_function())
    , progress_(
          opts.contains("report_epsilon")
              ? std::optional<value_t>(opts.get<value_t>("report_epsilon"))
              : std::nullopt,
          std::cout,
          opts.get<bool>("report_enabled"))
    , max_time(opts.get<double>("max_time"))
{
    progress_.register_print([&ss = *this->state_space_](std::ostream& out) {
        out << "registered=" << ss.get_num_registered_states();
    });
}

MDPSolver::~MDPSolver() = default;

void MDPSolver::solve()
{
    std::cout << "Running MDP engine " << get_engine_name();

    if (max_time != std::numeric_limits<double>::infinity()) {
        std::cout << " with a time limit of " << max_time << " seconds";
    }

    std::cout << "..." << std::endl;

    try {
        utils::Timer total_timer;
        std::unique_ptr<TaskMDPEngineInterface> engine = create_engine();

        const State& initial_state = state_space_->get_initial_state();

        Interval val = engine->solve(initial_state, max_time);
        progress_.force_print();
        total_timer.stop();

        std::cout << "analysis done. [t=" << utils::g_timer << "]" << std::endl;

        std::cout << std::endl;

        print_analysis_result(val);

        std::cout << std::endl;
        std::cout << "State space interface:" << std::endl;
        std::cout << "  Registered state(s): "
                  << state_space_->get_num_registered_states() << std::endl;
        state_space_->print_statistics(std::cout);

        std::cout << std::endl;
        std::cout << "Engine " << get_engine_name()
                  << " statistics:" << std::endl;
        std::cout << "  Actual solver time: " << total_timer << std::endl;
        engine->print_statistics(std::cout);

        print_additional_statistics();
    } catch (utils::TimeoutException&) {
        std::cout << "Time limit reached. Analysis was aborted." << std::endl;
        solution_found = false;
    }
}

void MDPSolver::add_options_to_feature(plugins::Feature& feature)
{
    feature.add_option<bool>("cache", "", "false");
    feature.add_list_option<std::shared_ptr<Evaluator>>(
        "path_dependent_evaluators",
        "",
        "[]");
    feature.add_option<value_t>("report_epsilon", "", "1e-4");
    feature.add_option<bool>("report_enabled", "", "true");
    feature.add_option<double>("max_time", "", "infinity");
}

} // namespace solvers
} // namespace probfd
