#include "option_parser.h"

#include "probfd/command_line.h"

#include "probfd/solver_interface.h"

#include "options/registries.h"

#include "utils/logging.h"
#include "utils/system.h"
#include "utils/timer.h"

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
        probfd::tasks::read_root_tasks(cin);
        utils::g_log << "done reading input!" << endl;
        ProbabilisticTaskProxy task_proxy(*probfd::tasks::g_root_task);
        unit_cost = probfd::task_properties::is_unit_cost(task_proxy);
    }

    shared_ptr<SolverInterface> engine;

    // The command line is parsed twice: once in dry-run mode, to
    // check for simple input errors, and then in normal mode.
    try {
        options::Registry registry(*options::RawRegistry::instance());
        parse_cmd_line(argc, argv, registry, true, unit_cost);
        engine = parse_cmd_line(argc, argv, registry, false, unit_cost);
    } catch (const ArgError& error) {
        error.print();
        usage(argv[0]);
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    } catch (const OptionParserError& error) {
        error.print();
        usage(argv[0]);
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    } catch (const ParseError& error) {
        error.print();
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    utils::g_search_timer.resume();
    engine->solve();
    utils::g_search_timer.stop();
    utils::g_timer.stop();

    engine->print_statistics();
    utils::g_log << "Search time: " << utils::g_search_timer << endl;
    utils::g_log << "Total time: " << utils::g_timer << endl;

    ExitCode exitcode = engine->found_solution()
                            ? ExitCode::SUCCESS
                            : ExitCode::SEARCH_UNSOLVED_INCOMPLETE;
    utils::report_exit_code_reentrant(exitcode);
    return static_cast<int>(exitcode);
}
