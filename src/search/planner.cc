#include "command_line.h"
#include "option_parser.h"
#include "solver_interface.h"

#include "globals.h"

#include "options/registries.h"

#include "utils/system.h"
#include "utils/timer.h"

#include <iostream>

using namespace std;

int main(int argc, const char **argv) {
    utils::register_event_handlers();

    if (argc < 2) {
        cout << usage(argv[0]) << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    if (string(argv[1]).compare("--help") != 0)
        read_everything(cin);

    std::shared_ptr<SolverInterface> engine = 0;

    // The command line is parsed twice: once in dry-run mode, to
    // check for simple input errors, and then in normal mode.
    bool unit_cost = is_unit_cost();
    try {
        options::Registry registry(*options::RawRegistry::instance());
        parse_cmd_line(argc, argv, registry, true, unit_cost);
        engine = parse_cmd_line(argc, argv, registry, false, unit_cost);
    } catch (const ArgError &error) {
        error.print();
        usage(argv[0]);
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    } catch (const OptionParserError &error) {
        error.print();
        usage(argv[0]);
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    } catch (const ParseError &error) {
        error.print();
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    std::cout << std::endl;
    std::cout << "Initialization completed! [t=" << utils::g_timer << "]" << std::endl;
    std::cout << std::endl;

    utils::Timer search_timer;
    engine->solve();
    search_timer.stop();
    utils::g_timer.stop();

    engine->save_result_if_necessary();
    engine->print_statistics();

    cout << endl;
    cout << "Search time: " << search_timer << endl;
    cout << "Total time: " << utils::g_timer << endl;

    if (engine->found_solution()) {
        utils::exit_with(utils::ExitCode::SUCCESS);
    } else {
        utils::exit_with(utils::ExitCode::SEARCH_UNSOLVED_INCOMPLETE);
    }
}
