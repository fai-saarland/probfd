import budget_compilation
import sas_tasks


def compile_finite_horizon_maximization(task: sas_tasks.SASTask, horizon: int):
    budget_compilation.augment_task_by_budget(
        task, horizon, budget_compilation.BudgetCostType.ONE
    )
    task.set_goal(budget_compilation.BUDGET_VAR, 0)

