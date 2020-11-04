(define (domain canadian-transport-l4-e8-t1-p5--minP200--maxP200--s388999)
(:requirements :typing :probabilistic-effects)
(:types horizon-value - object location package road status)
(:constants
    l0 l1 l2 l3 - location
    p0 p1 p2 p3 p4 - package
    r0 r1 r2 r3 r4 r5 r6 r7 - road
    unknown clear blocked - status)
(:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value) (at ?l - location) (p-at ?p - package ?l - location) (road-status ?r - road ?s - status) (trunk ?p - package))
(:functions (total-cost))
(:action load
    :parameters (?hcur ?hnew - horizon-value ?p - package ?l - location)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?l) (p-at ?p ?l))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (trunk ?p) (not (p-at ?p ?l)))
)
(:action unload
    :parameters (?hcur ?hnew - horizon-value ?p - package ?l - location)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?l) (trunk ?p))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (trunk ?p)) (p-at ?p ?l))
)
(:action try-driving-l0-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r0 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r0 unknown))
    (probabilistic
            200/1000 (and (road-status r0 blocked))
            800/1000 (and (road-status r0 clear) (at l1) (not (at l0)))))
)
(:action drive-l0-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r0 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l1) (not (at l0)))
)
(:action try-driving-l1-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r0 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r0 unknown))
    (probabilistic
            200/1000 (and (road-status r0 blocked))
            800/1000 (and (road-status r0 clear) (at l0) (not (at l1)))))
)
(:action drive-l1-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r0 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l0) (not (at l1)))
)
(:action try-driving-l0-l2
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r1 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r1 unknown))
    (probabilistic
            200/1000 (and (road-status r1 blocked))
            800/1000 (and (road-status r1 clear) (at l2) (not (at l0)))))
)
(:action drive-l0-l2
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r1 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l2) (not (at l0)))
)
(:action try-driving-l2-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l2) (road-status r1 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r1 unknown))
    (probabilistic
            200/1000 (and (road-status r1 blocked))
            800/1000 (and (road-status r1 clear) (at l0) (not (at l2)))))
)
(:action drive-l2-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l2) (road-status r1 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l0) (not (at l2)))
)
(:action try-driving-l0-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r2 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r2 unknown))
    (probabilistic
            200/1000 (and (road-status r2 blocked))
            800/1000 (and (road-status r2 clear) (at l3) (not (at l0)))))
)
(:action drive-l0-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r2 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l3) (not (at l0)))
)
(:action try-driving-l3-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r2 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r2 unknown))
    (probabilistic
            200/1000 (and (road-status r2 blocked))
            800/1000 (and (road-status r2 clear) (at l0) (not (at l3)))))
)
(:action drive-l3-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r2 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l0) (not (at l3)))
)
(:action try-driving-l1-l2
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r3 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r3 unknown))
    (probabilistic
            200/1000 (and (road-status r3 blocked))
            800/1000 (and (road-status r3 clear) (at l2) (not (at l1)))))
)
(:action drive-l1-l2
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r3 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l2) (not (at l1)))
)
(:action try-driving-l2-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l2) (road-status r3 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r3 unknown))
    (probabilistic
            200/1000 (and (road-status r3 blocked))
            800/1000 (and (road-status r3 clear) (at l1) (not (at l2)))))
)
(:action drive-l2-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l2) (road-status r3 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l1) (not (at l2)))
)
(:action try-driving-l1-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r4 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r4 unknown))
    (probabilistic
            200/1000 (and (road-status r4 blocked))
            800/1000 (and (road-status r4 clear) (at l3) (not (at l1)))))
)
(:action drive-l1-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r4 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l3) (not (at l1)))
)
(:action try-driving-l3-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r4 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r4 unknown))
    (probabilistic
            200/1000 (and (road-status r4 blocked))
            800/1000 (and (road-status r4 clear) (at l1) (not (at l3)))))
)
(:action drive-l3-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r4 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l1) (not (at l3)))
)
(:action try-driving-l2-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l2) (road-status r5 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r5 unknown))
    (probabilistic
            200/1000 (and (road-status r5 blocked))
            800/1000 (and (road-status r5 clear) (at l3) (not (at l2)))))
)
(:action drive-l2-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l2) (road-status r5 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l3) (not (at l2)))
)
(:action try-driving-l3-l2
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r5 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r5 unknown))
    (probabilistic
            200/1000 (and (road-status r5 blocked))
            800/1000 (and (road-status r5 clear) (at l2) (not (at l3)))))
)
(:action drive-l3-l2
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r5 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l2) (not (at l3)))
)
)
