#ifndef DOWNWARD_CLI_MUTEXES_MUTEX_FACTORY_CATEGORY_H
#define DOWNWARD_CLI_MUTEXES_MUTEX_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::mutexes {

void add_mutex_factory_category(
    downward::cli::plugins::Registry& raw_registry);

}

#endif