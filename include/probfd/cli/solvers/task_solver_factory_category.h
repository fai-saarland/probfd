#ifndef PROBFD_CLI_SOLVERS_TASK_SOLVER_FACTORY_CATEGORY_H
#define PROBFD_CLI_SOLVERS_TASK_SOLVER_FACTORY_CATEGORY_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::solvers {

void add_task_solver_factory_category(language::plugins::Namespace& nspace);

}

#endif