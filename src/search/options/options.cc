#include "options/options.h"

using namespace std;

namespace options {
Options::Options(bool help_mode)
    : unparsed_config("<missing>"),
      help_mode(help_mode) {
}

int Options::get_enum(const string &key) const {
    return get<int>(key);
}

bool Options::contains(const string &key) const {
    return storage.find(key) != storage.end();
}

const string &Options::get_unparsed_config() const {
    return unparsed_config;
}

void Options::set_unparsed_config(const string &config) {
    unparsed_config = config;
}

std::vector<std::string> Options::get_keys() const
{
    std::vector<std::string> keys; keys.reserve(storage.size());
    for (auto it = storage.begin(); it != storage.end(); it++) {
        keys.emplace_back(it->first);
    }
    return keys;
}

}
