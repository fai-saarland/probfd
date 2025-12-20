#include "language/plugins/options.h"

using namespace std;

namespace language::plugins {

Options::Options(std::size_t num_opts)
    : storage(num_opts)
{
}

std::any Options::get_raw(std::size_t i, const Context& context) const
{
    if (i >= storage.size()) {
        context.error("Attempt to retrieve non-existing object at index {}", i);
    }

    if (const auto& obj = storage[i]; obj.has_value()) { return obj; }

    context.error("Attempt to retrieve unset object at index {}", i);
}

bool Options::contains(size_t i, const Context& context) const
{
    if (i >= storage.size()) {
        context.error("Attempt to retrieve non-existing object at index {}", i);
    }

    return storage[i].has_value();
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
