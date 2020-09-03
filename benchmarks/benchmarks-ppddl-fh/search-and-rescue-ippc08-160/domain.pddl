;; -*-lisp-*-
;; Search & Rescue domain:
;;
;;   Florent Teichteil and Patrick Fabiani, 2008
;; modified by me

(define (domain search-and-rescue)

(:requirements :typing :negative-preconditions :probabilistic-effects)

(:types horizon-value - object zone)

(:constants base - zone)

(:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value)
    (at ?loc - zone)
    (explored ?loc - zone)
    (landable ?loc - zone)
    (human-onboard)
    (human-alive)
    (human-rescued)
    (on-ground)
)

(:action goto-base-without-human
    :parameters (?hcur ?hnew - horizon-value ?loc - zone)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew)
        (at ?loc)
        (not (= ?loc base))
        (not (on-ground))
        (not (human-onboard))
    )
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
        (not (at ?loc))
        (at base)
    )
)

(:action goto-base-with-human
    :parameters (?hcur ?hnew - horizon-value ?loc - zone)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew)
        (at ?loc)
        (not (= ?loc base))
        (not (on-ground))
        (human-onboard)
    )
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
        (not (at ?loc))
        (at base)
        (probabilistic 0.05 (and (not (human-alive))))
    )
)

(:action goto-without-human
    :parameters (?hcur ?hnew - horizon-value ?loc ?newloc - zone)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew)
        (at ?loc)
        (not (= ?loc ?newloc))
        (not (= ?newloc base))
        (not (on-ground))
        (not (human-onboard))
        (human-alive)
    )
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
        (not (at ?loc))
        (at ?newloc)
    )
)

(:action goto-without-human
    :parameters (?hcur ?hnew - horizon-value ?loc ?newloc - zone)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew)
        (at ?loc)
        (not (= ?loc ?newloc))
        (not (= ?newloc base))
        (not (on-ground))
        (human-onboard)
        (human-alive)
    )
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
        (not (at ?loc))
        (at ?newloc)
        (probabilistic 0.05 (and (not (human-alive))))
    )
)

(:action explore
    :parameters (?hcur ?hnew - horizon-value ?loc - zone)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (= ?loc base)) (at ?loc) (not (on-ground)) (human-alive) (not (explored ?loc)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (explored ?loc) (probabilistic
        0.7 (landable ?loc)))
)

(:action land-base
    :parameters (?hcur ?hnew - horizon-value )
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at base) (not (on-ground)) (human-onboard) (human-alive))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (on-ground) (human-rescued))
)

(:action land
    :parameters (?hcur ?hnew - horizon-value ?loc - zone)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?loc) (not (on-ground)) (not (= ?loc base)) (human-alive) (landable ?loc))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (on-ground) (probabilistic
        0.2 (and (not (human-alive)))
        0.8 (human-onboard)
    ))
)

(:action takeoff
    :parameters (?hcur ?hnew - horizon-value ?loc - zone)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (on-ground) (at ?loc)) ;  (not (= ?loc base))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (on-ground)))
)

)
