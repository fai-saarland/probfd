#include "probfd/progress_report.h"

#include "utils/timer.h"

#include <cassert>

namespace probfd {

ProgressReport::ProgressReport(
    std::optional<value_t> tolerance,
    std::ostream& out,
    const bool enabled)
    : tolerance_(tolerance)
    , enabled_(enabled)
    , out_(out)
{
}

void ProgressReport::enable()
{
    enabled_ = true;
}

void ProgressReport::disable()
{
    enabled_ = false;
}

void ProgressReport::register_print(std::function<void(std::ostream&)> f)
{
    additional_informations_.push_back(f);
}

void ProgressReport::register_bound(
    const std::string& val_name,
    BoundProperty property)
{
    bound_infos_.emplace_back(val_name, property, Interval(0_vt));
}

void ProgressReport::force_print()
{
    advance_values(true);
    print_progress();
}

void ProgressReport::print()
{
    if (enabled_ && advance_values()) {
        print_progress();
    }
}

void ProgressReport::print_progress()
{
    out_ << "[";
    for (unsigned i = 0; i < bound_infos_.size(); i++) {
        const auto& info = bound_infos_[i];
        assert(info.last_printed.has_value());
        out_ << (i > 0 ? ", " : "") << info.name << "=" << *info.last_printed;
    }
    for (unsigned i = 0; i < additional_informations_.size(); i++) {
        out_ << ", ";
        additional_informations_[i](out_);
    }
    out_ << ", t=" << utils::g_timer << "]"
         << "\n";
}

bool ProgressReport::advance_values(bool force)
{
    bool print = force;

    std::vector<Interval> next_values;
    next_values.reserve(bound_infos_.size());

    for (auto& info : bound_infos_) {
        const Interval next_val = info.property();
        next_values.push_back(next_val);

        if (!info.last_printed || !tolerance_) {
            print = true;
            continue;
        }

        const Interval prev_val = *info.last_printed;
        print = print ||
                !is_approx_equal(next_val.lower, prev_val.lower, *tolerance_) ||
                !is_approx_equal(next_val.upper, prev_val.upper, *tolerance_);
    }

    if (print) {
        for (size_t i = 0; i != bound_infos_.size(); ++i) {
            bound_infos_[i].last_printed = next_values[i];
        }
    }

    return print;
}

} // namespace probfd
