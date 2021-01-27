# See http://www.fast-downward.org/ForDevelopers/AddingSourceFiles
# for general information on adding source files and CMake plugins.
#
# All plugins are enabled by default and users can disable them by specifying
#    -DPLUGIN_FOO_ENABLED=FALSE
# The default behavior can be changed so all non-essential plugins are
# disabled by default by specifying
#    -DDISABLE_PLUGINS_BY_DEFAULT=TRUE
# In that case, individual plugins can be enabled with
#    -DPLUGIN_FOO_ENABLED=TRUE
#
# Defining a new plugin:
#    fast_downward_plugin(
#        NAME <NAME>
#        [ DISPLAY_NAME <DISPLAY_NAME> ]
#        [ HELP <HELP> ]
#        SOURCES
#            <FILE_1> [ <FILE_2> ... ]
#        [ DEPENDS <PLUGIN_NAME_1> [ <PLUGIN_NAME_2> ... ] ]
#        [ DEPENDENCY_ONLY ]
#        [ CORE_PLUGIN ]
#    )
#
# <DISPLAY_NAME> defaults to lower case <NAME> and is used to group files
#   in IDEs and for messages.
# <HELP> defaults to <DISPLAY_NAME> and is used to describe the cmake option.
# SOURCES lists the source files that are part of the plugin. Entries are
#   listed without extension. For an entry <file>, both <file>.h and <file>.cc
#   are added if the files exist.
# DEPENDS lists plugins that will be automatically enabled if this plugin is
#   enabled. If the dependency was not enabled before, this will be logged.
# DEPENDENCY_ONLY disables the plugin unless it is needed as a dependency and
#   hides the option to enable the plugin in cmake GUIs like ccmake.
# CORE_PLUGIN always enables the plugin (even if DISABLE_PLUGINS_BY_DEFAULT
#   is used) and hides the option to disable it in CMake GUIs like ccmake.

option(
    DISABLE_PLUGINS_BY_DEFAULT
    "If set to YES only plugins that are specifically enabled will be compiled"
    NO)
# This option should not show up in CMake GUIs like ccmake where all
# plugins are enabled or disabled manually.
mark_as_advanced(DISABLE_PLUGINS_BY_DEFAULT)

fast_downward_plugin(
    NAME CORE_SOURCES
    HELP "Core source files"
    SOURCES
        planner

        command_line
        option_parser
        option_parser_util
        plugin
        solver_interface
        search_engine

        axioms
        causal_graph
        domain_transition_graph
        global_operator
        global_state
        partial_state
        operator_cost

        state_id
        state_registry
        per_state_information
        evaluator
        heuristic

        search_engine
        search_node_info
        search_progress
        search_space

        fact_set
        strips
        state_component_listener

        globals

    DEPENDS INT_HASH_SET INT_PACKER ORDERED_SET SEGMENTED_VECTOR SUBSCRIBER
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME OPTIONS
    HELP "Option parsing and plugin definition"
    SOURCES
        options/any
        options/bounds
        options/doc_printer
        options/doc_utils
        options/errors
        options/option_parser
        options/options
        options/parse_tree
        options/predefinitions
        options/plugin
        options/raw_registry
        options/registries
        options/type_namer
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME UTILS
    HELP "System utilities"
    SOURCES
        utils/collections
        utils/countdown_timer
        utils/exceptions
        utils/hash
        utils/language
        utils/logging
        utils/markup
        utils/math
        utils/memory
        utils/rng
        utils/rng_options
        utils/strings
        utils/system
        utils/system_unix
        utils/system_windows
        utils/timer
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME DYNAMIC_BITSET
    HELP "Poor man's version of boost::dynamic_bitset"
    SOURCES
        algorithms/dynamic_bitset
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME EQUIVALENCE_RELATION
    HELP "Equivalence relation over [1, ..., n] that can be iteratively refined"
    SOURCES
        algorithms/equivalence_relation
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME INT_HASH_SET
    HELP "Hash set storing non-negative integers"
    SOURCES
        algorithms/int_hash_set
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME INT_PACKER
    HELP "Greedy bin packing algorithm to pack integer variables with small domains tightly into memory"
    SOURCES
        algorithms/int_packer
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME MAX_CLIQUES
    HELP "Implementation of the Max Cliques algorithm by Tomita et al."
    SOURCES
        algorithms/max_cliques
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PRIORITY_QUEUES
    HELP "Three implementations of priority queue: HeapQueue, BucketQueue and AdaptiveQueue"
    SOURCES
        algorithms/priority_queues
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME ORDERED_SET
    HELP "Set of elements ordered by insertion time"
    SOURCES
        algorithms/ordered_set
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SEGMENTED_VECTOR
    HELP "Memory-friendly and vector-like data structure"
    SOURCES
        algorithms/segmented_vector
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SUBSCRIBER
    HELP "Allows object to subscribe to the destructor of other objects"
    SOURCES
        algorithms/subscriber
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SCCS
    HELP "Algorithm to compute the strongly connected components (SCCs) of a "
         "directed graph."
    SOURCES
        algorithms/sccs
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PERFECT_HASHING_FUNCTION
    HELP "Finite domain perfect hash computation"
    SOURCES
        algorithms/perfect_hashing_function
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SUCCESSOR_GENERATOR
    HELP "Successor generator"
    SOURCES
        successor_generator
        successor_generator_factory
        successor_generator_internals
)

fast_downward_plugin(
    NAME OPEN_LISTS
    HELP "open list"
    SOURCES
        open_lists/open_list
        open_lists/open_list_factory
        open_lists/alternation_open_list
        open_lists/standard_scalar_open_list
        open_lists/open_list_buckets
        open_lists/pareto_open_list
        open_lists/tiebreaking_open_list
        open_lists/plugins
)

fast_downward_plugin(
    NAME G_EVALUATOR
    HELP "The g-evaluator"
    SOURCES
        g_evaluator
)

fast_downward_plugin(
    NAME COMBINING_EVALUATOR
    HELP "The combining evaluator"
    SOURCES
        combining_evaluator
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME MAX_EVALUATOR
    HELP "The max evaluator"
    SOURCES
        max_evaluator
    DEPENDS COMBINING_EVALUATOR
)

fast_downward_plugin(
    NAME PREF_EVALUATOR
    HELP "The pref evaluator"
    SOURCES
        pref_evaluator
)

fast_downward_plugin(
    NAME WEIGHTED_EVALUATOR
    HELP "The weighted evaluator"
    SOURCES
        weighted_evaluator
)

fast_downward_plugin(
    NAME SUM_EVALUATOR
    HELP "The sum evaluator"
    SOURCES
        sum_evaluator
    DEPENDS COMBINING_EVALUATOR
)

fast_downward_plugin(
    NAME EAGER_SEARCH
    HELP "Eager search algorithm"
    SOURCES
        eager_search
        DEPENDS SUM_EVALUATOR G_EVALUATOR OPEN_LISTS BLIND_SEARCH_HEURISTIC
)

fast_downward_plugin(
    NAME ENFORCED_HILL_CLIMBING
    HELP "Enforced Hill-Climbing search algorithm"
    SOURCES
        enforced_hill_climbing_search
    DEPENDS SUM_EVALUATOR G_EVALUATOR PREF_EVALUATOR OPEN_LISTS BLIND_SEARCH_HEURISTIC
)

fast_downward_plugin(
    NAME LAZY_SEARCH
    HELP "Eager search algorithm with lazy heuristic evaluation"
    SOURCES
        lazy_search
    DEPENDS SUM_EVALUATOR G_EVALUATOR WEIGHTED_EVALUATOR OPEN_LISTS BLIND_SEARCH_HEURISTIC
)

fast_downward_plugin(
    NAME ITERATED_SEARCH
    HELP "Iterated search algorithm"
    SOURCES
        iterated_search
    DEPENDS 
)

fast_downward_plugin(
    NAME LP_SOLVER
    HELP "Interface to an LP solver"
    SOURCES
        lp/lp_solver
        lp/lp_internals
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME VARIABLE_ORDER_FINDER
    HELP "Variable order finder"
    SOURCES
        variable_order_finder
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SAMPLING
    HELP "Sampling"
    SOURCES
        sampling
    DEPENDS SUCCESSOR_GENERATOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME RELAXATION_HEURISTIC
    HELP "The base class for relaxation heuristics"
    SOURCES
        relaxation_heuristic
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME ADDITIVE_HEURISTIC
    HELP "The additive heuristic"
    SOURCES
        additive_heuristic
    DEPENDS PRIORITY_QUEUES RELAXATION_HEURISTIC
)

fast_downward_plugin(
    NAME CONTEXT_ENHANCED_ADDITIVE_HEURISTIC
    HELP "The context-enhanced additive heuristic"
    SOURCES
        cea_heuristic
    DEPENDS PRIORITY_QUEUES
)

fast_downward_plugin(
    NAME CG_HEURISTIC
    HELP "The causal graph heuristic"
    SOURCES cg_heuristic
            cg_cache
    DEPENDS PRIORITY_QUEUES
)

fast_downward_plugin(
    NAME FF_HEURISTIC
    HELP "The FF heuristic (an implementation of the RPG heuristic)"
    SOURCES
        ff_heuristic
    DEPENDS ADDITIVE_HEURISTIC
)

fast_downward_plugin(
    NAME GOAL_COUNT_HEURISTIC
    HELP "The goal-counting heuristic"
    SOURCES
        goal_count_heuristic
)

fast_downward_plugin(
    NAME HM_HEURISTIC
    HELP "The h^m heuristic"
    SOURCES
        hm_heuristic
)

fast_downward_plugin(
    NAME LANDMARK_CUT_HEURISTIC
    HELP "The LM-cut heuristic"
    SOURCES
        lm_cut_heuristic
    DEPENDS PRIORITY_QUEUES
)

fast_downward_plugin(
    NAME IPC_MAX_HEURISTIC
    HELP "Admissible combination of multiples heuristics via max"
    SOURCES
        ipc_max_heuristic
)

fast_downward_plugin(
    NAME MAX_HEURISTIC
    HELP "The Max heuristic"
    SOURCES
        max_heuristic
    DEPENDS PRIORITY_QUEUES RELAXATION_HEURISTIC
)

fast_downward_plugin(
    NAME BLIND_SEARCH_HEURISTIC
    HELP "The 'blind search' heuristic"
    SOURCES
        blind_search_heuristic
)

fast_downward_plugin(
    NAME MAS_HEURISTIC
    HELP "The Merge-and-Shrink heuristic"
    SOURCES
        merge_and_shrink/relaxation_heuristic
        merge_and_shrink/max_heuristic
        merge_and_shrink/additive_heuristic
        merge_and_shrink/ff_heuristic
        merge_and_shrink/abstraction
        merge_and_shrink/label
        merge_and_shrink/label_reducer
        merge_and_shrink/linear_merge_strategy
        merge_and_shrink/max_regression_operators
        merge_and_shrink/merge_and_shrink_heuristic
        merge_and_shrink/merge_strategy
        merge_and_shrink/relaxed_plan_operators
        merge_and_shrink/scc
        merge_and_shrink/shrink_bisimulation
        merge_and_shrink/shrink_bucket_based
        merge_and_shrink/shrink_empty_labels
        merge_and_shrink/shrink_fh
        merge_and_shrink/shrink_label_subset_bisimulation
        merge_and_shrink/shrink_none
        merge_and_shrink/shrink_random
        merge_and_shrink/shrink_budget_based
        merge_and_shrink/shrink_strategy
        merge_and_shrink/variable_order_finder
        merge_and_shrink/multiple_shrinking_strategies
        merge_and_shrink/utils
    DEPENDS PRIORITY_QUEUES
)

fast_downward_plugin(
    NAME LANDMARKS
    HELP "Plugin containing the code to reason with landmarks"
    SOURCES
        landmarks/exploration
        landmarks/h_m_landmarks
        #landmarks/lama_ff_synergy
        landmarks/landmark_cost_assignment
        landmarks/landmark_count_heuristic
        landmarks/landmark_factory
        landmarks/landmark_factory_rpg_exhaust
        landmarks/landmark_factory_rpg_sasp
        landmarks/landmark_factory_zhu_givan
        landmarks/landmark_graph
        landmarks/landmark_graph_merged
        landmarks/landmark_status_manager
        landmarks/util
    DEPENDS LP_SOLVER PRIORITY_QUEUES SUCCESSOR_GENERATOR
)

fast_downward_plugin(
    NAME OPERATOR_COUNTING
    HELP "Plugin containing the code for operator counting heuristics"
    SOURCES
        operator_counting/constraint_generator
        operator_counting/operator_counting_heuristic
        operator_counting/pho_constraints
        operator_counting/state_equation_constraints
    DEPENDS LP_SOLVER LANDMARK_CUT_HEURISTIC PDBS
)

fast_downward_plugin(
    NAME PDBS
    HELP "Plugin containing the code for PDBs"
    SOURCES
        pdbs/canonical_pdbs
        pdbs/canonical_pdbs_heuristic
        pdbs/dominance_pruning
        pdbs/incremental_canonical_pdbs
        pdbs/match_tree
        pdbs/pattern_cliques
        pdbs/pattern_collection_generator_combo
        pdbs/pattern_collection_generator_genetic
        pdbs/pattern_collection_generator_hillclimbing
        pdbs/pattern_collection_generator_manual
        pdbs/pattern_collection_generator_systematic
        pdbs/pattern_collection_information
        pdbs/pattern_database
        pdbs/pattern_generator
        pdbs/pattern_generator_greedy
        pdbs/pattern_generator_manual
        pdbs/pattern_information
        pdbs/pdb_heuristic
        pdbs/utils
        pdbs/validation
        pdbs/zero_one_pdbs
        pdbs/zero_one_pdbs_heuristic
    DEPENDS CAUSAL_GRAPH MAX_CLIQUES PRIORITY_QUEUES VARIABLE_ORDER_FINDER SAMPLING
)

fast_downward_plugin(
    NAME MDP
    HELP "Core source files for supporting MDPs"
    SOURCES
        mdps/value_type
        mdps/logging
        mdps/evaluation_result
        mdps/globals
        mdps/probabilistic_operator
        mdps/analysis_objective
        mdps/analysis_objectives/goal_probability_objective
        mdps/analysis_objectives/expected_cost_objective
        mdps/action_id_map
        mdps/state_id_map
        mdps/action_evaluator
        mdps/state_evaluator
        mdps/transition_generator
        mdps/solvers/mdp_solver
        mdps/progress_report
        mdps/quotient_system
        mdps/utils/distribution_random_sampler
        mdps/utils/distribution_uniform_sampler
        mdps/bisimulation/bisimilar_state_space
        mdps/heuristics/constant_evaluator
        mdps/heuristics/dead_end_pruning
        mdps/heuristics/budget_pruning
    DEPENDS SUCCESSOR_GENERATOR MAS_HEURISTIC
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME ACYCLIC_VALUE_ITERATION_SOLVER
    HELP "acyclic_vi"
    SOURCES
        mdps/solvers/acyclic_vi
    DEPENDS MDP
)

fast_downward_plugin(
    NAME TOPOLOGICAL_VALUE_ITERATION_SOLVER
    HELP "topological_vi"
    SOURCES
        mdps/solvers/topological_vi
    DEPENDS MDP
)

fast_downward_plugin(
    NAME INTERVAL_ITERATION_SOLVER
    HELP "interval_iteration"
    SOURCES
        mdps/solvers/interval_iteration
    DEPENDS MDP
)

fast_downward_plugin(
    NAME IDUAL_SOLVER
    HELP "idual & i2dual solvers"
    SOURCES
        mdps/solvers/idual
        mdps/solvers/i2dual
    DEPENDS MDP LP_SOLVER OCCUPATION_MEASURE_HEURISTICS
)

fast_downward_plugin(
    NAME BISIMULATION_BASED_SOLVER
    HELP "bisimulation_vi"
    SOURCES
        mdps/solvers/bisimulation_vi
    DEPENDS MDP
)

fast_downward_plugin(
    NAME MDP_HEURISTIC_SEARCH_BASE
    HELP "mdp heuristic search core"
    SOURCES
        mdps/heuristic_search_interfaceable
        mdps/policy_picker
        mdps/successor_sort
        mdps/new_state_handler
        mdps/transition_sampler
        mdps/dead_end_listener
        mdps/open_list
        mdps/open_lists/lifo_open_list
        mdps/open_lists/fifo_open_list
        mdps/open_lists/h_open_list
        mdps/open_lists/lifo_h_open_list
        mdps/open_lists/lifo_preferred_operators_open_list
        mdps/new_state_handlers/store_heuristic
        mdps/new_state_handlers/store_preferred_operators
        mdps/transition_sampler/hbiased_successor_sampler
        mdps/transition_sampler/most_likely_selector
        mdps/transition_sampler/arbitrary_selector
        mdps/transition_sampler/uniform_successor_sampler
        mdps/transition_sampler/random_successor_sampler
        mdps/transition_sampler/vbiased_successor_sampler
        mdps/transition_sampler/vdiff_successor_sampler
        mdps/policy_picker/arbitrary_tiebreaker
        mdps/policy_picker/hbased_tiebreaker
        mdps/policy_picker/operator_id_tiebreaker
        mdps/policy_picker/preferred_operators_tiebreaker
        mdps/policy_picker/random_tiebreaker
        mdps/policy_picker/vdiff_tiebreaker
        mdps/solvers/mdp_heuristic_search
        mdps/solvers/bisimulation_engine
    DEPENDENCY_ONLY
    DEPENDS MDP
)

fast_downward_plugin(
    NAME AO_SEARCH
    HELP "aostar"
    SOURCES
        mdps/solvers/aostar
        mdps/solvers/exhaustive_ao
    DEPENDS MDP_HEURISTIC_SEARCH_BASE)

fast_downward_plugin(
    NAME EXHDFS
    HELP "EXHDFS"
    SOURCES
        mdps/solvers/exhaustive_dfs
    DEPENDS MDP_HEURISTIC_SEARCH_BASE)

fast_downward_plugin(
    NAME LRTDP_SOLVER
    HELP "lrtdp"
    SOURCES
        mdps/solvers/lrtdp
    DEPENDS MDP_HEURISTIC_SEARCH_BASE)

fast_downward_plugin(
    NAME HDFS_SOLVERS
    HELP "hdfs"
    SOURCES
        mdps/solvers/hdfs
    DEPENDS MDP_HEURISTIC_SEARCH_BASE)

fast_downward_plugin(
    NAME OCCUPATION_MEASURE_HEURISTICS
    HELP "Occupation measure heuristics"
    SOURCES
        mdps/heuristics/occupation_measure/occupation_measure_heuristic
        mdps/heuristics/occupation_measure/regrouped_operator_counting_heuristic
    DEPENDS MDP LP_SOLVER
    )

fast_downward_plugin(
    NAME PROBABILISTIC_PDBS
    HELP "Probabilistic PDBS"
    SOURCES
        mdps/heuristics/pdbs/abstract_state
        mdps/heuristics/pdbs/abstract_operator
        mdps/heuristics/pdbs/qualitative_result_store
        mdps/heuristics/pdbs/quantitative_result_store
        mdps/heuristics/pdbs/engine_interfaces
        mdps/heuristics/pdbs/probabilistic_projection
        mdps/heuristics/pdbs/maxprob_pdb_heuristic
        mdps/heuristics/pdbs/multiplicativity
        mdps/heuristics/pdbs/syntactic_projection
        mdps/heuristics/pdbs/utils
        mdps/heuristics/pdbs/expected_cost/expected_cost_pdb_heuristic
    DEPENDS MDP SUCCESSOR_GENERATOR
    )

fast_downward_add_plugin_sources(PLANNER_SOURCES)

# The order in PLANNER_SOURCES influences the order in which object
# files are given to the linker, which can have a significant influence
# on performance (see issue67). The general recommendation seems to be
# to list files that define functions after files that use them.
# We approximate this by reversing the list, which will put the plugins
# first, followed by the core files, followed by the main file.
# This is certainly not optimal, but works well enough in practice.
list(REVERSE PLANNER_SOURCES)
