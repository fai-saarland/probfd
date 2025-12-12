#ifndef DOWNWARD_CLI_MUTEXES_FROM_FILE_MUTEX_FACTORY_FEATURE_H
#define DOWNWARD_CLI_MUTEXES_FROM_FILE_MUTEX_FACTORY_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::mutexes {

void add_from_file_mutex_factory_feature(
    language::plugins::Registry& registry);

}

#endif