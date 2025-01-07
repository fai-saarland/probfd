*Probabilistic Fast Downward* (probFD) is an extension of the Fast Downward planning system for fully observable
probabilistic planning domains.

# Overview

## Problem Description Language
ProbFD supports probabilistic planning problems modelled in probabilistic PPDL.
Both minimization and maximization of a single objective function is supported.
The planner is capable of optimizing for either expected rewards or costs, depending on the requirements
specified by the PPDDL input file.
Rewards are translated into costs internally.

## Algorithms
ProbFD supports a variety of different search algorithms to compute optimal policies, for example:

- Exhaustive Algorithms
  + Topological Value Iteration
- Heuristic Search Algorithms
  - Find-and-Revise (F&R) Framework
    + Labelled real-time dynamic programming (LRTDP)
    + Improved LAO* (iLAO*)
    + Heuristic Depth-First Search (HDP)
  - Find-Revise-Eliminate-Traps (FRET) Framework
    + Vanilla FRET with inner F&R algorithm
    + Trap elimination procedure within the F&R algorithm (trap-aware F&R variants)
  - Algorithms based on Linear Programming
    + i-Dual
    + i²-Dual

By default, all algorithms based on asynchronous value iteration use $\epsilon$-consistency in their convergence tests.
While this is usually not problematic in practice, strictly speaking this convergence criterion may lead to suboptimal
policies.
For reward-based problems, these algorithms can be also be configured to maintain a lower and upper-bounding and
use $\epsilon$-closeness of the value functions to guarantee convergence against the optimal policy.

## Heuristics

The planner currently implements the following families of heuristics:

- Determinization-based heuristics synthesized from a classical planning heuristic computed on the all-outcomes
  determinization of the planning task
- Occupation Measure Heuristics
  + The regrouped operator counting heuristic $h^{roc}$
  + The projection occupation measure heuristic $h^{pom}$
- Pattern Database Heuristics for SSPs
  + Pattern generation using a classical pattern generation algorithm on the all-outcomes determinization of the
    planning task
  + Pattern generation through policy-based CEGAR
  + Pattern generation via local search
- Cartesian Abstraction Heuristics for SSPs
- ~~Merge-and-Shrink Heuristics for SSPs~~ (*not merged into master yet*)
- Cost Partitioning Heuristics
  + Saturated Cost Partitioning over probabilistic PDBs
  + Uniform Cost Partitioning over probabilistic PDBs
  + Post-Hoc Optimization over probabilistic PDBs