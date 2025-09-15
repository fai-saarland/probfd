#ifndef DOWNWARD_CLI_MUTEXES_FROM_FILE_MUTEX_FACTORY_FEATURE_H
#define DOWNWARD_CLI_MUTEXES_FROM_FILE_MUTEX_FACTORY_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::mutexes {

void add_from_file_mutex_factory_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif