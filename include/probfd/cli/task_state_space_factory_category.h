#ifndef PROBFD_CLI_TASK_STATE_SPACE_FACTORY_CATEGORY_H
#define PROBFD_CLI_TASK_STATE_SPACE_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli {

void add_task_state_space_factory_category(
    downward::cli::plugins::Registry& raw_registry);

}

#endif