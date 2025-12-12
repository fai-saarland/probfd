#include "downward/utils/logging.h"

using namespace std;

namespace downward::utils {
/*
  NOTE: When adding more options to Log, make sure to adapt the if block in
  get_log_from_options below to test for *all* default values used for
  global_log here. Also add the options to dump_options().
*/

static shared_ptr<Log> global_log = make_shared<Log>(Verbosity::NORMAL);

LogProxy get_log_for_verbosity(const Verbosity& verbosity)
{
    if (verbosity == Verbosity::NORMAL) { return LogProxy(global_log); }
    return LogProxy(make_shared<Log>(verbosity));
}

LogProxy get_silent_log()
{
    return utils::get_log_for_verbosity(utils::Verbosity::SILENT);
}

} // namespace downward::utils
