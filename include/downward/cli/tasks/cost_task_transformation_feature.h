#ifndef DOWNWARD_CLI_TASKS_COST_TASK_TRANSFORMATION_FEATURE_H
#define DOWNWARD_CLI_TASKS_COST_TASK_TRANSFORMATION_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::tasks {

void add_cost_task_transformation_features(
    language::plugins::Registry& registry);

}

#endif