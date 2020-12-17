(define (domain sysadmin-nocount)
(:requirements :probabilistic-effects :negative-preconditions :typing)
(:types horizon-value - object computer - object)
(:constants horzn1 - horizon-value 
  comp0 comp1 comp2 comp3 comp4 comp5 comp6 comp7 comp8 comp9 comp10 comp11 comp12 comp13 comp14 comp15 comp16 comp17 comp18 comp19 comp20 comp21 comp22 comp23 comp24 comp25 comp26 comp27 comp28 comp29 comp30 comp31 comp32 comp33 comp34 comp35 comp36 comp37 comp38 comp39 comp40 comp41 comp42 comp43 comp44 comp45 comp46 comp47 comp48 comp49 comp50 comp51 comp52 comp53 comp54 comp55 comp56 comp57 comp58 comp59 - computer
)
(:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 ?h2 - horizon-value)
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
  :precondition (and (horizon ?hcur) (horizon-decrement ?hcur horzn1 ?hnew)
    (all-updated)
    (not (running ?c))
  )
  :effect (and (not (horizon ?hcur)) (horizon ?hnew)
    (increase (total-cost) 1)
    (not (all-updated))
    (running ?c)
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
    (running comp41)
    (running comp57)
    (running comp59)
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (all-on comp0)
  )
)
(:action evaluate-comp0-off-comp41
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp41))
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (one-off comp0)
  )
)
(:action evaluate-comp0-off-comp57
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp57))
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (one-off comp0)
  )
)
(:action evaluate-comp0-off-comp59
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp59))
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
    (running comp16)
    (running comp20)
    (running comp23)
    (running comp38)
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
(:action evaluate-comp4-off-comp16
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (not (running comp16))
  )
  :effect (and
    (not (evaluate comp4))
    (evaluate comp5)
    (one-off comp4)
  )
)
(:action evaluate-comp4-off-comp20
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (not (running comp20))
  )
  :effect (and
    (not (evaluate comp4))
    (evaluate comp5)
    (one-off comp4)
  )
)
(:action evaluate-comp4-off-comp23
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (not (running comp23))
  )
  :effect (and
    (not (evaluate comp4))
    (evaluate comp5)
    (one-off comp4)
  )
)
(:action evaluate-comp4-off-comp38
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (not (running comp38))
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
    (running comp52)
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
(:action evaluate-comp5-off-comp52
  :parameters ()
  :precondition (and
    (evaluate comp5)
    (not (rebooted comp5))
    (not (running comp52))
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
    (running comp22)
    (running comp33)
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
(:action evaluate-comp6-off-comp22
  :parameters ()
  :precondition (and
    (evaluate comp6)
    (not (rebooted comp6))
    (not (running comp22))
  )
  :effect (and
    (not (evaluate comp6))
    (evaluate comp7)
    (one-off comp6)
  )
)
(:action evaluate-comp6-off-comp33
  :parameters ()
  :precondition (and
    (evaluate comp6)
    (not (rebooted comp6))
    (not (running comp33))
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
    (running comp0)
    (running comp12)
    (running comp48)
  )
  :effect (and
    (not (evaluate comp13))
    (evaluate comp14)
    (all-on comp13)
  )
)
(:action evaluate-comp13-off-comp0
  :parameters ()
  :precondition (and
    (evaluate comp13)
    (not (rebooted comp13))
    (not (running comp0))
  )
  :effect (and
    (not (evaluate comp13))
    (evaluate comp14)
    (one-off comp13)
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
(:action evaluate-comp13-off-comp48
  :parameters ()
  :precondition (and
    (evaluate comp13)
    (not (rebooted comp13))
    (not (running comp48))
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
    (running comp35)
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
(:action evaluate-comp14-off-comp35
  :parameters ()
  :precondition (and
    (evaluate comp14)
    (not (rebooted comp14))
    (not (running comp35))
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
    (running comp15)
  )
  :effect (and
    (not (evaluate comp16))
    (evaluate comp17)
    (all-on comp16)
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
    (running comp38)
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
(:action evaluate-comp23-off-comp38
  :parameters ()
  :precondition (and
    (evaluate comp23)
    (not (rebooted comp23))
    (not (running comp38))
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
    (evaluate comp25)
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
    (evaluate comp25)
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
    (evaluate comp25)
    (one-off comp24)
  )
)
(:action evaluate-comp25-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp25)
    (rebooted comp25)
  )
  :effect (and
    (not (evaluate comp25))
    (evaluate comp26)
  )
)
(:action evaluate-comp25-all-on
  :parameters ()
  :precondition (and
    (evaluate comp25)
    (not (rebooted comp25))
    (running comp24)
    (running comp44)
  )
  :effect (and
    (not (evaluate comp25))
    (evaluate comp26)
    (all-on comp25)
  )
)
(:action evaluate-comp25-off-comp24
  :parameters ()
  :precondition (and
    (evaluate comp25)
    (not (rebooted comp25))
    (not (running comp24))
  )
  :effect (and
    (not (evaluate comp25))
    (evaluate comp26)
    (one-off comp25)
  )
)
(:action evaluate-comp25-off-comp44
  :parameters ()
  :precondition (and
    (evaluate comp25)
    (not (rebooted comp25))
    (not (running comp44))
  )
  :effect (and
    (not (evaluate comp25))
    (evaluate comp26)
    (one-off comp25)
  )
)
(:action evaluate-comp26-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp26)
    (rebooted comp26)
  )
  :effect (and
    (not (evaluate comp26))
    (evaluate comp27)
  )
)
(:action evaluate-comp26-all-on
  :parameters ()
  :precondition (and
    (evaluate comp26)
    (not (rebooted comp26))
    (running comp25)
  )
  :effect (and
    (not (evaluate comp26))
    (evaluate comp27)
    (all-on comp26)
  )
)
(:action evaluate-comp26-off-comp25
  :parameters ()
  :precondition (and
    (evaluate comp26)
    (not (rebooted comp26))
    (not (running comp25))
  )
  :effect (and
    (not (evaluate comp26))
    (evaluate comp27)
    (one-off comp26)
  )
)
(:action evaluate-comp27-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp27)
    (rebooted comp27)
  )
  :effect (and
    (not (evaluate comp27))
    (evaluate comp28)
  )
)
(:action evaluate-comp27-all-on
  :parameters ()
  :precondition (and
    (evaluate comp27)
    (not (rebooted comp27))
    (running comp26)
    (running comp43)
  )
  :effect (and
    (not (evaluate comp27))
    (evaluate comp28)
    (all-on comp27)
  )
)
(:action evaluate-comp27-off-comp26
  :parameters ()
  :precondition (and
    (evaluate comp27)
    (not (rebooted comp27))
    (not (running comp26))
  )
  :effect (and
    (not (evaluate comp27))
    (evaluate comp28)
    (one-off comp27)
  )
)
(:action evaluate-comp27-off-comp43
  :parameters ()
  :precondition (and
    (evaluate comp27)
    (not (rebooted comp27))
    (not (running comp43))
  )
  :effect (and
    (not (evaluate comp27))
    (evaluate comp28)
    (one-off comp27)
  )
)
(:action evaluate-comp28-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp28)
    (rebooted comp28)
  )
  :effect (and
    (not (evaluate comp28))
    (evaluate comp29)
  )
)
(:action evaluate-comp28-all-on
  :parameters ()
  :precondition (and
    (evaluate comp28)
    (not (rebooted comp28))
    (running comp25)
    (running comp27)
    (running comp45)
    (running comp57)
  )
  :effect (and
    (not (evaluate comp28))
    (evaluate comp29)
    (all-on comp28)
  )
)
(:action evaluate-comp28-off-comp25
  :parameters ()
  :precondition (and
    (evaluate comp28)
    (not (rebooted comp28))
    (not (running comp25))
  )
  :effect (and
    (not (evaluate comp28))
    (evaluate comp29)
    (one-off comp28)
  )
)
(:action evaluate-comp28-off-comp27
  :parameters ()
  :precondition (and
    (evaluate comp28)
    (not (rebooted comp28))
    (not (running comp27))
  )
  :effect (and
    (not (evaluate comp28))
    (evaluate comp29)
    (one-off comp28)
  )
)
(:action evaluate-comp28-off-comp45
  :parameters ()
  :precondition (and
    (evaluate comp28)
    (not (rebooted comp28))
    (not (running comp45))
  )
  :effect (and
    (not (evaluate comp28))
    (evaluate comp29)
    (one-off comp28)
  )
)
(:action evaluate-comp28-off-comp57
  :parameters ()
  :precondition (and
    (evaluate comp28)
    (not (rebooted comp28))
    (not (running comp57))
  )
  :effect (and
    (not (evaluate comp28))
    (evaluate comp29)
    (one-off comp28)
  )
)
(:action evaluate-comp29-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp29)
    (rebooted comp29)
  )
  :effect (and
    (not (evaluate comp29))
    (evaluate comp30)
  )
)
(:action evaluate-comp29-all-on
  :parameters ()
  :precondition (and
    (evaluate comp29)
    (not (rebooted comp29))
    (running comp28)
  )
  :effect (and
    (not (evaluate comp29))
    (evaluate comp30)
    (all-on comp29)
  )
)
(:action evaluate-comp29-off-comp28
  :parameters ()
  :precondition (and
    (evaluate comp29)
    (not (rebooted comp29))
    (not (running comp28))
  )
  :effect (and
    (not (evaluate comp29))
    (evaluate comp30)
    (one-off comp29)
  )
)
(:action evaluate-comp30-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp30)
    (rebooted comp30)
  )
  :effect (and
    (not (evaluate comp30))
    (evaluate comp31)
  )
)
(:action evaluate-comp30-all-on
  :parameters ()
  :precondition (and
    (evaluate comp30)
    (not (rebooted comp30))
    (running comp29)
    (running comp55)
  )
  :effect (and
    (not (evaluate comp30))
    (evaluate comp31)
    (all-on comp30)
  )
)
(:action evaluate-comp30-off-comp29
  :parameters ()
  :precondition (and
    (evaluate comp30)
    (not (rebooted comp30))
    (not (running comp29))
  )
  :effect (and
    (not (evaluate comp30))
    (evaluate comp31)
    (one-off comp30)
  )
)
(:action evaluate-comp30-off-comp55
  :parameters ()
  :precondition (and
    (evaluate comp30)
    (not (rebooted comp30))
    (not (running comp55))
  )
  :effect (and
    (not (evaluate comp30))
    (evaluate comp31)
    (one-off comp30)
  )
)
(:action evaluate-comp31-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp31)
    (rebooted comp31)
  )
  :effect (and
    (not (evaluate comp31))
    (evaluate comp32)
  )
)
(:action evaluate-comp31-all-on
  :parameters ()
  :precondition (and
    (evaluate comp31)
    (not (rebooted comp31))
    (running comp30)
  )
  :effect (and
    (not (evaluate comp31))
    (evaluate comp32)
    (all-on comp31)
  )
)
(:action evaluate-comp31-off-comp30
  :parameters ()
  :precondition (and
    (evaluate comp31)
    (not (rebooted comp31))
    (not (running comp30))
  )
  :effect (and
    (not (evaluate comp31))
    (evaluate comp32)
    (one-off comp31)
  )
)
(:action evaluate-comp32-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp32)
    (rebooted comp32)
  )
  :effect (and
    (not (evaluate comp32))
    (evaluate comp33)
  )
)
(:action evaluate-comp32-all-on
  :parameters ()
  :precondition (and
    (evaluate comp32)
    (not (rebooted comp32))
    (running comp0)
    (running comp28)
    (running comp31)
    (running comp48)
  )
  :effect (and
    (not (evaluate comp32))
    (evaluate comp33)
    (all-on comp32)
  )
)
(:action evaluate-comp32-off-comp0
  :parameters ()
  :precondition (and
    (evaluate comp32)
    (not (rebooted comp32))
    (not (running comp0))
  )
  :effect (and
    (not (evaluate comp32))
    (evaluate comp33)
    (one-off comp32)
  )
)
(:action evaluate-comp32-off-comp28
  :parameters ()
  :precondition (and
    (evaluate comp32)
    (not (rebooted comp32))
    (not (running comp28))
  )
  :effect (and
    (not (evaluate comp32))
    (evaluate comp33)
    (one-off comp32)
  )
)
(:action evaluate-comp32-off-comp31
  :parameters ()
  :precondition (and
    (evaluate comp32)
    (not (rebooted comp32))
    (not (running comp31))
  )
  :effect (and
    (not (evaluate comp32))
    (evaluate comp33)
    (one-off comp32)
  )
)
(:action evaluate-comp32-off-comp48
  :parameters ()
  :precondition (and
    (evaluate comp32)
    (not (rebooted comp32))
    (not (running comp48))
  )
  :effect (and
    (not (evaluate comp32))
    (evaluate comp33)
    (one-off comp32)
  )
)
(:action evaluate-comp33-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp33)
    (rebooted comp33)
  )
  :effect (and
    (not (evaluate comp33))
    (evaluate comp34)
  )
)
(:action evaluate-comp33-all-on
  :parameters ()
  :precondition (and
    (evaluate comp33)
    (not (rebooted comp33))
    (running comp32)
    (running comp55)
  )
  :effect (and
    (not (evaluate comp33))
    (evaluate comp34)
    (all-on comp33)
  )
)
(:action evaluate-comp33-off-comp32
  :parameters ()
  :precondition (and
    (evaluate comp33)
    (not (rebooted comp33))
    (not (running comp32))
  )
  :effect (and
    (not (evaluate comp33))
    (evaluate comp34)
    (one-off comp33)
  )
)
(:action evaluate-comp33-off-comp55
  :parameters ()
  :precondition (and
    (evaluate comp33)
    (not (rebooted comp33))
    (not (running comp55))
  )
  :effect (and
    (not (evaluate comp33))
    (evaluate comp34)
    (one-off comp33)
  )
)
(:action evaluate-comp34-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp34)
    (rebooted comp34)
  )
  :effect (and
    (not (evaluate comp34))
    (evaluate comp35)
  )
)
(:action evaluate-comp34-all-on
  :parameters ()
  :precondition (and
    (evaluate comp34)
    (not (rebooted comp34))
    (running comp33)
  )
  :effect (and
    (not (evaluate comp34))
    (evaluate comp35)
    (all-on comp34)
  )
)
(:action evaluate-comp34-off-comp33
  :parameters ()
  :precondition (and
    (evaluate comp34)
    (not (rebooted comp34))
    (not (running comp33))
  )
  :effect (and
    (not (evaluate comp34))
    (evaluate comp35)
    (one-off comp34)
  )
)
(:action evaluate-comp35-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp35)
    (rebooted comp35)
  )
  :effect (and
    (not (evaluate comp35))
    (evaluate comp36)
  )
)
(:action evaluate-comp35-all-on
  :parameters ()
  :precondition (and
    (evaluate comp35)
    (not (rebooted comp35))
    (running comp34)
  )
  :effect (and
    (not (evaluate comp35))
    (evaluate comp36)
    (all-on comp35)
  )
)
(:action evaluate-comp35-off-comp34
  :parameters ()
  :precondition (and
    (evaluate comp35)
    (not (rebooted comp35))
    (not (running comp34))
  )
  :effect (and
    (not (evaluate comp35))
    (evaluate comp36)
    (one-off comp35)
  )
)
(:action evaluate-comp36-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp36)
    (rebooted comp36)
  )
  :effect (and
    (not (evaluate comp36))
    (evaluate comp37)
  )
)
(:action evaluate-comp36-all-on
  :parameters ()
  :precondition (and
    (evaluate comp36)
    (not (rebooted comp36))
    (running comp35)
  )
  :effect (and
    (not (evaluate comp36))
    (evaluate comp37)
    (all-on comp36)
  )
)
(:action evaluate-comp36-off-comp35
  :parameters ()
  :precondition (and
    (evaluate comp36)
    (not (rebooted comp36))
    (not (running comp35))
  )
  :effect (and
    (not (evaluate comp36))
    (evaluate comp37)
    (one-off comp36)
  )
)
(:action evaluate-comp37-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp37)
    (rebooted comp37)
  )
  :effect (and
    (not (evaluate comp37))
    (evaluate comp38)
  )
)
(:action evaluate-comp37-all-on
  :parameters ()
  :precondition (and
    (evaluate comp37)
    (not (rebooted comp37))
    (running comp36)
  )
  :effect (and
    (not (evaluate comp37))
    (evaluate comp38)
    (all-on comp37)
  )
)
(:action evaluate-comp37-off-comp36
  :parameters ()
  :precondition (and
    (evaluate comp37)
    (not (rebooted comp37))
    (not (running comp36))
  )
  :effect (and
    (not (evaluate comp37))
    (evaluate comp38)
    (one-off comp37)
  )
)
(:action evaluate-comp38-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp38)
    (rebooted comp38)
  )
  :effect (and
    (not (evaluate comp38))
    (evaluate comp39)
  )
)
(:action evaluate-comp38-all-on
  :parameters ()
  :precondition (and
    (evaluate comp38)
    (not (rebooted comp38))
    (running comp37)
  )
  :effect (and
    (not (evaluate comp38))
    (evaluate comp39)
    (all-on comp38)
  )
)
(:action evaluate-comp38-off-comp37
  :parameters ()
  :precondition (and
    (evaluate comp38)
    (not (rebooted comp38))
    (not (running comp37))
  )
  :effect (and
    (not (evaluate comp38))
    (evaluate comp39)
    (one-off comp38)
  )
)
(:action evaluate-comp39-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp39)
    (rebooted comp39)
  )
  :effect (and
    (not (evaluate comp39))
    (evaluate comp40)
  )
)
(:action evaluate-comp39-all-on
  :parameters ()
  :precondition (and
    (evaluate comp39)
    (not (rebooted comp39))
    (running comp38)
  )
  :effect (and
    (not (evaluate comp39))
    (evaluate comp40)
    (all-on comp39)
  )
)
(:action evaluate-comp39-off-comp38
  :parameters ()
  :precondition (and
    (evaluate comp39)
    (not (rebooted comp39))
    (not (running comp38))
  )
  :effect (and
    (not (evaluate comp39))
    (evaluate comp40)
    (one-off comp39)
  )
)
(:action evaluate-comp40-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp40)
    (rebooted comp40)
  )
  :effect (and
    (not (evaluate comp40))
    (evaluate comp41)
  )
)
(:action evaluate-comp40-all-on
  :parameters ()
  :precondition (and
    (evaluate comp40)
    (not (rebooted comp40))
    (running comp39)
  )
  :effect (and
    (not (evaluate comp40))
    (evaluate comp41)
    (all-on comp40)
  )
)
(:action evaluate-comp40-off-comp39
  :parameters ()
  :precondition (and
    (evaluate comp40)
    (not (rebooted comp40))
    (not (running comp39))
  )
  :effect (and
    (not (evaluate comp40))
    (evaluate comp41)
    (one-off comp40)
  )
)
(:action evaluate-comp41-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp41)
    (rebooted comp41)
  )
  :effect (and
    (not (evaluate comp41))
    (evaluate comp42)
  )
)
(:action evaluate-comp41-all-on
  :parameters ()
  :precondition (and
    (evaluate comp41)
    (not (rebooted comp41))
    (running comp40)
  )
  :effect (and
    (not (evaluate comp41))
    (evaluate comp42)
    (all-on comp41)
  )
)
(:action evaluate-comp41-off-comp40
  :parameters ()
  :precondition (and
    (evaluate comp41)
    (not (rebooted comp41))
    (not (running comp40))
  )
  :effect (and
    (not (evaluate comp41))
    (evaluate comp42)
    (one-off comp41)
  )
)
(:action evaluate-comp42-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp42)
    (rebooted comp42)
  )
  :effect (and
    (not (evaluate comp42))
    (evaluate comp43)
  )
)
(:action evaluate-comp42-all-on
  :parameters ()
  :precondition (and
    (evaluate comp42)
    (not (rebooted comp42))
    (running comp41)
  )
  :effect (and
    (not (evaluate comp42))
    (evaluate comp43)
    (all-on comp42)
  )
)
(:action evaluate-comp42-off-comp41
  :parameters ()
  :precondition (and
    (evaluate comp42)
    (not (rebooted comp42))
    (not (running comp41))
  )
  :effect (and
    (not (evaluate comp42))
    (evaluate comp43)
    (one-off comp42)
  )
)
(:action evaluate-comp43-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp43)
    (rebooted comp43)
  )
  :effect (and
    (not (evaluate comp43))
    (evaluate comp44)
  )
)
(:action evaluate-comp43-all-on
  :parameters ()
  :precondition (and
    (evaluate comp43)
    (not (rebooted comp43))
    (running comp42)
  )
  :effect (and
    (not (evaluate comp43))
    (evaluate comp44)
    (all-on comp43)
  )
)
(:action evaluate-comp43-off-comp42
  :parameters ()
  :precondition (and
    (evaluate comp43)
    (not (rebooted comp43))
    (not (running comp42))
  )
  :effect (and
    (not (evaluate comp43))
    (evaluate comp44)
    (one-off comp43)
  )
)
(:action evaluate-comp44-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp44)
    (rebooted comp44)
  )
  :effect (and
    (not (evaluate comp44))
    (evaluate comp45)
  )
)
(:action evaluate-comp44-all-on
  :parameters ()
  :precondition (and
    (evaluate comp44)
    (not (rebooted comp44))
    (running comp43)
    (running comp47)
  )
  :effect (and
    (not (evaluate comp44))
    (evaluate comp45)
    (all-on comp44)
  )
)
(:action evaluate-comp44-off-comp43
  :parameters ()
  :precondition (and
    (evaluate comp44)
    (not (rebooted comp44))
    (not (running comp43))
  )
  :effect (and
    (not (evaluate comp44))
    (evaluate comp45)
    (one-off comp44)
  )
)
(:action evaluate-comp44-off-comp47
  :parameters ()
  :precondition (and
    (evaluate comp44)
    (not (rebooted comp44))
    (not (running comp47))
  )
  :effect (and
    (not (evaluate comp44))
    (evaluate comp45)
    (one-off comp44)
  )
)
(:action evaluate-comp45-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp45)
    (rebooted comp45)
  )
  :effect (and
    (not (evaluate comp45))
    (evaluate comp46)
  )
)
(:action evaluate-comp45-all-on
  :parameters ()
  :precondition (and
    (evaluate comp45)
    (not (rebooted comp45))
    (running comp18)
    (running comp44)
  )
  :effect (and
    (not (evaluate comp45))
    (evaluate comp46)
    (all-on comp45)
  )
)
(:action evaluate-comp45-off-comp18
  :parameters ()
  :precondition (and
    (evaluate comp45)
    (not (rebooted comp45))
    (not (running comp18))
  )
  :effect (and
    (not (evaluate comp45))
    (evaluate comp46)
    (one-off comp45)
  )
)
(:action evaluate-comp45-off-comp44
  :parameters ()
  :precondition (and
    (evaluate comp45)
    (not (rebooted comp45))
    (not (running comp44))
  )
  :effect (and
    (not (evaluate comp45))
    (evaluate comp46)
    (one-off comp45)
  )
)
(:action evaluate-comp46-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp46)
    (rebooted comp46)
  )
  :effect (and
    (not (evaluate comp46))
    (evaluate comp47)
  )
)
(:action evaluate-comp46-all-on
  :parameters ()
  :precondition (and
    (evaluate comp46)
    (not (rebooted comp46))
    (running comp45)
  )
  :effect (and
    (not (evaluate comp46))
    (evaluate comp47)
    (all-on comp46)
  )
)
(:action evaluate-comp46-off-comp45
  :parameters ()
  :precondition (and
    (evaluate comp46)
    (not (rebooted comp46))
    (not (running comp45))
  )
  :effect (and
    (not (evaluate comp46))
    (evaluate comp47)
    (one-off comp46)
  )
)
(:action evaluate-comp47-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp47)
    (rebooted comp47)
  )
  :effect (and
    (not (evaluate comp47))
    (evaluate comp48)
  )
)
(:action evaluate-comp47-all-on
  :parameters ()
  :precondition (and
    (evaluate comp47)
    (not (rebooted comp47))
    (running comp46)
  )
  :effect (and
    (not (evaluate comp47))
    (evaluate comp48)
    (all-on comp47)
  )
)
(:action evaluate-comp47-off-comp46
  :parameters ()
  :precondition (and
    (evaluate comp47)
    (not (rebooted comp47))
    (not (running comp46))
  )
  :effect (and
    (not (evaluate comp47))
    (evaluate comp48)
    (one-off comp47)
  )
)
(:action evaluate-comp48-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp48)
    (rebooted comp48)
  )
  :effect (and
    (not (evaluate comp48))
    (evaluate comp49)
  )
)
(:action evaluate-comp48-all-on
  :parameters ()
  :precondition (and
    (evaluate comp48)
    (not (rebooted comp48))
    (running comp47)
  )
  :effect (and
    (not (evaluate comp48))
    (evaluate comp49)
    (all-on comp48)
  )
)
(:action evaluate-comp48-off-comp47
  :parameters ()
  :precondition (and
    (evaluate comp48)
    (not (rebooted comp48))
    (not (running comp47))
  )
  :effect (and
    (not (evaluate comp48))
    (evaluate comp49)
    (one-off comp48)
  )
)
(:action evaluate-comp49-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp49)
    (rebooted comp49)
  )
  :effect (and
    (not (evaluate comp49))
    (evaluate comp50)
  )
)
(:action evaluate-comp49-all-on
  :parameters ()
  :precondition (and
    (evaluate comp49)
    (not (rebooted comp49))
    (running comp9)
    (running comp27)
    (running comp48)
  )
  :effect (and
    (not (evaluate comp49))
    (evaluate comp50)
    (all-on comp49)
  )
)
(:action evaluate-comp49-off-comp9
  :parameters ()
  :precondition (and
    (evaluate comp49)
    (not (rebooted comp49))
    (not (running comp9))
  )
  :effect (and
    (not (evaluate comp49))
    (evaluate comp50)
    (one-off comp49)
  )
)
(:action evaluate-comp49-off-comp27
  :parameters ()
  :precondition (and
    (evaluate comp49)
    (not (rebooted comp49))
    (not (running comp27))
  )
  :effect (and
    (not (evaluate comp49))
    (evaluate comp50)
    (one-off comp49)
  )
)
(:action evaluate-comp49-off-comp48
  :parameters ()
  :precondition (and
    (evaluate comp49)
    (not (rebooted comp49))
    (not (running comp48))
  )
  :effect (and
    (not (evaluate comp49))
    (evaluate comp50)
    (one-off comp49)
  )
)
(:action evaluate-comp50-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp50)
    (rebooted comp50)
  )
  :effect (and
    (not (evaluate comp50))
    (evaluate comp51)
  )
)
(:action evaluate-comp50-all-on
  :parameters ()
  :precondition (and
    (evaluate comp50)
    (not (rebooted comp50))
    (running comp49)
  )
  :effect (and
    (not (evaluate comp50))
    (evaluate comp51)
    (all-on comp50)
  )
)
(:action evaluate-comp50-off-comp49
  :parameters ()
  :precondition (and
    (evaluate comp50)
    (not (rebooted comp50))
    (not (running comp49))
  )
  :effect (and
    (not (evaluate comp50))
    (evaluate comp51)
    (one-off comp50)
  )
)
(:action evaluate-comp51-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp51)
    (rebooted comp51)
  )
  :effect (and
    (not (evaluate comp51))
    (evaluate comp52)
  )
)
(:action evaluate-comp51-all-on
  :parameters ()
  :precondition (and
    (evaluate comp51)
    (not (rebooted comp51))
    (running comp50)
  )
  :effect (and
    (not (evaluate comp51))
    (evaluate comp52)
    (all-on comp51)
  )
)
(:action evaluate-comp51-off-comp50
  :parameters ()
  :precondition (and
    (evaluate comp51)
    (not (rebooted comp51))
    (not (running comp50))
  )
  :effect (and
    (not (evaluate comp51))
    (evaluate comp52)
    (one-off comp51)
  )
)
(:action evaluate-comp52-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp52)
    (rebooted comp52)
  )
  :effect (and
    (not (evaluate comp52))
    (evaluate comp53)
  )
)
(:action evaluate-comp52-all-on
  :parameters ()
  :precondition (and
    (evaluate comp52)
    (not (rebooted comp52))
    (running comp51)
  )
  :effect (and
    (not (evaluate comp52))
    (evaluate comp53)
    (all-on comp52)
  )
)
(:action evaluate-comp52-off-comp51
  :parameters ()
  :precondition (and
    (evaluate comp52)
    (not (rebooted comp52))
    (not (running comp51))
  )
  :effect (and
    (not (evaluate comp52))
    (evaluate comp53)
    (one-off comp52)
  )
)
(:action evaluate-comp53-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp53)
    (rebooted comp53)
  )
  :effect (and
    (not (evaluate comp53))
    (evaluate comp54)
  )
)
(:action evaluate-comp53-all-on
  :parameters ()
  :precondition (and
    (evaluate comp53)
    (not (rebooted comp53))
    (running comp52)
  )
  :effect (and
    (not (evaluate comp53))
    (evaluate comp54)
    (all-on comp53)
  )
)
(:action evaluate-comp53-off-comp52
  :parameters ()
  :precondition (and
    (evaluate comp53)
    (not (rebooted comp53))
    (not (running comp52))
  )
  :effect (and
    (not (evaluate comp53))
    (evaluate comp54)
    (one-off comp53)
  )
)
(:action evaluate-comp54-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp54)
    (rebooted comp54)
  )
  :effect (and
    (not (evaluate comp54))
    (evaluate comp55)
  )
)
(:action evaluate-comp54-all-on
  :parameters ()
  :precondition (and
    (evaluate comp54)
    (not (rebooted comp54))
    (running comp53)
  )
  :effect (and
    (not (evaluate comp54))
    (evaluate comp55)
    (all-on comp54)
  )
)
(:action evaluate-comp54-off-comp53
  :parameters ()
  :precondition (and
    (evaluate comp54)
    (not (rebooted comp54))
    (not (running comp53))
  )
  :effect (and
    (not (evaluate comp54))
    (evaluate comp55)
    (one-off comp54)
  )
)
(:action evaluate-comp55-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp55)
    (rebooted comp55)
  )
  :effect (and
    (not (evaluate comp55))
    (evaluate comp56)
  )
)
(:action evaluate-comp55-all-on
  :parameters ()
  :precondition (and
    (evaluate comp55)
    (not (rebooted comp55))
    (running comp11)
    (running comp54)
  )
  :effect (and
    (not (evaluate comp55))
    (evaluate comp56)
    (all-on comp55)
  )
)
(:action evaluate-comp55-off-comp11
  :parameters ()
  :precondition (and
    (evaluate comp55)
    (not (rebooted comp55))
    (not (running comp11))
  )
  :effect (and
    (not (evaluate comp55))
    (evaluate comp56)
    (one-off comp55)
  )
)
(:action evaluate-comp55-off-comp54
  :parameters ()
  :precondition (and
    (evaluate comp55)
    (not (rebooted comp55))
    (not (running comp54))
  )
  :effect (and
    (not (evaluate comp55))
    (evaluate comp56)
    (one-off comp55)
  )
)
(:action evaluate-comp56-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp56)
    (rebooted comp56)
  )
  :effect (and
    (not (evaluate comp56))
    (evaluate comp57)
  )
)
(:action evaluate-comp56-all-on
  :parameters ()
  :precondition (and
    (evaluate comp56)
    (not (rebooted comp56))
    (running comp55)
  )
  :effect (and
    (not (evaluate comp56))
    (evaluate comp57)
    (all-on comp56)
  )
)
(:action evaluate-comp56-off-comp55
  :parameters ()
  :precondition (and
    (evaluate comp56)
    (not (rebooted comp56))
    (not (running comp55))
  )
  :effect (and
    (not (evaluate comp56))
    (evaluate comp57)
    (one-off comp56)
  )
)
(:action evaluate-comp57-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp57)
    (rebooted comp57)
  )
  :effect (and
    (not (evaluate comp57))
    (evaluate comp58)
  )
)
(:action evaluate-comp57-all-on
  :parameters ()
  :precondition (and
    (evaluate comp57)
    (not (rebooted comp57))
    (running comp20)
    (running comp56)
  )
  :effect (and
    (not (evaluate comp57))
    (evaluate comp58)
    (all-on comp57)
  )
)
(:action evaluate-comp57-off-comp20
  :parameters ()
  :precondition (and
    (evaluate comp57)
    (not (rebooted comp57))
    (not (running comp20))
  )
  :effect (and
    (not (evaluate comp57))
    (evaluate comp58)
    (one-off comp57)
  )
)
(:action evaluate-comp57-off-comp56
  :parameters ()
  :precondition (and
    (evaluate comp57)
    (not (rebooted comp57))
    (not (running comp56))
  )
  :effect (and
    (not (evaluate comp57))
    (evaluate comp58)
    (one-off comp57)
  )
)
(:action evaluate-comp58-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp58)
    (rebooted comp58)
  )
  :effect (and
    (not (evaluate comp58))
    (evaluate comp59)
  )
)
(:action evaluate-comp58-all-on
  :parameters ()
  :precondition (and
    (evaluate comp58)
    (not (rebooted comp58))
    (running comp23)
    (running comp57)
  )
  :effect (and
    (not (evaluate comp58))
    (evaluate comp59)
    (all-on comp58)
  )
)
(:action evaluate-comp58-off-comp23
  :parameters ()
  :precondition (and
    (evaluate comp58)
    (not (rebooted comp58))
    (not (running comp23))
  )
  :effect (and
    (not (evaluate comp58))
    (evaluate comp59)
    (one-off comp58)
  )
)
(:action evaluate-comp58-off-comp57
  :parameters ()
  :precondition (and
    (evaluate comp58)
    (not (rebooted comp58))
    (not (running comp57))
  )
  :effect (and
    (not (evaluate comp58))
    (evaluate comp59)
    (one-off comp58)
  )
)
(:action evaluate-comp59-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp59)
    (rebooted comp59)
  )
  :effect (and
    (not (evaluate comp59))
    (update-status comp0)
  )
)
(:action evaluate-comp59-all-on
  :parameters ()
  :precondition (and
    (evaluate comp59)
    (not (rebooted comp59))
    (running comp58)
  )
  :effect (and
    (not (evaluate comp59))
    (update-status comp0)
    (all-on comp59)
  )
)
(:action evaluate-comp59-off-comp58
  :parameters ()
  :precondition (and
    (evaluate comp59)
    (not (rebooted comp59))
    (not (running comp58))
  )
  :effect (and
    (not (evaluate comp59))
    (update-status comp0)
    (one-off comp59)
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
    (probabilistic 9/10 (and (running comp0)) 1/10 (and (not (running comp0))))
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
    (probabilistic 9/10 (and (running comp1)) 1/10 (and (not (running comp1))))
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
    (probabilistic 9/10 (and (running comp2)) 1/10 (and (not (running comp2))))
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
    (probabilistic 9/10 (and (running comp3)) 1/10 (and (not (running comp3))))
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
    (probabilistic 9/10 (and (running comp4)) 1/10 (and (not (running comp4))))
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
    (probabilistic 9/10 (and (running comp5)) 1/10 (and (not (running comp5))))
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
    (probabilistic 9/10 (and (running comp6)) 1/10 (and (not (running comp6))))
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
    (probabilistic 9/10 (and (running comp7)) 1/10 (and (not (running comp7))))
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
    (probabilistic 9/10 (and (running comp8)) 1/10 (and (not (running comp8))))
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
    (probabilistic 9/10 (and (running comp9)) 1/10 (and (not (running comp9))))
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
    (probabilistic 9/10 (and (running comp10)) 1/10 (and (not (running comp10))))
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
    (probabilistic 9/10 (and (running comp11)) 1/10 (and (not (running comp11))))
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
    (probabilistic 9/10 (and (running comp12)) 1/10 (and (not (running comp12))))
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
    (probabilistic 9/10 (and (running comp13)) 1/10 (and (not (running comp13))))
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
    (probabilistic 9/10 (and (running comp14)) 1/10 (and (not (running comp14))))
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
    (probabilistic 9/10 (and (running comp15)) 1/10 (and (not (running comp15))))
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
    (probabilistic 9/10 (and (running comp16)) 1/10 (and (not (running comp16))))
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
    (probabilistic 9/10 (and (running comp17)) 1/10 (and (not (running comp17))))
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
    (probabilistic 9/10 (and (running comp18)) 1/10 (and (not (running comp18))))
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
    (probabilistic 9/10 (and (running comp19)) 1/10 (and (not (running comp19))))
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
    (probabilistic 9/10 (and (running comp20)) 1/10 (and (not (running comp20))))
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
    (probabilistic 9/10 (and (running comp21)) 1/10 (and (not (running comp21))))
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
    (probabilistic 9/10 (and (running comp22)) 1/10 (and (not (running comp22))))
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
    (probabilistic 9/10 (and (running comp23)) 1/10 (and (not (running comp23))))
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
    (update-status comp25)
    (not (rebooted comp24))
    (probabilistic 9/10 (and (running comp24)) 1/10 (and (not (running comp24))))
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
    (update-status comp25)
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
    (update-status comp25)
    (not (one-off comp24))
    (probabilistic 1/4 (and (not (running comp24))))
  )
)
(:action update-status-comp25-rebooted
  :parameters ()
  :precondition (and
    (update-status comp25)
    (rebooted comp25)
  )
  :effect (and
    (not (update-status comp25))
    (update-status comp26)
    (not (rebooted comp25))
    (probabilistic 9/10 (and (running comp25)) 1/10 (and (not (running comp25))))
  )
)
(:action update-status-comp25-all-on
  :parameters ()
  :precondition (and
    (update-status comp25)
    (not (rebooted comp25))
    (all-on comp25)
  )
  :effect (and
    (not (update-status comp25))
    (update-status comp26)
    (not (all-on comp25))
    (probabilistic 1/20 (and (not (running comp25))))
  )
)
(:action update-status-comp25-one-off
  :parameters ()
  :precondition (and
    (update-status comp25)
    (not (rebooted comp25))
    (one-off comp25)
  )
  :effect (and
    (not (update-status comp25))
    (update-status comp26)
    (not (one-off comp25))
    (probabilistic 1/4 (and (not (running comp25))))
  )
)
(:action update-status-comp26-rebooted
  :parameters ()
  :precondition (and
    (update-status comp26)
    (rebooted comp26)
  )
  :effect (and
    (not (update-status comp26))
    (update-status comp27)
    (not (rebooted comp26))
    (probabilistic 9/10 (and (running comp26)) 1/10 (and (not (running comp26))))
  )
)
(:action update-status-comp26-all-on
  :parameters ()
  :precondition (and
    (update-status comp26)
    (not (rebooted comp26))
    (all-on comp26)
  )
  :effect (and
    (not (update-status comp26))
    (update-status comp27)
    (not (all-on comp26))
    (probabilistic 1/20 (and (not (running comp26))))
  )
)
(:action update-status-comp26-one-off
  :parameters ()
  :precondition (and
    (update-status comp26)
    (not (rebooted comp26))
    (one-off comp26)
  )
  :effect (and
    (not (update-status comp26))
    (update-status comp27)
    (not (one-off comp26))
    (probabilistic 1/4 (and (not (running comp26))))
  )
)
(:action update-status-comp27-rebooted
  :parameters ()
  :precondition (and
    (update-status comp27)
    (rebooted comp27)
  )
  :effect (and
    (not (update-status comp27))
    (update-status comp28)
    (not (rebooted comp27))
    (probabilistic 9/10 (and (running comp27)) 1/10 (and (not (running comp27))))
  )
)
(:action update-status-comp27-all-on
  :parameters ()
  :precondition (and
    (update-status comp27)
    (not (rebooted comp27))
    (all-on comp27)
  )
  :effect (and
    (not (update-status comp27))
    (update-status comp28)
    (not (all-on comp27))
    (probabilistic 1/20 (and (not (running comp27))))
  )
)
(:action update-status-comp27-one-off
  :parameters ()
  :precondition (and
    (update-status comp27)
    (not (rebooted comp27))
    (one-off comp27)
  )
  :effect (and
    (not (update-status comp27))
    (update-status comp28)
    (not (one-off comp27))
    (probabilistic 1/4 (and (not (running comp27))))
  )
)
(:action update-status-comp28-rebooted
  :parameters ()
  :precondition (and
    (update-status comp28)
    (rebooted comp28)
  )
  :effect (and
    (not (update-status comp28))
    (update-status comp29)
    (not (rebooted comp28))
    (probabilistic 9/10 (and (running comp28)) 1/10 (and (not (running comp28))))
  )
)
(:action update-status-comp28-all-on
  :parameters ()
  :precondition (and
    (update-status comp28)
    (not (rebooted comp28))
    (all-on comp28)
  )
  :effect (and
    (not (update-status comp28))
    (update-status comp29)
    (not (all-on comp28))
    (probabilistic 1/20 (and (not (running comp28))))
  )
)
(:action update-status-comp28-one-off
  :parameters ()
  :precondition (and
    (update-status comp28)
    (not (rebooted comp28))
    (one-off comp28)
  )
  :effect (and
    (not (update-status comp28))
    (update-status comp29)
    (not (one-off comp28))
    (probabilistic 1/4 (and (not (running comp28))))
  )
)
(:action update-status-comp29-rebooted
  :parameters ()
  :precondition (and
    (update-status comp29)
    (rebooted comp29)
  )
  :effect (and
    (not (update-status comp29))
    (update-status comp30)
    (not (rebooted comp29))
    (probabilistic 9/10 (and (running comp29)) 1/10 (and (not (running comp29))))
  )
)
(:action update-status-comp29-all-on
  :parameters ()
  :precondition (and
    (update-status comp29)
    (not (rebooted comp29))
    (all-on comp29)
  )
  :effect (and
    (not (update-status comp29))
    (update-status comp30)
    (not (all-on comp29))
    (probabilistic 1/20 (and (not (running comp29))))
  )
)
(:action update-status-comp29-one-off
  :parameters ()
  :precondition (and
    (update-status comp29)
    (not (rebooted comp29))
    (one-off comp29)
  )
  :effect (and
    (not (update-status comp29))
    (update-status comp30)
    (not (one-off comp29))
    (probabilistic 1/4 (and (not (running comp29))))
  )
)
(:action update-status-comp30-rebooted
  :parameters ()
  :precondition (and
    (update-status comp30)
    (rebooted comp30)
  )
  :effect (and
    (not (update-status comp30))
    (update-status comp31)
    (not (rebooted comp30))
    (probabilistic 9/10 (and (running comp30)) 1/10 (and (not (running comp30))))
  )
)
(:action update-status-comp30-all-on
  :parameters ()
  :precondition (and
    (update-status comp30)
    (not (rebooted comp30))
    (all-on comp30)
  )
  :effect (and
    (not (update-status comp30))
    (update-status comp31)
    (not (all-on comp30))
    (probabilistic 1/20 (and (not (running comp30))))
  )
)
(:action update-status-comp30-one-off
  :parameters ()
  :precondition (and
    (update-status comp30)
    (not (rebooted comp30))
    (one-off comp30)
  )
  :effect (and
    (not (update-status comp30))
    (update-status comp31)
    (not (one-off comp30))
    (probabilistic 1/4 (and (not (running comp30))))
  )
)
(:action update-status-comp31-rebooted
  :parameters ()
  :precondition (and
    (update-status comp31)
    (rebooted comp31)
  )
  :effect (and
    (not (update-status comp31))
    (update-status comp32)
    (not (rebooted comp31))
    (probabilistic 9/10 (and (running comp31)) 1/10 (and (not (running comp31))))
  )
)
(:action update-status-comp31-all-on
  :parameters ()
  :precondition (and
    (update-status comp31)
    (not (rebooted comp31))
    (all-on comp31)
  )
  :effect (and
    (not (update-status comp31))
    (update-status comp32)
    (not (all-on comp31))
    (probabilistic 1/20 (and (not (running comp31))))
  )
)
(:action update-status-comp31-one-off
  :parameters ()
  :precondition (and
    (update-status comp31)
    (not (rebooted comp31))
    (one-off comp31)
  )
  :effect (and
    (not (update-status comp31))
    (update-status comp32)
    (not (one-off comp31))
    (probabilistic 1/4 (and (not (running comp31))))
  )
)
(:action update-status-comp32-rebooted
  :parameters ()
  :precondition (and
    (update-status comp32)
    (rebooted comp32)
  )
  :effect (and
    (not (update-status comp32))
    (update-status comp33)
    (not (rebooted comp32))
    (probabilistic 9/10 (and (running comp32)) 1/10 (and (not (running comp32))))
  )
)
(:action update-status-comp32-all-on
  :parameters ()
  :precondition (and
    (update-status comp32)
    (not (rebooted comp32))
    (all-on comp32)
  )
  :effect (and
    (not (update-status comp32))
    (update-status comp33)
    (not (all-on comp32))
    (probabilistic 1/20 (and (not (running comp32))))
  )
)
(:action update-status-comp32-one-off
  :parameters ()
  :precondition (and
    (update-status comp32)
    (not (rebooted comp32))
    (one-off comp32)
  )
  :effect (and
    (not (update-status comp32))
    (update-status comp33)
    (not (one-off comp32))
    (probabilistic 1/4 (and (not (running comp32))))
  )
)
(:action update-status-comp33-rebooted
  :parameters ()
  :precondition (and
    (update-status comp33)
    (rebooted comp33)
  )
  :effect (and
    (not (update-status comp33))
    (update-status comp34)
    (not (rebooted comp33))
    (probabilistic 9/10 (and (running comp33)) 1/10 (and (not (running comp33))))
  )
)
(:action update-status-comp33-all-on
  :parameters ()
  :precondition (and
    (update-status comp33)
    (not (rebooted comp33))
    (all-on comp33)
  )
  :effect (and
    (not (update-status comp33))
    (update-status comp34)
    (not (all-on comp33))
    (probabilistic 1/20 (and (not (running comp33))))
  )
)
(:action update-status-comp33-one-off
  :parameters ()
  :precondition (and
    (update-status comp33)
    (not (rebooted comp33))
    (one-off comp33)
  )
  :effect (and
    (not (update-status comp33))
    (update-status comp34)
    (not (one-off comp33))
    (probabilistic 1/4 (and (not (running comp33))))
  )
)
(:action update-status-comp34-rebooted
  :parameters ()
  :precondition (and
    (update-status comp34)
    (rebooted comp34)
  )
  :effect (and
    (not (update-status comp34))
    (update-status comp35)
    (not (rebooted comp34))
    (probabilistic 9/10 (and (running comp34)) 1/10 (and (not (running comp34))))
  )
)
(:action update-status-comp34-all-on
  :parameters ()
  :precondition (and
    (update-status comp34)
    (not (rebooted comp34))
    (all-on comp34)
  )
  :effect (and
    (not (update-status comp34))
    (update-status comp35)
    (not (all-on comp34))
    (probabilistic 1/20 (and (not (running comp34))))
  )
)
(:action update-status-comp34-one-off
  :parameters ()
  :precondition (and
    (update-status comp34)
    (not (rebooted comp34))
    (one-off comp34)
  )
  :effect (and
    (not (update-status comp34))
    (update-status comp35)
    (not (one-off comp34))
    (probabilistic 1/4 (and (not (running comp34))))
  )
)
(:action update-status-comp35-rebooted
  :parameters ()
  :precondition (and
    (update-status comp35)
    (rebooted comp35)
  )
  :effect (and
    (not (update-status comp35))
    (update-status comp36)
    (not (rebooted comp35))
    (probabilistic 9/10 (and (running comp35)) 1/10 (and (not (running comp35))))
  )
)
(:action update-status-comp35-all-on
  :parameters ()
  :precondition (and
    (update-status comp35)
    (not (rebooted comp35))
    (all-on comp35)
  )
  :effect (and
    (not (update-status comp35))
    (update-status comp36)
    (not (all-on comp35))
    (probabilistic 1/20 (and (not (running comp35))))
  )
)
(:action update-status-comp35-one-off
  :parameters ()
  :precondition (and
    (update-status comp35)
    (not (rebooted comp35))
    (one-off comp35)
  )
  :effect (and
    (not (update-status comp35))
    (update-status comp36)
    (not (one-off comp35))
    (probabilistic 1/4 (and (not (running comp35))))
  )
)
(:action update-status-comp36-rebooted
  :parameters ()
  :precondition (and
    (update-status comp36)
    (rebooted comp36)
  )
  :effect (and
    (not (update-status comp36))
    (update-status comp37)
    (not (rebooted comp36))
    (probabilistic 9/10 (and (running comp36)) 1/10 (and (not (running comp36))))
  )
)
(:action update-status-comp36-all-on
  :parameters ()
  :precondition (and
    (update-status comp36)
    (not (rebooted comp36))
    (all-on comp36)
  )
  :effect (and
    (not (update-status comp36))
    (update-status comp37)
    (not (all-on comp36))
    (probabilistic 1/20 (and (not (running comp36))))
  )
)
(:action update-status-comp36-one-off
  :parameters ()
  :precondition (and
    (update-status comp36)
    (not (rebooted comp36))
    (one-off comp36)
  )
  :effect (and
    (not (update-status comp36))
    (update-status comp37)
    (not (one-off comp36))
    (probabilistic 1/4 (and (not (running comp36))))
  )
)
(:action update-status-comp37-rebooted
  :parameters ()
  :precondition (and
    (update-status comp37)
    (rebooted comp37)
  )
  :effect (and
    (not (update-status comp37))
    (update-status comp38)
    (not (rebooted comp37))
    (probabilistic 9/10 (and (running comp37)) 1/10 (and (not (running comp37))))
  )
)
(:action update-status-comp37-all-on
  :parameters ()
  :precondition (and
    (update-status comp37)
    (not (rebooted comp37))
    (all-on comp37)
  )
  :effect (and
    (not (update-status comp37))
    (update-status comp38)
    (not (all-on comp37))
    (probabilistic 1/20 (and (not (running comp37))))
  )
)
(:action update-status-comp37-one-off
  :parameters ()
  :precondition (and
    (update-status comp37)
    (not (rebooted comp37))
    (one-off comp37)
  )
  :effect (and
    (not (update-status comp37))
    (update-status comp38)
    (not (one-off comp37))
    (probabilistic 1/4 (and (not (running comp37))))
  )
)
(:action update-status-comp38-rebooted
  :parameters ()
  :precondition (and
    (update-status comp38)
    (rebooted comp38)
  )
  :effect (and
    (not (update-status comp38))
    (update-status comp39)
    (not (rebooted comp38))
    (probabilistic 9/10 (and (running comp38)) 1/10 (and (not (running comp38))))
  )
)
(:action update-status-comp38-all-on
  :parameters ()
  :precondition (and
    (update-status comp38)
    (not (rebooted comp38))
    (all-on comp38)
  )
  :effect (and
    (not (update-status comp38))
    (update-status comp39)
    (not (all-on comp38))
    (probabilistic 1/20 (and (not (running comp38))))
  )
)
(:action update-status-comp38-one-off
  :parameters ()
  :precondition (and
    (update-status comp38)
    (not (rebooted comp38))
    (one-off comp38)
  )
  :effect (and
    (not (update-status comp38))
    (update-status comp39)
    (not (one-off comp38))
    (probabilistic 1/4 (and (not (running comp38))))
  )
)
(:action update-status-comp39-rebooted
  :parameters ()
  :precondition (and
    (update-status comp39)
    (rebooted comp39)
  )
  :effect (and
    (not (update-status comp39))
    (update-status comp40)
    (not (rebooted comp39))
    (probabilistic 9/10 (and (running comp39)) 1/10 (and (not (running comp39))))
  )
)
(:action update-status-comp39-all-on
  :parameters ()
  :precondition (and
    (update-status comp39)
    (not (rebooted comp39))
    (all-on comp39)
  )
  :effect (and
    (not (update-status comp39))
    (update-status comp40)
    (not (all-on comp39))
    (probabilistic 1/20 (and (not (running comp39))))
  )
)
(:action update-status-comp39-one-off
  :parameters ()
  :precondition (and
    (update-status comp39)
    (not (rebooted comp39))
    (one-off comp39)
  )
  :effect (and
    (not (update-status comp39))
    (update-status comp40)
    (not (one-off comp39))
    (probabilistic 1/4 (and (not (running comp39))))
  )
)
(:action update-status-comp40-rebooted
  :parameters ()
  :precondition (and
    (update-status comp40)
    (rebooted comp40)
  )
  :effect (and
    (not (update-status comp40))
    (update-status comp41)
    (not (rebooted comp40))
    (probabilistic 9/10 (and (running comp40)) 1/10 (and (not (running comp40))))
  )
)
(:action update-status-comp40-all-on
  :parameters ()
  :precondition (and
    (update-status comp40)
    (not (rebooted comp40))
    (all-on comp40)
  )
  :effect (and
    (not (update-status comp40))
    (update-status comp41)
    (not (all-on comp40))
    (probabilistic 1/20 (and (not (running comp40))))
  )
)
(:action update-status-comp40-one-off
  :parameters ()
  :precondition (and
    (update-status comp40)
    (not (rebooted comp40))
    (one-off comp40)
  )
  :effect (and
    (not (update-status comp40))
    (update-status comp41)
    (not (one-off comp40))
    (probabilistic 1/4 (and (not (running comp40))))
  )
)
(:action update-status-comp41-rebooted
  :parameters ()
  :precondition (and
    (update-status comp41)
    (rebooted comp41)
  )
  :effect (and
    (not (update-status comp41))
    (update-status comp42)
    (not (rebooted comp41))
    (probabilistic 9/10 (and (running comp41)) 1/10 (and (not (running comp41))))
  )
)
(:action update-status-comp41-all-on
  :parameters ()
  :precondition (and
    (update-status comp41)
    (not (rebooted comp41))
    (all-on comp41)
  )
  :effect (and
    (not (update-status comp41))
    (update-status comp42)
    (not (all-on comp41))
    (probabilistic 1/20 (and (not (running comp41))))
  )
)
(:action update-status-comp41-one-off
  :parameters ()
  :precondition (and
    (update-status comp41)
    (not (rebooted comp41))
    (one-off comp41)
  )
  :effect (and
    (not (update-status comp41))
    (update-status comp42)
    (not (one-off comp41))
    (probabilistic 1/4 (and (not (running comp41))))
  )
)
(:action update-status-comp42-rebooted
  :parameters ()
  :precondition (and
    (update-status comp42)
    (rebooted comp42)
  )
  :effect (and
    (not (update-status comp42))
    (update-status comp43)
    (not (rebooted comp42))
    (probabilistic 9/10 (and (running comp42)) 1/10 (and (not (running comp42))))
  )
)
(:action update-status-comp42-all-on
  :parameters ()
  :precondition (and
    (update-status comp42)
    (not (rebooted comp42))
    (all-on comp42)
  )
  :effect (and
    (not (update-status comp42))
    (update-status comp43)
    (not (all-on comp42))
    (probabilistic 1/20 (and (not (running comp42))))
  )
)
(:action update-status-comp42-one-off
  :parameters ()
  :precondition (and
    (update-status comp42)
    (not (rebooted comp42))
    (one-off comp42)
  )
  :effect (and
    (not (update-status comp42))
    (update-status comp43)
    (not (one-off comp42))
    (probabilistic 1/4 (and (not (running comp42))))
  )
)
(:action update-status-comp43-rebooted
  :parameters ()
  :precondition (and
    (update-status comp43)
    (rebooted comp43)
  )
  :effect (and
    (not (update-status comp43))
    (update-status comp44)
    (not (rebooted comp43))
    (probabilistic 9/10 (and (running comp43)) 1/10 (and (not (running comp43))))
  )
)
(:action update-status-comp43-all-on
  :parameters ()
  :precondition (and
    (update-status comp43)
    (not (rebooted comp43))
    (all-on comp43)
  )
  :effect (and
    (not (update-status comp43))
    (update-status comp44)
    (not (all-on comp43))
    (probabilistic 1/20 (and (not (running comp43))))
  )
)
(:action update-status-comp43-one-off
  :parameters ()
  :precondition (and
    (update-status comp43)
    (not (rebooted comp43))
    (one-off comp43)
  )
  :effect (and
    (not (update-status comp43))
    (update-status comp44)
    (not (one-off comp43))
    (probabilistic 1/4 (and (not (running comp43))))
  )
)
(:action update-status-comp44-rebooted
  :parameters ()
  :precondition (and
    (update-status comp44)
    (rebooted comp44)
  )
  :effect (and
    (not (update-status comp44))
    (update-status comp45)
    (not (rebooted comp44))
    (probabilistic 9/10 (and (running comp44)) 1/10 (and (not (running comp44))))
  )
)
(:action update-status-comp44-all-on
  :parameters ()
  :precondition (and
    (update-status comp44)
    (not (rebooted comp44))
    (all-on comp44)
  )
  :effect (and
    (not (update-status comp44))
    (update-status comp45)
    (not (all-on comp44))
    (probabilistic 1/20 (and (not (running comp44))))
  )
)
(:action update-status-comp44-one-off
  :parameters ()
  :precondition (and
    (update-status comp44)
    (not (rebooted comp44))
    (one-off comp44)
  )
  :effect (and
    (not (update-status comp44))
    (update-status comp45)
    (not (one-off comp44))
    (probabilistic 1/4 (and (not (running comp44))))
  )
)
(:action update-status-comp45-rebooted
  :parameters ()
  :precondition (and
    (update-status comp45)
    (rebooted comp45)
  )
  :effect (and
    (not (update-status comp45))
    (update-status comp46)
    (not (rebooted comp45))
    (probabilistic 9/10 (and (running comp45)) 1/10 (and (not (running comp45))))
  )
)
(:action update-status-comp45-all-on
  :parameters ()
  :precondition (and
    (update-status comp45)
    (not (rebooted comp45))
    (all-on comp45)
  )
  :effect (and
    (not (update-status comp45))
    (update-status comp46)
    (not (all-on comp45))
    (probabilistic 1/20 (and (not (running comp45))))
  )
)
(:action update-status-comp45-one-off
  :parameters ()
  :precondition (and
    (update-status comp45)
    (not (rebooted comp45))
    (one-off comp45)
  )
  :effect (and
    (not (update-status comp45))
    (update-status comp46)
    (not (one-off comp45))
    (probabilistic 1/4 (and (not (running comp45))))
  )
)
(:action update-status-comp46-rebooted
  :parameters ()
  :precondition (and
    (update-status comp46)
    (rebooted comp46)
  )
  :effect (and
    (not (update-status comp46))
    (update-status comp47)
    (not (rebooted comp46))
    (probabilistic 9/10 (and (running comp46)) 1/10 (and (not (running comp46))))
  )
)
(:action update-status-comp46-all-on
  :parameters ()
  :precondition (and
    (update-status comp46)
    (not (rebooted comp46))
    (all-on comp46)
  )
  :effect (and
    (not (update-status comp46))
    (update-status comp47)
    (not (all-on comp46))
    (probabilistic 1/20 (and (not (running comp46))))
  )
)
(:action update-status-comp46-one-off
  :parameters ()
  :precondition (and
    (update-status comp46)
    (not (rebooted comp46))
    (one-off comp46)
  )
  :effect (and
    (not (update-status comp46))
    (update-status comp47)
    (not (one-off comp46))
    (probabilistic 1/4 (and (not (running comp46))))
  )
)
(:action update-status-comp47-rebooted
  :parameters ()
  :precondition (and
    (update-status comp47)
    (rebooted comp47)
  )
  :effect (and
    (not (update-status comp47))
    (update-status comp48)
    (not (rebooted comp47))
    (probabilistic 9/10 (and (running comp47)) 1/10 (and (not (running comp47))))
  )
)
(:action update-status-comp47-all-on
  :parameters ()
  :precondition (and
    (update-status comp47)
    (not (rebooted comp47))
    (all-on comp47)
  )
  :effect (and
    (not (update-status comp47))
    (update-status comp48)
    (not (all-on comp47))
    (probabilistic 1/20 (and (not (running comp47))))
  )
)
(:action update-status-comp47-one-off
  :parameters ()
  :precondition (and
    (update-status comp47)
    (not (rebooted comp47))
    (one-off comp47)
  )
  :effect (and
    (not (update-status comp47))
    (update-status comp48)
    (not (one-off comp47))
    (probabilistic 1/4 (and (not (running comp47))))
  )
)
(:action update-status-comp48-rebooted
  :parameters ()
  :precondition (and
    (update-status comp48)
    (rebooted comp48)
  )
  :effect (and
    (not (update-status comp48))
    (update-status comp49)
    (not (rebooted comp48))
    (probabilistic 9/10 (and (running comp48)) 1/10 (and (not (running comp48))))
  )
)
(:action update-status-comp48-all-on
  :parameters ()
  :precondition (and
    (update-status comp48)
    (not (rebooted comp48))
    (all-on comp48)
  )
  :effect (and
    (not (update-status comp48))
    (update-status comp49)
    (not (all-on comp48))
    (probabilistic 1/20 (and (not (running comp48))))
  )
)
(:action update-status-comp48-one-off
  :parameters ()
  :precondition (and
    (update-status comp48)
    (not (rebooted comp48))
    (one-off comp48)
  )
  :effect (and
    (not (update-status comp48))
    (update-status comp49)
    (not (one-off comp48))
    (probabilistic 1/4 (and (not (running comp48))))
  )
)
(:action update-status-comp49-rebooted
  :parameters ()
  :precondition (and
    (update-status comp49)
    (rebooted comp49)
  )
  :effect (and
    (not (update-status comp49))
    (update-status comp50)
    (not (rebooted comp49))
    (probabilistic 9/10 (and (running comp49)) 1/10 (and (not (running comp49))))
  )
)
(:action update-status-comp49-all-on
  :parameters ()
  :precondition (and
    (update-status comp49)
    (not (rebooted comp49))
    (all-on comp49)
  )
  :effect (and
    (not (update-status comp49))
    (update-status comp50)
    (not (all-on comp49))
    (probabilistic 1/20 (and (not (running comp49))))
  )
)
(:action update-status-comp49-one-off
  :parameters ()
  :precondition (and
    (update-status comp49)
    (not (rebooted comp49))
    (one-off comp49)
  )
  :effect (and
    (not (update-status comp49))
    (update-status comp50)
    (not (one-off comp49))
    (probabilistic 1/4 (and (not (running comp49))))
  )
)
(:action update-status-comp50-rebooted
  :parameters ()
  :precondition (and
    (update-status comp50)
    (rebooted comp50)
  )
  :effect (and
    (not (update-status comp50))
    (update-status comp51)
    (not (rebooted comp50))
    (probabilistic 9/10 (and (running comp50)) 1/10 (and (not (running comp50))))
  )
)
(:action update-status-comp50-all-on
  :parameters ()
  :precondition (and
    (update-status comp50)
    (not (rebooted comp50))
    (all-on comp50)
  )
  :effect (and
    (not (update-status comp50))
    (update-status comp51)
    (not (all-on comp50))
    (probabilistic 1/20 (and (not (running comp50))))
  )
)
(:action update-status-comp50-one-off
  :parameters ()
  :precondition (and
    (update-status comp50)
    (not (rebooted comp50))
    (one-off comp50)
  )
  :effect (and
    (not (update-status comp50))
    (update-status comp51)
    (not (one-off comp50))
    (probabilistic 1/4 (and (not (running comp50))))
  )
)
(:action update-status-comp51-rebooted
  :parameters ()
  :precondition (and
    (update-status comp51)
    (rebooted comp51)
  )
  :effect (and
    (not (update-status comp51))
    (update-status comp52)
    (not (rebooted comp51))
    (probabilistic 9/10 (and (running comp51)) 1/10 (and (not (running comp51))))
  )
)
(:action update-status-comp51-all-on
  :parameters ()
  :precondition (and
    (update-status comp51)
    (not (rebooted comp51))
    (all-on comp51)
  )
  :effect (and
    (not (update-status comp51))
    (update-status comp52)
    (not (all-on comp51))
    (probabilistic 1/20 (and (not (running comp51))))
  )
)
(:action update-status-comp51-one-off
  :parameters ()
  :precondition (and
    (update-status comp51)
    (not (rebooted comp51))
    (one-off comp51)
  )
  :effect (and
    (not (update-status comp51))
    (update-status comp52)
    (not (one-off comp51))
    (probabilistic 1/4 (and (not (running comp51))))
  )
)
(:action update-status-comp52-rebooted
  :parameters ()
  :precondition (and
    (update-status comp52)
    (rebooted comp52)
  )
  :effect (and
    (not (update-status comp52))
    (update-status comp53)
    (not (rebooted comp52))
    (probabilistic 9/10 (and (running comp52)) 1/10 (and (not (running comp52))))
  )
)
(:action update-status-comp52-all-on
  :parameters ()
  :precondition (and
    (update-status comp52)
    (not (rebooted comp52))
    (all-on comp52)
  )
  :effect (and
    (not (update-status comp52))
    (update-status comp53)
    (not (all-on comp52))
    (probabilistic 1/20 (and (not (running comp52))))
  )
)
(:action update-status-comp52-one-off
  :parameters ()
  :precondition (and
    (update-status comp52)
    (not (rebooted comp52))
    (one-off comp52)
  )
  :effect (and
    (not (update-status comp52))
    (update-status comp53)
    (not (one-off comp52))
    (probabilistic 1/4 (and (not (running comp52))))
  )
)
(:action update-status-comp53-rebooted
  :parameters ()
  :precondition (and
    (update-status comp53)
    (rebooted comp53)
  )
  :effect (and
    (not (update-status comp53))
    (update-status comp54)
    (not (rebooted comp53))
    (probabilistic 9/10 (and (running comp53)) 1/10 (and (not (running comp53))))
  )
)
(:action update-status-comp53-all-on
  :parameters ()
  :precondition (and
    (update-status comp53)
    (not (rebooted comp53))
    (all-on comp53)
  )
  :effect (and
    (not (update-status comp53))
    (update-status comp54)
    (not (all-on comp53))
    (probabilistic 1/20 (and (not (running comp53))))
  )
)
(:action update-status-comp53-one-off
  :parameters ()
  :precondition (and
    (update-status comp53)
    (not (rebooted comp53))
    (one-off comp53)
  )
  :effect (and
    (not (update-status comp53))
    (update-status comp54)
    (not (one-off comp53))
    (probabilistic 1/4 (and (not (running comp53))))
  )
)
(:action update-status-comp54-rebooted
  :parameters ()
  :precondition (and
    (update-status comp54)
    (rebooted comp54)
  )
  :effect (and
    (not (update-status comp54))
    (update-status comp55)
    (not (rebooted comp54))
    (probabilistic 9/10 (and (running comp54)) 1/10 (and (not (running comp54))))
  )
)
(:action update-status-comp54-all-on
  :parameters ()
  :precondition (and
    (update-status comp54)
    (not (rebooted comp54))
    (all-on comp54)
  )
  :effect (and
    (not (update-status comp54))
    (update-status comp55)
    (not (all-on comp54))
    (probabilistic 1/20 (and (not (running comp54))))
  )
)
(:action update-status-comp54-one-off
  :parameters ()
  :precondition (and
    (update-status comp54)
    (not (rebooted comp54))
    (one-off comp54)
  )
  :effect (and
    (not (update-status comp54))
    (update-status comp55)
    (not (one-off comp54))
    (probabilistic 1/4 (and (not (running comp54))))
  )
)
(:action update-status-comp55-rebooted
  :parameters ()
  :precondition (and
    (update-status comp55)
    (rebooted comp55)
  )
  :effect (and
    (not (update-status comp55))
    (update-status comp56)
    (not (rebooted comp55))
    (probabilistic 9/10 (and (running comp55)) 1/10 (and (not (running comp55))))
  )
)
(:action update-status-comp55-all-on
  :parameters ()
  :precondition (and
    (update-status comp55)
    (not (rebooted comp55))
    (all-on comp55)
  )
  :effect (and
    (not (update-status comp55))
    (update-status comp56)
    (not (all-on comp55))
    (probabilistic 1/20 (and (not (running comp55))))
  )
)
(:action update-status-comp55-one-off
  :parameters ()
  :precondition (and
    (update-status comp55)
    (not (rebooted comp55))
    (one-off comp55)
  )
  :effect (and
    (not (update-status comp55))
    (update-status comp56)
    (not (one-off comp55))
    (probabilistic 1/4 (and (not (running comp55))))
  )
)
(:action update-status-comp56-rebooted
  :parameters ()
  :precondition (and
    (update-status comp56)
    (rebooted comp56)
  )
  :effect (and
    (not (update-status comp56))
    (update-status comp57)
    (not (rebooted comp56))
    (probabilistic 9/10 (and (running comp56)) 1/10 (and (not (running comp56))))
  )
)
(:action update-status-comp56-all-on
  :parameters ()
  :precondition (and
    (update-status comp56)
    (not (rebooted comp56))
    (all-on comp56)
  )
  :effect (and
    (not (update-status comp56))
    (update-status comp57)
    (not (all-on comp56))
    (probabilistic 1/20 (and (not (running comp56))))
  )
)
(:action update-status-comp56-one-off
  :parameters ()
  :precondition (and
    (update-status comp56)
    (not (rebooted comp56))
    (one-off comp56)
  )
  :effect (and
    (not (update-status comp56))
    (update-status comp57)
    (not (one-off comp56))
    (probabilistic 1/4 (and (not (running comp56))))
  )
)
(:action update-status-comp57-rebooted
  :parameters ()
  :precondition (and
    (update-status comp57)
    (rebooted comp57)
  )
  :effect (and
    (not (update-status comp57))
    (update-status comp58)
    (not (rebooted comp57))
    (probabilistic 9/10 (and (running comp57)) 1/10 (and (not (running comp57))))
  )
)
(:action update-status-comp57-all-on
  :parameters ()
  :precondition (and
    (update-status comp57)
    (not (rebooted comp57))
    (all-on comp57)
  )
  :effect (and
    (not (update-status comp57))
    (update-status comp58)
    (not (all-on comp57))
    (probabilistic 1/20 (and (not (running comp57))))
  )
)
(:action update-status-comp57-one-off
  :parameters ()
  :precondition (and
    (update-status comp57)
    (not (rebooted comp57))
    (one-off comp57)
  )
  :effect (and
    (not (update-status comp57))
    (update-status comp58)
    (not (one-off comp57))
    (probabilistic 1/4 (and (not (running comp57))))
  )
)
(:action update-status-comp58-rebooted
  :parameters ()
  :precondition (and
    (update-status comp58)
    (rebooted comp58)
  )
  :effect (and
    (not (update-status comp58))
    (update-status comp59)
    (not (rebooted comp58))
    (probabilistic 9/10 (and (running comp58)) 1/10 (and (not (running comp58))))
  )
)
(:action update-status-comp58-all-on
  :parameters ()
  :precondition (and
    (update-status comp58)
    (not (rebooted comp58))
    (all-on comp58)
  )
  :effect (and
    (not (update-status comp58))
    (update-status comp59)
    (not (all-on comp58))
    (probabilistic 1/20 (and (not (running comp58))))
  )
)
(:action update-status-comp58-one-off
  :parameters ()
  :precondition (and
    (update-status comp58)
    (not (rebooted comp58))
    (one-off comp58)
  )
  :effect (and
    (not (update-status comp58))
    (update-status comp59)
    (not (one-off comp58))
    (probabilistic 1/4 (and (not (running comp58))))
  )
)
(:action update-status-comp59-rebooted
  :parameters ()
  :precondition (and
    (update-status comp59)
    (rebooted comp59)
  )
  :effect (and
    (not (update-status comp59))
    (all-updated)
    (not (rebooted comp59))
    (probabilistic 9/10 (and (running comp59)) 1/10 (and (not (running comp59))))
  )
)
(:action update-status-comp59-all-on
  :parameters ()
  :precondition (and
    (update-status comp59)
    (not (rebooted comp59))
    (all-on comp59)
  )
  :effect (and
    (not (update-status comp59))
    (all-updated)
    (not (all-on comp59))
    (probabilistic 1/20 (and (not (running comp59))))
  )
)
(:action update-status-comp59-one-off
  :parameters ()
  :precondition (and
    (update-status comp59)
    (not (rebooted comp59))
    (one-off comp59)
  )
  :effect (and
    (not (update-status comp59))
    (all-updated)
    (not (one-off comp59))
    (probabilistic 1/4 (and (not (running comp59))))
  )
)
)