#include "downward/utils/exceptions.h"

#include <iostream>

using namespace std;

namespace downward::utils {
Exception::Exception(const string& msg)
    : msg(msg)
{
}

string Exception::get_message() const
{
    return msg;
}

void Exception::print() const
{
    cerr << msg << endl;
}
} // namespace utils
