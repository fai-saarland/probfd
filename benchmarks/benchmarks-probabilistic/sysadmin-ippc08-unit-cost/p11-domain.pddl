(define (domain sysadmin-nocount)
(:requirements :probabilistic-effects :negative-preconditions :typing)
(:types computer - object)
(:constants
  comp0 comp1 comp2 comp3 comp4 comp5 comp6 comp7 comp8 comp9 comp10 comp11 comp12 comp13 comp14 comp15 comp16 comp17 comp18 comp19 comp20 comp21 comp22 comp23 comp24 comp25 comp26 comp27 comp28 comp29 comp30 comp31 comp32 comp33 comp34 comp35 comp36 comp37 comp38 comp39 comp40 comp41 comp42 comp43 comp44 comp45 comp46 comp47 comp48 comp49 comp50 comp51 comp52 comp53 comp54 comp55 comp56 comp57 comp58 comp59 comp60 comp61 comp62 comp63 comp64 comp65 comp66 comp67 comp68 comp69 comp70 comp71 comp72 comp73 comp74 comp75 comp76 comp77 comp78 comp79 comp80 comp81 comp82 comp83 comp84 comp85 comp86 comp87 comp88 comp89 comp90 comp91 comp92 comp93 comp94 comp95 comp96 comp97 comp98 comp99 comp100 comp101 comp102 comp103 comp104 comp105 comp106 comp107 comp108 comp109 comp110 comp111 comp112 comp113 comp114 comp115 comp116 comp117 comp118 comp119 - computer
)
(:predicates
  (running ?c - computer)
  (rebooted ?c - computer)
  (evaluate ?c - computer)
  (update-status ?c - computer)
  (one-off ?c - computer)
  (all-on ?c - computer)
  (all-updated)
)
(:action reboot
  :parameters (?c - computer)
  :precondition (and
    (all-updated)
  )
  :effect (and
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
    (running comp119)
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (all-on comp0)
  )
)
(:action evaluate-comp0-off-comp119
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp119))
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
    (running comp32)
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
(:action evaluate-comp4-off-comp32
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (not (running comp32))
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
    (running comp52)
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
(:action evaluate-comp6-off-comp52
  :parameters ()
  :precondition (and
    (evaluate comp6)
    (not (rebooted comp6))
    (not (running comp52))
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
    (running comp48)
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
(:action evaluate-comp7-off-comp48
  :parameters ()
  :precondition (and
    (evaluate comp7)
    (not (rebooted comp7))
    (not (running comp48))
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
    (running comp1)
    (running comp8)
  )
  :effect (and
    (not (evaluate comp9))
    (evaluate comp10)
    (all-on comp9)
  )
)
(:action evaluate-comp9-off-comp1
  :parameters ()
  :precondition (and
    (evaluate comp9)
    (not (rebooted comp9))
    (not (running comp1))
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
    (running comp55)
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
(:action evaluate-comp10-off-comp55
  :parameters ()
  :precondition (and
    (evaluate comp10)
    (not (rebooted comp10))
    (not (running comp55))
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
    (running comp35)
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
(:action evaluate-comp12-off-comp35
  :parameters ()
  :precondition (and
    (evaluate comp12)
    (not (rebooted comp12))
    (not (running comp35))
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
    (running comp37)
    (running comp41)
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
(:action evaluate-comp14-off-comp37
  :parameters ()
  :precondition (and
    (evaluate comp14)
    (not (rebooted comp14))
    (not (running comp37))
  )
  :effect (and
    (not (evaluate comp14))
    (evaluate comp15)
    (one-off comp14)
  )
)
(:action evaluate-comp14-off-comp41
  :parameters ()
  :precondition (and
    (evaluate comp14)
    (not (rebooted comp14))
    (not (running comp41))
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
    (running comp33)
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
(:action evaluate-comp16-off-comp33
  :parameters ()
  :precondition (and
    (evaluate comp16)
    (not (rebooted comp16))
    (not (running comp33))
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
    (running comp76)
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
(:action evaluate-comp18-off-comp76
  :parameters ()
  :precondition (and
    (evaluate comp18)
    (not (rebooted comp18))
    (not (running comp76))
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
    (running comp64)
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
(:action evaluate-comp19-off-comp64
  :parameters ()
  :precondition (and
    (evaluate comp19)
    (not (rebooted comp19))
    (not (running comp64))
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
    (running comp64)
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
(:action evaluate-comp22-off-comp64
  :parameters ()
  :precondition (and
    (evaluate comp22)
    (not (rebooted comp22))
    (not (running comp64))
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
    (evaluate comp25)
  )
)
(:action evaluate-comp24-all-on
  :parameters ()
  :precondition (and
    (evaluate comp24)
    (not (rebooted comp24))
    (running comp1)
    (running comp3)
    (running comp23)
  )
  :effect (and
    (not (evaluate comp24))
    (evaluate comp25)
    (all-on comp24)
  )
)
(:action evaluate-comp24-off-comp1
  :parameters ()
  :precondition (and
    (evaluate comp24)
    (not (rebooted comp24))
    (not (running comp1))
  )
  :effect (and
    (not (evaluate comp24))
    (evaluate comp25)
    (one-off comp24)
  )
)
(:action evaluate-comp24-off-comp3
  :parameters ()
  :precondition (and
    (evaluate comp24)
    (not (rebooted comp24))
    (not (running comp3))
  )
  :effect (and
    (not (evaluate comp24))
    (evaluate comp25)
    (one-off comp24)
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
    (running comp78)
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
(:action evaluate-comp25-off-comp78
  :parameters ()
  :precondition (and
    (evaluate comp25)
    (not (rebooted comp25))
    (not (running comp78))
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
    (running comp27)
    (running comp38)
  )
  :effect (and
    (not (evaluate comp28))
    (evaluate comp29)
    (all-on comp28)
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
(:action evaluate-comp28-off-comp38
  :parameters ()
  :precondition (and
    (evaluate comp28)
    (not (rebooted comp28))
    (not (running comp38))
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
    (running comp103)
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
(:action evaluate-comp29-off-comp103
  :parameters ()
  :precondition (and
    (evaluate comp29)
    (not (rebooted comp29))
    (not (running comp103))
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
    (running comp31)
    (running comp55)
  )
  :effect (and
    (not (evaluate comp32))
    (evaluate comp33)
    (all-on comp32)
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
(:action evaluate-comp32-off-comp55
  :parameters ()
  :precondition (and
    (evaluate comp32)
    (not (rebooted comp32))
    (not (running comp55))
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
    (running comp98)
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
(:action evaluate-comp34-off-comp98
  :parameters ()
  :precondition (and
    (evaluate comp34)
    (not (rebooted comp34))
    (not (running comp98))
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
    (running comp89)
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
(:action evaluate-comp36-off-comp89
  :parameters ()
  :precondition (and
    (evaluate comp36)
    (not (rebooted comp36))
    (not (running comp89))
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
    (running comp26)
    (running comp37)
  )
  :effect (and
    (not (evaluate comp38))
    (evaluate comp39)
    (all-on comp38)
  )
)
(:action evaluate-comp38-off-comp26
  :parameters ()
  :precondition (and
    (evaluate comp38)
    (not (rebooted comp38))
    (not (running comp26))
  )
  :effect (and
    (not (evaluate comp38))
    (evaluate comp39)
    (one-off comp38)
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
    (running comp5)
    (running comp41)
    (running comp113)
  )
  :effect (and
    (not (evaluate comp42))
    (evaluate comp43)
    (all-on comp42)
  )
)
(:action evaluate-comp42-off-comp5
  :parameters ()
  :precondition (and
    (evaluate comp42)
    (not (rebooted comp42))
    (not (running comp5))
  )
  :effect (and
    (not (evaluate comp42))
    (evaluate comp43)
    (one-off comp42)
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
(:action evaluate-comp42-off-comp113
  :parameters ()
  :precondition (and
    (evaluate comp42)
    (not (rebooted comp42))
    (not (running comp113))
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
    (running comp44)
  )
  :effect (and
    (not (evaluate comp45))
    (evaluate comp46)
    (all-on comp45)
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
    (running comp19)
    (running comp45)
  )
  :effect (and
    (not (evaluate comp46))
    (evaluate comp47)
    (all-on comp46)
  )
)
(:action evaluate-comp46-off-comp19
  :parameters ()
  :precondition (and
    (evaluate comp46)
    (not (rebooted comp46))
    (not (running comp19))
  )
  :effect (and
    (not (evaluate comp46))
    (evaluate comp47)
    (one-off comp46)
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
    (running comp4)
    (running comp46)
  )
  :effect (and
    (not (evaluate comp47))
    (evaluate comp48)
    (all-on comp47)
  )
)
(:action evaluate-comp47-off-comp4
  :parameters ()
  :precondition (and
    (evaluate comp47)
    (not (rebooted comp47))
    (not (running comp4))
  )
  :effect (and
    (not (evaluate comp47))
    (evaluate comp48)
    (one-off comp47)
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
    (running comp106)
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
(:action evaluate-comp48-off-comp106
  :parameters ()
  :precondition (and
    (evaluate comp48)
    (not (rebooted comp48))
    (not (running comp106))
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
    (running comp48)
    (running comp103)
  )
  :effect (and
    (not (evaluate comp49))
    (evaluate comp50)
    (all-on comp49)
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
(:action evaluate-comp49-off-comp103
  :parameters ()
  :precondition (and
    (evaluate comp49)
    (not (rebooted comp49))
    (not (running comp103))
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
    (running comp31)
    (running comp49)
  )
  :effect (and
    (not (evaluate comp50))
    (evaluate comp51)
    (all-on comp50)
  )
)
(:action evaluate-comp50-off-comp31
  :parameters ()
  :precondition (and
    (evaluate comp50)
    (not (rebooted comp50))
    (not (running comp31))
  )
  :effect (and
    (not (evaluate comp50))
    (evaluate comp51)
    (one-off comp50)
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
    (running comp3)
    (running comp50)
  )
  :effect (and
    (not (evaluate comp51))
    (evaluate comp52)
    (all-on comp51)
  )
)
(:action evaluate-comp51-off-comp3
  :parameters ()
  :precondition (and
    (evaluate comp51)
    (not (rebooted comp51))
    (not (running comp3))
  )
  :effect (and
    (not (evaluate comp51))
    (evaluate comp52)
    (one-off comp51)
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
    (running comp113)
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
(:action evaluate-comp52-off-comp113
  :parameters ()
  :precondition (and
    (evaluate comp52)
    (not (rebooted comp52))
    (not (running comp113))
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
    (running comp12)
    (running comp53)
  )
  :effect (and
    (not (evaluate comp54))
    (evaluate comp55)
    (all-on comp54)
  )
)
(:action evaluate-comp54-off-comp12
  :parameters ()
  :precondition (and
    (evaluate comp54)
    (not (rebooted comp54))
    (not (running comp12))
  )
  :effect (and
    (not (evaluate comp54))
    (evaluate comp55)
    (one-off comp54)
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
    (running comp54)
  )
  :effect (and
    (not (evaluate comp55))
    (evaluate comp56)
    (all-on comp55)
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
    (running comp56)
  )
  :effect (and
    (not (evaluate comp57))
    (evaluate comp58)
    (all-on comp57)
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
    (running comp37)
    (running comp57)
  )
  :effect (and
    (not (evaluate comp58))
    (evaluate comp59)
    (all-on comp58)
  )
)
(:action evaluate-comp58-off-comp37
  :parameters ()
  :precondition (and
    (evaluate comp58)
    (not (rebooted comp58))
    (not (running comp37))
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
    (evaluate comp60)
  )
)
(:action evaluate-comp59-all-on
  :parameters ()
  :precondition (and
    (evaluate comp59)
    (not (rebooted comp59))
    (running comp52)
    (running comp58)
  )
  :effect (and
    (not (evaluate comp59))
    (evaluate comp60)
    (all-on comp59)
  )
)
(:action evaluate-comp59-off-comp52
  :parameters ()
  :precondition (and
    (evaluate comp59)
    (not (rebooted comp59))
    (not (running comp52))
  )
  :effect (and
    (not (evaluate comp59))
    (evaluate comp60)
    (one-off comp59)
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
    (evaluate comp60)
    (one-off comp59)
  )
)
(:action evaluate-comp60-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp60)
    (rebooted comp60)
  )
  :effect (and
    (not (evaluate comp60))
    (evaluate comp61)
  )
)
(:action evaluate-comp60-all-on
  :parameters ()
  :precondition (and
    (evaluate comp60)
    (not (rebooted comp60))
    (running comp59)
  )
  :effect (and
    (not (evaluate comp60))
    (evaluate comp61)
    (all-on comp60)
  )
)
(:action evaluate-comp60-off-comp59
  :parameters ()
  :precondition (and
    (evaluate comp60)
    (not (rebooted comp60))
    (not (running comp59))
  )
  :effect (and
    (not (evaluate comp60))
    (evaluate comp61)
    (one-off comp60)
  )
)
(:action evaluate-comp61-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp61)
    (rebooted comp61)
  )
  :effect (and
    (not (evaluate comp61))
    (evaluate comp62)
  )
)
(:action evaluate-comp61-all-on
  :parameters ()
  :precondition (and
    (evaluate comp61)
    (not (rebooted comp61))
    (running comp60)
  )
  :effect (and
    (not (evaluate comp61))
    (evaluate comp62)
    (all-on comp61)
  )
)
(:action evaluate-comp61-off-comp60
  :parameters ()
  :precondition (and
    (evaluate comp61)
    (not (rebooted comp61))
    (not (running comp60))
  )
  :effect (and
    (not (evaluate comp61))
    (evaluate comp62)
    (one-off comp61)
  )
)
(:action evaluate-comp62-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp62)
    (rebooted comp62)
  )
  :effect (and
    (not (evaluate comp62))
    (evaluate comp63)
  )
)
(:action evaluate-comp62-all-on
  :parameters ()
  :precondition (and
    (evaluate comp62)
    (not (rebooted comp62))
    (running comp61)
  )
  :effect (and
    (not (evaluate comp62))
    (evaluate comp63)
    (all-on comp62)
  )
)
(:action evaluate-comp62-off-comp61
  :parameters ()
  :precondition (and
    (evaluate comp62)
    (not (rebooted comp62))
    (not (running comp61))
  )
  :effect (and
    (not (evaluate comp62))
    (evaluate comp63)
    (one-off comp62)
  )
)
(:action evaluate-comp63-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp63)
    (rebooted comp63)
  )
  :effect (and
    (not (evaluate comp63))
    (evaluate comp64)
  )
)
(:action evaluate-comp63-all-on
  :parameters ()
  :precondition (and
    (evaluate comp63)
    (not (rebooted comp63))
    (running comp40)
    (running comp62)
  )
  :effect (and
    (not (evaluate comp63))
    (evaluate comp64)
    (all-on comp63)
  )
)
(:action evaluate-comp63-off-comp40
  :parameters ()
  :precondition (and
    (evaluate comp63)
    (not (rebooted comp63))
    (not (running comp40))
  )
  :effect (and
    (not (evaluate comp63))
    (evaluate comp64)
    (one-off comp63)
  )
)
(:action evaluate-comp63-off-comp62
  :parameters ()
  :precondition (and
    (evaluate comp63)
    (not (rebooted comp63))
    (not (running comp62))
  )
  :effect (and
    (not (evaluate comp63))
    (evaluate comp64)
    (one-off comp63)
  )
)
(:action evaluate-comp64-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp64)
    (rebooted comp64)
  )
  :effect (and
    (not (evaluate comp64))
    (evaluate comp65)
  )
)
(:action evaluate-comp64-all-on
  :parameters ()
  :precondition (and
    (evaluate comp64)
    (not (rebooted comp64))
    (running comp20)
    (running comp63)
  )
  :effect (and
    (not (evaluate comp64))
    (evaluate comp65)
    (all-on comp64)
  )
)
(:action evaluate-comp64-off-comp20
  :parameters ()
  :precondition (and
    (evaluate comp64)
    (not (rebooted comp64))
    (not (running comp20))
  )
  :effect (and
    (not (evaluate comp64))
    (evaluate comp65)
    (one-off comp64)
  )
)
(:action evaluate-comp64-off-comp63
  :parameters ()
  :precondition (and
    (evaluate comp64)
    (not (rebooted comp64))
    (not (running comp63))
  )
  :effect (and
    (not (evaluate comp64))
    (evaluate comp65)
    (one-off comp64)
  )
)
(:action evaluate-comp65-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp65)
    (rebooted comp65)
  )
  :effect (and
    (not (evaluate comp65))
    (evaluate comp66)
  )
)
(:action evaluate-comp65-all-on
  :parameters ()
  :precondition (and
    (evaluate comp65)
    (not (rebooted comp65))
    (running comp64)
    (running comp119)
  )
  :effect (and
    (not (evaluate comp65))
    (evaluate comp66)
    (all-on comp65)
  )
)
(:action evaluate-comp65-off-comp64
  :parameters ()
  :precondition (and
    (evaluate comp65)
    (not (rebooted comp65))
    (not (running comp64))
  )
  :effect (and
    (not (evaluate comp65))
    (evaluate comp66)
    (one-off comp65)
  )
)
(:action evaluate-comp65-off-comp119
  :parameters ()
  :precondition (and
    (evaluate comp65)
    (not (rebooted comp65))
    (not (running comp119))
  )
  :effect (and
    (not (evaluate comp65))
    (evaluate comp66)
    (one-off comp65)
  )
)
(:action evaluate-comp66-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp66)
    (rebooted comp66)
  )
  :effect (and
    (not (evaluate comp66))
    (evaluate comp67)
  )
)
(:action evaluate-comp66-all-on
  :parameters ()
  :precondition (and
    (evaluate comp66)
    (not (rebooted comp66))
    (running comp65)
  )
  :effect (and
    (not (evaluate comp66))
    (evaluate comp67)
    (all-on comp66)
  )
)
(:action evaluate-comp66-off-comp65
  :parameters ()
  :precondition (and
    (evaluate comp66)
    (not (rebooted comp66))
    (not (running comp65))
  )
  :effect (and
    (not (evaluate comp66))
    (evaluate comp67)
    (one-off comp66)
  )
)
(:action evaluate-comp67-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp67)
    (rebooted comp67)
  )
  :effect (and
    (not (evaluate comp67))
    (evaluate comp68)
  )
)
(:action evaluate-comp67-all-on
  :parameters ()
  :precondition (and
    (evaluate comp67)
    (not (rebooted comp67))
    (running comp66)
  )
  :effect (and
    (not (evaluate comp67))
    (evaluate comp68)
    (all-on comp67)
  )
)
(:action evaluate-comp67-off-comp66
  :parameters ()
  :precondition (and
    (evaluate comp67)
    (not (rebooted comp67))
    (not (running comp66))
  )
  :effect (and
    (not (evaluate comp67))
    (evaluate comp68)
    (one-off comp67)
  )
)
(:action evaluate-comp68-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp68)
    (rebooted comp68)
  )
  :effect (and
    (not (evaluate comp68))
    (evaluate comp69)
  )
)
(:action evaluate-comp68-all-on
  :parameters ()
  :precondition (and
    (evaluate comp68)
    (not (rebooted comp68))
    (running comp51)
    (running comp67)
    (running comp73)
  )
  :effect (and
    (not (evaluate comp68))
    (evaluate comp69)
    (all-on comp68)
  )
)
(:action evaluate-comp68-off-comp51
  :parameters ()
  :precondition (and
    (evaluate comp68)
    (not (rebooted comp68))
    (not (running comp51))
  )
  :effect (and
    (not (evaluate comp68))
    (evaluate comp69)
    (one-off comp68)
  )
)
(:action evaluate-comp68-off-comp67
  :parameters ()
  :precondition (and
    (evaluate comp68)
    (not (rebooted comp68))
    (not (running comp67))
  )
  :effect (and
    (not (evaluate comp68))
    (evaluate comp69)
    (one-off comp68)
  )
)
(:action evaluate-comp68-off-comp73
  :parameters ()
  :precondition (and
    (evaluate comp68)
    (not (rebooted comp68))
    (not (running comp73))
  )
  :effect (and
    (not (evaluate comp68))
    (evaluate comp69)
    (one-off comp68)
  )
)
(:action evaluate-comp69-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp69)
    (rebooted comp69)
  )
  :effect (and
    (not (evaluate comp69))
    (evaluate comp70)
  )
)
(:action evaluate-comp69-all-on
  :parameters ()
  :precondition (and
    (evaluate comp69)
    (not (rebooted comp69))
    (running comp23)
    (running comp68)
  )
  :effect (and
    (not (evaluate comp69))
    (evaluate comp70)
    (all-on comp69)
  )
)
(:action evaluate-comp69-off-comp23
  :parameters ()
  :precondition (and
    (evaluate comp69)
    (not (rebooted comp69))
    (not (running comp23))
  )
  :effect (and
    (not (evaluate comp69))
    (evaluate comp70)
    (one-off comp69)
  )
)
(:action evaluate-comp69-off-comp68
  :parameters ()
  :precondition (and
    (evaluate comp69)
    (not (rebooted comp69))
    (not (running comp68))
  )
  :effect (and
    (not (evaluate comp69))
    (evaluate comp70)
    (one-off comp69)
  )
)
(:action evaluate-comp70-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp70)
    (rebooted comp70)
  )
  :effect (and
    (not (evaluate comp70))
    (evaluate comp71)
  )
)
(:action evaluate-comp70-all-on
  :parameters ()
  :precondition (and
    (evaluate comp70)
    (not (rebooted comp70))
    (running comp66)
    (running comp69)
  )
  :effect (and
    (not (evaluate comp70))
    (evaluate comp71)
    (all-on comp70)
  )
)
(:action evaluate-comp70-off-comp66
  :parameters ()
  :precondition (and
    (evaluate comp70)
    (not (rebooted comp70))
    (not (running comp66))
  )
  :effect (and
    (not (evaluate comp70))
    (evaluate comp71)
    (one-off comp70)
  )
)
(:action evaluate-comp70-off-comp69
  :parameters ()
  :precondition (and
    (evaluate comp70)
    (not (rebooted comp70))
    (not (running comp69))
  )
  :effect (and
    (not (evaluate comp70))
    (evaluate comp71)
    (one-off comp70)
  )
)
(:action evaluate-comp71-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp71)
    (rebooted comp71)
  )
  :effect (and
    (not (evaluate comp71))
    (evaluate comp72)
  )
)
(:action evaluate-comp71-all-on
  :parameters ()
  :precondition (and
    (evaluate comp71)
    (not (rebooted comp71))
    (running comp68)
    (running comp70)
  )
  :effect (and
    (not (evaluate comp71))
    (evaluate comp72)
    (all-on comp71)
  )
)
(:action evaluate-comp71-off-comp68
  :parameters ()
  :precondition (and
    (evaluate comp71)
    (not (rebooted comp71))
    (not (running comp68))
  )
  :effect (and
    (not (evaluate comp71))
    (evaluate comp72)
    (one-off comp71)
  )
)
(:action evaluate-comp71-off-comp70
  :parameters ()
  :precondition (and
    (evaluate comp71)
    (not (rebooted comp71))
    (not (running comp70))
  )
  :effect (and
    (not (evaluate comp71))
    (evaluate comp72)
    (one-off comp71)
  )
)
(:action evaluate-comp72-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp72)
    (rebooted comp72)
  )
  :effect (and
    (not (evaluate comp72))
    (evaluate comp73)
  )
)
(:action evaluate-comp72-all-on
  :parameters ()
  :precondition (and
    (evaluate comp72)
    (not (rebooted comp72))
    (running comp71)
  )
  :effect (and
    (not (evaluate comp72))
    (evaluate comp73)
    (all-on comp72)
  )
)
(:action evaluate-comp72-off-comp71
  :parameters ()
  :precondition (and
    (evaluate comp72)
    (not (rebooted comp72))
    (not (running comp71))
  )
  :effect (and
    (not (evaluate comp72))
    (evaluate comp73)
    (one-off comp72)
  )
)
(:action evaluate-comp73-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp73)
    (rebooted comp73)
  )
  :effect (and
    (not (evaluate comp73))
    (evaluate comp74)
  )
)
(:action evaluate-comp73-all-on
  :parameters ()
  :precondition (and
    (evaluate comp73)
    (not (rebooted comp73))
    (running comp72)
  )
  :effect (and
    (not (evaluate comp73))
    (evaluate comp74)
    (all-on comp73)
  )
)
(:action evaluate-comp73-off-comp72
  :parameters ()
  :precondition (and
    (evaluate comp73)
    (not (rebooted comp73))
    (not (running comp72))
  )
  :effect (and
    (not (evaluate comp73))
    (evaluate comp74)
    (one-off comp73)
  )
)
(:action evaluate-comp74-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp74)
    (rebooted comp74)
  )
  :effect (and
    (not (evaluate comp74))
    (evaluate comp75)
  )
)
(:action evaluate-comp74-all-on
  :parameters ()
  :precondition (and
    (evaluate comp74)
    (not (rebooted comp74))
    (running comp73)
  )
  :effect (and
    (not (evaluate comp74))
    (evaluate comp75)
    (all-on comp74)
  )
)
(:action evaluate-comp74-off-comp73
  :parameters ()
  :precondition (and
    (evaluate comp74)
    (not (rebooted comp74))
    (not (running comp73))
  )
  :effect (and
    (not (evaluate comp74))
    (evaluate comp75)
    (one-off comp74)
  )
)
(:action evaluate-comp75-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp75)
    (rebooted comp75)
  )
  :effect (and
    (not (evaluate comp75))
    (evaluate comp76)
  )
)
(:action evaluate-comp75-all-on
  :parameters ()
  :precondition (and
    (evaluate comp75)
    (not (rebooted comp75))
    (running comp74)
  )
  :effect (and
    (not (evaluate comp75))
    (evaluate comp76)
    (all-on comp75)
  )
)
(:action evaluate-comp75-off-comp74
  :parameters ()
  :precondition (and
    (evaluate comp75)
    (not (rebooted comp75))
    (not (running comp74))
  )
  :effect (and
    (not (evaluate comp75))
    (evaluate comp76)
    (one-off comp75)
  )
)
(:action evaluate-comp76-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp76)
    (rebooted comp76)
  )
  :effect (and
    (not (evaluate comp76))
    (evaluate comp77)
  )
)
(:action evaluate-comp76-all-on
  :parameters ()
  :precondition (and
    (evaluate comp76)
    (not (rebooted comp76))
    (running comp75)
  )
  :effect (and
    (not (evaluate comp76))
    (evaluate comp77)
    (all-on comp76)
  )
)
(:action evaluate-comp76-off-comp75
  :parameters ()
  :precondition (and
    (evaluate comp76)
    (not (rebooted comp76))
    (not (running comp75))
  )
  :effect (and
    (not (evaluate comp76))
    (evaluate comp77)
    (one-off comp76)
  )
)
(:action evaluate-comp77-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp77)
    (rebooted comp77)
  )
  :effect (and
    (not (evaluate comp77))
    (evaluate comp78)
  )
)
(:action evaluate-comp77-all-on
  :parameters ()
  :precondition (and
    (evaluate comp77)
    (not (rebooted comp77))
    (running comp76)
  )
  :effect (and
    (not (evaluate comp77))
    (evaluate comp78)
    (all-on comp77)
  )
)
(:action evaluate-comp77-off-comp76
  :parameters ()
  :precondition (and
    (evaluate comp77)
    (not (rebooted comp77))
    (not (running comp76))
  )
  :effect (and
    (not (evaluate comp77))
    (evaluate comp78)
    (one-off comp77)
  )
)
(:action evaluate-comp78-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp78)
    (rebooted comp78)
  )
  :effect (and
    (not (evaluate comp78))
    (evaluate comp79)
  )
)
(:action evaluate-comp78-all-on
  :parameters ()
  :precondition (and
    (evaluate comp78)
    (not (rebooted comp78))
    (running comp54)
    (running comp77)
  )
  :effect (and
    (not (evaluate comp78))
    (evaluate comp79)
    (all-on comp78)
  )
)
(:action evaluate-comp78-off-comp54
  :parameters ()
  :precondition (and
    (evaluate comp78)
    (not (rebooted comp78))
    (not (running comp54))
  )
  :effect (and
    (not (evaluate comp78))
    (evaluate comp79)
    (one-off comp78)
  )
)
(:action evaluate-comp78-off-comp77
  :parameters ()
  :precondition (and
    (evaluate comp78)
    (not (rebooted comp78))
    (not (running comp77))
  )
  :effect (and
    (not (evaluate comp78))
    (evaluate comp79)
    (one-off comp78)
  )
)
(:action evaluate-comp79-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp79)
    (rebooted comp79)
  )
  :effect (and
    (not (evaluate comp79))
    (evaluate comp80)
  )
)
(:action evaluate-comp79-all-on
  :parameters ()
  :precondition (and
    (evaluate comp79)
    (not (rebooted comp79))
    (running comp78)
  )
  :effect (and
    (not (evaluate comp79))
    (evaluate comp80)
    (all-on comp79)
  )
)
(:action evaluate-comp79-off-comp78
  :parameters ()
  :precondition (and
    (evaluate comp79)
    (not (rebooted comp79))
    (not (running comp78))
  )
  :effect (and
    (not (evaluate comp79))
    (evaluate comp80)
    (one-off comp79)
  )
)
(:action evaluate-comp80-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp80)
    (rebooted comp80)
  )
  :effect (and
    (not (evaluate comp80))
    (evaluate comp81)
  )
)
(:action evaluate-comp80-all-on
  :parameters ()
  :precondition (and
    (evaluate comp80)
    (not (rebooted comp80))
    (running comp79)
  )
  :effect (and
    (not (evaluate comp80))
    (evaluate comp81)
    (all-on comp80)
  )
)
(:action evaluate-comp80-off-comp79
  :parameters ()
  :precondition (and
    (evaluate comp80)
    (not (rebooted comp80))
    (not (running comp79))
  )
  :effect (and
    (not (evaluate comp80))
    (evaluate comp81)
    (one-off comp80)
  )
)
(:action evaluate-comp81-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp81)
    (rebooted comp81)
  )
  :effect (and
    (not (evaluate comp81))
    (evaluate comp82)
  )
)
(:action evaluate-comp81-all-on
  :parameters ()
  :precondition (and
    (evaluate comp81)
    (not (rebooted comp81))
    (running comp80)
    (running comp103)
  )
  :effect (and
    (not (evaluate comp81))
    (evaluate comp82)
    (all-on comp81)
  )
)
(:action evaluate-comp81-off-comp80
  :parameters ()
  :precondition (and
    (evaluate comp81)
    (not (rebooted comp81))
    (not (running comp80))
  )
  :effect (and
    (not (evaluate comp81))
    (evaluate comp82)
    (one-off comp81)
  )
)
(:action evaluate-comp81-off-comp103
  :parameters ()
  :precondition (and
    (evaluate comp81)
    (not (rebooted comp81))
    (not (running comp103))
  )
  :effect (and
    (not (evaluate comp81))
    (evaluate comp82)
    (one-off comp81)
  )
)
(:action evaluate-comp82-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp82)
    (rebooted comp82)
  )
  :effect (and
    (not (evaluate comp82))
    (evaluate comp83)
  )
)
(:action evaluate-comp82-all-on
  :parameters ()
  :precondition (and
    (evaluate comp82)
    (not (rebooted comp82))
    (running comp81)
  )
  :effect (and
    (not (evaluate comp82))
    (evaluate comp83)
    (all-on comp82)
  )
)
(:action evaluate-comp82-off-comp81
  :parameters ()
  :precondition (and
    (evaluate comp82)
    (not (rebooted comp82))
    (not (running comp81))
  )
  :effect (and
    (not (evaluate comp82))
    (evaluate comp83)
    (one-off comp82)
  )
)
(:action evaluate-comp83-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp83)
    (rebooted comp83)
  )
  :effect (and
    (not (evaluate comp83))
    (evaluate comp84)
  )
)
(:action evaluate-comp83-all-on
  :parameters ()
  :precondition (and
    (evaluate comp83)
    (not (rebooted comp83))
    (running comp82)
  )
  :effect (and
    (not (evaluate comp83))
    (evaluate comp84)
    (all-on comp83)
  )
)
(:action evaluate-comp83-off-comp82
  :parameters ()
  :precondition (and
    (evaluate comp83)
    (not (rebooted comp83))
    (not (running comp82))
  )
  :effect (and
    (not (evaluate comp83))
    (evaluate comp84)
    (one-off comp83)
  )
)
(:action evaluate-comp84-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp84)
    (rebooted comp84)
  )
  :effect (and
    (not (evaluate comp84))
    (evaluate comp85)
  )
)
(:action evaluate-comp84-all-on
  :parameters ()
  :precondition (and
    (evaluate comp84)
    (not (rebooted comp84))
    (running comp83)
  )
  :effect (and
    (not (evaluate comp84))
    (evaluate comp85)
    (all-on comp84)
  )
)
(:action evaluate-comp84-off-comp83
  :parameters ()
  :precondition (and
    (evaluate comp84)
    (not (rebooted comp84))
    (not (running comp83))
  )
  :effect (and
    (not (evaluate comp84))
    (evaluate comp85)
    (one-off comp84)
  )
)
(:action evaluate-comp85-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp85)
    (rebooted comp85)
  )
  :effect (and
    (not (evaluate comp85))
    (evaluate comp86)
  )
)
(:action evaluate-comp85-all-on
  :parameters ()
  :precondition (and
    (evaluate comp85)
    (not (rebooted comp85))
    (running comp84)
  )
  :effect (and
    (not (evaluate comp85))
    (evaluate comp86)
    (all-on comp85)
  )
)
(:action evaluate-comp85-off-comp84
  :parameters ()
  :precondition (and
    (evaluate comp85)
    (not (rebooted comp85))
    (not (running comp84))
  )
  :effect (and
    (not (evaluate comp85))
    (evaluate comp86)
    (one-off comp85)
  )
)
(:action evaluate-comp86-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp86)
    (rebooted comp86)
  )
  :effect (and
    (not (evaluate comp86))
    (evaluate comp87)
  )
)
(:action evaluate-comp86-all-on
  :parameters ()
  :precondition (and
    (evaluate comp86)
    (not (rebooted comp86))
    (running comp85)
  )
  :effect (and
    (not (evaluate comp86))
    (evaluate comp87)
    (all-on comp86)
  )
)
(:action evaluate-comp86-off-comp85
  :parameters ()
  :precondition (and
    (evaluate comp86)
    (not (rebooted comp86))
    (not (running comp85))
  )
  :effect (and
    (not (evaluate comp86))
    (evaluate comp87)
    (one-off comp86)
  )
)
(:action evaluate-comp87-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp87)
    (rebooted comp87)
  )
  :effect (and
    (not (evaluate comp87))
    (evaluate comp88)
  )
)
(:action evaluate-comp87-all-on
  :parameters ()
  :precondition (and
    (evaluate comp87)
    (not (rebooted comp87))
    (running comp86)
    (running comp107)
  )
  :effect (and
    (not (evaluate comp87))
    (evaluate comp88)
    (all-on comp87)
  )
)
(:action evaluate-comp87-off-comp86
  :parameters ()
  :precondition (and
    (evaluate comp87)
    (not (rebooted comp87))
    (not (running comp86))
  )
  :effect (and
    (not (evaluate comp87))
    (evaluate comp88)
    (one-off comp87)
  )
)
(:action evaluate-comp87-off-comp107
  :parameters ()
  :precondition (and
    (evaluate comp87)
    (not (rebooted comp87))
    (not (running comp107))
  )
  :effect (and
    (not (evaluate comp87))
    (evaluate comp88)
    (one-off comp87)
  )
)
(:action evaluate-comp88-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp88)
    (rebooted comp88)
  )
  :effect (and
    (not (evaluate comp88))
    (evaluate comp89)
  )
)
(:action evaluate-comp88-all-on
  :parameters ()
  :precondition (and
    (evaluate comp88)
    (not (rebooted comp88))
    (running comp87)
  )
  :effect (and
    (not (evaluate comp88))
    (evaluate comp89)
    (all-on comp88)
  )
)
(:action evaluate-comp88-off-comp87
  :parameters ()
  :precondition (and
    (evaluate comp88)
    (not (rebooted comp88))
    (not (running comp87))
  )
  :effect (and
    (not (evaluate comp88))
    (evaluate comp89)
    (one-off comp88)
  )
)
(:action evaluate-comp89-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp89)
    (rebooted comp89)
  )
  :effect (and
    (not (evaluate comp89))
    (evaluate comp90)
  )
)
(:action evaluate-comp89-all-on
  :parameters ()
  :precondition (and
    (evaluate comp89)
    (not (rebooted comp89))
    (running comp88)
  )
  :effect (and
    (not (evaluate comp89))
    (evaluate comp90)
    (all-on comp89)
  )
)
(:action evaluate-comp89-off-comp88
  :parameters ()
  :precondition (and
    (evaluate comp89)
    (not (rebooted comp89))
    (not (running comp88))
  )
  :effect (and
    (not (evaluate comp89))
    (evaluate comp90)
    (one-off comp89)
  )
)
(:action evaluate-comp90-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp90)
    (rebooted comp90)
  )
  :effect (and
    (not (evaluate comp90))
    (evaluate comp91)
  )
)
(:action evaluate-comp90-all-on
  :parameters ()
  :precondition (and
    (evaluate comp90)
    (not (rebooted comp90))
    (running comp89)
  )
  :effect (and
    (not (evaluate comp90))
    (evaluate comp91)
    (all-on comp90)
  )
)
(:action evaluate-comp90-off-comp89
  :parameters ()
  :precondition (and
    (evaluate comp90)
    (not (rebooted comp90))
    (not (running comp89))
  )
  :effect (and
    (not (evaluate comp90))
    (evaluate comp91)
    (one-off comp90)
  )
)
(:action evaluate-comp91-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp91)
    (rebooted comp91)
  )
  :effect (and
    (not (evaluate comp91))
    (evaluate comp92)
  )
)
(:action evaluate-comp91-all-on
  :parameters ()
  :precondition (and
    (evaluate comp91)
    (not (rebooted comp91))
    (running comp27)
    (running comp87)
    (running comp90)
  )
  :effect (and
    (not (evaluate comp91))
    (evaluate comp92)
    (all-on comp91)
  )
)
(:action evaluate-comp91-off-comp27
  :parameters ()
  :precondition (and
    (evaluate comp91)
    (not (rebooted comp91))
    (not (running comp27))
  )
  :effect (and
    (not (evaluate comp91))
    (evaluate comp92)
    (one-off comp91)
  )
)
(:action evaluate-comp91-off-comp87
  :parameters ()
  :precondition (and
    (evaluate comp91)
    (not (rebooted comp91))
    (not (running comp87))
  )
  :effect (and
    (not (evaluate comp91))
    (evaluate comp92)
    (one-off comp91)
  )
)
(:action evaluate-comp91-off-comp90
  :parameters ()
  :precondition (and
    (evaluate comp91)
    (not (rebooted comp91))
    (not (running comp90))
  )
  :effect (and
    (not (evaluate comp91))
    (evaluate comp92)
    (one-off comp91)
  )
)
(:action evaluate-comp92-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp92)
    (rebooted comp92)
  )
  :effect (and
    (not (evaluate comp92))
    (evaluate comp93)
  )
)
(:action evaluate-comp92-all-on
  :parameters ()
  :precondition (and
    (evaluate comp92)
    (not (rebooted comp92))
    (running comp16)
    (running comp91)
  )
  :effect (and
    (not (evaluate comp92))
    (evaluate comp93)
    (all-on comp92)
  )
)
(:action evaluate-comp92-off-comp16
  :parameters ()
  :precondition (and
    (evaluate comp92)
    (not (rebooted comp92))
    (not (running comp16))
  )
  :effect (and
    (not (evaluate comp92))
    (evaluate comp93)
    (one-off comp92)
  )
)
(:action evaluate-comp92-off-comp91
  :parameters ()
  :precondition (and
    (evaluate comp92)
    (not (rebooted comp92))
    (not (running comp91))
  )
  :effect (and
    (not (evaluate comp92))
    (evaluate comp93)
    (one-off comp92)
  )
)
(:action evaluate-comp93-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp93)
    (rebooted comp93)
  )
  :effect (and
    (not (evaluate comp93))
    (evaluate comp94)
  )
)
(:action evaluate-comp93-all-on
  :parameters ()
  :precondition (and
    (evaluate comp93)
    (not (rebooted comp93))
    (running comp73)
    (running comp92)
  )
  :effect (and
    (not (evaluate comp93))
    (evaluate comp94)
    (all-on comp93)
  )
)
(:action evaluate-comp93-off-comp73
  :parameters ()
  :precondition (and
    (evaluate comp93)
    (not (rebooted comp93))
    (not (running comp73))
  )
  :effect (and
    (not (evaluate comp93))
    (evaluate comp94)
    (one-off comp93)
  )
)
(:action evaluate-comp93-off-comp92
  :parameters ()
  :precondition (and
    (evaluate comp93)
    (not (rebooted comp93))
    (not (running comp92))
  )
  :effect (and
    (not (evaluate comp93))
    (evaluate comp94)
    (one-off comp93)
  )
)
(:action evaluate-comp94-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp94)
    (rebooted comp94)
  )
  :effect (and
    (not (evaluate comp94))
    (evaluate comp95)
  )
)
(:action evaluate-comp94-all-on
  :parameters ()
  :precondition (and
    (evaluate comp94)
    (not (rebooted comp94))
    (running comp93)
  )
  :effect (and
    (not (evaluate comp94))
    (evaluate comp95)
    (all-on comp94)
  )
)
(:action evaluate-comp94-off-comp93
  :parameters ()
  :precondition (and
    (evaluate comp94)
    (not (rebooted comp94))
    (not (running comp93))
  )
  :effect (and
    (not (evaluate comp94))
    (evaluate comp95)
    (one-off comp94)
  )
)
(:action evaluate-comp95-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp95)
    (rebooted comp95)
  )
  :effect (and
    (not (evaluate comp95))
    (evaluate comp96)
  )
)
(:action evaluate-comp95-all-on
  :parameters ()
  :precondition (and
    (evaluate comp95)
    (not (rebooted comp95))
    (running comp84)
    (running comp94)
  )
  :effect (and
    (not (evaluate comp95))
    (evaluate comp96)
    (all-on comp95)
  )
)
(:action evaluate-comp95-off-comp84
  :parameters ()
  :precondition (and
    (evaluate comp95)
    (not (rebooted comp95))
    (not (running comp84))
  )
  :effect (and
    (not (evaluate comp95))
    (evaluate comp96)
    (one-off comp95)
  )
)
(:action evaluate-comp95-off-comp94
  :parameters ()
  :precondition (and
    (evaluate comp95)
    (not (rebooted comp95))
    (not (running comp94))
  )
  :effect (and
    (not (evaluate comp95))
    (evaluate comp96)
    (one-off comp95)
  )
)
(:action evaluate-comp96-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp96)
    (rebooted comp96)
  )
  :effect (and
    (not (evaluate comp96))
    (evaluate comp97)
  )
)
(:action evaluate-comp96-all-on
  :parameters ()
  :precondition (and
    (evaluate comp96)
    (not (rebooted comp96))
    (running comp72)
    (running comp95)
  )
  :effect (and
    (not (evaluate comp96))
    (evaluate comp97)
    (all-on comp96)
  )
)
(:action evaluate-comp96-off-comp72
  :parameters ()
  :precondition (and
    (evaluate comp96)
    (not (rebooted comp96))
    (not (running comp72))
  )
  :effect (and
    (not (evaluate comp96))
    (evaluate comp97)
    (one-off comp96)
  )
)
(:action evaluate-comp96-off-comp95
  :parameters ()
  :precondition (and
    (evaluate comp96)
    (not (rebooted comp96))
    (not (running comp95))
  )
  :effect (and
    (not (evaluate comp96))
    (evaluate comp97)
    (one-off comp96)
  )
)
(:action evaluate-comp97-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp97)
    (rebooted comp97)
  )
  :effect (and
    (not (evaluate comp97))
    (evaluate comp98)
  )
)
(:action evaluate-comp97-all-on
  :parameters ()
  :precondition (and
    (evaluate comp97)
    (not (rebooted comp97))
    (running comp96)
    (running comp112)
  )
  :effect (and
    (not (evaluate comp97))
    (evaluate comp98)
    (all-on comp97)
  )
)
(:action evaluate-comp97-off-comp96
  :parameters ()
  :precondition (and
    (evaluate comp97)
    (not (rebooted comp97))
    (not (running comp96))
  )
  :effect (and
    (not (evaluate comp97))
    (evaluate comp98)
    (one-off comp97)
  )
)
(:action evaluate-comp97-off-comp112
  :parameters ()
  :precondition (and
    (evaluate comp97)
    (not (rebooted comp97))
    (not (running comp112))
  )
  :effect (and
    (not (evaluate comp97))
    (evaluate comp98)
    (one-off comp97)
  )
)
(:action evaluate-comp98-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp98)
    (rebooted comp98)
  )
  :effect (and
    (not (evaluate comp98))
    (evaluate comp99)
  )
)
(:action evaluate-comp98-all-on
  :parameters ()
  :precondition (and
    (evaluate comp98)
    (not (rebooted comp98))
    (running comp76)
    (running comp95)
    (running comp97)
  )
  :effect (and
    (not (evaluate comp98))
    (evaluate comp99)
    (all-on comp98)
  )
)
(:action evaluate-comp98-off-comp76
  :parameters ()
  :precondition (and
    (evaluate comp98)
    (not (rebooted comp98))
    (not (running comp76))
  )
  :effect (and
    (not (evaluate comp98))
    (evaluate comp99)
    (one-off comp98)
  )
)
(:action evaluate-comp98-off-comp95
  :parameters ()
  :precondition (and
    (evaluate comp98)
    (not (rebooted comp98))
    (not (running comp95))
  )
  :effect (and
    (not (evaluate comp98))
    (evaluate comp99)
    (one-off comp98)
  )
)
(:action evaluate-comp98-off-comp97
  :parameters ()
  :precondition (and
    (evaluate comp98)
    (not (rebooted comp98))
    (not (running comp97))
  )
  :effect (and
    (not (evaluate comp98))
    (evaluate comp99)
    (one-off comp98)
  )
)
(:action evaluate-comp99-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp99)
    (rebooted comp99)
  )
  :effect (and
    (not (evaluate comp99))
    (evaluate comp100)
  )
)
(:action evaluate-comp99-all-on
  :parameters ()
  :precondition (and
    (evaluate comp99)
    (not (rebooted comp99))
    (running comp98)
  )
  :effect (and
    (not (evaluate comp99))
    (evaluate comp100)
    (all-on comp99)
  )
)
(:action evaluate-comp99-off-comp98
  :parameters ()
  :precondition (and
    (evaluate comp99)
    (not (rebooted comp99))
    (not (running comp98))
  )
  :effect (and
    (not (evaluate comp99))
    (evaluate comp100)
    (one-off comp99)
  )
)
(:action evaluate-comp100-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp100)
    (rebooted comp100)
  )
  :effect (and
    (not (evaluate comp100))
    (evaluate comp101)
  )
)
(:action evaluate-comp100-all-on
  :parameters ()
  :precondition (and
    (evaluate comp100)
    (not (rebooted comp100))
    (running comp99)
  )
  :effect (and
    (not (evaluate comp100))
    (evaluate comp101)
    (all-on comp100)
  )
)
(:action evaluate-comp100-off-comp99
  :parameters ()
  :precondition (and
    (evaluate comp100)
    (not (rebooted comp100))
    (not (running comp99))
  )
  :effect (and
    (not (evaluate comp100))
    (evaluate comp101)
    (one-off comp100)
  )
)
(:action evaluate-comp101-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp101)
    (rebooted comp101)
  )
  :effect (and
    (not (evaluate comp101))
    (evaluate comp102)
  )
)
(:action evaluate-comp101-all-on
  :parameters ()
  :precondition (and
    (evaluate comp101)
    (not (rebooted comp101))
    (running comp100)
  )
  :effect (and
    (not (evaluate comp101))
    (evaluate comp102)
    (all-on comp101)
  )
)
(:action evaluate-comp101-off-comp100
  :parameters ()
  :precondition (and
    (evaluate comp101)
    (not (rebooted comp101))
    (not (running comp100))
  )
  :effect (and
    (not (evaluate comp101))
    (evaluate comp102)
    (one-off comp101)
  )
)
(:action evaluate-comp102-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp102)
    (rebooted comp102)
  )
  :effect (and
    (not (evaluate comp102))
    (evaluate comp103)
  )
)
(:action evaluate-comp102-all-on
  :parameters ()
  :precondition (and
    (evaluate comp102)
    (not (rebooted comp102))
    (running comp95)
    (running comp101)
  )
  :effect (and
    (not (evaluate comp102))
    (evaluate comp103)
    (all-on comp102)
  )
)
(:action evaluate-comp102-off-comp95
  :parameters ()
  :precondition (and
    (evaluate comp102)
    (not (rebooted comp102))
    (not (running comp95))
  )
  :effect (and
    (not (evaluate comp102))
    (evaluate comp103)
    (one-off comp102)
  )
)
(:action evaluate-comp102-off-comp101
  :parameters ()
  :precondition (and
    (evaluate comp102)
    (not (rebooted comp102))
    (not (running comp101))
  )
  :effect (and
    (not (evaluate comp102))
    (evaluate comp103)
    (one-off comp102)
  )
)
(:action evaluate-comp103-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp103)
    (rebooted comp103)
  )
  :effect (and
    (not (evaluate comp103))
    (evaluate comp104)
  )
)
(:action evaluate-comp103-all-on
  :parameters ()
  :precondition (and
    (evaluate comp103)
    (not (rebooted comp103))
    (running comp102)
  )
  :effect (and
    (not (evaluate comp103))
    (evaluate comp104)
    (all-on comp103)
  )
)
(:action evaluate-comp103-off-comp102
  :parameters ()
  :precondition (and
    (evaluate comp103)
    (not (rebooted comp103))
    (not (running comp102))
  )
  :effect (and
    (not (evaluate comp103))
    (evaluate comp104)
    (one-off comp103)
  )
)
(:action evaluate-comp104-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp104)
    (rebooted comp104)
  )
  :effect (and
    (not (evaluate comp104))
    (evaluate comp105)
  )
)
(:action evaluate-comp104-all-on
  :parameters ()
  :precondition (and
    (evaluate comp104)
    (not (rebooted comp104))
    (running comp103)
  )
  :effect (and
    (not (evaluate comp104))
    (evaluate comp105)
    (all-on comp104)
  )
)
(:action evaluate-comp104-off-comp103
  :parameters ()
  :precondition (and
    (evaluate comp104)
    (not (rebooted comp104))
    (not (running comp103))
  )
  :effect (and
    (not (evaluate comp104))
    (evaluate comp105)
    (one-off comp104)
  )
)
(:action evaluate-comp105-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp105)
    (rebooted comp105)
  )
  :effect (and
    (not (evaluate comp105))
    (evaluate comp106)
  )
)
(:action evaluate-comp105-all-on
  :parameters ()
  :precondition (and
    (evaluate comp105)
    (not (rebooted comp105))
    (running comp104)
  )
  :effect (and
    (not (evaluate comp105))
    (evaluate comp106)
    (all-on comp105)
  )
)
(:action evaluate-comp105-off-comp104
  :parameters ()
  :precondition (and
    (evaluate comp105)
    (not (rebooted comp105))
    (not (running comp104))
  )
  :effect (and
    (not (evaluate comp105))
    (evaluate comp106)
    (one-off comp105)
  )
)
(:action evaluate-comp106-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp106)
    (rebooted comp106)
  )
  :effect (and
    (not (evaluate comp106))
    (evaluate comp107)
  )
)
(:action evaluate-comp106-all-on
  :parameters ()
  :precondition (and
    (evaluate comp106)
    (not (rebooted comp106))
    (running comp105)
  )
  :effect (and
    (not (evaluate comp106))
    (evaluate comp107)
    (all-on comp106)
  )
)
(:action evaluate-comp106-off-comp105
  :parameters ()
  :precondition (and
    (evaluate comp106)
    (not (rebooted comp106))
    (not (running comp105))
  )
  :effect (and
    (not (evaluate comp106))
    (evaluate comp107)
    (one-off comp106)
  )
)
(:action evaluate-comp107-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp107)
    (rebooted comp107)
  )
  :effect (and
    (not (evaluate comp107))
    (evaluate comp108)
  )
)
(:action evaluate-comp107-all-on
  :parameters ()
  :precondition (and
    (evaluate comp107)
    (not (rebooted comp107))
    (running comp106)
  )
  :effect (and
    (not (evaluate comp107))
    (evaluate comp108)
    (all-on comp107)
  )
)
(:action evaluate-comp107-off-comp106
  :parameters ()
  :precondition (and
    (evaluate comp107)
    (not (rebooted comp107))
    (not (running comp106))
  )
  :effect (and
    (not (evaluate comp107))
    (evaluate comp108)
    (one-off comp107)
  )
)
(:action evaluate-comp108-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp108)
    (rebooted comp108)
  )
  :effect (and
    (not (evaluate comp108))
    (evaluate comp109)
  )
)
(:action evaluate-comp108-all-on
  :parameters ()
  :precondition (and
    (evaluate comp108)
    (not (rebooted comp108))
    (running comp5)
    (running comp38)
    (running comp107)
  )
  :effect (and
    (not (evaluate comp108))
    (evaluate comp109)
    (all-on comp108)
  )
)
(:action evaluate-comp108-off-comp5
  :parameters ()
  :precondition (and
    (evaluate comp108)
    (not (rebooted comp108))
    (not (running comp5))
  )
  :effect (and
    (not (evaluate comp108))
    (evaluate comp109)
    (one-off comp108)
  )
)
(:action evaluate-comp108-off-comp38
  :parameters ()
  :precondition (and
    (evaluate comp108)
    (not (rebooted comp108))
    (not (running comp38))
  )
  :effect (and
    (not (evaluate comp108))
    (evaluate comp109)
    (one-off comp108)
  )
)
(:action evaluate-comp108-off-comp107
  :parameters ()
  :precondition (and
    (evaluate comp108)
    (not (rebooted comp108))
    (not (running comp107))
  )
  :effect (and
    (not (evaluate comp108))
    (evaluate comp109)
    (one-off comp108)
  )
)
(:action evaluate-comp109-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp109)
    (rebooted comp109)
  )
  :effect (and
    (not (evaluate comp109))
    (evaluate comp110)
  )
)
(:action evaluate-comp109-all-on
  :parameters ()
  :precondition (and
    (evaluate comp109)
    (not (rebooted comp109))
    (running comp88)
    (running comp108)
  )
  :effect (and
    (not (evaluate comp109))
    (evaluate comp110)
    (all-on comp109)
  )
)
(:action evaluate-comp109-off-comp88
  :parameters ()
  :precondition (and
    (evaluate comp109)
    (not (rebooted comp109))
    (not (running comp88))
  )
  :effect (and
    (not (evaluate comp109))
    (evaluate comp110)
    (one-off comp109)
  )
)
(:action evaluate-comp109-off-comp108
  :parameters ()
  :precondition (and
    (evaluate comp109)
    (not (rebooted comp109))
    (not (running comp108))
  )
  :effect (and
    (not (evaluate comp109))
    (evaluate comp110)
    (one-off comp109)
  )
)
(:action evaluate-comp110-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp110)
    (rebooted comp110)
  )
  :effect (and
    (not (evaluate comp110))
    (evaluate comp111)
  )
)
(:action evaluate-comp110-all-on
  :parameters ()
  :precondition (and
    (evaluate comp110)
    (not (rebooted comp110))
    (running comp109)
  )
  :effect (and
    (not (evaluate comp110))
    (evaluate comp111)
    (all-on comp110)
  )
)
(:action evaluate-comp110-off-comp109
  :parameters ()
  :precondition (and
    (evaluate comp110)
    (not (rebooted comp110))
    (not (running comp109))
  )
  :effect (and
    (not (evaluate comp110))
    (evaluate comp111)
    (one-off comp110)
  )
)
(:action evaluate-comp111-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp111)
    (rebooted comp111)
  )
  :effect (and
    (not (evaluate comp111))
    (evaluate comp112)
  )
)
(:action evaluate-comp111-all-on
  :parameters ()
  :precondition (and
    (evaluate comp111)
    (not (rebooted comp111))
    (running comp110)
  )
  :effect (and
    (not (evaluate comp111))
    (evaluate comp112)
    (all-on comp111)
  )
)
(:action evaluate-comp111-off-comp110
  :parameters ()
  :precondition (and
    (evaluate comp111)
    (not (rebooted comp111))
    (not (running comp110))
  )
  :effect (and
    (not (evaluate comp111))
    (evaluate comp112)
    (one-off comp111)
  )
)
(:action evaluate-comp112-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp112)
    (rebooted comp112)
  )
  :effect (and
    (not (evaluate comp112))
    (evaluate comp113)
  )
)
(:action evaluate-comp112-all-on
  :parameters ()
  :precondition (and
    (evaluate comp112)
    (not (rebooted comp112))
    (running comp77)
    (running comp111)
  )
  :effect (and
    (not (evaluate comp112))
    (evaluate comp113)
    (all-on comp112)
  )
)
(:action evaluate-comp112-off-comp77
  :parameters ()
  :precondition (and
    (evaluate comp112)
    (not (rebooted comp112))
    (not (running comp77))
  )
  :effect (and
    (not (evaluate comp112))
    (evaluate comp113)
    (one-off comp112)
  )
)
(:action evaluate-comp112-off-comp111
  :parameters ()
  :precondition (and
    (evaluate comp112)
    (not (rebooted comp112))
    (not (running comp111))
  )
  :effect (and
    (not (evaluate comp112))
    (evaluate comp113)
    (one-off comp112)
  )
)
(:action evaluate-comp113-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp113)
    (rebooted comp113)
  )
  :effect (and
    (not (evaluate comp113))
    (evaluate comp114)
  )
)
(:action evaluate-comp113-all-on
  :parameters ()
  :precondition (and
    (evaluate comp113)
    (not (rebooted comp113))
    (running comp112)
  )
  :effect (and
    (not (evaluate comp113))
    (evaluate comp114)
    (all-on comp113)
  )
)
(:action evaluate-comp113-off-comp112
  :parameters ()
  :precondition (and
    (evaluate comp113)
    (not (rebooted comp113))
    (not (running comp112))
  )
  :effect (and
    (not (evaluate comp113))
    (evaluate comp114)
    (one-off comp113)
  )
)
(:action evaluate-comp114-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp114)
    (rebooted comp114)
  )
  :effect (and
    (not (evaluate comp114))
    (evaluate comp115)
  )
)
(:action evaluate-comp114-all-on
  :parameters ()
  :precondition (and
    (evaluate comp114)
    (not (rebooted comp114))
    (running comp113)
    (running comp117)
  )
  :effect (and
    (not (evaluate comp114))
    (evaluate comp115)
    (all-on comp114)
  )
)
(:action evaluate-comp114-off-comp113
  :parameters ()
  :precondition (and
    (evaluate comp114)
    (not (rebooted comp114))
    (not (running comp113))
  )
  :effect (and
    (not (evaluate comp114))
    (evaluate comp115)
    (one-off comp114)
  )
)
(:action evaluate-comp114-off-comp117
  :parameters ()
  :precondition (and
    (evaluate comp114)
    (not (rebooted comp114))
    (not (running comp117))
  )
  :effect (and
    (not (evaluate comp114))
    (evaluate comp115)
    (one-off comp114)
  )
)
(:action evaluate-comp115-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp115)
    (rebooted comp115)
  )
  :effect (and
    (not (evaluate comp115))
    (evaluate comp116)
  )
)
(:action evaluate-comp115-all-on
  :parameters ()
  :precondition (and
    (evaluate comp115)
    (not (rebooted comp115))
    (running comp114)
  )
  :effect (and
    (not (evaluate comp115))
    (evaluate comp116)
    (all-on comp115)
  )
)
(:action evaluate-comp115-off-comp114
  :parameters ()
  :precondition (and
    (evaluate comp115)
    (not (rebooted comp115))
    (not (running comp114))
  )
  :effect (and
    (not (evaluate comp115))
    (evaluate comp116)
    (one-off comp115)
  )
)
(:action evaluate-comp116-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp116)
    (rebooted comp116)
  )
  :effect (and
    (not (evaluate comp116))
    (evaluate comp117)
  )
)
(:action evaluate-comp116-all-on
  :parameters ()
  :precondition (and
    (evaluate comp116)
    (not (rebooted comp116))
    (running comp115)
  )
  :effect (and
    (not (evaluate comp116))
    (evaluate comp117)
    (all-on comp116)
  )
)
(:action evaluate-comp116-off-comp115
  :parameters ()
  :precondition (and
    (evaluate comp116)
    (not (rebooted comp116))
    (not (running comp115))
  )
  :effect (and
    (not (evaluate comp116))
    (evaluate comp117)
    (one-off comp116)
  )
)
(:action evaluate-comp117-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp117)
    (rebooted comp117)
  )
  :effect (and
    (not (evaluate comp117))
    (evaluate comp118)
  )
)
(:action evaluate-comp117-all-on
  :parameters ()
  :precondition (and
    (evaluate comp117)
    (not (rebooted comp117))
    (running comp116)
  )
  :effect (and
    (not (evaluate comp117))
    (evaluate comp118)
    (all-on comp117)
  )
)
(:action evaluate-comp117-off-comp116
  :parameters ()
  :precondition (and
    (evaluate comp117)
    (not (rebooted comp117))
    (not (running comp116))
  )
  :effect (and
    (not (evaluate comp117))
    (evaluate comp118)
    (one-off comp117)
  )
)
(:action evaluate-comp118-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp118)
    (rebooted comp118)
  )
  :effect (and
    (not (evaluate comp118))
    (evaluate comp119)
  )
)
(:action evaluate-comp118-all-on
  :parameters ()
  :precondition (and
    (evaluate comp118)
    (not (rebooted comp118))
    (running comp25)
    (running comp117)
  )
  :effect (and
    (not (evaluate comp118))
    (evaluate comp119)
    (all-on comp118)
  )
)
(:action evaluate-comp118-off-comp25
  :parameters ()
  :precondition (and
    (evaluate comp118)
    (not (rebooted comp118))
    (not (running comp25))
  )
  :effect (and
    (not (evaluate comp118))
    (evaluate comp119)
    (one-off comp118)
  )
)
(:action evaluate-comp118-off-comp117
  :parameters ()
  :precondition (and
    (evaluate comp118)
    (not (rebooted comp118))
    (not (running comp117))
  )
  :effect (and
    (not (evaluate comp118))
    (evaluate comp119)
    (one-off comp118)
  )
)
(:action evaluate-comp119-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp119)
    (rebooted comp119)
  )
  :effect (and
    (not (evaluate comp119))
    (update-status comp0)
  )
)
(:action evaluate-comp119-all-on
  :parameters ()
  :precondition (and
    (evaluate comp119)
    (not (rebooted comp119))
    (running comp118)
  )
  :effect (and
    (not (evaluate comp119))
    (update-status comp0)
    (all-on comp119)
  )
)
(:action evaluate-comp119-off-comp118
  :parameters ()
  :precondition (and
    (evaluate comp119)
    (not (rebooted comp119))
    (not (running comp118))
  )
  :effect (and
    (not (evaluate comp119))
    (update-status comp0)
    (one-off comp119)
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
    (update-status comp25)
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
    (probabilistic 9/10 (and (running comp25)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp26)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp27)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp28)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp29)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp30)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp31)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp32)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp33)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp34)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp35)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp36)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp37)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp38)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp39)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp40)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp41)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp42)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp43)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp44)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp45)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp46)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp47)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp48)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp49)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp50)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp51)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp52)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp53)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp54)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp55)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp56)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp57)) 1/10 (and))
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
    (probabilistic 9/10 (and (running comp58)) 1/10 (and))
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
    (update-status comp60)
    (not (rebooted comp59))
    (probabilistic 9/10 (and (running comp59)) 1/10 (and))
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
    (update-status comp60)
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
    (update-status comp60)
    (not (one-off comp59))
    (probabilistic 1/4 (and (not (running comp59))))
  )
)
(:action update-status-comp60-rebooted
  :parameters ()
  :precondition (and
    (update-status comp60)
    (rebooted comp60)
  )
  :effect (and
    (not (update-status comp60))
    (update-status comp61)
    (not (rebooted comp60))
    (probabilistic 9/10 (and (running comp60)) 1/10 (and))
  )
)
(:action update-status-comp60-all-on
  :parameters ()
  :precondition (and
    (update-status comp60)
    (not (rebooted comp60))
    (all-on comp60)
  )
  :effect (and
    (not (update-status comp60))
    (update-status comp61)
    (not (all-on comp60))
    (probabilistic 1/20 (and (not (running comp60))))
  )
)
(:action update-status-comp60-one-off
  :parameters ()
  :precondition (and
    (update-status comp60)
    (not (rebooted comp60))
    (one-off comp60)
  )
  :effect (and
    (not (update-status comp60))
    (update-status comp61)
    (not (one-off comp60))
    (probabilistic 1/4 (and (not (running comp60))))
  )
)
(:action update-status-comp61-rebooted
  :parameters ()
  :precondition (and
    (update-status comp61)
    (rebooted comp61)
  )
  :effect (and
    (not (update-status comp61))
    (update-status comp62)
    (not (rebooted comp61))
    (probabilistic 9/10 (and (running comp61)) 1/10 (and))
  )
)
(:action update-status-comp61-all-on
  :parameters ()
  :precondition (and
    (update-status comp61)
    (not (rebooted comp61))
    (all-on comp61)
  )
  :effect (and
    (not (update-status comp61))
    (update-status comp62)
    (not (all-on comp61))
    (probabilistic 1/20 (and (not (running comp61))))
  )
)
(:action update-status-comp61-one-off
  :parameters ()
  :precondition (and
    (update-status comp61)
    (not (rebooted comp61))
    (one-off comp61)
  )
  :effect (and
    (not (update-status comp61))
    (update-status comp62)
    (not (one-off comp61))
    (probabilistic 1/4 (and (not (running comp61))))
  )
)
(:action update-status-comp62-rebooted
  :parameters ()
  :precondition (and
    (update-status comp62)
    (rebooted comp62)
  )
  :effect (and
    (not (update-status comp62))
    (update-status comp63)
    (not (rebooted comp62))
    (probabilistic 9/10 (and (running comp62)) 1/10 (and))
  )
)
(:action update-status-comp62-all-on
  :parameters ()
  :precondition (and
    (update-status comp62)
    (not (rebooted comp62))
    (all-on comp62)
  )
  :effect (and
    (not (update-status comp62))
    (update-status comp63)
    (not (all-on comp62))
    (probabilistic 1/20 (and (not (running comp62))))
  )
)
(:action update-status-comp62-one-off
  :parameters ()
  :precondition (and
    (update-status comp62)
    (not (rebooted comp62))
    (one-off comp62)
  )
  :effect (and
    (not (update-status comp62))
    (update-status comp63)
    (not (one-off comp62))
    (probabilistic 1/4 (and (not (running comp62))))
  )
)
(:action update-status-comp63-rebooted
  :parameters ()
  :precondition (and
    (update-status comp63)
    (rebooted comp63)
  )
  :effect (and
    (not (update-status comp63))
    (update-status comp64)
    (not (rebooted comp63))
    (probabilistic 9/10 (and (running comp63)) 1/10 (and))
  )
)
(:action update-status-comp63-all-on
  :parameters ()
  :precondition (and
    (update-status comp63)
    (not (rebooted comp63))
    (all-on comp63)
  )
  :effect (and
    (not (update-status comp63))
    (update-status comp64)
    (not (all-on comp63))
    (probabilistic 1/20 (and (not (running comp63))))
  )
)
(:action update-status-comp63-one-off
  :parameters ()
  :precondition (and
    (update-status comp63)
    (not (rebooted comp63))
    (one-off comp63)
  )
  :effect (and
    (not (update-status comp63))
    (update-status comp64)
    (not (one-off comp63))
    (probabilistic 1/4 (and (not (running comp63))))
  )
)
(:action update-status-comp64-rebooted
  :parameters ()
  :precondition (and
    (update-status comp64)
    (rebooted comp64)
  )
  :effect (and
    (not (update-status comp64))
    (update-status comp65)
    (not (rebooted comp64))
    (probabilistic 9/10 (and (running comp64)) 1/10 (and))
  )
)
(:action update-status-comp64-all-on
  :parameters ()
  :precondition (and
    (update-status comp64)
    (not (rebooted comp64))
    (all-on comp64)
  )
  :effect (and
    (not (update-status comp64))
    (update-status comp65)
    (not (all-on comp64))
    (probabilistic 1/20 (and (not (running comp64))))
  )
)
(:action update-status-comp64-one-off
  :parameters ()
  :precondition (and
    (update-status comp64)
    (not (rebooted comp64))
    (one-off comp64)
  )
  :effect (and
    (not (update-status comp64))
    (update-status comp65)
    (not (one-off comp64))
    (probabilistic 1/4 (and (not (running comp64))))
  )
)
(:action update-status-comp65-rebooted
  :parameters ()
  :precondition (and
    (update-status comp65)
    (rebooted comp65)
  )
  :effect (and
    (not (update-status comp65))
    (update-status comp66)
    (not (rebooted comp65))
    (probabilistic 9/10 (and (running comp65)) 1/10 (and))
  )
)
(:action update-status-comp65-all-on
  :parameters ()
  :precondition (and
    (update-status comp65)
    (not (rebooted comp65))
    (all-on comp65)
  )
  :effect (and
    (not (update-status comp65))
    (update-status comp66)
    (not (all-on comp65))
    (probabilistic 1/20 (and (not (running comp65))))
  )
)
(:action update-status-comp65-one-off
  :parameters ()
  :precondition (and
    (update-status comp65)
    (not (rebooted comp65))
    (one-off comp65)
  )
  :effect (and
    (not (update-status comp65))
    (update-status comp66)
    (not (one-off comp65))
    (probabilistic 1/4 (and (not (running comp65))))
  )
)
(:action update-status-comp66-rebooted
  :parameters ()
  :precondition (and
    (update-status comp66)
    (rebooted comp66)
  )
  :effect (and
    (not (update-status comp66))
    (update-status comp67)
    (not (rebooted comp66))
    (probabilistic 9/10 (and (running comp66)) 1/10 (and))
  )
)
(:action update-status-comp66-all-on
  :parameters ()
  :precondition (and
    (update-status comp66)
    (not (rebooted comp66))
    (all-on comp66)
  )
  :effect (and
    (not (update-status comp66))
    (update-status comp67)
    (not (all-on comp66))
    (probabilistic 1/20 (and (not (running comp66))))
  )
)
(:action update-status-comp66-one-off
  :parameters ()
  :precondition (and
    (update-status comp66)
    (not (rebooted comp66))
    (one-off comp66)
  )
  :effect (and
    (not (update-status comp66))
    (update-status comp67)
    (not (one-off comp66))
    (probabilistic 1/4 (and (not (running comp66))))
  )
)
(:action update-status-comp67-rebooted
  :parameters ()
  :precondition (and
    (update-status comp67)
    (rebooted comp67)
  )
  :effect (and
    (not (update-status comp67))
    (update-status comp68)
    (not (rebooted comp67))
    (probabilistic 9/10 (and (running comp67)) 1/10 (and))
  )
)
(:action update-status-comp67-all-on
  :parameters ()
  :precondition (and
    (update-status comp67)
    (not (rebooted comp67))
    (all-on comp67)
  )
  :effect (and
    (not (update-status comp67))
    (update-status comp68)
    (not (all-on comp67))
    (probabilistic 1/20 (and (not (running comp67))))
  )
)
(:action update-status-comp67-one-off
  :parameters ()
  :precondition (and
    (update-status comp67)
    (not (rebooted comp67))
    (one-off comp67)
  )
  :effect (and
    (not (update-status comp67))
    (update-status comp68)
    (not (one-off comp67))
    (probabilistic 1/4 (and (not (running comp67))))
  )
)
(:action update-status-comp68-rebooted
  :parameters ()
  :precondition (and
    (update-status comp68)
    (rebooted comp68)
  )
  :effect (and
    (not (update-status comp68))
    (update-status comp69)
    (not (rebooted comp68))
    (probabilistic 9/10 (and (running comp68)) 1/10 (and))
  )
)
(:action update-status-comp68-all-on
  :parameters ()
  :precondition (and
    (update-status comp68)
    (not (rebooted comp68))
    (all-on comp68)
  )
  :effect (and
    (not (update-status comp68))
    (update-status comp69)
    (not (all-on comp68))
    (probabilistic 1/20 (and (not (running comp68))))
  )
)
(:action update-status-comp68-one-off
  :parameters ()
  :precondition (and
    (update-status comp68)
    (not (rebooted comp68))
    (one-off comp68)
  )
  :effect (and
    (not (update-status comp68))
    (update-status comp69)
    (not (one-off comp68))
    (probabilistic 1/4 (and (not (running comp68))))
  )
)
(:action update-status-comp69-rebooted
  :parameters ()
  :precondition (and
    (update-status comp69)
    (rebooted comp69)
  )
  :effect (and
    (not (update-status comp69))
    (update-status comp70)
    (not (rebooted comp69))
    (probabilistic 9/10 (and (running comp69)) 1/10 (and))
  )
)
(:action update-status-comp69-all-on
  :parameters ()
  :precondition (and
    (update-status comp69)
    (not (rebooted comp69))
    (all-on comp69)
  )
  :effect (and
    (not (update-status comp69))
    (update-status comp70)
    (not (all-on comp69))
    (probabilistic 1/20 (and (not (running comp69))))
  )
)
(:action update-status-comp69-one-off
  :parameters ()
  :precondition (and
    (update-status comp69)
    (not (rebooted comp69))
    (one-off comp69)
  )
  :effect (and
    (not (update-status comp69))
    (update-status comp70)
    (not (one-off comp69))
    (probabilistic 1/4 (and (not (running comp69))))
  )
)
(:action update-status-comp70-rebooted
  :parameters ()
  :precondition (and
    (update-status comp70)
    (rebooted comp70)
  )
  :effect (and
    (not (update-status comp70))
    (update-status comp71)
    (not (rebooted comp70))
    (probabilistic 9/10 (and (running comp70)) 1/10 (and))
  )
)
(:action update-status-comp70-all-on
  :parameters ()
  :precondition (and
    (update-status comp70)
    (not (rebooted comp70))
    (all-on comp70)
  )
  :effect (and
    (not (update-status comp70))
    (update-status comp71)
    (not (all-on comp70))
    (probabilistic 1/20 (and (not (running comp70))))
  )
)
(:action update-status-comp70-one-off
  :parameters ()
  :precondition (and
    (update-status comp70)
    (not (rebooted comp70))
    (one-off comp70)
  )
  :effect (and
    (not (update-status comp70))
    (update-status comp71)
    (not (one-off comp70))
    (probabilistic 1/4 (and (not (running comp70))))
  )
)
(:action update-status-comp71-rebooted
  :parameters ()
  :precondition (and
    (update-status comp71)
    (rebooted comp71)
  )
  :effect (and
    (not (update-status comp71))
    (update-status comp72)
    (not (rebooted comp71))
    (probabilistic 9/10 (and (running comp71)) 1/10 (and))
  )
)
(:action update-status-comp71-all-on
  :parameters ()
  :precondition (and
    (update-status comp71)
    (not (rebooted comp71))
    (all-on comp71)
  )
  :effect (and
    (not (update-status comp71))
    (update-status comp72)
    (not (all-on comp71))
    (probabilistic 1/20 (and (not (running comp71))))
  )
)
(:action update-status-comp71-one-off
  :parameters ()
  :precondition (and
    (update-status comp71)
    (not (rebooted comp71))
    (one-off comp71)
  )
  :effect (and
    (not (update-status comp71))
    (update-status comp72)
    (not (one-off comp71))
    (probabilistic 1/4 (and (not (running comp71))))
  )
)
(:action update-status-comp72-rebooted
  :parameters ()
  :precondition (and
    (update-status comp72)
    (rebooted comp72)
  )
  :effect (and
    (not (update-status comp72))
    (update-status comp73)
    (not (rebooted comp72))
    (probabilistic 9/10 (and (running comp72)) 1/10 (and))
  )
)
(:action update-status-comp72-all-on
  :parameters ()
  :precondition (and
    (update-status comp72)
    (not (rebooted comp72))
    (all-on comp72)
  )
  :effect (and
    (not (update-status comp72))
    (update-status comp73)
    (not (all-on comp72))
    (probabilistic 1/20 (and (not (running comp72))))
  )
)
(:action update-status-comp72-one-off
  :parameters ()
  :precondition (and
    (update-status comp72)
    (not (rebooted comp72))
    (one-off comp72)
  )
  :effect (and
    (not (update-status comp72))
    (update-status comp73)
    (not (one-off comp72))
    (probabilistic 1/4 (and (not (running comp72))))
  )
)
(:action update-status-comp73-rebooted
  :parameters ()
  :precondition (and
    (update-status comp73)
    (rebooted comp73)
  )
  :effect (and
    (not (update-status comp73))
    (update-status comp74)
    (not (rebooted comp73))
    (probabilistic 9/10 (and (running comp73)) 1/10 (and))
  )
)
(:action update-status-comp73-all-on
  :parameters ()
  :precondition (and
    (update-status comp73)
    (not (rebooted comp73))
    (all-on comp73)
  )
  :effect (and
    (not (update-status comp73))
    (update-status comp74)
    (not (all-on comp73))
    (probabilistic 1/20 (and (not (running comp73))))
  )
)
(:action update-status-comp73-one-off
  :parameters ()
  :precondition (and
    (update-status comp73)
    (not (rebooted comp73))
    (one-off comp73)
  )
  :effect (and
    (not (update-status comp73))
    (update-status comp74)
    (not (one-off comp73))
    (probabilistic 1/4 (and (not (running comp73))))
  )
)
(:action update-status-comp74-rebooted
  :parameters ()
  :precondition (and
    (update-status comp74)
    (rebooted comp74)
  )
  :effect (and
    (not (update-status comp74))
    (update-status comp75)
    (not (rebooted comp74))
    (probabilistic 9/10 (and (running comp74)) 1/10 (and))
  )
)
(:action update-status-comp74-all-on
  :parameters ()
  :precondition (and
    (update-status comp74)
    (not (rebooted comp74))
    (all-on comp74)
  )
  :effect (and
    (not (update-status comp74))
    (update-status comp75)
    (not (all-on comp74))
    (probabilistic 1/20 (and (not (running comp74))))
  )
)
(:action update-status-comp74-one-off
  :parameters ()
  :precondition (and
    (update-status comp74)
    (not (rebooted comp74))
    (one-off comp74)
  )
  :effect (and
    (not (update-status comp74))
    (update-status comp75)
    (not (one-off comp74))
    (probabilistic 1/4 (and (not (running comp74))))
  )
)
(:action update-status-comp75-rebooted
  :parameters ()
  :precondition (and
    (update-status comp75)
    (rebooted comp75)
  )
  :effect (and
    (not (update-status comp75))
    (update-status comp76)
    (not (rebooted comp75))
    (probabilistic 9/10 (and (running comp75)) 1/10 (and))
  )
)
(:action update-status-comp75-all-on
  :parameters ()
  :precondition (and
    (update-status comp75)
    (not (rebooted comp75))
    (all-on comp75)
  )
  :effect (and
    (not (update-status comp75))
    (update-status comp76)
    (not (all-on comp75))
    (probabilistic 1/20 (and (not (running comp75))))
  )
)
(:action update-status-comp75-one-off
  :parameters ()
  :precondition (and
    (update-status comp75)
    (not (rebooted comp75))
    (one-off comp75)
  )
  :effect (and
    (not (update-status comp75))
    (update-status comp76)
    (not (one-off comp75))
    (probabilistic 1/4 (and (not (running comp75))))
  )
)
(:action update-status-comp76-rebooted
  :parameters ()
  :precondition (and
    (update-status comp76)
    (rebooted comp76)
  )
  :effect (and
    (not (update-status comp76))
    (update-status comp77)
    (not (rebooted comp76))
    (probabilistic 9/10 (and (running comp76)) 1/10 (and))
  )
)
(:action update-status-comp76-all-on
  :parameters ()
  :precondition (and
    (update-status comp76)
    (not (rebooted comp76))
    (all-on comp76)
  )
  :effect (and
    (not (update-status comp76))
    (update-status comp77)
    (not (all-on comp76))
    (probabilistic 1/20 (and (not (running comp76))))
  )
)
(:action update-status-comp76-one-off
  :parameters ()
  :precondition (and
    (update-status comp76)
    (not (rebooted comp76))
    (one-off comp76)
  )
  :effect (and
    (not (update-status comp76))
    (update-status comp77)
    (not (one-off comp76))
    (probabilistic 1/4 (and (not (running comp76))))
  )
)
(:action update-status-comp77-rebooted
  :parameters ()
  :precondition (and
    (update-status comp77)
    (rebooted comp77)
  )
  :effect (and
    (not (update-status comp77))
    (update-status comp78)
    (not (rebooted comp77))
    (probabilistic 9/10 (and (running comp77)) 1/10 (and))
  )
)
(:action update-status-comp77-all-on
  :parameters ()
  :precondition (and
    (update-status comp77)
    (not (rebooted comp77))
    (all-on comp77)
  )
  :effect (and
    (not (update-status comp77))
    (update-status comp78)
    (not (all-on comp77))
    (probabilistic 1/20 (and (not (running comp77))))
  )
)
(:action update-status-comp77-one-off
  :parameters ()
  :precondition (and
    (update-status comp77)
    (not (rebooted comp77))
    (one-off comp77)
  )
  :effect (and
    (not (update-status comp77))
    (update-status comp78)
    (not (one-off comp77))
    (probabilistic 1/4 (and (not (running comp77))))
  )
)
(:action update-status-comp78-rebooted
  :parameters ()
  :precondition (and
    (update-status comp78)
    (rebooted comp78)
  )
  :effect (and
    (not (update-status comp78))
    (update-status comp79)
    (not (rebooted comp78))
    (probabilistic 9/10 (and (running comp78)) 1/10 (and))
  )
)
(:action update-status-comp78-all-on
  :parameters ()
  :precondition (and
    (update-status comp78)
    (not (rebooted comp78))
    (all-on comp78)
  )
  :effect (and
    (not (update-status comp78))
    (update-status comp79)
    (not (all-on comp78))
    (probabilistic 1/20 (and (not (running comp78))))
  )
)
(:action update-status-comp78-one-off
  :parameters ()
  :precondition (and
    (update-status comp78)
    (not (rebooted comp78))
    (one-off comp78)
  )
  :effect (and
    (not (update-status comp78))
    (update-status comp79)
    (not (one-off comp78))
    (probabilistic 1/4 (and (not (running comp78))))
  )
)
(:action update-status-comp79-rebooted
  :parameters ()
  :precondition (and
    (update-status comp79)
    (rebooted comp79)
  )
  :effect (and
    (not (update-status comp79))
    (update-status comp80)
    (not (rebooted comp79))
    (probabilistic 9/10 (and (running comp79)) 1/10 (and))
  )
)
(:action update-status-comp79-all-on
  :parameters ()
  :precondition (and
    (update-status comp79)
    (not (rebooted comp79))
    (all-on comp79)
  )
  :effect (and
    (not (update-status comp79))
    (update-status comp80)
    (not (all-on comp79))
    (probabilistic 1/20 (and (not (running comp79))))
  )
)
(:action update-status-comp79-one-off
  :parameters ()
  :precondition (and
    (update-status comp79)
    (not (rebooted comp79))
    (one-off comp79)
  )
  :effect (and
    (not (update-status comp79))
    (update-status comp80)
    (not (one-off comp79))
    (probabilistic 1/4 (and (not (running comp79))))
  )
)
(:action update-status-comp80-rebooted
  :parameters ()
  :precondition (and
    (update-status comp80)
    (rebooted comp80)
  )
  :effect (and
    (not (update-status comp80))
    (update-status comp81)
    (not (rebooted comp80))
    (probabilistic 9/10 (and (running comp80)) 1/10 (and))
  )
)
(:action update-status-comp80-all-on
  :parameters ()
  :precondition (and
    (update-status comp80)
    (not (rebooted comp80))
    (all-on comp80)
  )
  :effect (and
    (not (update-status comp80))
    (update-status comp81)
    (not (all-on comp80))
    (probabilistic 1/20 (and (not (running comp80))))
  )
)
(:action update-status-comp80-one-off
  :parameters ()
  :precondition (and
    (update-status comp80)
    (not (rebooted comp80))
    (one-off comp80)
  )
  :effect (and
    (not (update-status comp80))
    (update-status comp81)
    (not (one-off comp80))
    (probabilistic 1/4 (and (not (running comp80))))
  )
)
(:action update-status-comp81-rebooted
  :parameters ()
  :precondition (and
    (update-status comp81)
    (rebooted comp81)
  )
  :effect (and
    (not (update-status comp81))
    (update-status comp82)
    (not (rebooted comp81))
    (probabilistic 9/10 (and (running comp81)) 1/10 (and))
  )
)
(:action update-status-comp81-all-on
  :parameters ()
  :precondition (and
    (update-status comp81)
    (not (rebooted comp81))
    (all-on comp81)
  )
  :effect (and
    (not (update-status comp81))
    (update-status comp82)
    (not (all-on comp81))
    (probabilistic 1/20 (and (not (running comp81))))
  )
)
(:action update-status-comp81-one-off
  :parameters ()
  :precondition (and
    (update-status comp81)
    (not (rebooted comp81))
    (one-off comp81)
  )
  :effect (and
    (not (update-status comp81))
    (update-status comp82)
    (not (one-off comp81))
    (probabilistic 1/4 (and (not (running comp81))))
  )
)
(:action update-status-comp82-rebooted
  :parameters ()
  :precondition (and
    (update-status comp82)
    (rebooted comp82)
  )
  :effect (and
    (not (update-status comp82))
    (update-status comp83)
    (not (rebooted comp82))
    (probabilistic 9/10 (and (running comp82)) 1/10 (and))
  )
)
(:action update-status-comp82-all-on
  :parameters ()
  :precondition (and
    (update-status comp82)
    (not (rebooted comp82))
    (all-on comp82)
  )
  :effect (and
    (not (update-status comp82))
    (update-status comp83)
    (not (all-on comp82))
    (probabilistic 1/20 (and (not (running comp82))))
  )
)
(:action update-status-comp82-one-off
  :parameters ()
  :precondition (and
    (update-status comp82)
    (not (rebooted comp82))
    (one-off comp82)
  )
  :effect (and
    (not (update-status comp82))
    (update-status comp83)
    (not (one-off comp82))
    (probabilistic 1/4 (and (not (running comp82))))
  )
)
(:action update-status-comp83-rebooted
  :parameters ()
  :precondition (and
    (update-status comp83)
    (rebooted comp83)
  )
  :effect (and
    (not (update-status comp83))
    (update-status comp84)
    (not (rebooted comp83))
    (probabilistic 9/10 (and (running comp83)) 1/10 (and))
  )
)
(:action update-status-comp83-all-on
  :parameters ()
  :precondition (and
    (update-status comp83)
    (not (rebooted comp83))
    (all-on comp83)
  )
  :effect (and
    (not (update-status comp83))
    (update-status comp84)
    (not (all-on comp83))
    (probabilistic 1/20 (and (not (running comp83))))
  )
)
(:action update-status-comp83-one-off
  :parameters ()
  :precondition (and
    (update-status comp83)
    (not (rebooted comp83))
    (one-off comp83)
  )
  :effect (and
    (not (update-status comp83))
    (update-status comp84)
    (not (one-off comp83))
    (probabilistic 1/4 (and (not (running comp83))))
  )
)
(:action update-status-comp84-rebooted
  :parameters ()
  :precondition (and
    (update-status comp84)
    (rebooted comp84)
  )
  :effect (and
    (not (update-status comp84))
    (update-status comp85)
    (not (rebooted comp84))
    (probabilistic 9/10 (and (running comp84)) 1/10 (and))
  )
)
(:action update-status-comp84-all-on
  :parameters ()
  :precondition (and
    (update-status comp84)
    (not (rebooted comp84))
    (all-on comp84)
  )
  :effect (and
    (not (update-status comp84))
    (update-status comp85)
    (not (all-on comp84))
    (probabilistic 1/20 (and (not (running comp84))))
  )
)
(:action update-status-comp84-one-off
  :parameters ()
  :precondition (and
    (update-status comp84)
    (not (rebooted comp84))
    (one-off comp84)
  )
  :effect (and
    (not (update-status comp84))
    (update-status comp85)
    (not (one-off comp84))
    (probabilistic 1/4 (and (not (running comp84))))
  )
)
(:action update-status-comp85-rebooted
  :parameters ()
  :precondition (and
    (update-status comp85)
    (rebooted comp85)
  )
  :effect (and
    (not (update-status comp85))
    (update-status comp86)
    (not (rebooted comp85))
    (probabilistic 9/10 (and (running comp85)) 1/10 (and))
  )
)
(:action update-status-comp85-all-on
  :parameters ()
  :precondition (and
    (update-status comp85)
    (not (rebooted comp85))
    (all-on comp85)
  )
  :effect (and
    (not (update-status comp85))
    (update-status comp86)
    (not (all-on comp85))
    (probabilistic 1/20 (and (not (running comp85))))
  )
)
(:action update-status-comp85-one-off
  :parameters ()
  :precondition (and
    (update-status comp85)
    (not (rebooted comp85))
    (one-off comp85)
  )
  :effect (and
    (not (update-status comp85))
    (update-status comp86)
    (not (one-off comp85))
    (probabilistic 1/4 (and (not (running comp85))))
  )
)
(:action update-status-comp86-rebooted
  :parameters ()
  :precondition (and
    (update-status comp86)
    (rebooted comp86)
  )
  :effect (and
    (not (update-status comp86))
    (update-status comp87)
    (not (rebooted comp86))
    (probabilistic 9/10 (and (running comp86)) 1/10 (and))
  )
)
(:action update-status-comp86-all-on
  :parameters ()
  :precondition (and
    (update-status comp86)
    (not (rebooted comp86))
    (all-on comp86)
  )
  :effect (and
    (not (update-status comp86))
    (update-status comp87)
    (not (all-on comp86))
    (probabilistic 1/20 (and (not (running comp86))))
  )
)
(:action update-status-comp86-one-off
  :parameters ()
  :precondition (and
    (update-status comp86)
    (not (rebooted comp86))
    (one-off comp86)
  )
  :effect (and
    (not (update-status comp86))
    (update-status comp87)
    (not (one-off comp86))
    (probabilistic 1/4 (and (not (running comp86))))
  )
)
(:action update-status-comp87-rebooted
  :parameters ()
  :precondition (and
    (update-status comp87)
    (rebooted comp87)
  )
  :effect (and
    (not (update-status comp87))
    (update-status comp88)
    (not (rebooted comp87))
    (probabilistic 9/10 (and (running comp87)) 1/10 (and))
  )
)
(:action update-status-comp87-all-on
  :parameters ()
  :precondition (and
    (update-status comp87)
    (not (rebooted comp87))
    (all-on comp87)
  )
  :effect (and
    (not (update-status comp87))
    (update-status comp88)
    (not (all-on comp87))
    (probabilistic 1/20 (and (not (running comp87))))
  )
)
(:action update-status-comp87-one-off
  :parameters ()
  :precondition (and
    (update-status comp87)
    (not (rebooted comp87))
    (one-off comp87)
  )
  :effect (and
    (not (update-status comp87))
    (update-status comp88)
    (not (one-off comp87))
    (probabilistic 1/4 (and (not (running comp87))))
  )
)
(:action update-status-comp88-rebooted
  :parameters ()
  :precondition (and
    (update-status comp88)
    (rebooted comp88)
  )
  :effect (and
    (not (update-status comp88))
    (update-status comp89)
    (not (rebooted comp88))
    (probabilistic 9/10 (and (running comp88)) 1/10 (and))
  )
)
(:action update-status-comp88-all-on
  :parameters ()
  :precondition (and
    (update-status comp88)
    (not (rebooted comp88))
    (all-on comp88)
  )
  :effect (and
    (not (update-status comp88))
    (update-status comp89)
    (not (all-on comp88))
    (probabilistic 1/20 (and (not (running comp88))))
  )
)
(:action update-status-comp88-one-off
  :parameters ()
  :precondition (and
    (update-status comp88)
    (not (rebooted comp88))
    (one-off comp88)
  )
  :effect (and
    (not (update-status comp88))
    (update-status comp89)
    (not (one-off comp88))
    (probabilistic 1/4 (and (not (running comp88))))
  )
)
(:action update-status-comp89-rebooted
  :parameters ()
  :precondition (and
    (update-status comp89)
    (rebooted comp89)
  )
  :effect (and
    (not (update-status comp89))
    (update-status comp90)
    (not (rebooted comp89))
    (probabilistic 9/10 (and (running comp89)) 1/10 (and))
  )
)
(:action update-status-comp89-all-on
  :parameters ()
  :precondition (and
    (update-status comp89)
    (not (rebooted comp89))
    (all-on comp89)
  )
  :effect (and
    (not (update-status comp89))
    (update-status comp90)
    (not (all-on comp89))
    (probabilistic 1/20 (and (not (running comp89))))
  )
)
(:action update-status-comp89-one-off
  :parameters ()
  :precondition (and
    (update-status comp89)
    (not (rebooted comp89))
    (one-off comp89)
  )
  :effect (and
    (not (update-status comp89))
    (update-status comp90)
    (not (one-off comp89))
    (probabilistic 1/4 (and (not (running comp89))))
  )
)
(:action update-status-comp90-rebooted
  :parameters ()
  :precondition (and
    (update-status comp90)
    (rebooted comp90)
  )
  :effect (and
    (not (update-status comp90))
    (update-status comp91)
    (not (rebooted comp90))
    (probabilistic 9/10 (and (running comp90)) 1/10 (and))
  )
)
(:action update-status-comp90-all-on
  :parameters ()
  :precondition (and
    (update-status comp90)
    (not (rebooted comp90))
    (all-on comp90)
  )
  :effect (and
    (not (update-status comp90))
    (update-status comp91)
    (not (all-on comp90))
    (probabilistic 1/20 (and (not (running comp90))))
  )
)
(:action update-status-comp90-one-off
  :parameters ()
  :precondition (and
    (update-status comp90)
    (not (rebooted comp90))
    (one-off comp90)
  )
  :effect (and
    (not (update-status comp90))
    (update-status comp91)
    (not (one-off comp90))
    (probabilistic 1/4 (and (not (running comp90))))
  )
)
(:action update-status-comp91-rebooted
  :parameters ()
  :precondition (and
    (update-status comp91)
    (rebooted comp91)
  )
  :effect (and
    (not (update-status comp91))
    (update-status comp92)
    (not (rebooted comp91))
    (probabilistic 9/10 (and (running comp91)) 1/10 (and))
  )
)
(:action update-status-comp91-all-on
  :parameters ()
  :precondition (and
    (update-status comp91)
    (not (rebooted comp91))
    (all-on comp91)
  )
  :effect (and
    (not (update-status comp91))
    (update-status comp92)
    (not (all-on comp91))
    (probabilistic 1/20 (and (not (running comp91))))
  )
)
(:action update-status-comp91-one-off
  :parameters ()
  :precondition (and
    (update-status comp91)
    (not (rebooted comp91))
    (one-off comp91)
  )
  :effect (and
    (not (update-status comp91))
    (update-status comp92)
    (not (one-off comp91))
    (probabilistic 1/4 (and (not (running comp91))))
  )
)
(:action update-status-comp92-rebooted
  :parameters ()
  :precondition (and
    (update-status comp92)
    (rebooted comp92)
  )
  :effect (and
    (not (update-status comp92))
    (update-status comp93)
    (not (rebooted comp92))
    (probabilistic 9/10 (and (running comp92)) 1/10 (and))
  )
)
(:action update-status-comp92-all-on
  :parameters ()
  :precondition (and
    (update-status comp92)
    (not (rebooted comp92))
    (all-on comp92)
  )
  :effect (and
    (not (update-status comp92))
    (update-status comp93)
    (not (all-on comp92))
    (probabilistic 1/20 (and (not (running comp92))))
  )
)
(:action update-status-comp92-one-off
  :parameters ()
  :precondition (and
    (update-status comp92)
    (not (rebooted comp92))
    (one-off comp92)
  )
  :effect (and
    (not (update-status comp92))
    (update-status comp93)
    (not (one-off comp92))
    (probabilistic 1/4 (and (not (running comp92))))
  )
)
(:action update-status-comp93-rebooted
  :parameters ()
  :precondition (and
    (update-status comp93)
    (rebooted comp93)
  )
  :effect (and
    (not (update-status comp93))
    (update-status comp94)
    (not (rebooted comp93))
    (probabilistic 9/10 (and (running comp93)) 1/10 (and))
  )
)
(:action update-status-comp93-all-on
  :parameters ()
  :precondition (and
    (update-status comp93)
    (not (rebooted comp93))
    (all-on comp93)
  )
  :effect (and
    (not (update-status comp93))
    (update-status comp94)
    (not (all-on comp93))
    (probabilistic 1/20 (and (not (running comp93))))
  )
)
(:action update-status-comp93-one-off
  :parameters ()
  :precondition (and
    (update-status comp93)
    (not (rebooted comp93))
    (one-off comp93)
  )
  :effect (and
    (not (update-status comp93))
    (update-status comp94)
    (not (one-off comp93))
    (probabilistic 1/4 (and (not (running comp93))))
  )
)
(:action update-status-comp94-rebooted
  :parameters ()
  :precondition (and
    (update-status comp94)
    (rebooted comp94)
  )
  :effect (and
    (not (update-status comp94))
    (update-status comp95)
    (not (rebooted comp94))
    (probabilistic 9/10 (and (running comp94)) 1/10 (and))
  )
)
(:action update-status-comp94-all-on
  :parameters ()
  :precondition (and
    (update-status comp94)
    (not (rebooted comp94))
    (all-on comp94)
  )
  :effect (and
    (not (update-status comp94))
    (update-status comp95)
    (not (all-on comp94))
    (probabilistic 1/20 (and (not (running comp94))))
  )
)
(:action update-status-comp94-one-off
  :parameters ()
  :precondition (and
    (update-status comp94)
    (not (rebooted comp94))
    (one-off comp94)
  )
  :effect (and
    (not (update-status comp94))
    (update-status comp95)
    (not (one-off comp94))
    (probabilistic 1/4 (and (not (running comp94))))
  )
)
(:action update-status-comp95-rebooted
  :parameters ()
  :precondition (and
    (update-status comp95)
    (rebooted comp95)
  )
  :effect (and
    (not (update-status comp95))
    (update-status comp96)
    (not (rebooted comp95))
    (probabilistic 9/10 (and (running comp95)) 1/10 (and))
  )
)
(:action update-status-comp95-all-on
  :parameters ()
  :precondition (and
    (update-status comp95)
    (not (rebooted comp95))
    (all-on comp95)
  )
  :effect (and
    (not (update-status comp95))
    (update-status comp96)
    (not (all-on comp95))
    (probabilistic 1/20 (and (not (running comp95))))
  )
)
(:action update-status-comp95-one-off
  :parameters ()
  :precondition (and
    (update-status comp95)
    (not (rebooted comp95))
    (one-off comp95)
  )
  :effect (and
    (not (update-status comp95))
    (update-status comp96)
    (not (one-off comp95))
    (probabilistic 1/4 (and (not (running comp95))))
  )
)
(:action update-status-comp96-rebooted
  :parameters ()
  :precondition (and
    (update-status comp96)
    (rebooted comp96)
  )
  :effect (and
    (not (update-status comp96))
    (update-status comp97)
    (not (rebooted comp96))
    (probabilistic 9/10 (and (running comp96)) 1/10 (and))
  )
)
(:action update-status-comp96-all-on
  :parameters ()
  :precondition (and
    (update-status comp96)
    (not (rebooted comp96))
    (all-on comp96)
  )
  :effect (and
    (not (update-status comp96))
    (update-status comp97)
    (not (all-on comp96))
    (probabilistic 1/20 (and (not (running comp96))))
  )
)
(:action update-status-comp96-one-off
  :parameters ()
  :precondition (and
    (update-status comp96)
    (not (rebooted comp96))
    (one-off comp96)
  )
  :effect (and
    (not (update-status comp96))
    (update-status comp97)
    (not (one-off comp96))
    (probabilistic 1/4 (and (not (running comp96))))
  )
)
(:action update-status-comp97-rebooted
  :parameters ()
  :precondition (and
    (update-status comp97)
    (rebooted comp97)
  )
  :effect (and
    (not (update-status comp97))
    (update-status comp98)
    (not (rebooted comp97))
    (probabilistic 9/10 (and (running comp97)) 1/10 (and))
  )
)
(:action update-status-comp97-all-on
  :parameters ()
  :precondition (and
    (update-status comp97)
    (not (rebooted comp97))
    (all-on comp97)
  )
  :effect (and
    (not (update-status comp97))
    (update-status comp98)
    (not (all-on comp97))
    (probabilistic 1/20 (and (not (running comp97))))
  )
)
(:action update-status-comp97-one-off
  :parameters ()
  :precondition (and
    (update-status comp97)
    (not (rebooted comp97))
    (one-off comp97)
  )
  :effect (and
    (not (update-status comp97))
    (update-status comp98)
    (not (one-off comp97))
    (probabilistic 1/4 (and (not (running comp97))))
  )
)
(:action update-status-comp98-rebooted
  :parameters ()
  :precondition (and
    (update-status comp98)
    (rebooted comp98)
  )
  :effect (and
    (not (update-status comp98))
    (update-status comp99)
    (not (rebooted comp98))
    (probabilistic 9/10 (and (running comp98)) 1/10 (and))
  )
)
(:action update-status-comp98-all-on
  :parameters ()
  :precondition (and
    (update-status comp98)
    (not (rebooted comp98))
    (all-on comp98)
  )
  :effect (and
    (not (update-status comp98))
    (update-status comp99)
    (not (all-on comp98))
    (probabilistic 1/20 (and (not (running comp98))))
  )
)
(:action update-status-comp98-one-off
  :parameters ()
  :precondition (and
    (update-status comp98)
    (not (rebooted comp98))
    (one-off comp98)
  )
  :effect (and
    (not (update-status comp98))
    (update-status comp99)
    (not (one-off comp98))
    (probabilistic 1/4 (and (not (running comp98))))
  )
)
(:action update-status-comp99-rebooted
  :parameters ()
  :precondition (and
    (update-status comp99)
    (rebooted comp99)
  )
  :effect (and
    (not (update-status comp99))
    (update-status comp100)
    (not (rebooted comp99))
    (probabilistic 9/10 (and (running comp99)) 1/10 (and))
  )
)
(:action update-status-comp99-all-on
  :parameters ()
  :precondition (and
    (update-status comp99)
    (not (rebooted comp99))
    (all-on comp99)
  )
  :effect (and
    (not (update-status comp99))
    (update-status comp100)
    (not (all-on comp99))
    (probabilistic 1/20 (and (not (running comp99))))
  )
)
(:action update-status-comp99-one-off
  :parameters ()
  :precondition (and
    (update-status comp99)
    (not (rebooted comp99))
    (one-off comp99)
  )
  :effect (and
    (not (update-status comp99))
    (update-status comp100)
    (not (one-off comp99))
    (probabilistic 1/4 (and (not (running comp99))))
  )
)
(:action update-status-comp100-rebooted
  :parameters ()
  :precondition (and
    (update-status comp100)
    (rebooted comp100)
  )
  :effect (and
    (not (update-status comp100))
    (update-status comp101)
    (not (rebooted comp100))
    (probabilistic 9/10 (and (running comp100)) 1/10 (and))
  )
)
(:action update-status-comp100-all-on
  :parameters ()
  :precondition (and
    (update-status comp100)
    (not (rebooted comp100))
    (all-on comp100)
  )
  :effect (and
    (not (update-status comp100))
    (update-status comp101)
    (not (all-on comp100))
    (probabilistic 1/20 (and (not (running comp100))))
  )
)
(:action update-status-comp100-one-off
  :parameters ()
  :precondition (and
    (update-status comp100)
    (not (rebooted comp100))
    (one-off comp100)
  )
  :effect (and
    (not (update-status comp100))
    (update-status comp101)
    (not (one-off comp100))
    (probabilistic 1/4 (and (not (running comp100))))
  )
)
(:action update-status-comp101-rebooted
  :parameters ()
  :precondition (and
    (update-status comp101)
    (rebooted comp101)
  )
  :effect (and
    (not (update-status comp101))
    (update-status comp102)
    (not (rebooted comp101))
    (probabilistic 9/10 (and (running comp101)) 1/10 (and))
  )
)
(:action update-status-comp101-all-on
  :parameters ()
  :precondition (and
    (update-status comp101)
    (not (rebooted comp101))
    (all-on comp101)
  )
  :effect (and
    (not (update-status comp101))
    (update-status comp102)
    (not (all-on comp101))
    (probabilistic 1/20 (and (not (running comp101))))
  )
)
(:action update-status-comp101-one-off
  :parameters ()
  :precondition (and
    (update-status comp101)
    (not (rebooted comp101))
    (one-off comp101)
  )
  :effect (and
    (not (update-status comp101))
    (update-status comp102)
    (not (one-off comp101))
    (probabilistic 1/4 (and (not (running comp101))))
  )
)
(:action update-status-comp102-rebooted
  :parameters ()
  :precondition (and
    (update-status comp102)
    (rebooted comp102)
  )
  :effect (and
    (not (update-status comp102))
    (update-status comp103)
    (not (rebooted comp102))
    (probabilistic 9/10 (and (running comp102)) 1/10 (and))
  )
)
(:action update-status-comp102-all-on
  :parameters ()
  :precondition (and
    (update-status comp102)
    (not (rebooted comp102))
    (all-on comp102)
  )
  :effect (and
    (not (update-status comp102))
    (update-status comp103)
    (not (all-on comp102))
    (probabilistic 1/20 (and (not (running comp102))))
  )
)
(:action update-status-comp102-one-off
  :parameters ()
  :precondition (and
    (update-status comp102)
    (not (rebooted comp102))
    (one-off comp102)
  )
  :effect (and
    (not (update-status comp102))
    (update-status comp103)
    (not (one-off comp102))
    (probabilistic 1/4 (and (not (running comp102))))
  )
)
(:action update-status-comp103-rebooted
  :parameters ()
  :precondition (and
    (update-status comp103)
    (rebooted comp103)
  )
  :effect (and
    (not (update-status comp103))
    (update-status comp104)
    (not (rebooted comp103))
    (probabilistic 9/10 (and (running comp103)) 1/10 (and))
  )
)
(:action update-status-comp103-all-on
  :parameters ()
  :precondition (and
    (update-status comp103)
    (not (rebooted comp103))
    (all-on comp103)
  )
  :effect (and
    (not (update-status comp103))
    (update-status comp104)
    (not (all-on comp103))
    (probabilistic 1/20 (and (not (running comp103))))
  )
)
(:action update-status-comp103-one-off
  :parameters ()
  :precondition (and
    (update-status comp103)
    (not (rebooted comp103))
    (one-off comp103)
  )
  :effect (and
    (not (update-status comp103))
    (update-status comp104)
    (not (one-off comp103))
    (probabilistic 1/4 (and (not (running comp103))))
  )
)
(:action update-status-comp104-rebooted
  :parameters ()
  :precondition (and
    (update-status comp104)
    (rebooted comp104)
  )
  :effect (and
    (not (update-status comp104))
    (update-status comp105)
    (not (rebooted comp104))
    (probabilistic 9/10 (and (running comp104)) 1/10 (and))
  )
)
(:action update-status-comp104-all-on
  :parameters ()
  :precondition (and
    (update-status comp104)
    (not (rebooted comp104))
    (all-on comp104)
  )
  :effect (and
    (not (update-status comp104))
    (update-status comp105)
    (not (all-on comp104))
    (probabilistic 1/20 (and (not (running comp104))))
  )
)
(:action update-status-comp104-one-off
  :parameters ()
  :precondition (and
    (update-status comp104)
    (not (rebooted comp104))
    (one-off comp104)
  )
  :effect (and
    (not (update-status comp104))
    (update-status comp105)
    (not (one-off comp104))
    (probabilistic 1/4 (and (not (running comp104))))
  )
)
(:action update-status-comp105-rebooted
  :parameters ()
  :precondition (and
    (update-status comp105)
    (rebooted comp105)
  )
  :effect (and
    (not (update-status comp105))
    (update-status comp106)
    (not (rebooted comp105))
    (probabilistic 9/10 (and (running comp105)) 1/10 (and))
  )
)
(:action update-status-comp105-all-on
  :parameters ()
  :precondition (and
    (update-status comp105)
    (not (rebooted comp105))
    (all-on comp105)
  )
  :effect (and
    (not (update-status comp105))
    (update-status comp106)
    (not (all-on comp105))
    (probabilistic 1/20 (and (not (running comp105))))
  )
)
(:action update-status-comp105-one-off
  :parameters ()
  :precondition (and
    (update-status comp105)
    (not (rebooted comp105))
    (one-off comp105)
  )
  :effect (and
    (not (update-status comp105))
    (update-status comp106)
    (not (one-off comp105))
    (probabilistic 1/4 (and (not (running comp105))))
  )
)
(:action update-status-comp106-rebooted
  :parameters ()
  :precondition (and
    (update-status comp106)
    (rebooted comp106)
  )
  :effect (and
    (not (update-status comp106))
    (update-status comp107)
    (not (rebooted comp106))
    (probabilistic 9/10 (and (running comp106)) 1/10 (and))
  )
)
(:action update-status-comp106-all-on
  :parameters ()
  :precondition (and
    (update-status comp106)
    (not (rebooted comp106))
    (all-on comp106)
  )
  :effect (and
    (not (update-status comp106))
    (update-status comp107)
    (not (all-on comp106))
    (probabilistic 1/20 (and (not (running comp106))))
  )
)
(:action update-status-comp106-one-off
  :parameters ()
  :precondition (and
    (update-status comp106)
    (not (rebooted comp106))
    (one-off comp106)
  )
  :effect (and
    (not (update-status comp106))
    (update-status comp107)
    (not (one-off comp106))
    (probabilistic 1/4 (and (not (running comp106))))
  )
)
(:action update-status-comp107-rebooted
  :parameters ()
  :precondition (and
    (update-status comp107)
    (rebooted comp107)
  )
  :effect (and
    (not (update-status comp107))
    (update-status comp108)
    (not (rebooted comp107))
    (probabilistic 9/10 (and (running comp107)) 1/10 (and))
  )
)
(:action update-status-comp107-all-on
  :parameters ()
  :precondition (and
    (update-status comp107)
    (not (rebooted comp107))
    (all-on comp107)
  )
  :effect (and
    (not (update-status comp107))
    (update-status comp108)
    (not (all-on comp107))
    (probabilistic 1/20 (and (not (running comp107))))
  )
)
(:action update-status-comp107-one-off
  :parameters ()
  :precondition (and
    (update-status comp107)
    (not (rebooted comp107))
    (one-off comp107)
  )
  :effect (and
    (not (update-status comp107))
    (update-status comp108)
    (not (one-off comp107))
    (probabilistic 1/4 (and (not (running comp107))))
  )
)
(:action update-status-comp108-rebooted
  :parameters ()
  :precondition (and
    (update-status comp108)
    (rebooted comp108)
  )
  :effect (and
    (not (update-status comp108))
    (update-status comp109)
    (not (rebooted comp108))
    (probabilistic 9/10 (and (running comp108)) 1/10 (and))
  )
)
(:action update-status-comp108-all-on
  :parameters ()
  :precondition (and
    (update-status comp108)
    (not (rebooted comp108))
    (all-on comp108)
  )
  :effect (and
    (not (update-status comp108))
    (update-status comp109)
    (not (all-on comp108))
    (probabilistic 1/20 (and (not (running comp108))))
  )
)
(:action update-status-comp108-one-off
  :parameters ()
  :precondition (and
    (update-status comp108)
    (not (rebooted comp108))
    (one-off comp108)
  )
  :effect (and
    (not (update-status comp108))
    (update-status comp109)
    (not (one-off comp108))
    (probabilistic 1/4 (and (not (running comp108))))
  )
)
(:action update-status-comp109-rebooted
  :parameters ()
  :precondition (and
    (update-status comp109)
    (rebooted comp109)
  )
  :effect (and
    (not (update-status comp109))
    (update-status comp110)
    (not (rebooted comp109))
    (probabilistic 9/10 (and (running comp109)) 1/10 (and))
  )
)
(:action update-status-comp109-all-on
  :parameters ()
  :precondition (and
    (update-status comp109)
    (not (rebooted comp109))
    (all-on comp109)
  )
  :effect (and
    (not (update-status comp109))
    (update-status comp110)
    (not (all-on comp109))
    (probabilistic 1/20 (and (not (running comp109))))
  )
)
(:action update-status-comp109-one-off
  :parameters ()
  :precondition (and
    (update-status comp109)
    (not (rebooted comp109))
    (one-off comp109)
  )
  :effect (and
    (not (update-status comp109))
    (update-status comp110)
    (not (one-off comp109))
    (probabilistic 1/4 (and (not (running comp109))))
  )
)
(:action update-status-comp110-rebooted
  :parameters ()
  :precondition (and
    (update-status comp110)
    (rebooted comp110)
  )
  :effect (and
    (not (update-status comp110))
    (update-status comp111)
    (not (rebooted comp110))
    (probabilistic 9/10 (and (running comp110)) 1/10 (and))
  )
)
(:action update-status-comp110-all-on
  :parameters ()
  :precondition (and
    (update-status comp110)
    (not (rebooted comp110))
    (all-on comp110)
  )
  :effect (and
    (not (update-status comp110))
    (update-status comp111)
    (not (all-on comp110))
    (probabilistic 1/20 (and (not (running comp110))))
  )
)
(:action update-status-comp110-one-off
  :parameters ()
  :precondition (and
    (update-status comp110)
    (not (rebooted comp110))
    (one-off comp110)
  )
  :effect (and
    (not (update-status comp110))
    (update-status comp111)
    (not (one-off comp110))
    (probabilistic 1/4 (and (not (running comp110))))
  )
)
(:action update-status-comp111-rebooted
  :parameters ()
  :precondition (and
    (update-status comp111)
    (rebooted comp111)
  )
  :effect (and
    (not (update-status comp111))
    (update-status comp112)
    (not (rebooted comp111))
    (probabilistic 9/10 (and (running comp111)) 1/10 (and))
  )
)
(:action update-status-comp111-all-on
  :parameters ()
  :precondition (and
    (update-status comp111)
    (not (rebooted comp111))
    (all-on comp111)
  )
  :effect (and
    (not (update-status comp111))
    (update-status comp112)
    (not (all-on comp111))
    (probabilistic 1/20 (and (not (running comp111))))
  )
)
(:action update-status-comp111-one-off
  :parameters ()
  :precondition (and
    (update-status comp111)
    (not (rebooted comp111))
    (one-off comp111)
  )
  :effect (and
    (not (update-status comp111))
    (update-status comp112)
    (not (one-off comp111))
    (probabilistic 1/4 (and (not (running comp111))))
  )
)
(:action update-status-comp112-rebooted
  :parameters ()
  :precondition (and
    (update-status comp112)
    (rebooted comp112)
  )
  :effect (and
    (not (update-status comp112))
    (update-status comp113)
    (not (rebooted comp112))
    (probabilistic 9/10 (and (running comp112)) 1/10 (and))
  )
)
(:action update-status-comp112-all-on
  :parameters ()
  :precondition (and
    (update-status comp112)
    (not (rebooted comp112))
    (all-on comp112)
  )
  :effect (and
    (not (update-status comp112))
    (update-status comp113)
    (not (all-on comp112))
    (probabilistic 1/20 (and (not (running comp112))))
  )
)
(:action update-status-comp112-one-off
  :parameters ()
  :precondition (and
    (update-status comp112)
    (not (rebooted comp112))
    (one-off comp112)
  )
  :effect (and
    (not (update-status comp112))
    (update-status comp113)
    (not (one-off comp112))
    (probabilistic 1/4 (and (not (running comp112))))
  )
)
(:action update-status-comp113-rebooted
  :parameters ()
  :precondition (and
    (update-status comp113)
    (rebooted comp113)
  )
  :effect (and
    (not (update-status comp113))
    (update-status comp114)
    (not (rebooted comp113))
    (probabilistic 9/10 (and (running comp113)) 1/10 (and))
  )
)
(:action update-status-comp113-all-on
  :parameters ()
  :precondition (and
    (update-status comp113)
    (not (rebooted comp113))
    (all-on comp113)
  )
  :effect (and
    (not (update-status comp113))
    (update-status comp114)
    (not (all-on comp113))
    (probabilistic 1/20 (and (not (running comp113))))
  )
)
(:action update-status-comp113-one-off
  :parameters ()
  :precondition (and
    (update-status comp113)
    (not (rebooted comp113))
    (one-off comp113)
  )
  :effect (and
    (not (update-status comp113))
    (update-status comp114)
    (not (one-off comp113))
    (probabilistic 1/4 (and (not (running comp113))))
  )
)
(:action update-status-comp114-rebooted
  :parameters ()
  :precondition (and
    (update-status comp114)
    (rebooted comp114)
  )
  :effect (and
    (not (update-status comp114))
    (update-status comp115)
    (not (rebooted comp114))
    (probabilistic 9/10 (and (running comp114)) 1/10 (and))
  )
)
(:action update-status-comp114-all-on
  :parameters ()
  :precondition (and
    (update-status comp114)
    (not (rebooted comp114))
    (all-on comp114)
  )
  :effect (and
    (not (update-status comp114))
    (update-status comp115)
    (not (all-on comp114))
    (probabilistic 1/20 (and (not (running comp114))))
  )
)
(:action update-status-comp114-one-off
  :parameters ()
  :precondition (and
    (update-status comp114)
    (not (rebooted comp114))
    (one-off comp114)
  )
  :effect (and
    (not (update-status comp114))
    (update-status comp115)
    (not (one-off comp114))
    (probabilistic 1/4 (and (not (running comp114))))
  )
)
(:action update-status-comp115-rebooted
  :parameters ()
  :precondition (and
    (update-status comp115)
    (rebooted comp115)
  )
  :effect (and
    (not (update-status comp115))
    (update-status comp116)
    (not (rebooted comp115))
    (probabilistic 9/10 (and (running comp115)) 1/10 (and))
  )
)
(:action update-status-comp115-all-on
  :parameters ()
  :precondition (and
    (update-status comp115)
    (not (rebooted comp115))
    (all-on comp115)
  )
  :effect (and
    (not (update-status comp115))
    (update-status comp116)
    (not (all-on comp115))
    (probabilistic 1/20 (and (not (running comp115))))
  )
)
(:action update-status-comp115-one-off
  :parameters ()
  :precondition (and
    (update-status comp115)
    (not (rebooted comp115))
    (one-off comp115)
  )
  :effect (and
    (not (update-status comp115))
    (update-status comp116)
    (not (one-off comp115))
    (probabilistic 1/4 (and (not (running comp115))))
  )
)
(:action update-status-comp116-rebooted
  :parameters ()
  :precondition (and
    (update-status comp116)
    (rebooted comp116)
  )
  :effect (and
    (not (update-status comp116))
    (update-status comp117)
    (not (rebooted comp116))
    (probabilistic 9/10 (and (running comp116)) 1/10 (and))
  )
)
(:action update-status-comp116-all-on
  :parameters ()
  :precondition (and
    (update-status comp116)
    (not (rebooted comp116))
    (all-on comp116)
  )
  :effect (and
    (not (update-status comp116))
    (update-status comp117)
    (not (all-on comp116))
    (probabilistic 1/20 (and (not (running comp116))))
  )
)
(:action update-status-comp116-one-off
  :parameters ()
  :precondition (and
    (update-status comp116)
    (not (rebooted comp116))
    (one-off comp116)
  )
  :effect (and
    (not (update-status comp116))
    (update-status comp117)
    (not (one-off comp116))
    (probabilistic 1/4 (and (not (running comp116))))
  )
)
(:action update-status-comp117-rebooted
  :parameters ()
  :precondition (and
    (update-status comp117)
    (rebooted comp117)
  )
  :effect (and
    (not (update-status comp117))
    (update-status comp118)
    (not (rebooted comp117))
    (probabilistic 9/10 (and (running comp117)) 1/10 (and))
  )
)
(:action update-status-comp117-all-on
  :parameters ()
  :precondition (and
    (update-status comp117)
    (not (rebooted comp117))
    (all-on comp117)
  )
  :effect (and
    (not (update-status comp117))
    (update-status comp118)
    (not (all-on comp117))
    (probabilistic 1/20 (and (not (running comp117))))
  )
)
(:action update-status-comp117-one-off
  :parameters ()
  :precondition (and
    (update-status comp117)
    (not (rebooted comp117))
    (one-off comp117)
  )
  :effect (and
    (not (update-status comp117))
    (update-status comp118)
    (not (one-off comp117))
    (probabilistic 1/4 (and (not (running comp117))))
  )
)
(:action update-status-comp118-rebooted
  :parameters ()
  :precondition (and
    (update-status comp118)
    (rebooted comp118)
  )
  :effect (and
    (not (update-status comp118))
    (update-status comp119)
    (not (rebooted comp118))
    (probabilistic 9/10 (and (running comp118)) 1/10 (and))
  )
)
(:action update-status-comp118-all-on
  :parameters ()
  :precondition (and
    (update-status comp118)
    (not (rebooted comp118))
    (all-on comp118)
  )
  :effect (and
    (not (update-status comp118))
    (update-status comp119)
    (not (all-on comp118))
    (probabilistic 1/20 (and (not (running comp118))))
  )
)
(:action update-status-comp118-one-off
  :parameters ()
  :precondition (and
    (update-status comp118)
    (not (rebooted comp118))
    (one-off comp118)
  )
  :effect (and
    (not (update-status comp118))
    (update-status comp119)
    (not (one-off comp118))
    (probabilistic 1/4 (and (not (running comp118))))
  )
)
(:action update-status-comp119-rebooted
  :parameters ()
  :precondition (and
    (update-status comp119)
    (rebooted comp119)
  )
  :effect (and
    (not (update-status comp119))
    (all-updated)
    (not (rebooted comp119))
    (probabilistic 9/10 (and (running comp119)) 1/10 (and))
  )
)
(:action update-status-comp119-all-on
  :parameters ()
  :precondition (and
    (update-status comp119)
    (not (rebooted comp119))
    (all-on comp119)
  )
  :effect (and
    (not (update-status comp119))
    (all-updated)
    (not (all-on comp119))
    (probabilistic 1/20 (and (not (running comp119))))
  )
)
(:action update-status-comp119-one-off
  :parameters ()
  :precondition (and
    (update-status comp119)
    (not (rebooted comp119))
    (one-off comp119)
  )
  :effect (and
    (not (update-status comp119))
    (all-updated)
    (not (one-off comp119))
    (probabilistic 1/4 (and (not (running comp119))))
  )
)
)