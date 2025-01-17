#include "command_line.h"

#include "downward/utils/system.h"

#include <argparse/argparse.hpp>

#include <iostream>

using namespace std;
using namespace probfd;

using utils::ExitCode;

int main(int argc, const char** argv)
{
    argparse::ArgumentParser prog(
        "probfd",
        "0.0",
        argparse::default_arguments::help);

    setup_argparser(prog);

    try {
        prog.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << '\n' << std::endl;

        if (auto subcommand_parser = prog.get_used_subcommand_parser()) {
            std::cerr << *subcommand_parser;
        } else {
            std::cerr << prog;
        }

        return static_cast<int>(ExitCode::SEARCH_INPUT_ERROR);
    }

    if (auto subcommand_parser = prog.get_used_subcommand_parser()) {
        auto subcommand = subcommand_parser->get().get<SubCommandFn>("fn");
        return subcommand(*subcommand_parser);
    }

    std::cerr << prog;
    return static_cast<int>(ExitCode::SEARCH_INPUT_ERROR);
}
