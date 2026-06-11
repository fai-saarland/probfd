#ifndef DOWNWARD_CLI_MUTEXES_MUTEX_FACTORY_CATEGORY_H
#define DOWNWARD_CLI_MUTEXES_MUTEX_FACTORY_CATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::mutexes {

void add_mutex_factory_category(
    language::plugins::RawRegistry& raw_registry);

}

#endif