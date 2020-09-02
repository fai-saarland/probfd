#include "analysis_result.h"
#include "interfaces/i_printable.h"

#include <utility>

namespace probabilistic {

AnalysisResult::AnalysisResult()
    : result()
    , statistics(nullptr)
{
}

AnalysisResult::AnalysisResult(
    const value_type::value_t& result,
    IPrintable* statistics)
    : result(result)
    , statistics(statistics)
{
}

AnalysisResult::AnalysisResult(AnalysisResult&& other)
    : result(std::move(other.result))
    , statistics(std::move(other.statistics))
{
    other.statistics = nullptr;
}

AnalysisResult::~AnalysisResult()
{
    if (statistics != nullptr) {
        delete (statistics);
    }
}

AnalysisResult&
AnalysisResult::operator=(AnalysisResult&& other)
{
    result = std::move(other.result);
    statistics = std::move(other.statistics);
    other.statistics = nullptr;
    return *this;
}

} // namespace probabilistic
