#ifndef PROBFD_PROGRESS_REPORT_H
#define PROBFD_PROGRESS_REPORT_H

#include "probfd/interval.h"

#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace probfd {

/// A function that prints something to an output stream.
using Printer = std::function<void(std::ostream&)>;

/// A function that produces a state value bound.
using BoundProperty = std::function<Interval()>;

/**
 * @brief A registry for print functions related to search progress.
 *
 * This class manages a list of printers (functions accepting an output stream)
 * as well as a list of named bound properties (functions returning a
 * bounding interval for some quantity, e.g. the optimal state value of the
 * initial state).
 *
 * The bound properties are printed before the out of the registered printers.
 * Within these groups, the output is printed in a FIFO manner, i.e. the printer
 * that was registered first is also printed first.
 */
class ProgressReport {
    struct BoundPropertyInfo {
        std::string name;
        BoundProperty property;
        std::optional<Interval> last_printed = std::nullopt;
    };

    const std::optional<value_t> tolerance_;
    bool enabled_;
    std::ostream& out_;

    std::vector<BoundPropertyInfo> bound_infos_;
    std::vector<Printer> additional_informations_;

public:
    /**
     * @brief Construct a new progress report.
     *
     * @param tolerance - An optional tolerance for bound property changes. If
     * no bound property changes by more than this value (lower or upper
     * bound), no output is produced.
     * @param out - The output stream to direct the output to.
     * @param enabled - Whether printing is enabled or not.
     */
    ProgressReport(
        std::optional<value_t> tolerance = std::nullopt,
        std::ostream& out = std::cout,
        const bool enabled = true);

    ~ProgressReport() = default;

    /**
     * @brief Prints the output to the internal output stream, even if disabled
     * and a bound change tolerance is set.
     */
    void force_print();

    /**
     * @brief Prints the output to the internal output stream, if enabled.
     *
     * If a bound change tolerance is set, only prints if any lower or upper
     * bound of any bound property changed by more than the specified tolerance.
     */
    void print();

    /**
     * @brief Enables printing.
     */
    void enable();

    /**
     * @brief Disables printing.
     */
    void disable();

    /**
     * @brief Appends a new printer to the list of printers.
     */
    void register_print(Printer f);

    /**
     * @brief Appends a new bound property with a given name to the list of
     * bound properties to be printed when the report is advanced.
     */
    void
    register_bound(const std::string& property_name, BoundProperty property);

private:
    void print_progress();
    bool advance_values(bool force = false);
};

} // namespace probfd

#endif // __PROGRESS_REPORT_H__