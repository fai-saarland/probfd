#pragma once

#include "../analysis_result.h"

namespace probabilistic {

template<typename State>
class IMDPEngine {
public:
    virtual ~IMDPEngine() = default;
    virtual void reset_solver_state() {}
    virtual AnalysisResult solve(const State& state) = 0;
};

} // namespace probabilistic
