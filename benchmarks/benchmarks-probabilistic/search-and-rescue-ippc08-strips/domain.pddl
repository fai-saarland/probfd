;; -*-lisp-*-
;; Search & Rescue domain:
;;
;;   Florent Teichteil and Patrick Fabiani, 2008
;; modified by me

(define (domain search-and-rescue)

(:requirements :typing :negative-preconditions :probabilistic-effects)

(:types zone)

(:constants base - zone)

(:predicates
    (at ?loc - zone)
    (explored ?loc - zone)
    (landable ?loc - zone)
    (human-onboard)
    (human-alive)
    (human-rescued)
    (on-ground)
)

(:action goto-base-without-human
    :parameters (?loc - zone)
    :precondition (and
        (at ?loc)
        (not (= ?loc base))
        (not (on-ground))
        (not (human-onboard))
    )
    :effect (and (increase (total-cost) 1)
        (not (at ?loc))
        (at base)
    )
)

(:action goto-base-with-human
    :parameters (?loc - zone)
    :precondition (and
        (at ?loc)
        (not (= ?loc base))
        (not (on-ground))
        (human-onboard)
    )
    :effect (and (increase (total-cost) 1)
        (not (at ?loc))
        (at base)
        (probabilistic 0.05 (and (not (human-alive))))
    )
)

(:action goto-without-human
    :parameters (?loc ?newloc - zone)
    :precondition (and
        (at ?loc)
        (not (= ?loc ?newloc))
        (not (= ?newloc base))
        (not (on-ground))
        (not (human-onboard))
        (human-alive)
    )
    :effect (and (increase (total-cost) 1)
        (not (at ?loc))
        (at ?newloc)
    )
)

(:action goto-without-human
    :parameters (?loc ?newloc - zone)
    :precondition (and
        (at ?loc)
        (not (= ?loc ?newloc))
        (not (= ?newloc base))
        (not (on-ground))
        (human-onboard)
        (human-alive)
    )
    :effect (and (increase (total-cost) 1)
        (not (at ?loc))
        (at ?newloc)
        (probabilistic 0.05 (and (not (human-alive))))
    )
)

(:action explore
    :parameters (?loc - zone)
    :precondition (and (not (= ?loc base)) (at ?loc) (not (on-ground)) (human-alive) (not (explored ?loc)))
    :effect (and (increase (total-cost) 1) (explored ?loc) (probabilistic
        0.7 (landable ?loc)))
)

(:action land-base
    :parameters ()
    :precondition (and (at base) (not (on-ground)) (human-onboard) (human-alive))
    :effect (and (increase (total-cost) 1) (on-ground) (human-rescued))
)

(:action land
    :parameters (?loc - zone)
    :precondition (and (at ?loc) (not (on-ground)) (not (= ?loc base)) (human-alive) (landable ?loc))
    :effect (and (increase (total-cost) 1) (on-ground) (probabilistic
        0.2 (and (not (human-alive)))
        0.8 (human-onboard)
    ))
)

(:action takeoff
    :parameters (?loc - zone)
    :precondition (and (on-ground) (at ?loc)) ;  (not (= ?loc base))
    :effect (and (increase (total-cost) 1) (not (on-ground)))
)

)
