#include "downward/utils/exceptions.h"

#include <iostream>
#include <print>

using namespace std;

namespace downward::utils {

void Exception::print(std::ostream& out) const
{
    std::print(out, "{}", this->what());
}

} // namespace utils
