#include "downward/utils/strings.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>

using namespace std;

namespace downward::utils {

string tolower(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

} // namespace utils
