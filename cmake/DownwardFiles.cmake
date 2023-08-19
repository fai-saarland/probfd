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
        downward/abstract_task
        downward/axioms
        #downward/command_line
        downward/evaluation_context
        downward/evaluation_result
        downward/evaluator
        downward/evaluator_cache
        downward/heuristic
        downward/open_list
        downward/open_list_factory
        downward/operator_cost
        downward/operator_id
        downward/per_state_array
        downward/per_state_bitset
        downward/per_state_information
        downward/per_task_information
        downward/plan_manager
        downward/pruning_method
        downward/search_algorithm
        downward/search_node_info
        downward/search_progress
        downward/search_space
        downward/search_statistics
        downward/state_id
        downward/state_registry
        downward/task_id
        downward/task_proxy
    DEPENDS CAUSAL_GRAPH INT_HASH_SET INT_PACKER ORDERED_SET SEGMENTED_VECTOR SUBSCRIBER SUCCESSOR_GENERATOR TASK_PROPERTIES
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME PLUGINS
    HELP "Plugin definition"
    SOURCES
        downward/plugins/any
        downward/plugins/bounds
        downward/plugins/doc_printer
        downward/plugins/options
        downward/plugins/plugin
        downward/plugins/plugin_info
        downward/plugins/raw_registry
        downward/plugins/registry
        downward/plugins/registry_types
        downward/plugins/types
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME PARSER
    HELP "Option parsing"
    SOURCES
        downward/parser/abstract_syntax_tree
        downward/parser/decorated_abstract_syntax_tree
        downward/parser/lexical_analyzer
        downward/parser/syntax_analyzer
        downward/parser/token_stream
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME UTILS
    HELP "System utilities"
    SOURCES
        downward/utils/collections
        downward/utils/countdown_timer
        downward/utils/exceptions
        downward/utils/hash
        downward/utils/logging
        downward/utils/markup
        downward/utils/math
        downward/utils/memory
        downward/utils/rng
        downward/utils/rng_options
        downward/utils/strings
        downward/utils/system
        downward/utils/system_unix
        downward/utils/system_windows
        downward/utils/timer
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME ALTERNATION_OPEN_LIST
    HELP "Open list that alternates between underlying open lists in a round-robin manner"
    SOURCES
        downward/open_lists/alternation_open_list
)

fast_downward_plugin(
    NAME BEST_FIRST_OPEN_LIST
    HELP "Open list that selects the best element according to a single evaluation function"
    SOURCES
        downward/open_lists/best_first_open_list
)

fast_downward_plugin(
    NAME EPSILON_GREEDY_OPEN_LIST
    HELP "Open list that chooses an entry randomly with probability epsilon"
    SOURCES
        downward/open_lists/epsilon_greedy_open_list
)

fast_downward_plugin(
    NAME PARETO_OPEN_LIST
    HELP "Pareto open list"
    SOURCES
        downward/open_lists/pareto_open_list
)

fast_downward_plugin(
    NAME TIEBREAKING_OPEN_LIST
    HELP "Tiebreaking open list"
    SOURCES
        downward/open_lists/tiebreaking_open_list
)

fast_downward_plugin(
    NAME TYPE_BASED_OPEN_LIST
    HELP "Type-based open list"
    SOURCES
        downward/open_lists/type_based_open_list
)

fast_downward_plugin(
    NAME DYNAMIC_BITSET
    HELP "Poor man's version of boost::dynamic_bitset"
    SOURCES
        downward/algorithms/dynamic_bitset
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME NAMED_VECTOR
    HELP "Generic vector with associated name for each element"
    SOURCES
        downward/algorithms/named_vector
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME EQUIVALENCE_RELATION
    HELP "Equivalence relation over [1, ..., n] that can be iteratively refined"
    SOURCES
        downward/algorithms/equivalence_relation
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME INT_HASH_SET
    HELP "Hash set storing non-negative integers"
    SOURCES
        downward/algorithms/int_hash_set
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME INT_PACKER
    HELP "Greedy bin packing algorithm to pack integer variables with small domains tightly into memory"
    SOURCES
        downward/algorithms/int_packer
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME MAX_CLIQUES
    HELP "Implementation of the Max Cliques algorithm by Tomita et al."
    SOURCES
        downward/algorithms/max_cliques
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PRIORITY_QUEUES
    HELP "Three implementations of priority queue: HeapQueue, BucketQueue and AdaptiveQueue"
    SOURCES
        downward/algorithms/priority_queues
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME ORDERED_SET
    HELP "Set of elements ordered by insertion time"
    SOURCES
        downward/algorithms/ordered_set
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SEGMENTED_VECTOR
    HELP "Memory-friendly and vector-like data structure"
    SOURCES
        downward/algorithms/segmented_vector
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SUBSCRIBER
    HELP "Allows object to subscribe to the destructor of other objects"
    SOURCES
        downward/algorithms/subscriber
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME EVALUATORS_SUBCATEGORY
    HELP "Subcategory plugin for basic evaluators"
    SOURCES
        downward/evaluators/subcategory
)


fast_downward_plugin(
    NAME CONST_EVALUATOR
    HELP "The constant evaluator"
    SOURCES
        downward/evaluators/const_evaluator
    DEPENDS EVALUATORS_SUBCATEGORY
)

fast_downward_plugin(
    NAME G_EVALUATOR
    HELP "The g-evaluator"
    SOURCES
        downward/evaluators/g_evaluator
    DEPENDS EVALUATORS_SUBCATEGORY
)

fast_downward_plugin(
    NAME COMBINING_EVALUATOR
    HELP "The combining evaluator"
    SOURCES
        downward/evaluators/combining_evaluator
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME MAX_EVALUATOR
    HELP "The max evaluator"
    SOURCES
        downward/evaluators/max_evaluator
    DEPENDS COMBINING_EVALUATOR EVALUATORS_SUBCATEGORY
)

fast_downward_plugin(
    NAME PREF_EVALUATOR
    HELP "The pref evaluator"
    SOURCES
        downward/evaluators/pref_evaluator
    DEPENDS EVALUATORS_SUBCATEGORY
)

fast_downward_plugin(
    NAME WEIGHTED_EVALUATOR
    HELP "The weighted evaluator"
    SOURCES
        downward/evaluators/weighted_evaluator
    DEPENDS EVALUATORS_SUBCATEGORY
)

fast_downward_plugin(
    NAME SUM_EVALUATOR
    HELP "The sum evaluator"
    SOURCES
        downward/evaluators/sum_evaluator
    DEPENDS COMBINING_EVALUATOR EVALUATORS_SUBCATEGORY
)

fast_downward_plugin(
    NAME NULL_PRUNING_METHOD
    HELP "Pruning method that does nothing"
    SOURCES
        downward/pruning/null_pruning_method
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME LIMITED_PRUNING
    HELP "Method for limiting another pruning method"
    SOURCES
        downward/pruning/limited_pruning
)

fast_downward_plugin(
    NAME STUBBORN_SETS
    HELP "Base class for all stubborn set partial order reduction methods"
    SOURCES
        downward/pruning/stubborn_sets
    DEPENDS TASK_PROPERTIES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME STUBBORN_SETS_ACTION_CENTRIC
    HELP "Base class for all action-centric stubborn set partial order reduction methods"
    SOURCES
        downward/pruning/stubborn_sets_action_centric
    DEPENDS STUBBORN_SETS
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME STUBBORN_SETS_ATOM_CENTRIC
    HELP "Atom-centric stubborn sets"
    SOURCES
        downward/pruning/stubborn_sets_atom_centric
    DEPENDS STUBBORN_SETS
)

fast_downward_plugin(
    NAME STUBBORN_SETS_SIMPLE
    HELP "Stubborn sets simple"
    SOURCES
        downward/pruning/stubborn_sets_simple
    DEPENDS STUBBORN_SETS_ACTION_CENTRIC
)

fast_downward_plugin(
    NAME STUBBORN_SETS_EC
    HELP "Stubborn set method that dominates expansion core"
    SOURCES
        downward/pruning/stubborn_sets_ec
    DEPENDS STUBBORN_SETS_ACTION_CENTRIC TASK_PROPERTIES
)

fast_downward_plugin(
    NAME SEARCH_COMMON
    HELP "Basic classes used for all search engines"
    SOURCES
        downward/search_algorithms/search_common
    DEPENDS ALTERNATION_OPEN_LIST G_EVALUATOR BEST_FIRST_OPEN_LIST SUM_EVALUATOR TIEBREAKING_OPEN_LIST WEIGHTED_EVALUATOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME EAGER_SEARCH
    HELP "Eager search algorithm"
    SOURCES
        downward/search_algorithms/eager_search
    DEPENDS NULL_PRUNING_METHOD ORDERED_SET SUCCESSOR_GENERATOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME PLUGIN_ASTAR
    HELP "A* search"
    SOURCES
        downward/search_algorithms/plugin_astar
    DEPENDS EAGER_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_EAGER
    HELP "Eager (i.e., normal) best-first search"
    SOURCES
        downward/search_algorithms/plugin_eager
    DEPENDS EAGER_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_EAGER_GREEDY
    HELP "Eager greedy best-first search"
    SOURCES
        downward/search_algorithms/plugin_eager_greedy
    DEPENDS EAGER_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_EAGER_WASTAR
    HELP "Weighted eager A* search"
    SOURCES
        downward/search_algorithms/plugin_eager_wastar
    DEPENDS EAGER_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_LAZY
    HELP "Best-first search with deferred evaluation (lazy)"
    SOURCES
        downward/search_algorithms/plugin_lazy
    DEPENDS LAZY_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_LAZY_GREEDY
    HELP "Greedy best-first search with deferred evaluation (lazy)"
    SOURCES
        downward/search_algorithms/plugin_lazy_greedy
    DEPENDS LAZY_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME PLUGIN_LAZY_WASTAR
    HELP "Weighted A* search with deferred evaluation (lazy)"
    SOURCES
        downward/search_algorithms/plugin_lazy_wastar
    DEPENDS LAZY_SEARCH SEARCH_COMMON
)

fast_downward_plugin(
    NAME ENFORCED_HILL_CLIMBING_SEARCH
    HELP "Lazy enforced hill-climbing search algorithm"
    SOURCES
        downward/search_algorithms/enforced_hill_climbing_search
    DEPENDS G_EVALUATOR ORDERED_SET PREF_EVALUATOR SEARCH_COMMON SUCCESSOR_GENERATOR
)

fast_downward_plugin(
    NAME ITERATED_SEARCH
    HELP "Iterated search algorithm"
    SOURCES
        downward/search_algorithms/iterated_search
)

fast_downward_plugin(
    NAME LAZY_SEARCH
    HELP "Lazy search algorithm"
    SOURCES
        downward/search_algorithms/lazy_search
    DEPENDS ORDERED_SET SUCCESSOR_GENERATOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME LP_SOLVER
    HELP "Interface to an LP solver"
    SOURCES
        downward/lp/lp_internals
        downward/lp/lp_solver
    DEPENDS NAMED_VECTOR
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME RELAXATION_HEURISTIC
    HELP "The base class for relaxation heuristics"
    SOURCES
        downward/heuristics/array_pool
        downward/heuristics/relaxation_heuristic
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME ADDITIVE_HEURISTIC
    HELP "The additive heuristic"
    SOURCES
        downward/heuristics/additive_heuristic
    DEPENDS PRIORITY_QUEUES RELAXATION_HEURISTIC TASK_PROPERTIES
)

fast_downward_plugin(
    NAME BLIND_SEARCH_HEURISTIC
    HELP "The 'blind search' heuristic"
    SOURCES
        downward/heuristics/blind_search_heuristic
    DEPENDS TASK_PROPERTIES
)

fast_downward_plugin(
    NAME CONTEXT_ENHANCED_ADDITIVE_HEURISTIC
    HELP "The context-enhanced additive heuristic"
    SOURCES
        downward/heuristics/cea_heuristic
    DEPENDS DOMAIN_TRANSITION_GRAPH PRIORITY_QUEUES TASK_PROPERTIES
)

fast_downward_plugin(
    NAME CG_HEURISTIC
    HELP "The causal graph heuristic"
    SOURCES
        downward/heuristics/cg_heuristic
        downward/heuristics/cg_cache
    DEPENDS DOMAIN_TRANSITION_GRAPH PRIORITY_QUEUES TASK_PROPERTIES
)

fast_downward_plugin(
    NAME DOMAIN_TRANSITION_GRAPH
    HELP "DTGs used by cg and cea heuristic"
    SOURCES
        downward/heuristics/domain_transition_graph
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME FF_HEURISTIC
    HELP "The FF heuristic (an implementation of the RPG heuristic)"
    SOURCES
        downward/heuristics/ff_heuristic
    DEPENDS ADDITIVE_HEURISTIC TASK_PROPERTIES
)

fast_downward_plugin(
    NAME GOAL_COUNT_HEURISTIC
    HELP "The goal-counting heuristic"
    SOURCES
        downward/heuristics/goal_count_heuristic
)

fast_downward_plugin(
    NAME HM_HEURISTIC
    HELP "The h^m heuristic"
    SOURCES
        downward/heuristics/hm_heuristic
    DEPENDS TASK_PROPERTIES
)

fast_downward_plugin(
    NAME LANDMARK_CUT_HEURISTIC
    HELP "The LM-cut heuristic"
    SOURCES
        downward/heuristics/lm_cut_heuristic
        downward/heuristics/lm_cut_landmarks
    DEPENDS PRIORITY_QUEUES TASK_PROPERTIES
)

fast_downward_plugin(
    NAME MAX_HEURISTIC
    HELP "The Max heuristic"
    SOURCES
        downward/heuristics/max_heuristic
    DEPENDS PRIORITY_QUEUES RELAXATION_HEURISTIC
)

fast_downward_plugin(
    NAME CORE_TASKS
    HELP "Core task transformations"
    SOURCES
        downward/tasks/cost_adapted_task
        downward/tasks/delegating_task
        downward/tasks/root_task
    CORE_PLUGIN
)

fast_downward_plugin(
    NAME EXTRA_TASKS
    HELP "Non-core task transformations"
    SOURCES
        downward/tasks/domain_abstracted_task
        downward/tasks/domain_abstracted_task_factory
        downward/tasks/modified_goals_task
        downward/tasks/modified_operator_costs_task
    DEPENDS TASK_PROPERTIES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME CAUSAL_GRAPH
    HELP "Causal Graph"
    SOURCES
        downward/task_utils/causal_graph
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SAMPLING
    HELP "Sampling"
    SOURCES
        downward/task_utils/sampling
    DEPENDS SUCCESSOR_GENERATOR TASK_PROPERTIES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME SUCCESSOR_GENERATOR
    HELP "Successor generator"
    SOURCES
        downward/task_utils/successor_generator
        downward/task_utils/successor_generator_factory
        downward/task_utils/successor_generator_internals
    DEPENDS TASK_PROPERTIES
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME TASK_PROPERTIES
    HELP "Task properties"
    SOURCES
        downward/task_utils/task_properties
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME VARIABLE_ORDER_FINDER
    HELP "Variable order finder"
    SOURCES
        downward/task_utils/variable_order_finder
    DEPENDENCY_ONLY
)

fast_downward_plugin(
    NAME CARTESIAN_ABSTRACTIONS
    HELP "Plugin containing the code for Cartesian CEGAR heuristics"
    SOURCES
        downward/cartesian_abstractions/abstraction
        downward/cartesian_abstractions/abstract_search
        downward/cartesian_abstractions/abstract_state
        downward/cartesian_abstractions/additive_cartesian_heuristic
        downward/cartesian_abstractions/cartesian_heuristic_function
        downward/cartesian_abstractions/cartesian_set
        downward/cartesian_abstractions/cegar
        downward/cartesian_abstractions/cost_saturation
        downward/cartesian_abstractions/refinement_hierarchy
        downward/cartesian_abstractions/split_selector
        downward/cartesian_abstractions/subtask_generators
        downward/cartesian_abstractions/transition
        downward/cartesian_abstractions/transition_system
        downward/cartesian_abstractions/types
        downward/cartesian_abstractions/utils
        downward/cartesian_abstractions/utils_landmarks
    DEPENDS ADDITIVE_HEURISTIC DYNAMIC_BITSET EXTRA_TASKS LANDMARKS PRIORITY_QUEUES TASK_PROPERTIES
)

fast_downward_plugin(
    NAME MAS_HEURISTIC
    HELP "The Merge-and-Shrink heuristic"
    SOURCES
        downward/merge_and_shrink/distances
        downward/merge_and_shrink/factored_transition_system
        downward/merge_and_shrink/fts_factory
        downward/merge_and_shrink/label_reduction
        downward/merge_and_shrink/labels
        downward/merge_and_shrink/merge_and_shrink_algorithm
        downward/merge_and_shrink/merge_and_shrink_heuristic
        downward/merge_and_shrink/merge_and_shrink_representation
        downward/merge_and_shrink/merge_scoring_function
        downward/merge_and_shrink/merge_scoring_function_dfp
        downward/merge_and_shrink/merge_scoring_function_goal_relevance
        downward/merge_and_shrink/merge_scoring_function_miasm
        downward/merge_and_shrink/merge_scoring_function_miasm_utils
        downward/merge_and_shrink/merge_scoring_function_single_random
        downward/merge_and_shrink/merge_scoring_function_total_order
        downward/merge_and_shrink/merge_selector
        downward/merge_and_shrink/merge_selector_score_based_filtering
        downward/merge_and_shrink/merge_strategy
        downward/merge_and_shrink/merge_strategy_factory
        downward/merge_and_shrink/merge_strategy_factory_precomputed
        downward/merge_and_shrink/merge_strategy_factory_sccs
        downward/merge_and_shrink/merge_strategy_factory_stateless
        downward/merge_and_shrink/merge_strategy_precomputed
        downward/merge_and_shrink/merge_strategy_sccs
        downward/merge_and_shrink/merge_strategy_stateless
        downward/merge_and_shrink/merge_tree
        downward/merge_and_shrink/merge_tree_factory
        downward/merge_and_shrink/merge_tree_factory_linear
        downward/merge_and_shrink/shrink_bisimulation
        downward/merge_and_shrink/shrink_bucket_based
        downward/merge_and_shrink/shrink_fh
        downward/merge_and_shrink/shrink_random
        downward/merge_and_shrink/shrink_strategy
        downward/merge_and_shrink/transition_system
        downward/merge_and_shrink/types
        downward/merge_and_shrink/utils
    DEPENDS PRIORITY_QUEUES EQUIVALENCE_RELATION SCCS TASK_PROPERTIES VARIABLE_ORDER_FINDER
)

fast_downward_plugin(
    NAME LANDMARKS
    HELP "Plugin containing the code to reason with landmarks"
    SOURCES
        downward/landmarks/exploration
        downward/landmarks/landmark
        downward/landmarks/landmark_cost_assignment
        downward/landmarks/landmark_cost_partitioning_heuristic
        downward/landmarks/landmark_factory
        downward/landmarks/landmark_factory_h_m
        downward/landmarks/landmark_factory_reasonable_orders_hps
        downward/landmarks/landmark_factory_merged
        downward/landmarks/landmark_factory_relaxation
        downward/landmarks/landmark_factory_rpg_exhaust
        downward/landmarks/landmark_factory_rpg_sasp
        downward/landmarks/landmark_factory_zhu_givan
        downward/landmarks/landmark_graph
        downward/landmarks/landmark_heuristic
        downward/landmarks/landmark_status_manager
        downward/landmarks/landmark_sum_heuristic
        downward/landmarks/util
    DEPENDS LP_SOLVER PRIORITY_QUEUES SUCCESSOR_GENERATOR TASK_PROPERTIES
)

fast_downward_plugin(
    NAME OPERATOR_COUNTING
    HELP "Plugin containing the code for operator-counting heuristics"
    SOURCES
        downward/operator_counting/constraint_generator
        downward/operator_counting/delete_relaxation_constraints
        downward/operator_counting/lm_cut_constraints
        downward/operator_counting/operator_counting_heuristic
        downward/operator_counting/pho_constraints
        downward/operator_counting/state_equation_constraints
    DEPENDS LP_SOLVER LANDMARK_CUT_HEURISTIC PDBS TASK_PROPERTIES
)

fast_downward_plugin(
    NAME PDBS
    HELP "Plugin containing the code for PDBs"
    SOURCES
        downward/pdbs/canonical_pdbs
        downward/pdbs/canonical_pdbs_heuristic
        downward/pdbs/cegar
        downward/pdbs/dominance_pruning
        downward/pdbs/incremental_canonical_pdbs
        downward/pdbs/match_tree
        downward/pdbs/max_cliques
        downward/pdbs/pattern_cliques
        downward/pdbs/pattern_collection_information
        downward/pdbs/pattern_collection_generator_combo
        downward/pdbs/pattern_collection_generator_disjoint_cegar
        downward/pdbs/pattern_collection_generator_genetic
        downward/pdbs/pattern_collection_generator_hillclimbing
        downward/pdbs/pattern_collection_generator_manual
        downward/pdbs/pattern_collection_generator_multiple_cegar
        downward/pdbs/pattern_collection_generator_multiple_random
        downward/pdbs/pattern_collection_generator_multiple
        downward/pdbs/pattern_collection_generator_systematic
        downward/pdbs/pattern_database
        downward/pdbs/pattern_generator_cegar
        downward/pdbs/pattern_generator_greedy
        downward/pdbs/pattern_generator_manual
        downward/pdbs/pattern_generator_random
        downward/pdbs/pattern_generator
        downward/pdbs/pattern_information
        downward/pdbs/pdb_heuristic
        downward/pdbs/random_pattern
        downward/pdbs/subcategory
        downward/pdbs/types
        downward/pdbs/utils
        downward/pdbs/validation
        downward/pdbs/zero_one_pdbs
        downward/pdbs/zero_one_pdbs_heuristic
    DEPENDS CAUSAL_GRAPH MAX_CLIQUES PRIORITY_QUEUES SAMPLING SUCCESSOR_GENERATOR TASK_PROPERTIES VARIABLE_ORDER_FINDER
)

fast_downward_plugin(
    NAME POTENTIALS
    HELP "Plugin containing the code for potential heuristics"
    SOURCES
        downward/potentials/diverse_potential_heuristics
        downward/potentials/potential_function
        downward/potentials/potential_heuristic
        downward/potentials/potential_max_heuristic
        downward/potentials/potential_optimizer
        downward/potentials/sample_based_potential_heuristics
        downward/potentials/single_potential_heuristics
        downward/potentials/subcategory
        downward/potentials/util
    DEPENDS LP_SOLVER SAMPLING SUCCESSOR_GENERATOR TASK_PROPERTIES
)

fast_downward_plugin(
    NAME SCCS
    HELP "Algorithm to compute the strongly connected components (SCCs) of a "
         "directed graph."
    SOURCES
        downward/algorithms/sccs
    DEPENDENCY_ONLY
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
