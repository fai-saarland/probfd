#include "probfd/progress_report.h"

#include "utils/timer.h"

namespace probfd {

ProgressReport::ProgressReport(
    const value_type::value_t min_change,
    std::ostream& out,
    const bool enabled)
    : min_change_(min_change)
    , enabled_(enabled)
    , out_(&out)
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

void ProgressReport::register_value(
    const std::string& val_name,
    std::function<value_type::value_t()> getter)
{
    value_names_.push_back(val_name);
    value_getters_.push_back(getter);
    extracted_values_.push_back(value_type::zero);
}

void ProgressReport::print()
{
    extract_values();
    if (!extracted_values_.empty()) {
        print_progress();
    }
}

void ProgressReport::print_progress()
{
    (*out_) << "[";
    for (unsigned i = 0; i < value_names_.size(); i++) {
        (*out_) << (i > 0 ? ", " : "") << value_names_[i] << "="
                << extracted_values_[i];
    }
    //(*out_) << (value_names_.empty() ? "" : ", ")
    //        << "registered=" << g_state_registry->size();
    for (unsigned i = 0; i < additional_informations_.size(); i++) {
        (*out_) << ", ";
        additional_informations_[i](*out_);
    }
    (*out_) << ", t=" << utils::g_timer << "]"
            << "\n";
}

bool ProgressReport::extract_values()
{
    bool print = last_printed_values_.size() != value_getters_.size();
    last_printed_values_.resize(value_getters_.size(), value_type::zero);
    for (int i = value_getters_.size() - 1; i >= 0; --i) {
        const value_type::value_t val = value_getters_[i]();
        print = print ||
                (value_type::abs(last_printed_values_[i] - val) >= min_change_);
        extracted_values_[i] = val;
    }
    return print;
}

void ProgressReport::operator()()
{
    if (!enabled_) {
        return;
    }
    if (extract_values()) {
        print_progress();
        last_printed_values_.swap(extracted_values_);
    }
}

} // namespace probfd
