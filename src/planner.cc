#include "list_features.h"
#include "search.h"

#include "downward/utils/system.h"

#include <argparse/argparse.hpp>

#include <iostream>

using namespace std;
using namespace probfd;

using utils::ExitCode;

static void setup_argparser(argparse::ArgumentParser& arg_parser)
{
    add_search_subcommand(arg_parser);
    add_list_features_subcommand(arg_parser);
}

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

        if (auto subcommand = prog.get_used_subcommand()) {
            const auto& subcommand_parser = subcommand->first;
            std::cerr << subcommand_parser;
        } else {
            std::cerr << prog;
        }

        return static_cast<int>(ExitCode::SEARCH_INPUT_ERROR);
    }

    if (auto subcommand = prog.get_used_subcommand()) {
        auto& [subcommand_parser, main] = *subcommand;
        return main(subcommand_parser);
    }

    std::cerr << prog;
    return static_cast<int>(ExitCode::SEARCH_INPUT_ERROR);
}
