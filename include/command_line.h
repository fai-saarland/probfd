#ifndef PROBFD_COMMAND_LINE_H
#define PROBFD_COMMAND_LINE_H

#include <functional>

// Forward Declarations
namespace argparse {
class ArgumentParser;
}

namespace probfd {

using SubCommandFn = std::function<int(argparse::ArgumentParser&)>;

void setup_argparser(argparse::ArgumentParser& arg_parser);

} // namespace probfd

#endif
