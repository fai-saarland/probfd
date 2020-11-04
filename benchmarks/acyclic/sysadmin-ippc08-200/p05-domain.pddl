(define (domain sysadmin-nocount)
(:requirements :probabilistic-effects :negative-preconditions :typing)
(:types horizon-value - object computer - object)
(:constants
  comp0 comp1 comp2 comp3 comp4 comp5 comp6 comp7 comp8 comp9 comp10 comp11 - computer
)
(:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value)
  (running ?c - computer)
  (rebooted ?c - computer)
  (evaluate ?c - computer)
  (update-status ?c - computer)
  (one-off ?c - computer)
  (all-on ?c - computer)
  (all-updated)
)
(:action reboot
  :parameters (?hcur ?hnew - horizon-value ?c - computer)
  :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew)
    (all-updated)
  )
  :effect (and (not (horizon ?hcur)) (horizon ?hnew)
    (increase (total-cost) 1)
    (not (all-updated))
    (evaluate comp0)
    (rebooted ?c)
  )
)
(:action evaluate-comp0-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (rebooted comp0)
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
  )
)
(:action evaluate-comp0-all-on
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (running comp8)
    (running comp9)
    (running comp11)
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (all-on comp0)
  )
)
(:action evaluate-comp0-off-comp8
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp8))
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (one-off comp0)
  )
)
(:action evaluate-comp0-off-comp9
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp9))
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (one-off comp0)
  )
)
(:action evaluate-comp0-off-comp11
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp11))
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (one-off comp0)
  )
)
(:action evaluate-comp1-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp1)
    (rebooted comp1)
  )
  :effect (and
    (not (evaluate comp1))
    (evaluate comp2)
  )
)
(:action evaluate-comp1-all-on
  :parameters ()
  :precondition (and
    (evaluate comp1)
    (not (rebooted comp1))
    (running comp0)
  )
  :effect (and
    (not (evaluate comp1))
    (evaluate comp2)
    (all-on comp1)
  )
)
(:action evaluate-comp1-off-comp0
  :parameters ()
  :precondition (and
    (evaluate comp1)
    (not (rebooted comp1))
    (not (running comp0))
  )
  :effect (and
    (not (evaluate comp1))
    (evaluate comp2)
    (one-off comp1)
  )
)
(:action evaluate-comp2-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp2)
    (rebooted comp2)
  )
  :effect (and
    (not (evaluate comp2))
    (evaluate comp3)
  )
)
(:action evaluate-comp2-all-on
  :parameters ()
  :precondition (and
    (evaluate comp2)
    (not (rebooted comp2))
    (running comp1)
    (running comp7)
  )
  :effect (and
    (not (evaluate comp2))
    (evaluate comp3)
    (all-on comp2)
  )
)
(:action evaluate-comp2-off-comp1
  :parameters ()
  :precondition (and
    (evaluate comp2)
    (not (rebooted comp2))
    (not (running comp1))
  )
  :effect (and
    (not (evaluate comp2))
    (evaluate comp3)
    (one-off comp2)
  )
)
(:action evaluate-comp2-off-comp7
  :parameters ()
  :precondition (and
    (evaluate comp2)
    (not (rebooted comp2))
    (not (running comp7))
  )
  :effect (and
    (not (evaluate comp2))
    (evaluate comp3)
    (one-off comp2)
  )
)
(:action evaluate-comp3-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp3)
    (rebooted comp3)
  )
  :effect (and
    (not (evaluate comp3))
    (evaluate comp4)
  )
)
(:action evaluate-comp3-all-on
  :parameters ()
  :precondition (and
    (evaluate comp3)
    (not (rebooted comp3))
    (running comp2)
  )
  :effect (and
    (not (evaluate comp3))
    (evaluate comp4)
    (all-on comp3)
  )
)
(:action evaluate-comp3-off-comp2
  :parameters ()
  :precondition (and
    (evaluate comp3)
    (not (rebooted comp3))
    (not (running comp2))
  )
  :effect (and
    (not (evaluate comp3))
    (evaluate comp4)
    (one-off comp3)
  )
)
(:action evaluate-comp4-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (rebooted comp4)
  )
  :effect (and
    (not (evaluate comp4))
    (evaluate comp5)
  )
)
(:action evaluate-comp4-all-on
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (running comp3)
    (running comp6)
  )
  :effect (and
    (not (evaluate comp4))
    (evaluate comp5)
    (all-on comp4)
  )
)
(:action evaluate-comp4-off-comp3
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (not (running comp3))
  )
  :effect (and
    (not (evaluate comp4))
    (evaluate comp5)
    (one-off comp4)
  )
)
(:action evaluate-comp4-off-comp6
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (not (running comp6))
  )
  :effect (and
    (not (evaluate comp4))
    (evaluate comp5)
    (one-off comp4)
  )
)
(:action evaluate-comp5-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp5)
    (rebooted comp5)
  )
  :effect (and
    (not (evaluate comp5))
    (evaluate comp6)
  )
)
(:action evaluate-comp5-all-on
  :parameters ()
  :precondition (and
    (evaluate comp5)
    (not (rebooted comp5))
    (running comp4)
  )
  :effect (and
    (not (evaluate comp5))
    (evaluate comp6)
    (all-on comp5)
  )
)
(:action evaluate-comp5-off-comp4
  :parameters ()
  :precondition (and
    (evaluate comp5)
    (not (rebooted comp5))
    (not (running comp4))
  )
  :effect (and
    (not (evaluate comp5))
    (evaluate comp6)
    (one-off comp5)
  )
)
(:action evaluate-comp6-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp6)
    (rebooted comp6)
  )
  :effect (and
    (not (evaluate comp6))
    (evaluate comp7)
  )
)
(:action evaluate-comp6-all-on
  :parameters ()
  :precondition (and
    (evaluate comp6)
    (not (rebooted comp6))
    (running comp5)
  )
  :effect (and
    (not (evaluate comp6))
    (evaluate comp7)
    (all-on comp6)
  )
)
(:action evaluate-comp6-off-comp5
  :parameters ()
  :precondition (and
    (evaluate comp6)
    (not (rebooted comp6))
    (not (running comp5))
  )
  :effect (and
    (not (evaluate comp6))
    (evaluate comp7)
    (one-off comp6)
  )
)
(:action evaluate-comp7-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp7)
    (rebooted comp7)
  )
  :effect (and
    (not (evaluate comp7))
    (evaluate comp8)
  )
)
(:action evaluate-comp7-all-on
  :parameters ()
  :precondition (and
    (evaluate comp7)
    (not (rebooted comp7))
    (running comp6)
    (running comp8)
  )
  :effect (and
    (not (evaluate comp7))
    (evaluate comp8)
    (all-on comp7)
  )
)
(:action evaluate-comp7-off-comp6
  :parameters ()
  :precondition (and
    (evaluate comp7)
    (not (rebooted comp7))
    (not (running comp6))
  )
  :effect (and
    (not (evaluate comp7))
    (evaluate comp8)
    (one-off comp7)
  )
)
(:action evaluate-comp7-off-comp8
  :parameters ()
  :precondition (and
    (evaluate comp7)
    (not (rebooted comp7))
    (not (running comp8))
  )
  :effect (and
    (not (evaluate comp7))
    (evaluate comp8)
    (one-off comp7)
  )
)
(:action evaluate-comp8-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp8)
    (rebooted comp8)
  )
  :effect (and
    (not (evaluate comp8))
    (evaluate comp9)
  )
)
(:action evaluate-comp8-all-on
  :parameters ()
  :precondition (and
    (evaluate comp8)
    (not (rebooted comp8))
    (running comp7)
  )
  :effect (and
    (not (evaluate comp8))
    (evaluate comp9)
    (all-on comp8)
  )
)
(:action evaluate-comp8-off-comp7
  :parameters ()
  :precondition (and
    (evaluate comp8)
    (not (rebooted comp8))
    (not (running comp7))
  )
  :effect (and
    (not (evaluate comp8))
    (evaluate comp9)
    (one-off comp8)
  )
)
(:action evaluate-comp9-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp9)
    (rebooted comp9)
  )
  :effect (and
    (not (evaluate comp9))
    (evaluate comp10)
  )
)
(:action evaluate-comp9-all-on
  :parameters ()
  :precondition (and
    (evaluate comp9)
    (not (rebooted comp9))
    (running comp8)
  )
  :effect (and
    (not (evaluate comp9))
    (evaluate comp10)
    (all-on comp9)
  )
)
(:action evaluate-comp9-off-comp8
  :parameters ()
  :precondition (and
    (evaluate comp9)
    (not (rebooted comp9))
    (not (running comp8))
  )
  :effect (and
    (not (evaluate comp9))
    (evaluate comp10)
    (one-off comp9)
  )
)
(:action evaluate-comp10-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp10)
    (rebooted comp10)
  )
  :effect (and
    (not (evaluate comp10))
    (evaluate comp11)
  )
)
(:action evaluate-comp10-all-on
  :parameters ()
  :precondition (and
    (evaluate comp10)
    (not (rebooted comp10))
    (running comp6)
    (running comp9)
  )
  :effect (and
    (not (evaluate comp10))
    (evaluate comp11)
    (all-on comp10)
  )
)
(:action evaluate-comp10-off-comp6
  :parameters ()
  :precondition (and
    (evaluate comp10)
    (not (rebooted comp10))
    (not (running comp6))
  )
  :effect (and
    (not (evaluate comp10))
    (evaluate comp11)
    (one-off comp10)
  )
)
(:action evaluate-comp10-off-comp9
  :parameters ()
  :precondition (and
    (evaluate comp10)
    (not (rebooted comp10))
    (not (running comp9))
  )
  :effect (and
    (not (evaluate comp10))
    (evaluate comp11)
    (one-off comp10)
  )
)
(:action evaluate-comp11-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp11)
    (rebooted comp11)
  )
  :effect (and
    (not (evaluate comp11))
    (update-status comp0)
  )
)
(:action evaluate-comp11-all-on
  :parameters ()
  :precondition (and
    (evaluate comp11)
    (not (rebooted comp11))
    (running comp10)
  )
  :effect (and
    (not (evaluate comp11))
    (update-status comp0)
    (all-on comp11)
  )
)
(:action evaluate-comp11-off-comp10
  :parameters ()
  :precondition (and
    (evaluate comp11)
    (not (rebooted comp11))
    (not (running comp10))
  )
  :effect (and
    (not (evaluate comp11))
    (update-status comp0)
    (one-off comp11)
  )
)
(:action update-status-comp0-rebooted
  :parameters ()
  :precondition (and
    (update-status comp0)
    (rebooted comp0)
  )
  :effect (and
    (not (update-status comp0))
    (update-status comp1)
    (not (rebooted comp0))
    (probabilistic 9/10 (and (running comp0)) 1/10 (and))
  )
)
(:action update-status-comp0-all-on
  :parameters ()
  :precondition (and
    (update-status comp0)
    (not (rebooted comp0))
    (all-on comp0)
  )
  :effect (and
    (not (update-status comp0))
    (update-status comp1)
    (not (all-on comp0))
    (probabilistic 1/20 (and (not (running comp0))))
  )
)
(:action update-status-comp0-one-off
  :parameters ()
  :precondition (and
    (update-status comp0)
    (not (rebooted comp0))
    (one-off comp0)
  )
  :effect (and
    (not (update-status comp0))
    (update-status comp1)
    (not (one-off comp0))
    (probabilistic 1/4 (and (not (running comp0))))
  )
)
(:action update-status-comp1-rebooted
  :parameters ()
  :precondition (and
    (update-status comp1)
    (rebooted comp1)
  )
  :effect (and
    (not (update-status comp1))
    (update-status comp2)
    (not (rebooted comp1))
    (probabilistic 9/10 (and (running comp1)) 1/10 (and))
  )
)
(:action update-status-comp1-all-on
  :parameters ()
  :precondition (and
    (update-status comp1)
    (not (rebooted comp1))
    (all-on comp1)
  )
  :effect (and
    (not (update-status comp1))
    (update-status comp2)
    (not (all-on comp1))
    (probabilistic 1/20 (and (not (running comp1))))
  )
)
(:action update-status-comp1-one-off
  :parameters ()
  :precondition (and
    (update-status comp1)
    (not (rebooted comp1))
    (one-off comp1)
  )
  :effect (and
    (not (update-status comp1))
    (update-status comp2)
    (not (one-off comp1))
    (probabilistic 1/4 (and (not (running comp1))))
  )
)
(:action update-status-comp2-rebooted
  :parameters ()
  :precondition (and
    (update-status comp2)
    (rebooted comp2)
  )
  :effect (and
    (not (update-status comp2))
    (update-status comp3)
    (not (rebooted comp2))
    (probabilistic 9/10 (and (running comp2)) 1/10 (and))
  )
)
(:action update-status-comp2-all-on
  :parameters ()
  :precondition (and
    (update-status comp2)
    (not (rebooted comp2))
    (all-on comp2)
  )
  :effect (and
    (not (update-status comp2))
    (update-status comp3)
    (not (all-on comp2))
    (probabilistic 1/20 (and (not (running comp2))))
  )
)
(:action update-status-comp2-one-off
  :parameters ()
  :precondition (and
    (update-status comp2)
    (not (rebooted comp2))
    (one-off comp2)
  )
  :effect (and
    (not (update-status comp2))
    (update-status comp3)
    (not (one-off comp2))
    (probabilistic 1/4 (and (not (running comp2))))
  )
)
(:action update-status-comp3-rebooted
  :parameters ()
  :precondition (and
    (update-status comp3)
    (rebooted comp3)
  )
  :effect (and
    (not (update-status comp3))
    (update-status comp4)
    (not (rebooted comp3))
    (probabilistic 9/10 (and (running comp3)) 1/10 (and))
  )
)
(:action update-status-comp3-all-on
  :parameters ()
  :precondition (and
    (update-status comp3)
    (not (rebooted comp3))
    (all-on comp3)
  )
  :effect (and
    (not (update-status comp3))
    (update-status comp4)
    (not (all-on comp3))
    (probabilistic 1/20 (and (not (running comp3))))
  )
)
(:action update-status-comp3-one-off
  :parameters ()
  :precondition (and
    (update-status comp3)
    (not (rebooted comp3))
    (one-off comp3)
  )
  :effect (and
    (not (update-status comp3))
    (update-status comp4)
    (not (one-off comp3))
    (probabilistic 1/4 (and (not (running comp3))))
  )
)
(:action update-status-comp4-rebooted
  :parameters ()
  :precondition (and
    (update-status comp4)
    (rebooted comp4)
  )
  :effect (and
    (not (update-status comp4))
    (update-status comp5)
    (not (rebooted comp4))
    (probabilistic 9/10 (and (running comp4)) 1/10 (and))
  )
)
(:action update-status-comp4-all-on
  :parameters ()
  :precondition (and
    (update-status comp4)
    (not (rebooted comp4))
    (all-on comp4)
  )
  :effect (and
    (not (update-status comp4))
    (update-status comp5)
    (not (all-on comp4))
    (probabilistic 1/20 (and (not (running comp4))))
  )
)
(:action update-status-comp4-one-off
  :parameters ()
  :precondition (and
    (update-status comp4)
    (not (rebooted comp4))
    (one-off comp4)
  )
  :effect (and
    (not (update-status comp4))
    (update-status comp5)
    (not (one-off comp4))
    (probabilistic 1/4 (and (not (running comp4))))
  )
)
(:action update-status-comp5-rebooted
  :parameters ()
  :precondition (and
    (update-status comp5)
    (rebooted comp5)
  )
  :effect (and
    (not (update-status comp5))
    (update-status comp6)
    (not (rebooted comp5))
    (probabilistic 9/10 (and (running comp5)) 1/10 (and))
  )
)
(:action update-status-comp5-all-on
  :parameters ()
  :precondition (and
    (update-status comp5)
    (not (rebooted comp5))
    (all-on comp5)
  )
  :effect (and
    (not (update-status comp5))
    (update-status comp6)
    (not (all-on comp5))
    (probabilistic 1/20 (and (not (running comp5))))
  )
)
(:action update-status-comp5-one-off
  :parameters ()
  :precondition (and
    (update-status comp5)
    (not (rebooted comp5))
    (one-off comp5)
  )
  :effect (and
    (not (update-status comp5))
    (update-status comp6)
    (not (one-off comp5))
    (probabilistic 1/4 (and (not (running comp5))))
  )
)
(:action update-status-comp6-rebooted
  :parameters ()
  :precondition (and
    (update-status comp6)
    (rebooted comp6)
  )
  :effect (and
    (not (update-status comp6))
    (update-status comp7)
    (not (rebooted comp6))
    (probabilistic 9/10 (and (running comp6)) 1/10 (and))
  )
)
(:action update-status-comp6-all-on
  :parameters ()
  :precondition (and
    (update-status comp6)
    (not (rebooted comp6))
    (all-on comp6)
  )
  :effect (and
    (not (update-status comp6))
    (update-status comp7)
    (not (all-on comp6))
    (probabilistic 1/20 (and (not (running comp6))))
  )
)
(:action update-status-comp6-one-off
  :parameters ()
  :precondition (and
    (update-status comp6)
    (not (rebooted comp6))
    (one-off comp6)
  )
  :effect (and
    (not (update-status comp6))
    (update-status comp7)
    (not (one-off comp6))
    (probabilistic 1/4 (and (not (running comp6))))
  )
)
(:action update-status-comp7-rebooted
  :parameters ()
  :precondition (and
    (update-status comp7)
    (rebooted comp7)
  )
  :effect (and
    (not (update-status comp7))
    (update-status comp8)
    (not (rebooted comp7))
    (probabilistic 9/10 (and (running comp7)) 1/10 (and))
  )
)
(:action update-status-comp7-all-on
  :parameters ()
  :precondition (and
    (update-status comp7)
    (not (rebooted comp7))
    (all-on comp7)
  )
  :effect (and
    (not (update-status comp7))
    (update-status comp8)
    (not (all-on comp7))
    (probabilistic 1/20 (and (not (running comp7))))
  )
)
(:action update-status-comp7-one-off
  :parameters ()
  :precondition (and
    (update-status comp7)
    (not (rebooted comp7))
    (one-off comp7)
  )
  :effect (and
    (not (update-status comp7))
    (update-status comp8)
    (not (one-off comp7))
    (probabilistic 1/4 (and (not (running comp7))))
  )
)
(:action update-status-comp8-rebooted
  :parameters ()
  :precondition (and
    (update-status comp8)
    (rebooted comp8)
  )
  :effect (and
    (not (update-status comp8))
    (update-status comp9)
    (not (rebooted comp8))
    (probabilistic 9/10 (and (running comp8)) 1/10 (and))
  )
)
(:action update-status-comp8-all-on
  :parameters ()
  :precondition (and
    (update-status comp8)
    (not (rebooted comp8))
    (all-on comp8)
  )
  :effect (and
    (not (update-status comp8))
    (update-status comp9)
    (not (all-on comp8))
    (probabilistic 1/20 (and (not (running comp8))))
  )
)
(:action update-status-comp8-one-off
  :parameters ()
  :precondition (and
    (update-status comp8)
    (not (rebooted comp8))
    (one-off comp8)
  )
  :effect (and
    (not (update-status comp8))
    (update-status comp9)
    (not (one-off comp8))
    (probabilistic 1/4 (and (not (running comp8))))
  )
)
(:action update-status-comp9-rebooted
  :parameters ()
  :precondition (and
    (update-status comp9)
    (rebooted comp9)
  )
  :effect (and
    (not (update-status comp9))
    (update-status comp10)
    (not (rebooted comp9))
    (probabilistic 9/10 (and (running comp9)) 1/10 (and))
  )
)
(:action update-status-comp9-all-on
  :parameters ()
  :precondition (and
    (update-status comp9)
    (not (rebooted comp9))
    (all-on comp9)
  )
  :effect (and
    (not (update-status comp9))
    (update-status comp10)
    (not (all-on comp9))
    (probabilistic 1/20 (and (not (running comp9))))
  )
)
(:action update-status-comp9-one-off
  :parameters ()
  :precondition (and
    (update-status comp9)
    (not (rebooted comp9))
    (one-off comp9)
  )
  :effect (and
    (not (update-status comp9))
    (update-status comp10)
    (not (one-off comp9))
    (probabilistic 1/4 (and (not (running comp9))))
  )
)
(:action update-status-comp10-rebooted
  :parameters ()
  :precondition (and
    (update-status comp10)
    (rebooted comp10)
  )
  :effect (and
    (not (update-status comp10))
    (update-status comp11)
    (not (rebooted comp10))
    (probabilistic 9/10 (and (running comp10)) 1/10 (and))
  )
)
(:action update-status-comp10-all-on
  :parameters ()
  :precondition (and
    (update-status comp10)
    (not (rebooted comp10))
    (all-on comp10)
  )
  :effect (and
    (not (update-status comp10))
    (update-status comp11)
    (not (all-on comp10))
    (probabilistic 1/20 (and (not (running comp10))))
  )
)
(:action update-status-comp10-one-off
  :parameters ()
  :precondition (and
    (update-status comp10)
    (not (rebooted comp10))
    (one-off comp10)
  )
  :effect (and
    (not (update-status comp10))
    (update-status comp11)
    (not (one-off comp10))
    (probabilistic 1/4 (and (not (running comp10))))
  )
)
(:action update-status-comp11-rebooted
  :parameters ()
  :precondition (and
    (update-status comp11)
    (rebooted comp11)
  )
  :effect (and
    (not (update-status comp11))
    (all-updated)
    (not (rebooted comp11))
    (probabilistic 9/10 (and (running comp11)) 1/10 (and))
  )
)
(:action update-status-comp11-all-on
  :parameters ()
  :precondition (and
    (update-status comp11)
    (not (rebooted comp11))
    (all-on comp11)
  )
  :effect (and
    (not (update-status comp11))
    (all-updated)
    (not (all-on comp11))
    (probabilistic 1/20 (and (not (running comp11))))
  )
)
(:action update-status-comp11-one-off
  :parameters ()
  :precondition (and
    (update-status comp11)
    (not (rebooted comp11))
    (one-off comp11)
  )
  :effect (and
    (not (update-status comp11))
    (all-updated)
    (not (one-off comp11))
    (probabilistic 1/4 (and (not (running comp11))))
  )
)
)