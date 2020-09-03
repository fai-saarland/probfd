(define (domain sysadmin-nocount)
(:requirements :probabilistic-effects :negative-preconditions :typing)
(:types horizon-value - object computer - object)
(:constants
  comp0 comp1 comp2 comp3 comp4 comp5 comp6 comp7 comp8 comp9 comp10 comp11 comp12 comp13 comp14 comp15 comp16 comp17 comp18 comp19 comp20 comp21 comp22 comp23 comp24 - computer
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
    (running comp22)
    (running comp24)
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (all-on comp0)
  )
)
(:action evaluate-comp0-off-comp22
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp22))
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (one-off comp0)
  )
)
(:action evaluate-comp0-off-comp24
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp24))
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
    (running comp15)
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
(:action evaluate-comp2-off-comp15
  :parameters ()
  :precondition (and
    (evaluate comp2)
    (not (rebooted comp2))
    (not (running comp15))
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
    (running comp11)
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
(:action evaluate-comp3-off-comp11
  :parameters ()
  :precondition (and
    (evaluate comp3)
    (not (rebooted comp3))
    (not (running comp11))
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
    (running comp17)
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
(:action evaluate-comp6-off-comp17
  :parameters ()
  :precondition (and
    (evaluate comp6)
    (not (rebooted comp6))
    (not (running comp17))
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
    (running comp5)
    (running comp6)
  )
  :effect (and
    (not (evaluate comp7))
    (evaluate comp8)
    (all-on comp7)
  )
)
(:action evaluate-comp7-off-comp5
  :parameters ()
  :precondition (and
    (evaluate comp7)
    (not (rebooted comp7))
    (not (running comp5))
  )
  :effect (and
    (not (evaluate comp7))
    (evaluate comp8)
    (one-off comp7)
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
    (running comp5)
    (running comp7)
    (running comp22)
  )
  :effect (and
    (not (evaluate comp8))
    (evaluate comp9)
    (all-on comp8)
  )
)
(:action evaluate-comp8-off-comp5
  :parameters ()
  :precondition (and
    (evaluate comp8)
    (not (rebooted comp8))
    (not (running comp5))
  )
  :effect (and
    (not (evaluate comp8))
    (evaluate comp9)
    (one-off comp8)
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
(:action evaluate-comp8-off-comp22
  :parameters ()
  :precondition (and
    (evaluate comp8)
    (not (rebooted comp8))
    (not (running comp22))
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
    (running comp0)
    (running comp8)
  )
  :effect (and
    (not (evaluate comp9))
    (evaluate comp10)
    (all-on comp9)
  )
)
(:action evaluate-comp9-off-comp0
  :parameters ()
  :precondition (and
    (evaluate comp9)
    (not (rebooted comp9))
    (not (running comp0))
  )
  :effect (and
    (not (evaluate comp9))
    (evaluate comp10)
    (one-off comp9)
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
    (running comp9)
  )
  :effect (and
    (not (evaluate comp10))
    (evaluate comp11)
    (all-on comp10)
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
    (evaluate comp12)
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
    (evaluate comp12)
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
    (evaluate comp12)
    (one-off comp11)
  )
)
(:action evaluate-comp12-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp12)
    (rebooted comp12)
  )
  :effect (and
    (not (evaluate comp12))
    (evaluate comp13)
  )
)
(:action evaluate-comp12-all-on
  :parameters ()
  :precondition (and
    (evaluate comp12)
    (not (rebooted comp12))
    (running comp11)
  )
  :effect (and
    (not (evaluate comp12))
    (evaluate comp13)
    (all-on comp12)
  )
)
(:action evaluate-comp12-off-comp11
  :parameters ()
  :precondition (and
    (evaluate comp12)
    (not (rebooted comp12))
    (not (running comp11))
  )
  :effect (and
    (not (evaluate comp12))
    (evaluate comp13)
    (one-off comp12)
  )
)
(:action evaluate-comp13-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp13)
    (rebooted comp13)
  )
  :effect (and
    (not (evaluate comp13))
    (evaluate comp14)
  )
)
(:action evaluate-comp13-all-on
  :parameters ()
  :precondition (and
    (evaluate comp13)
    (not (rebooted comp13))
    (running comp12)
    (running comp21)
  )
  :effect (and
    (not (evaluate comp13))
    (evaluate comp14)
    (all-on comp13)
  )
)
(:action evaluate-comp13-off-comp12
  :parameters ()
  :precondition (and
    (evaluate comp13)
    (not (rebooted comp13))
    (not (running comp12))
  )
  :effect (and
    (not (evaluate comp13))
    (evaluate comp14)
    (one-off comp13)
  )
)
(:action evaluate-comp13-off-comp21
  :parameters ()
  :precondition (and
    (evaluate comp13)
    (not (rebooted comp13))
    (not (running comp21))
  )
  :effect (and
    (not (evaluate comp13))
    (evaluate comp14)
    (one-off comp13)
  )
)
(:action evaluate-comp14-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp14)
    (rebooted comp14)
  )
  :effect (and
    (not (evaluate comp14))
    (evaluate comp15)
  )
)
(:action evaluate-comp14-all-on
  :parameters ()
  :precondition (and
    (evaluate comp14)
    (not (rebooted comp14))
    (running comp13)
  )
  :effect (and
    (not (evaluate comp14))
    (evaluate comp15)
    (all-on comp14)
  )
)
(:action evaluate-comp14-off-comp13
  :parameters ()
  :precondition (and
    (evaluate comp14)
    (not (rebooted comp14))
    (not (running comp13))
  )
  :effect (and
    (not (evaluate comp14))
    (evaluate comp15)
    (one-off comp14)
  )
)
(:action evaluate-comp15-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp15)
    (rebooted comp15)
  )
  :effect (and
    (not (evaluate comp15))
    (evaluate comp16)
  )
)
(:action evaluate-comp15-all-on
  :parameters ()
  :precondition (and
    (evaluate comp15)
    (not (rebooted comp15))
    (running comp14)
  )
  :effect (and
    (not (evaluate comp15))
    (evaluate comp16)
    (all-on comp15)
  )
)
(:action evaluate-comp15-off-comp14
  :parameters ()
  :precondition (and
    (evaluate comp15)
    (not (rebooted comp15))
    (not (running comp14))
  )
  :effect (and
    (not (evaluate comp15))
    (evaluate comp16)
    (one-off comp15)
  )
)
(:action evaluate-comp16-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp16)
    (rebooted comp16)
  )
  :effect (and
    (not (evaluate comp16))
    (evaluate comp17)
  )
)
(:action evaluate-comp16-all-on
  :parameters ()
  :precondition (and
    (evaluate comp16)
    (not (rebooted comp16))
    (running comp2)
    (running comp7)
    (running comp15)
  )
  :effect (and
    (not (evaluate comp16))
    (evaluate comp17)
    (all-on comp16)
  )
)
(:action evaluate-comp16-off-comp2
  :parameters ()
  :precondition (and
    (evaluate comp16)
    (not (rebooted comp16))
    (not (running comp2))
  )
  :effect (and
    (not (evaluate comp16))
    (evaluate comp17)
    (one-off comp16)
  )
)
(:action evaluate-comp16-off-comp7
  :parameters ()
  :precondition (and
    (evaluate comp16)
    (not (rebooted comp16))
    (not (running comp7))
  )
  :effect (and
    (not (evaluate comp16))
    (evaluate comp17)
    (one-off comp16)
  )
)
(:action evaluate-comp16-off-comp15
  :parameters ()
  :precondition (and
    (evaluate comp16)
    (not (rebooted comp16))
    (not (running comp15))
  )
  :effect (and
    (not (evaluate comp16))
    (evaluate comp17)
    (one-off comp16)
  )
)
(:action evaluate-comp17-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp17)
    (rebooted comp17)
  )
  :effect (and
    (not (evaluate comp17))
    (evaluate comp18)
  )
)
(:action evaluate-comp17-all-on
  :parameters ()
  :precondition (and
    (evaluate comp17)
    (not (rebooted comp17))
    (running comp16)
    (running comp21)
  )
  :effect (and
    (not (evaluate comp17))
    (evaluate comp18)
    (all-on comp17)
  )
)
(:action evaluate-comp17-off-comp16
  :parameters ()
  :precondition (and
    (evaluate comp17)
    (not (rebooted comp17))
    (not (running comp16))
  )
  :effect (and
    (not (evaluate comp17))
    (evaluate comp18)
    (one-off comp17)
  )
)
(:action evaluate-comp17-off-comp21
  :parameters ()
  :precondition (and
    (evaluate comp17)
    (not (rebooted comp17))
    (not (running comp21))
  )
  :effect (and
    (not (evaluate comp17))
    (evaluate comp18)
    (one-off comp17)
  )
)
(:action evaluate-comp18-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp18)
    (rebooted comp18)
  )
  :effect (and
    (not (evaluate comp18))
    (evaluate comp19)
  )
)
(:action evaluate-comp18-all-on
  :parameters ()
  :precondition (and
    (evaluate comp18)
    (not (rebooted comp18))
    (running comp17)
  )
  :effect (and
    (not (evaluate comp18))
    (evaluate comp19)
    (all-on comp18)
  )
)
(:action evaluate-comp18-off-comp17
  :parameters ()
  :precondition (and
    (evaluate comp18)
    (not (rebooted comp18))
    (not (running comp17))
  )
  :effect (and
    (not (evaluate comp18))
    (evaluate comp19)
    (one-off comp18)
  )
)
(:action evaluate-comp19-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp19)
    (rebooted comp19)
  )
  :effect (and
    (not (evaluate comp19))
    (evaluate comp20)
  )
)
(:action evaluate-comp19-all-on
  :parameters ()
  :precondition (and
    (evaluate comp19)
    (not (rebooted comp19))
    (running comp18)
  )
  :effect (and
    (not (evaluate comp19))
    (evaluate comp20)
    (all-on comp19)
  )
)
(:action evaluate-comp19-off-comp18
  :parameters ()
  :precondition (and
    (evaluate comp19)
    (not (rebooted comp19))
    (not (running comp18))
  )
  :effect (and
    (not (evaluate comp19))
    (evaluate comp20)
    (one-off comp19)
  )
)
(:action evaluate-comp20-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp20)
    (rebooted comp20)
  )
  :effect (and
    (not (evaluate comp20))
    (evaluate comp21)
  )
)
(:action evaluate-comp20-all-on
  :parameters ()
  :precondition (and
    (evaluate comp20)
    (not (rebooted comp20))
    (running comp19)
  )
  :effect (and
    (not (evaluate comp20))
    (evaluate comp21)
    (all-on comp20)
  )
)
(:action evaluate-comp20-off-comp19
  :parameters ()
  :precondition (and
    (evaluate comp20)
    (not (rebooted comp20))
    (not (running comp19))
  )
  :effect (and
    (not (evaluate comp20))
    (evaluate comp21)
    (one-off comp20)
  )
)
(:action evaluate-comp21-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp21)
    (rebooted comp21)
  )
  :effect (and
    (not (evaluate comp21))
    (evaluate comp22)
  )
)
(:action evaluate-comp21-all-on
  :parameters ()
  :precondition (and
    (evaluate comp21)
    (not (rebooted comp21))
    (running comp20)
  )
  :effect (and
    (not (evaluate comp21))
    (evaluate comp22)
    (all-on comp21)
  )
)
(:action evaluate-comp21-off-comp20
  :parameters ()
  :precondition (and
    (evaluate comp21)
    (not (rebooted comp21))
    (not (running comp20))
  )
  :effect (and
    (not (evaluate comp21))
    (evaluate comp22)
    (one-off comp21)
  )
)
(:action evaluate-comp22-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp22)
    (rebooted comp22)
  )
  :effect (and
    (not (evaluate comp22))
    (evaluate comp23)
  )
)
(:action evaluate-comp22-all-on
  :parameters ()
  :precondition (and
    (evaluate comp22)
    (not (rebooted comp22))
    (running comp21)
  )
  :effect (and
    (not (evaluate comp22))
    (evaluate comp23)
    (all-on comp22)
  )
)
(:action evaluate-comp22-off-comp21
  :parameters ()
  :precondition (and
    (evaluate comp22)
    (not (rebooted comp22))
    (not (running comp21))
  )
  :effect (and
    (not (evaluate comp22))
    (evaluate comp23)
    (one-off comp22)
  )
)
(:action evaluate-comp23-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp23)
    (rebooted comp23)
  )
  :effect (and
    (not (evaluate comp23))
    (evaluate comp24)
  )
)
(:action evaluate-comp23-all-on
  :parameters ()
  :precondition (and
    (evaluate comp23)
    (not (rebooted comp23))
    (running comp22)
  )
  :effect (and
    (not (evaluate comp23))
    (evaluate comp24)
    (all-on comp23)
  )
)
(:action evaluate-comp23-off-comp22
  :parameters ()
  :precondition (and
    (evaluate comp23)
    (not (rebooted comp23))
    (not (running comp22))
  )
  :effect (and
    (not (evaluate comp23))
    (evaluate comp24)
    (one-off comp23)
  )
)
(:action evaluate-comp24-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp24)
    (rebooted comp24)
  )
  :effect (and
    (not (evaluate comp24))
    (update-status comp0)
  )
)
(:action evaluate-comp24-all-on
  :parameters ()
  :precondition (and
    (evaluate comp24)
    (not (rebooted comp24))
    (running comp23)
  )
  :effect (and
    (not (evaluate comp24))
    (update-status comp0)
    (all-on comp24)
  )
)
(:action evaluate-comp24-off-comp23
  :parameters ()
  :precondition (and
    (evaluate comp24)
    (not (rebooted comp24))
    (not (running comp23))
  )
  :effect (and
    (not (evaluate comp24))
    (update-status comp0)
    (one-off comp24)
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
    (update-status comp12)
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
    (update-status comp12)
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
    (update-status comp12)
    (not (one-off comp11))
    (probabilistic 1/4 (and (not (running comp11))))
  )
)
(:action update-status-comp12-rebooted
  :parameters ()
  :precondition (and
    (update-status comp12)
    (rebooted comp12)
  )
  :effect (and
    (not (update-status comp12))
    (update-status comp13)
    (not (rebooted comp12))
    (probabilistic 9/10 (and (running comp12)) 1/10 (and))
  )
)
(:action update-status-comp12-all-on
  :parameters ()
  :precondition (and
    (update-status comp12)
    (not (rebooted comp12))
    (all-on comp12)
  )
  :effect (and
    (not (update-status comp12))
    (update-status comp13)
    (not (all-on comp12))
    (probabilistic 1/20 (and (not (running comp12))))
  )
)
(:action update-status-comp12-one-off
  :parameters ()
  :precondition (and
    (update-status comp12)
    (not (rebooted comp12))
    (one-off comp12)
  )
  :effect (and
    (not (update-status comp12))
    (update-status comp13)
    (not (one-off comp12))
    (probabilistic 1/4 (and (not (running comp12))))
  )
)
(:action update-status-comp13-rebooted
  :parameters ()
  :precondition (and
    (update-status comp13)
    (rebooted comp13)
  )
  :effect (and
    (not (update-status comp13))
    (update-status comp14)
    (not (rebooted comp13))
    (probabilistic 9/10 (and (running comp13)) 1/10 (and))
  )
)
(:action update-status-comp13-all-on
  :parameters ()
  :precondition (and
    (update-status comp13)
    (not (rebooted comp13))
    (all-on comp13)
  )
  :effect (and
    (not (update-status comp13))
    (update-status comp14)
    (not (all-on comp13))
    (probabilistic 1/20 (and (not (running comp13))))
  )
)
(:action update-status-comp13-one-off
  :parameters ()
  :precondition (and
    (update-status comp13)
    (not (rebooted comp13))
    (one-off comp13)
  )
  :effect (and
    (not (update-status comp13))
    (update-status comp14)
    (not (one-off comp13))
    (probabilistic 1/4 (and (not (running comp13))))
  )
)
(:action update-status-comp14-rebooted
  :parameters ()
  :precondition (and
    (update-status comp14)
    (rebooted comp14)
  )
  :effect (and
    (not (update-status comp14))
    (update-status comp15)
    (not (rebooted comp14))
    (probabilistic 9/10 (and (running comp14)) 1/10 (and))
  )
)
(:action update-status-comp14-all-on
  :parameters ()
  :precondition (and
    (update-status comp14)
    (not (rebooted comp14))
    (all-on comp14)
  )
  :effect (and
    (not (update-status comp14))
    (update-status comp15)
    (not (all-on comp14))
    (probabilistic 1/20 (and (not (running comp14))))
  )
)
(:action update-status-comp14-one-off
  :parameters ()
  :precondition (and
    (update-status comp14)
    (not (rebooted comp14))
    (one-off comp14)
  )
  :effect (and
    (not (update-status comp14))
    (update-status comp15)
    (not (one-off comp14))
    (probabilistic 1/4 (and (not (running comp14))))
  )
)
(:action update-status-comp15-rebooted
  :parameters ()
  :precondition (and
    (update-status comp15)
    (rebooted comp15)
  )
  :effect (and
    (not (update-status comp15))
    (update-status comp16)
    (not (rebooted comp15))
    (probabilistic 9/10 (and (running comp15)) 1/10 (and))
  )
)
(:action update-status-comp15-all-on
  :parameters ()
  :precondition (and
    (update-status comp15)
    (not (rebooted comp15))
    (all-on comp15)
  )
  :effect (and
    (not (update-status comp15))
    (update-status comp16)
    (not (all-on comp15))
    (probabilistic 1/20 (and (not (running comp15))))
  )
)
(:action update-status-comp15-one-off
  :parameters ()
  :precondition (and
    (update-status comp15)
    (not (rebooted comp15))
    (one-off comp15)
  )
  :effect (and
    (not (update-status comp15))
    (update-status comp16)
    (not (one-off comp15))
    (probabilistic 1/4 (and (not (running comp15))))
  )
)
(:action update-status-comp16-rebooted
  :parameters ()
  :precondition (and
    (update-status comp16)
    (rebooted comp16)
  )
  :effect (and
    (not (update-status comp16))
    (update-status comp17)
    (not (rebooted comp16))
    (probabilistic 9/10 (and (running comp16)) 1/10 (and))
  )
)
(:action update-status-comp16-all-on
  :parameters ()
  :precondition (and
    (update-status comp16)
    (not (rebooted comp16))
    (all-on comp16)
  )
  :effect (and
    (not (update-status comp16))
    (update-status comp17)
    (not (all-on comp16))
    (probabilistic 1/20 (and (not (running comp16))))
  )
)
(:action update-status-comp16-one-off
  :parameters ()
  :precondition (and
    (update-status comp16)
    (not (rebooted comp16))
    (one-off comp16)
  )
  :effect (and
    (not (update-status comp16))
    (update-status comp17)
    (not (one-off comp16))
    (probabilistic 1/4 (and (not (running comp16))))
  )
)
(:action update-status-comp17-rebooted
  :parameters ()
  :precondition (and
    (update-status comp17)
    (rebooted comp17)
  )
  :effect (and
    (not (update-status comp17))
    (update-status comp18)
    (not (rebooted comp17))
    (probabilistic 9/10 (and (running comp17)) 1/10 (and))
  )
)
(:action update-status-comp17-all-on
  :parameters ()
  :precondition (and
    (update-status comp17)
    (not (rebooted comp17))
    (all-on comp17)
  )
  :effect (and
    (not (update-status comp17))
    (update-status comp18)
    (not (all-on comp17))
    (probabilistic 1/20 (and (not (running comp17))))
  )
)
(:action update-status-comp17-one-off
  :parameters ()
  :precondition (and
    (update-status comp17)
    (not (rebooted comp17))
    (one-off comp17)
  )
  :effect (and
    (not (update-status comp17))
    (update-status comp18)
    (not (one-off comp17))
    (probabilistic 1/4 (and (not (running comp17))))
  )
)
(:action update-status-comp18-rebooted
  :parameters ()
  :precondition (and
    (update-status comp18)
    (rebooted comp18)
  )
  :effect (and
    (not (update-status comp18))
    (update-status comp19)
    (not (rebooted comp18))
    (probabilistic 9/10 (and (running comp18)) 1/10 (and))
  )
)
(:action update-status-comp18-all-on
  :parameters ()
  :precondition (and
    (update-status comp18)
    (not (rebooted comp18))
    (all-on comp18)
  )
  :effect (and
    (not (update-status comp18))
    (update-status comp19)
    (not (all-on comp18))
    (probabilistic 1/20 (and (not (running comp18))))
  )
)
(:action update-status-comp18-one-off
  :parameters ()
  :precondition (and
    (update-status comp18)
    (not (rebooted comp18))
    (one-off comp18)
  )
  :effect (and
    (not (update-status comp18))
    (update-status comp19)
    (not (one-off comp18))
    (probabilistic 1/4 (and (not (running comp18))))
  )
)
(:action update-status-comp19-rebooted
  :parameters ()
  :precondition (and
    (update-status comp19)
    (rebooted comp19)
  )
  :effect (and
    (not (update-status comp19))
    (update-status comp20)
    (not (rebooted comp19))
    (probabilistic 9/10 (and (running comp19)) 1/10 (and))
  )
)
(:action update-status-comp19-all-on
  :parameters ()
  :precondition (and
    (update-status comp19)
    (not (rebooted comp19))
    (all-on comp19)
  )
  :effect (and
    (not (update-status comp19))
    (update-status comp20)
    (not (all-on comp19))
    (probabilistic 1/20 (and (not (running comp19))))
  )
)
(:action update-status-comp19-one-off
  :parameters ()
  :precondition (and
    (update-status comp19)
    (not (rebooted comp19))
    (one-off comp19)
  )
  :effect (and
    (not (update-status comp19))
    (update-status comp20)
    (not (one-off comp19))
    (probabilistic 1/4 (and (not (running comp19))))
  )
)
(:action update-status-comp20-rebooted
  :parameters ()
  :precondition (and
    (update-status comp20)
    (rebooted comp20)
  )
  :effect (and
    (not (update-status comp20))
    (update-status comp21)
    (not (rebooted comp20))
    (probabilistic 9/10 (and (running comp20)) 1/10 (and))
  )
)
(:action update-status-comp20-all-on
  :parameters ()
  :precondition (and
    (update-status comp20)
    (not (rebooted comp20))
    (all-on comp20)
  )
  :effect (and
    (not (update-status comp20))
    (update-status comp21)
    (not (all-on comp20))
    (probabilistic 1/20 (and (not (running comp20))))
  )
)
(:action update-status-comp20-one-off
  :parameters ()
  :precondition (and
    (update-status comp20)
    (not (rebooted comp20))
    (one-off comp20)
  )
  :effect (and
    (not (update-status comp20))
    (update-status comp21)
    (not (one-off comp20))
    (probabilistic 1/4 (and (not (running comp20))))
  )
)
(:action update-status-comp21-rebooted
  :parameters ()
  :precondition (and
    (update-status comp21)
    (rebooted comp21)
  )
  :effect (and
    (not (update-status comp21))
    (update-status comp22)
    (not (rebooted comp21))
    (probabilistic 9/10 (and (running comp21)) 1/10 (and))
  )
)
(:action update-status-comp21-all-on
  :parameters ()
  :precondition (and
    (update-status comp21)
    (not (rebooted comp21))
    (all-on comp21)
  )
  :effect (and
    (not (update-status comp21))
    (update-status comp22)
    (not (all-on comp21))
    (probabilistic 1/20 (and (not (running comp21))))
  )
)
(:action update-status-comp21-one-off
  :parameters ()
  :precondition (and
    (update-status comp21)
    (not (rebooted comp21))
    (one-off comp21)
  )
  :effect (and
    (not (update-status comp21))
    (update-status comp22)
    (not (one-off comp21))
    (probabilistic 1/4 (and (not (running comp21))))
  )
)
(:action update-status-comp22-rebooted
  :parameters ()
  :precondition (and
    (update-status comp22)
    (rebooted comp22)
  )
  :effect (and
    (not (update-status comp22))
    (update-status comp23)
    (not (rebooted comp22))
    (probabilistic 9/10 (and (running comp22)) 1/10 (and))
  )
)
(:action update-status-comp22-all-on
  :parameters ()
  :precondition (and
    (update-status comp22)
    (not (rebooted comp22))
    (all-on comp22)
  )
  :effect (and
    (not (update-status comp22))
    (update-status comp23)
    (not (all-on comp22))
    (probabilistic 1/20 (and (not (running comp22))))
  )
)
(:action update-status-comp22-one-off
  :parameters ()
  :precondition (and
    (update-status comp22)
    (not (rebooted comp22))
    (one-off comp22)
  )
  :effect (and
    (not (update-status comp22))
    (update-status comp23)
    (not (one-off comp22))
    (probabilistic 1/4 (and (not (running comp22))))
  )
)
(:action update-status-comp23-rebooted
  :parameters ()
  :precondition (and
    (update-status comp23)
    (rebooted comp23)
  )
  :effect (and
    (not (update-status comp23))
    (update-status comp24)
    (not (rebooted comp23))
    (probabilistic 9/10 (and (running comp23)) 1/10 (and))
  )
)
(:action update-status-comp23-all-on
  :parameters ()
  :precondition (and
    (update-status comp23)
    (not (rebooted comp23))
    (all-on comp23)
  )
  :effect (and
    (not (update-status comp23))
    (update-status comp24)
    (not (all-on comp23))
    (probabilistic 1/20 (and (not (running comp23))))
  )
)
(:action update-status-comp23-one-off
  :parameters ()
  :precondition (and
    (update-status comp23)
    (not (rebooted comp23))
    (one-off comp23)
  )
  :effect (and
    (not (update-status comp23))
    (update-status comp24)
    (not (one-off comp23))
    (probabilistic 1/4 (and (not (running comp23))))
  )
)
(:action update-status-comp24-rebooted
  :parameters ()
  :precondition (and
    (update-status comp24)
    (rebooted comp24)
  )
  :effect (and
    (not (update-status comp24))
    (all-updated)
    (not (rebooted comp24))
    (probabilistic 9/10 (and (running comp24)) 1/10 (and))
  )
)
(:action update-status-comp24-all-on
  :parameters ()
  :precondition (and
    (update-status comp24)
    (not (rebooted comp24))
    (all-on comp24)
  )
  :effect (and
    (not (update-status comp24))
    (all-updated)
    (not (all-on comp24))
    (probabilistic 1/20 (and (not (running comp24))))
  )
)
(:action update-status-comp24-one-off
  :parameters ()
  :precondition (and
    (update-status comp24)
    (not (rebooted comp24))
    (one-off comp24)
  )
  :effect (and
    (not (update-status comp24))
    (all-updated)
    (not (one-off comp24))
    (probabilistic 1/4 (and (not (running comp24))))
  )
)
)