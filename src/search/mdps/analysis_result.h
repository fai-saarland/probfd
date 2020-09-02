#pragma once

#include "value_type.h"

namespace probabilistic {

struct IPrintable;

struct AnalysisResult {
    AnalysisResult();
    AnalysisResult(
        const value_type::value_t& result,
        IPrintable* statistics = nullptr);
    AnalysisResult(AnalysisResult&& other);
    AnalysisResult(const AnalysisResult& other) = delete;

    ~AnalysisResult();

    AnalysisResult& operator=(AnalysisResult&& other);
    AnalysisResult& operator=(const AnalysisResult&) = delete;

    value_type::value_t result;
    IPrintable* statistics;
};

} // namespace probabilistic
