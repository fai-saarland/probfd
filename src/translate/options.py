import argparse
import sys

from fractions import Fraction

from typing import Optional


def parse_args():
    argparser = argparse.ArgumentParser()
    argparser.add_argument("domain", help="path to domain pddl file")
    argparser.add_argument("task", help="path to task pddl file")
    argparser.add_argument("--relaxed",
                           dest="generate_relaxed_task",
                           action="store_true",
                           help="output relaxed task (no delete effects)")
    argparser.add_argument(
        "--full-encoding",
        dest="use_partial_encoding",
        action="store_false",
        help="By default we represent facts that occur in multiple "
             "mutex groups only in one variable. Using this parameter adds "
             "these facts to multiple variables. This can make the meaning "
             "of the variables clearer, but increases the number of facts.")
    argparser.add_argument(
        "--invariant-generation-max-candidates",
        default=100000,
        type=int,
        help="max number of candidates for invariant generation "
             "(default: %(default)d). Set to 0 to disable invariant "
             "generation and obtain only binary variables. The limit is "
             "needed for grounded input files that would otherwise produce "
             "too many candidates.")
    argparser.add_argument(
        "--sas-file",
        default="output.psas",
        help="path to the probabilistic SAS output file (default: %(default)s)")
    argparser.add_argument(
        "--invariant-generation-max-time",
        default=300,
        type=int,
        help="max time for invariant generation (default: %(default)ds)")
    argparser.add_argument(
        "--add-implied-preconditions",
        action="store_true",
        help="infer additional preconditions. This setting can cause a "
             "severe performance penalty due to weaker relevance analysis "
             "(see issue7).")
    argparser.add_argument(
        "--keep-unreachable-facts",
        dest="filter_unreachable_facts",
        action="store_false",
        help="keep facts that can't be reached from the initial state")
    argparser.add_argument(
        "--skip-variable-reordering",
        dest="reorder_variables",
        action="store_false",
        help="do not reorder variables based on the causal graph. Do not use "
             "this option with the causal graph heuristic!")
    argparser.add_argument(
        "--keep-unimportant-variables",
        dest="filter_unimportant_vars",
        action="store_false",
        help="keep variables that do not influence the goal in the causal graph"
    )
    argparser.add_argument("--drop-unimportant-operators",
                           dest="filter_unimportant_ops",
                           action="store_true",
                           help="keep operators with empty effect",
                           default=False)
    argparser.add_argument(
        "--dump-task",
        action="store_true",
        help="dump human-readable SAS+ representation of the task")
    argparser.add_argument(
        "--force-probabilistic",
        action="store_true",
        default=False,
        help="Enforces probabilistic sas output, even if all operators are deterministic")
    argparser.add_argument("--budget",
                           type=int,
                           default=None,
                           help="Budget compilation with given initial budget")
    argparser.add_argument("--budget-cost-type",
                           default="cost",
                           choices=["cost", "one", "plus_one", "min_one"],
                           help="How actions affect the budget")
    argparser.add_argument("--give-up-cost", default=None,
                           type=Optional[Fraction])
    argparser.add_argument(
        "--discount-factor",
        default=None,
        type=Optional[Fraction],
        help=
        "Discounted SSP compilation with given discount factor in range (0, 1)"
    )
    return argparser.parse_args()


def copy_args_to_module(args):
    module_dict = sys.modules[__name__].__dict__
    for key, value in vars(args).items():
        module_dict[key] = value


def setup():
    args = parse_args()
    copy_args_to_module(args)


setup()
