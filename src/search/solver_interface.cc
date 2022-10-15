#include "solver_interface.h"

#include "plugin.h"

SolverInterface::SolverInterface() {}

SolverInterface::~SolverInterface() = default;

void
SolverInterface::print_statistics() const
{
}

void
SolverInterface::save_result_if_necessary() const
{
}

static PluginTypePlugin<SolverInterface> _type_plugin(
    "SolverInterface",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");

