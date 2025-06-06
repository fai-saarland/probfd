#include "downward/utils/memory.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace downward::utils {
static char* extra_memory_padding = nullptr;
static bool has_gone_out_of_memory = false;

// Save standard out-of-memory handler.
static void (*standard_out_of_memory_handler)() = nullptr;

static void continuing_out_of_memory_handler()
{
    release_extra_memory_padding();
    cout << "Failed to allocate memory. Released extra memory padding." << endl;
    has_gone_out_of_memory = true;
}

void reserve_extra_memory_padding(int memory_in_mb)
{
    assert(!extra_memory_padding);
    extra_memory_padding = new char[memory_in_mb * 1024 * 1024];
    standard_out_of_memory_handler =
        set_new_handler(continuing_out_of_memory_handler);
    has_gone_out_of_memory = false;
}

void release_extra_memory_padding()
{
    assert(extra_memory_padding);
    delete[] extra_memory_padding;
    extra_memory_padding = nullptr;
    assert(standard_out_of_memory_handler);
    set_new_handler(standard_out_of_memory_handler);
}

bool extra_memory_padding_is_reserved()
{
    return extra_memory_padding;
}

bool is_out_of_memory()
{
    return has_gone_out_of_memory;
}
} // namespace utils
