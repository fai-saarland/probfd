#ifndef PROBFD_CLI_SOLVERS_TASK_SOLVER_FACTORY_CATEGORY_H
#define PROBFD_CLI_SOLVERS_TASK_SOLVER_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_task_solver_factory_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif