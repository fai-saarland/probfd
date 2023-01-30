#ifndef PROBFD_COMMAND_LINE_H
#define PROBFD_COMMAND_LINE_H

#include "utils/exceptions.h"

#include <memory>
#include <string>

namespace options {
class Registry;
}

namespace probfd {

class SolverInterface;

class ArgError : public utils::Exception {
    std::string msg;

public:
    explicit ArgError(const std::string& msg);

    virtual void print() const override;
};

extern std::shared_ptr<SolverInterface> parse_cmd_line(
    int argc,
    const char** argv,
    options::Registry& registry,
    bool dry_run,
    bool is_unit_cost);

extern std::string usage(const std::string& progname);

} // namespace probfd

#endif
