#ifndef DOWNWARD_CLI_TASKS_TASK_TRANSFORMATION_CATEGORY_H
#define DOWNWARD_CLI_TASKS_TASK_TRANSFORMATION_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::tasks {

void add_task_transformation_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif