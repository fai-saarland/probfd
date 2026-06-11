#include "language/plugins/options.h"

using namespace std;

namespace language::plugins {

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
} // namespace language::plugins
