#include "probfd/command_line.h"

#include "probfd/solver_interface.h"

#include "downward/utils/logging.h"
#include "downward/utils/system.h"
#include "downward/utils/timer.h"

#include "probfd/task_proxy.h"
#include "probfd/task_utils/task_properties.h"
#include "probfd/tasks/root_task.h"

#include <iostream>

using namespace std;
using namespace probfd;

using utils::ExitCode;

int main(int argc, const char** argv)
{
    utils::register_event_handlers();

    if (argc < 2) {
        utils::g_log << usage(argv[0]) << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    bool unit_cost = false;
    if (static_cast<string>(argv[1]) != "--help") {
        utils::g_log << "reading input..." << endl;
        auto input_task = probfd::tasks::read_root_tasks(cin);
        utils::g_log << "done reading input!" << endl;
        ProbabilisticTaskProxy task_proxy(*input_task);
        unit_cost = probfd::task_properties::is_unit_cost(task_proxy);
    }

    shared_ptr<SolverInterface> algorithm =
        parse_cmd_line(argc, argv, unit_cost);

    utils::g_search_timer.resume();
    bool found_solution = algorithm->solve();
    utils::g_search_timer.stop();
    utils::g_timer.stop();

    algorithm->print_statistics();
    utils::g_log << "Search time: " << utils::g_search_timer << endl;
    utils::g_log << "Total time: " << utils::g_timer << endl;

    ExitCode exitcode = found_solution ? ExitCode::SUCCESS
                                       : ExitCode::SEARCH_UNSOLVED_INCOMPLETE;
    utils::report_exit_code_reentrant(exitcode);
    return static_cast<int>(exitcode);
}
