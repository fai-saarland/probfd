#ifndef MDPS_PROGRESS_REPORT_H
#define MDPS_PROGRESS_REPORT_H

#include "probfd/value_type.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace probfd {

/**
 * @brief A registry for print functions related to search progress.
 *
 * Example
 * =======
 *
 ```
class MyEngine : public MDPEngine<State, OperatorID>
{
    // Constructors and other methods...

    value_t solve(const State& state) {
        int iterations = 0;

        // Print the number of iterations each time the
        // search progress is displayed.
        report.register_print([&iterations](std::ostream& out){
            out << "Current number of iterations: " << iterations << std::endl;
        });

        while (...) {
            // Do some stuff...

            report();
            ++iterations;
        }

        return ...;
    }

private:
    ProgressReport report;
};
 ```
 */
class ProgressReport {
public:
    ProgressReport(
        const value_t min_change,
        std::ostream& out = std::cout,
        const bool enabled = true);
    ~ProgressReport() = default;

    void print();
    void operator()();

    void enable();
    void disable();

    void register_print(std::function<void(std::ostream&)> f);

    void register_value(
        const std::string& val_name,
        std::function<value_t()> getter);

private:
    void print_progress();
    bool extract_values();

    const value_t min_change_;
    bool enabled_;
    std::ostream* out_;

    std::vector<std::string> value_names_;
    std::vector<std::function<value_t()>> value_getters_;
    std::vector<value_t> last_printed_values_;
    std::vector<value_t> extracted_values_;
    std::vector<std::function<void(std::ostream&)>> additional_informations_;
};

} // namespace probfd

#endif // __PROGRESS_REPORT_H__