#ifndef DOWNWARD_CLI_TASKS_TASK_TRANSFORMATION_CATEGORY_H
#define DOWNWARD_CLI_TASKS_TASK_TRANSFORMATION_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::tasks {

void add_task_transformation_category(
    language::plugins::Registry& registry);

}

#endif