#include "legacy/merge_and_shrink/linear_merge_strategy.h"

#include "legacy/merge_and_shrink/abstraction.h"

#include "legacy/globals.h"

#include "utils/rng.h"

#include "option_parser.h"
#include "plugin.h"

#include "task_utils/causal_graph.h"

#include <cassert>
#include <cstdlib>
#include <map>
#include <vector>

using namespace std;

namespace legacy {
namespace merge_and_shrink {

MultipleLinearMergeStrategy::MultipleLinearMergeStrategy(
    const std::vector<std::shared_ptr<MergeCriterion>>& merge_criteria,
    const MergeOrder& merge_order,
    bool /*is_first*/)
    : criteria(merge_criteria)
    , order(merge_order)
{

    for (size_t i = 0; i < merge_criteria.size(); ++i) {
        merge_criteria[i]->disable_incremental();
        merge_criteria[i]->init();
    }
}

MultipleLinearMergeStrategy::~MultipleLinearMergeStrategy()
{
}

void MultipleLinearMergeStrategy::next_vars(
    vector<int>& candidate_vars,
    Abstraction* abstraction) const
{
    // Apply the criteria in order, until its finished or there is only one
    // remaining variable
    for (size_t i = 0; candidate_vars.size() > 1 && i < criteria.size(); ++i) {
        criteria[i]->filter(candidate_vars, abstraction);
    }

    // Order the remaining variables based on the different criteria
    // std::sort(candidate_vars.begin(), candidate_vars.end(),
    //	    this->cmp);

    cout << "Candidates: ";
    for (size_t i = 0; i < candidate_vars.size(); i++) {
        cout << candidate_vars[i] << " ";
    }
    cout << endl;

    if (order == RANDOM) {
        legacy::g_rng.shuffle(candidate_vars);
    } else if (order == LEVEL) {
        std::sort(
            candidate_vars.begin(),
            candidate_vars.end(),
            std::greater<int>());
    } else if (order == REVERSE_LEVEL) {
        std::sort(candidate_vars.begin(), candidate_vars.end());
    }
}

bool MultipleLinearMergeStrategy::cmp(const int v1, const int v2)
{
    return v1 < v2;
}

LinearMergeStrategy::LinearMergeStrategy(
    const std::vector<std::shared_ptr<MergeCriterion>>& merge_criteria,
    const MergeOrder& merge_order,
    bool is_first)
    : criteria(merge_criteria)
    , order(merge_order)
{
    int var_count = g_variable_domain.size();
    if (order == REVERSE_LEVEL) {
        for (int i = 0; i < var_count; ++i) {
            remaining_vars.push_back(i);
        }
    } else {
        for (int i = var_count - 1; i >= 0; i--) {
            remaining_vars.push_back(i);
        }
    }

    if (order == RANDOM || !is_first) {
        legacy::g_rng.shuffle(remaining_vars);
    }

    for (size_t i = 0; i < merge_criteria.size(); ++i) {
        merge_criteria[i]->init();
    }
}

LinearMergeStrategy::~LinearMergeStrategy()
{
}

void LinearMergeStrategy::select_next(int var_no)
{
    vector<int>::iterator position =
        find(remaining_vars.begin(), remaining_vars.end(), var_no);
    assert(position != remaining_vars.end());
    remaining_vars.erase(position);
    selected_vars.push_back(var_no);
    for (size_t i = 0; i < criteria.size(); ++i) {
        criteria[i]->select_next(var_no);
    }
}

bool LinearMergeStrategy::done() const
{
    return remaining_vars.empty();
}

int LinearMergeStrategy::next(Abstraction* abstraction)
{
    assert(!done());
    vector<int> candidate_vars(remaining_vars);
    // Apply the criteria in order, until its finished or there is only one
    // remaining variable
    for (size_t i = 0; candidate_vars.size() > 1 && i < criteria.size(); ++i) {
        criteria[i]->filter(candidate_vars, abstraction);
    }
    assert(!candidate_vars.empty());

    cout << "Candidates: ";
    for (size_t i = 0; i < candidate_vars.size(); i++) {
        cout << candidate_vars[i] << " ";
    }
    cout << endl;

    int var = candidate_vars[0];
    select_next(var);
    return var;
}

void MergeCriterionCG::init()
{
    is_causal_predecessor.resize(g_variable_domain.size(), false);
}
void MergeCriterionCG::select_next(int var_no)
{
    if (allow_incremental) {
        const vector<int>& new_vars = g_causal_graph->get_predecessors(var_no);
        for (size_t i = 0; i < new_vars.size(); ++i) {
            is_causal_predecessor[new_vars[i]] = true;
        }
    }
}
void MergeCriterionCG::filter(std::vector<int>& vars, Abstraction* abstraction)
{
    if (!abstraction) {
        return;
    }
    if (!allow_incremental) {
        is_causal_predecessor.resize(g_variable_domain.size(), false);
        const vector<int>& varset = abstraction->get_varset();
        for (size_t v = 0; v < varset.size(); v++) {
            const vector<int>& new_vars =
                g_causal_graph->get_predecessors(varset[v]);
            for (size_t i = 0; i < new_vars.size(); ++i) {
                is_causal_predecessor[new_vars[i]] = true;
            }
        }
    }

    // cout << "variables Relevant: ";
    // for(int i = 0; i < is_causal_predecessor.size(); i++)
    // if(is_causal_predecessor[i]) cout << i << " "; cout << endl;
    MergeCriterion::filter(vars, is_causal_predecessor);
}

void MergeCriterionGoal::init()
{
    is_goal_variable.resize(g_variable_domain.size(), false);
    for (size_t i = 0; i < g_goal.size(); ++i) {
        is_goal_variable[g_goal[i].first] = true;
    }
}
void MergeCriterionGoal::select_next(int /*var_no*/)
{
}
void MergeCriterionGoal::filter(
    std::vector<int>& vars,
    Abstraction* /*abstraction*/)
{
    MergeCriterion::filter(vars, is_goal_variable);
}

void MergeCriterionMinSCC::init()
{
    is_causal_predecessor.resize(g_variable_domain.size(), false);
    if (reverse) {
        scc.compute_scc(g_causal_graph->get_inverse_arcs());
    } else {
        scc.compute_scc(g_causal_graph->get_arcs());
    }

    // for(int i = 0; i < scc.get_sccs().size(); i++){
    //   cout << "SCC ("<< scc.get_scc_layer()[i] << "): ";
    //   for(int j = 0; j < scc.get_sccs()[i].size(); j++){
    //     cout << scc.get_sccs()[i][j] << " ";
    //   }
    //   cout << endl;
    // }
}

void MergeCriterionMinSCC::select_next(int var_no)
{
    if (allow_incremental) {
        const vector<int>& new_vars = g_causal_graph->get_predecessors(var_no);
        for (size_t i = 0; i < new_vars.size(); ++i) {
            is_causal_predecessor[new_vars[i]] = true;
        }
    }
}
void MergeCriterionMinSCC::forbid_scc_descendants(
    int scc_index,
    const vector<set<int>>& scc_graph,
    vector<bool>& forbidden_sccs) const
{
    const set<int>& descendants = scc_graph[scc_index];
    for (set<int>::iterator it = descendants.begin(); it != descendants.end();
         ++it) {
        if (!forbidden_sccs[*it]) {
            forbidden_sccs[*it] = true;
            forbid_scc_descendants(*it, scc_graph, forbidden_sccs);
        }
    }
}
void MergeCriterionMinSCC::filter(
    std::vector<int>& vars,
    Abstraction* abstraction)
{
    if (!abstraction) {
        return;
    }
    if (!allow_incremental) {
        is_causal_predecessor.resize(g_variable_domain.size(), false);
        const vector<int>& varset = abstraction->get_varset();
        for (size_t v = 0; v < varset.size(); v++) {
            const vector<int>& new_vars =
                g_causal_graph->get_predecessors(varset[v]);
            for (size_t i = 0; i < new_vars.size(); ++i) {
                is_causal_predecessor[new_vars[i]] = true;
            }
        }
    }

    if (!MergeCriterion::filter(vars, is_causal_predecessor)) {
        return; // No CG relevant vars => we do not prefer any variable over
                // another
    }

    const vector<set<int>>& scc_graph = scc.get_scc_graph();
    const vector<int>& vars_scc = scc.get_vars_scc();
    vector<bool> forbidden_sccs(scc_graph.size(), false);
    // In each SCC,select only the variable whose "level" is "closer to the
    // root" We consider a variable closer to the root if it has lower id If
    // reverse is activated, we consider the opposite order
    map<int, int> vars_by_scc;
    // 1) forbid all sccs pointed by scc_var
    for (size_t i = 0; i < vars.size(); i++) {
        int var = vars[i];
        int scc_var = vars_scc[var];
        if (!forbidden_sccs[scc_var]) {
            forbid_scc_descendants(scc_var, scc_graph, forbidden_sccs);
            if (!vars_by_scc.count(scc_var) ||
                (!reverse && vars_by_scc[scc_var] > var) ||
                (reverse && vars_by_scc[scc_var] < var)) {
                vars_by_scc[scc_var] = var;
            }
        }
    }

    // 2) Filter all variables whose scc has been forbidden.
    vector<int> new_vars;
    if (tie_by_level) { // For valid sccs, include the selected variable
        for (map<int, int>::iterator it = vars_by_scc.begin();
             it != vars_by_scc.end();
             ++it) {
            if (!forbidden_sccs[it->first]) {
                new_vars.push_back(it->second);
            }
        }
    } else {
        // For valid sccs, include all variables
        for (size_t i = 0; i < vars.size(); i++) {
            int var = vars[i];
            int scc_var = vars_scc[vars[i]];
            if (!forbidden_sccs[scc_var]) {
                new_vars.push_back(var);
            }
        }
    }
    vars.swap(new_vars);
    /* cout << "Candidate variable after scc: ";
    for(int i = 0; i < vars.size(); i++)
      cout << vars[i] << " ";
      cout << endl;*/
}

void MergeCriterionEmpty::init()
{
}
void MergeCriterionEmpty::select_next(int /*var_no*/)
{
}
void MergeCriterionEmpty::filter(
    std::vector<int>& vars,
    Abstraction* abstraction)
{
    if (abstraction) { // Check if abstraction exists, because the first
                       // variable is selected without abstraction
        const vector<double>& score =
            abstraction->get_count_transitions_var_empty();
        // for(int i = 0; i < vars.size(); i++)
        //   cout << "ScoreEmpty " << vars[i] << ": " << score[vars[i]] << " "
        //   << endl;
        MergeCriterion::filter_best(vars, score, false);
    }
}

void MergeCriterionEmptyGoal::init()
{
}
void MergeCriterionEmptyGoal::select_next(int /*var_no*/)
{
}
void MergeCriterionEmptyGoal::filter(
    std::vector<int>& vars,
    Abstraction* abstraction)
{
    if (abstraction) { // Check if abstraction exists, because the first
                       // variable is selected without abstraction
        const vector<double>& score =
            abstraction->get_count_transitions_var_empty_goal();
        // for(int i = 0; i < vars.size(); i++)
        //   cout << "ScoreEmptyGoal " << vars[i] << ": " << score[vars[i]] << "
        //   " << endl;
        MergeCriterion::filter_best(vars, score, false);
    }
}

void MergeCriterionNum::init()
{
}
void MergeCriterionNum::select_next(int /*var_no*/)
{
}
void MergeCriterionNum::filter(std::vector<int>& vars, Abstraction* abstraction)
{
    if (abstraction) { // Check if abstraction exists, because the first
                       // variable is selected without abstraction
        const vector<double>& score = abstraction->get_count_transitions_var();
        // for(int i = 0; i < vars.size(); i++)
        //   cout << "ScoreNum " << vars[i] << ": " << score[vars[i]] << " " <<
        //   endl;
        MergeCriterion::filter_best(vars, score, false);
    }
}

void MergeCriterionRelevant::init()
{
    MergeCriterionCG::init();
    for (size_t i = 0; i < g_goal.size(); ++i) {
        is_causal_predecessor[g_goal[i].first] = true;
    }
}

template <class T>
static std::shared_ptr<MergeCriterion> _parse(options::OptionParser& parser)
{
    if (parser.dry_run()) {
        return 0;
    } else {
        return std::make_shared<T>();
    }
}

// TODO: This should be removed, because it is equivalent to scc(level=false)
static std::shared_ptr<MergeCriterion>
_parse_scc_no_level(options::OptionParser& parser)
{
    if (parser.dry_run()) {
        return 0;
    } else {
        return std::make_shared<MergeCriterionMinSCC>(false, false);
    }
}

MergeCriterionMinSCC::MergeCriterionMinSCC(const options::Options& opts)
    : reverse(opts.get<bool>("reverse"))
    , tie_by_level(opts.get<bool>("level"))
{
}

static std::shared_ptr<MergeCriterion> _parse_scc(options::OptionParser& parser)
{
    parser.add_option<bool>("reverse", "", "false");
    parser.add_option<bool>("level", "", "false");
    options::Options opts = parser.parse();

    if (parser.dry_run()) {
        return 0;
    } else {
        return std::make_shared<MergeCriterionMinSCC>(opts);
    }
}

static PluginTypePlugin<MergeCriterion>
    _plugin_type("LegacyLinearMergeCriterion", "");
static Plugin<MergeCriterion> _plugin_cg("legacy_cg", _parse<MergeCriterionCG>);
static Plugin<MergeCriterion>
    _plugin_goal("legacy_goal", _parse<MergeCriterionGoal>);
static Plugin<MergeCriterion> _plugin_scc("legacy_scc", _parse_scc);
static Plugin<MergeCriterion>
    _plugin_scc_no_level("legacy_scc_no_level", _parse_scc_no_level);
static Plugin<MergeCriterion>
    _plugin_empty("legacy_empty", _parse<MergeCriterionEmpty>);
static Plugin<MergeCriterion>
    _plugin_empty_goal("legacy_empty_goal", _parse<MergeCriterionEmptyGoal>);
static Plugin<MergeCriterion>
    _plugin_num("legacy_num", _parse<MergeCriterionNum>);
static Plugin<MergeCriterion>
    _plugin_rel("legacy_relevant", _parse<MergeCriterionRelevant>);

} // namespace merge_and_shrink
} // namespace legacy