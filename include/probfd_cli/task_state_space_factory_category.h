#ifndef PROBFD_CLI_TASK_STATE_SPACE_FACTORY_CATEGORY_H
#define PROBFD_CLI_TASK_STATE_SPACE_FACTORY_CATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli {

void add_task_state_space_factory_category(
    language::plugins::RawRegistry& raw_registry);

}

#endif