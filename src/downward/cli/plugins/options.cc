#include "downward/cli/plugins/options.h"

using namespace std;

namespace downward::cli::plugins {

bool Options::contains(const string& key) const
{
    return storage.contains(key);
}

const string& Options::get_unparsed_config() const
{
    return unparsed_config;
}

void Options::set_unparsed_config(const string& config)
{
    unparsed_config = config;
}
} // namespace downward::cli::plugins
