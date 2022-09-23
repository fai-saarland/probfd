(define (domain sysadmin-nocount)
(:requirements :probabilistic-effects :negative-preconditions :typing)
(:types computer - object)
(:constants
  comp0 comp1 comp2 comp3 comp4 comp5 comp6 comp7 comp8 comp9 comp10 comp11 comp12 comp13 comp14 comp15 comp16 comp17 comp18 comp19 comp20 comp21 comp22 comp23 comp24 comp25 comp26 comp27 comp28 comp29 comp30 comp31 comp32 comp33 comp34 comp35 comp36 comp37 comp38 comp39 comp40 comp41 comp42 comp43 comp44 comp45 comp46 comp47 comp48 comp49 comp50 comp51 comp52 comp53 comp54 comp55 comp56 comp57 comp58 comp59 comp60 comp61 comp62 comp63 comp64 comp65 comp66 comp67 comp68 comp69 comp70 comp71 comp72 comp73 comp74 comp75 comp76 comp77 comp78 comp79 comp80 comp81 comp82 comp83 comp84 comp85 comp86 comp87 comp88 comp89 comp90 comp91 comp92 comp93 comp94 comp95 comp96 comp97 comp98 comp99 comp100 comp101 comp102 comp103 comp104 comp105 comp106 comp107 comp108 comp109 comp110 comp111 comp112 comp113 comp114 comp115 comp116 comp117 comp118 comp119 comp120 comp121 comp122 comp123 comp124 comp125 comp126 comp127 comp128 comp129 comp130 comp131 comp132 comp133 comp134 comp135 comp136 comp137 comp138 comp139 comp140 comp141 comp142 comp143 comp144 comp145 comp146 comp147 comp148 comp149 comp150 comp151 comp152 comp153 comp154 comp155 comp156 comp157 comp158 comp159 comp160 comp161 comp162 comp163 comp164 comp165 comp166 comp167 comp168 comp169 comp170 comp171 comp172 comp173 comp174 comp175 comp176 comp177 comp178 comp179 comp180 comp181 comp182 comp183 comp184 comp185 comp186 comp187 comp188 comp189 comp190 comp191 comp192 comp193 comp194 comp195 comp196 comp197 comp198 comp199 comp200 comp201 comp202 comp203 comp204 comp205 comp206 comp207 comp208 comp209 comp210 comp211 comp212 comp213 comp214 comp215 comp216 comp217 comp218 comp219 comp220 comp221 comp222 comp223 comp224 comp225 comp226 comp227 comp228 comp229 comp230 comp231 comp232 comp233 comp234 comp235 comp236 comp237 comp238 comp239 - computer
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
    (running comp56)
    (running comp239)
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (all-on comp0)
  )
)
(:action evaluate-comp0-off-comp56
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp56))
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (one-off comp0)
  )
)
(:action evaluate-comp0-off-comp239
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp239))
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
    (running comp65)
    (running comp106)
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
(:action evaluate-comp2-off-comp65
  :parameters ()
  :precondition (and
    (evaluate comp2)
    (not (rebooted comp2))
    (not (running comp65))
  )
  :effect (and
    (not (evaluate comp2))
    (evaluate comp3)
    (one-off comp2)
  )
)
(:action evaluate-comp2-off-comp106
  :parameters ()
  :precondition (and
    (evaluate comp2)
    (not (rebooted comp2))
    (not (running comp106))
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
    (running comp130)
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
(:action evaluate-comp3-off-comp130
  :parameters ()
  :precondition (and
    (evaluate comp3)
    (not (rebooted comp3))
    (not (running comp130))
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
    (running comp134)
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
(:action evaluate-comp5-off-comp134
  :parameters ()
  :precondition (and
    (evaluate comp5)
    (not (rebooted comp5))
    (not (running comp134))
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
    (running comp198)
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
(:action evaluate-comp8-off-comp198
  :parameters ()
  :precondition (and
    (evaluate comp8)
    (not (rebooted comp8))
    (not (running comp198))
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
    (running comp149)
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
(:action evaluate-comp9-off-comp149
  :parameters ()
  :precondition (and
    (evaluate comp9)
    (not (rebooted comp9))
    (not (running comp149))
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
    (running comp131)
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
(:action evaluate-comp10-off-comp131
  :parameters ()
  :precondition (and
    (evaluate comp10)
    (not (rebooted comp10))
    (not (running comp131))
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
    (running comp202)
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
(:action evaluate-comp11-off-comp202
  :parameters ()
  :precondition (and
    (evaluate comp11)
    (not (rebooted comp11))
    (not (running comp202))
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
    (running comp51)
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
(:action evaluate-comp12-off-comp51
  :parameters ()
  :precondition (and
    (evaluate comp12)
    (not (rebooted comp12))
    (not (running comp51))
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
    (running comp71)
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
(:action evaluate-comp14-off-comp71
  :parameters ()
  :precondition (and
    (evaluate comp14)
    (not (rebooted comp14))
    (not (running comp71))
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
    (running comp228)
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
(:action evaluate-comp16-off-comp228
  :parameters ()
  :precondition (and
    (evaluate comp16)
    (not (rebooted comp16))
    (not (running comp228))
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
    (running comp41)
    (running comp103)
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
(:action evaluate-comp20-off-comp41
  :parameters ()
  :precondition (and
    (evaluate comp20)
    (not (rebooted comp20))
    (not (running comp41))
  )
  :effect (and
    (not (evaluate comp20))
    (evaluate comp21)
    (one-off comp20)
  )
)
(:action evaluate-comp20-off-comp103
  :parameters ()
  :precondition (and
    (evaluate comp20)
    (not (rebooted comp20))
    (not (running comp103))
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
    (running comp236)
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
(:action evaluate-comp22-off-comp236
  :parameters ()
  :precondition (and
    (evaluate comp22)
    (not (rebooted comp22))
    (not (running comp236))
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
    (running comp23)
    (running comp43)
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
(:action evaluate-comp24-off-comp43
  :parameters ()
  :precondition (and
    (evaluate comp24)
    (not (rebooted comp24))
    (not (running comp43))
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
    (running comp75)
    (running comp197)
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
(:action evaluate-comp31-off-comp75
  :parameters ()
  :precondition (and
    (evaluate comp31)
    (not (rebooted comp31))
    (not (running comp75))
  )
  :effect (and
    (not (evaluate comp31))
    (evaluate comp32)
    (one-off comp31)
  )
)
(:action evaluate-comp31-off-comp197
  :parameters ()
  :precondition (and
    (evaluate comp31)
    (not (rebooted comp31))
    (not (running comp197))
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
    (running comp53)
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
(:action evaluate-comp35-off-comp53
  :parameters ()
  :precondition (and
    (evaluate comp35)
    (not (rebooted comp35))
    (not (running comp53))
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
    (running comp223)
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
(:action evaluate-comp36-off-comp223
  :parameters ()
  :precondition (and
    (evaluate comp36)
    (not (rebooted comp36))
    (not (running comp223))
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
    (running comp220)
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
(:action evaluate-comp38-off-comp220
  :parameters ()
  :precondition (and
    (evaluate comp38)
    (not (rebooted comp38))
    (not (running comp220))
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
    (running comp58)
    (running comp203)
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
(:action evaluate-comp39-off-comp58
  :parameters ()
  :precondition (and
    (evaluate comp39)
    (not (rebooted comp39))
    (not (running comp58))
  )
  :effect (and
    (not (evaluate comp39))
    (evaluate comp40)
    (one-off comp39)
  )
)
(:action evaluate-comp39-off-comp203
  :parameters ()
  :precondition (and
    (evaluate comp39)
    (not (rebooted comp39))
    (not (running comp203))
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
    (running comp114)
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
(:action evaluate-comp40-off-comp114
  :parameters ()
  :precondition (and
    (evaluate comp40)
    (not (rebooted comp40))
    (not (running comp114))
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
    (running comp28)
    (running comp41)
  )
  :effect (and
    (not (evaluate comp42))
    (evaluate comp43)
    (all-on comp42)
  )
)
(:action evaluate-comp42-off-comp28
  :parameters ()
  :precondition (and
    (evaluate comp42)
    (not (rebooted comp42))
    (not (running comp28))
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
    (running comp48)
    (running comp162)
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
(:action evaluate-comp49-off-comp162
  :parameters ()
  :precondition (and
    (evaluate comp49)
    (not (rebooted comp49))
    (not (running comp162))
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
    (running comp142)
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
(:action evaluate-comp50-off-comp142
  :parameters ()
  :precondition (and
    (evaluate comp50)
    (not (rebooted comp50))
    (not (running comp142))
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
    (running comp32)
    (running comp47)
    (running comp50)
  )
  :effect (and
    (not (evaluate comp51))
    (evaluate comp52)
    (all-on comp51)
  )
)
(:action evaluate-comp51-off-comp32
  :parameters ()
  :precondition (and
    (evaluate comp51)
    (not (rebooted comp51))
    (not (running comp32))
  )
  :effect (and
    (not (evaluate comp51))
    (evaluate comp52)
    (one-off comp51)
  )
)
(:action evaluate-comp51-off-comp47
  :parameters ()
  :precondition (and
    (evaluate comp51)
    (not (rebooted comp51))
    (not (running comp47))
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
    (running comp28)
    (running comp52)
  )
  :effect (and
    (not (evaluate comp53))
    (evaluate comp54)
    (all-on comp53)
  )
)
(:action evaluate-comp53-off-comp28
  :parameters ()
  :precondition (and
    (evaluate comp53)
    (not (rebooted comp53))
    (not (running comp28))
  )
  :effect (and
    (not (evaluate comp53))
    (evaluate comp54)
    (one-off comp53)
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
    (running comp81)
    (running comp214)
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
(:action evaluate-comp56-off-comp81
  :parameters ()
  :precondition (and
    (evaluate comp56)
    (not (rebooted comp56))
    (not (running comp81))
  )
  :effect (and
    (not (evaluate comp56))
    (evaluate comp57)
    (one-off comp56)
  )
)
(:action evaluate-comp56-off-comp214
  :parameters ()
  :precondition (and
    (evaluate comp56)
    (not (rebooted comp56))
    (not (running comp214))
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
    (running comp57)
    (running comp229)
  )
  :effect (and
    (not (evaluate comp58))
    (evaluate comp59)
    (all-on comp58)
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
(:action evaluate-comp58-off-comp229
  :parameters ()
  :precondition (and
    (evaluate comp58)
    (not (rebooted comp58))
    (not (running comp229))
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
    (running comp58)
  )
  :effect (and
    (not (evaluate comp59))
    (evaluate comp60)
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
    (running comp51)
    (running comp59)
  )
  :effect (and
    (not (evaluate comp60))
    (evaluate comp61)
    (all-on comp60)
  )
)
(:action evaluate-comp60-off-comp51
  :parameters ()
  :precondition (and
    (evaluate comp60)
    (not (rebooted comp60))
    (not (running comp51))
  )
  :effect (and
    (not (evaluate comp60))
    (evaluate comp61)
    (one-off comp60)
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
    (running comp62)
    (running comp195)
  )
  :effect (and
    (not (evaluate comp63))
    (evaluate comp64)
    (all-on comp63)
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
(:action evaluate-comp63-off-comp195
  :parameters ()
  :precondition (and
    (evaluate comp63)
    (not (rebooted comp63))
    (not (running comp195))
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
    (running comp63)
    (running comp101)
    (running comp174)
  )
  :effect (and
    (not (evaluate comp64))
    (evaluate comp65)
    (all-on comp64)
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
(:action evaluate-comp64-off-comp101
  :parameters ()
  :precondition (and
    (evaluate comp64)
    (not (rebooted comp64))
    (not (running comp101))
  )
  :effect (and
    (not (evaluate comp64))
    (evaluate comp65)
    (one-off comp64)
  )
)
(:action evaluate-comp64-off-comp174
  :parameters ()
  :precondition (and
    (evaluate comp64)
    (not (rebooted comp64))
    (not (running comp174))
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
    (running comp150)
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
(:action evaluate-comp66-off-comp150
  :parameters ()
  :precondition (and
    (evaluate comp66)
    (not (rebooted comp66))
    (not (running comp150))
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
    (running comp177)
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
(:action evaluate-comp67-off-comp177
  :parameters ()
  :precondition (and
    (evaluate comp67)
    (not (rebooted comp67))
    (not (running comp177))
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
    (running comp67)
    (running comp169)
  )
  :effect (and
    (not (evaluate comp68))
    (evaluate comp69)
    (all-on comp68)
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
(:action evaluate-comp68-off-comp169
  :parameters ()
  :precondition (and
    (evaluate comp68)
    (not (rebooted comp68))
    (not (running comp169))
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
    (running comp68)
  )
  :effect (and
    (not (evaluate comp69))
    (evaluate comp70)
    (all-on comp69)
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
    (running comp69)
    (running comp95)
  )
  :effect (and
    (not (evaluate comp70))
    (evaluate comp71)
    (all-on comp70)
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
(:action evaluate-comp70-off-comp95
  :parameters ()
  :precondition (and
    (evaluate comp70)
    (not (rebooted comp70))
    (not (running comp95))
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
    (running comp70)
    (running comp215)
  )
  :effect (and
    (not (evaluate comp71))
    (evaluate comp72)
    (all-on comp71)
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
(:action evaluate-comp71-off-comp215
  :parameters ()
  :precondition (and
    (evaluate comp71)
    (not (rebooted comp71))
    (not (running comp215))
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
    (running comp149)
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
(:action evaluate-comp76-off-comp149
  :parameters ()
  :precondition (and
    (evaluate comp76)
    (not (rebooted comp76))
    (not (running comp149))
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
    (running comp134)
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
(:action evaluate-comp77-off-comp134
  :parameters ()
  :precondition (and
    (evaluate comp77)
    (not (rebooted comp77))
    (not (running comp134))
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
    (running comp77)
  )
  :effect (and
    (not (evaluate comp78))
    (evaluate comp79)
    (all-on comp78)
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
    (running comp193)
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
(:action evaluate-comp80-off-comp193
  :parameters ()
  :precondition (and
    (evaluate comp80)
    (not (rebooted comp80))
    (not (running comp193))
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
    (running comp66)
    (running comp82)
    (running comp215)
  )
  :effect (and
    (not (evaluate comp83))
    (evaluate comp84)
    (all-on comp83)
  )
)
(:action evaluate-comp83-off-comp66
  :parameters ()
  :precondition (and
    (evaluate comp83)
    (not (rebooted comp83))
    (not (running comp66))
  )
  :effect (and
    (not (evaluate comp83))
    (evaluate comp84)
    (one-off comp83)
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
(:action evaluate-comp83-off-comp215
  :parameters ()
  :precondition (and
    (evaluate comp83)
    (not (rebooted comp83))
    (not (running comp215))
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
    (running comp196)
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
(:action evaluate-comp84-off-comp196
  :parameters ()
  :precondition (and
    (evaluate comp84)
    (not (rebooted comp84))
    (not (running comp196))
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
    (running comp199)
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
(:action evaluate-comp86-off-comp199
  :parameters ()
  :precondition (and
    (evaluate comp86)
    (not (rebooted comp86))
    (not (running comp199))
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
    (running comp116)
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
(:action evaluate-comp90-off-comp116
  :parameters ()
  :precondition (and
    (evaluate comp90)
    (not (rebooted comp90))
    (not (running comp116))
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
    (running comp90)
    (running comp171)
  )
  :effect (and
    (not (evaluate comp91))
    (evaluate comp92)
    (all-on comp91)
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
(:action evaluate-comp91-off-comp171
  :parameters ()
  :precondition (and
    (evaluate comp91)
    (not (rebooted comp91))
    (not (running comp171))
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
    (running comp91)
    (running comp181)
  )
  :effect (and
    (not (evaluate comp92))
    (evaluate comp93)
    (all-on comp92)
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
(:action evaluate-comp92-off-comp181
  :parameters ()
  :precondition (and
    (evaluate comp92)
    (not (rebooted comp92))
    (not (running comp181))
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
    (running comp92)
    (running comp145)
  )
  :effect (and
    (not (evaluate comp93))
    (evaluate comp94)
    (all-on comp93)
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
(:action evaluate-comp93-off-comp145
  :parameters ()
  :precondition (and
    (evaluate comp93)
    (not (rebooted comp93))
    (not (running comp145))
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
    (running comp90)
    (running comp93)
  )
  :effect (and
    (not (evaluate comp94))
    (evaluate comp95)
    (all-on comp94)
  )
)
(:action evaluate-comp94-off-comp90
  :parameters ()
  :precondition (and
    (evaluate comp94)
    (not (rebooted comp94))
    (not (running comp90))
  )
  :effect (and
    (not (evaluate comp94))
    (evaluate comp95)
    (one-off comp94)
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
    (running comp94)
  )
  :effect (and
    (not (evaluate comp95))
    (evaluate comp96)
    (all-on comp95)
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
    (running comp95)
  )
  :effect (and
    (not (evaluate comp96))
    (evaluate comp97)
    (all-on comp96)
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
    (running comp22)
    (running comp96)
  )
  :effect (and
    (not (evaluate comp97))
    (evaluate comp98)
    (all-on comp97)
  )
)
(:action evaluate-comp97-off-comp22
  :parameters ()
  :precondition (and
    (evaluate comp97)
    (not (rebooted comp97))
    (not (running comp22))
  )
  :effect (and
    (not (evaluate comp97))
    (evaluate comp98)
    (one-off comp97)
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
    (running comp97)
  )
  :effect (and
    (not (evaluate comp98))
    (evaluate comp99)
    (all-on comp98)
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
    (running comp97)
    (running comp98)
  )
  :effect (and
    (not (evaluate comp99))
    (evaluate comp100)
    (all-on comp99)
  )
)
(:action evaluate-comp99-off-comp97
  :parameters ()
  :precondition (and
    (evaluate comp99)
    (not (rebooted comp99))
    (not (running comp97))
  )
  :effect (and
    (not (evaluate comp99))
    (evaluate comp100)
    (one-off comp99)
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
    (running comp146)
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
(:action evaluate-comp100-off-comp146
  :parameters ()
  :precondition (and
    (evaluate comp100)
    (not (rebooted comp100))
    (not (running comp146))
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
    (running comp30)
    (running comp101)
  )
  :effect (and
    (not (evaluate comp102))
    (evaluate comp103)
    (all-on comp102)
  )
)
(:action evaluate-comp102-off-comp30
  :parameters ()
  :precondition (and
    (evaluate comp102)
    (not (rebooted comp102))
    (not (running comp30))
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
    (running comp72)
    (running comp104)
  )
  :effect (and
    (not (evaluate comp105))
    (evaluate comp106)
    (all-on comp105)
  )
)
(:action evaluate-comp105-off-comp72
  :parameters ()
  :precondition (and
    (evaluate comp105)
    (not (rebooted comp105))
    (not (running comp72))
  )
  :effect (and
    (not (evaluate comp105))
    (evaluate comp106)
    (one-off comp105)
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
    (running comp142)
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
(:action evaluate-comp106-off-comp142
  :parameters ()
  :precondition (and
    (evaluate comp106)
    (not (rebooted comp106))
    (not (running comp142))
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
    (running comp84)
    (running comp106)
  )
  :effect (and
    (not (evaluate comp107))
    (evaluate comp108)
    (all-on comp107)
  )
)
(:action evaluate-comp107-off-comp84
  :parameters ()
  :precondition (and
    (evaluate comp107)
    (not (rebooted comp107))
    (not (running comp84))
  )
  :effect (and
    (not (evaluate comp107))
    (evaluate comp108)
    (one-off comp107)
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
    (running comp107)
  )
  :effect (and
    (not (evaluate comp108))
    (evaluate comp109)
    (all-on comp108)
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
    (running comp108)
  )
  :effect (and
    (not (evaluate comp109))
    (evaluate comp110)
    (all-on comp109)
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
    (running comp197)
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
(:action evaluate-comp110-off-comp197
  :parameters ()
  :precondition (and
    (evaluate comp110)
    (not (rebooted comp110))
    (not (running comp197))
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
    (running comp111)
  )
  :effect (and
    (not (evaluate comp112))
    (evaluate comp113)
    (all-on comp112)
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
    (running comp218)
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
(:action evaluate-comp116-off-comp218
  :parameters ()
  :precondition (and
    (evaluate comp116)
    (not (rebooted comp116))
    (not (running comp218))
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
    (running comp136)
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
(:action evaluate-comp117-off-comp136
  :parameters ()
  :precondition (and
    (evaluate comp117)
    (not (rebooted comp117))
    (not (running comp136))
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
    (running comp117)
  )
  :effect (and
    (not (evaluate comp118))
    (evaluate comp119)
    (all-on comp118)
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
    (evaluate comp120)
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
    (evaluate comp120)
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
    (evaluate comp120)
    (one-off comp119)
  )
)
(:action evaluate-comp120-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp120)
    (rebooted comp120)
  )
  :effect (and
    (not (evaluate comp120))
    (evaluate comp121)
  )
)
(:action evaluate-comp120-all-on
  :parameters ()
  :precondition (and
    (evaluate comp120)
    (not (rebooted comp120))
    (running comp119)
    (running comp199)
  )
  :effect (and
    (not (evaluate comp120))
    (evaluate comp121)
    (all-on comp120)
  )
)
(:action evaluate-comp120-off-comp119
  :parameters ()
  :precondition (and
    (evaluate comp120)
    (not (rebooted comp120))
    (not (running comp119))
  )
  :effect (and
    (not (evaluate comp120))
    (evaluate comp121)
    (one-off comp120)
  )
)
(:action evaluate-comp120-off-comp199
  :parameters ()
  :precondition (and
    (evaluate comp120)
    (not (rebooted comp120))
    (not (running comp199))
  )
  :effect (and
    (not (evaluate comp120))
    (evaluate comp121)
    (one-off comp120)
  )
)
(:action evaluate-comp121-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp121)
    (rebooted comp121)
  )
  :effect (and
    (not (evaluate comp121))
    (evaluate comp122)
  )
)
(:action evaluate-comp121-all-on
  :parameters ()
  :precondition (and
    (evaluate comp121)
    (not (rebooted comp121))
    (running comp120)
  )
  :effect (and
    (not (evaluate comp121))
    (evaluate comp122)
    (all-on comp121)
  )
)
(:action evaluate-comp121-off-comp120
  :parameters ()
  :precondition (and
    (evaluate comp121)
    (not (rebooted comp121))
    (not (running comp120))
  )
  :effect (and
    (not (evaluate comp121))
    (evaluate comp122)
    (one-off comp121)
  )
)
(:action evaluate-comp122-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp122)
    (rebooted comp122)
  )
  :effect (and
    (not (evaluate comp122))
    (evaluate comp123)
  )
)
(:action evaluate-comp122-all-on
  :parameters ()
  :precondition (and
    (evaluate comp122)
    (not (rebooted comp122))
    (running comp121)
  )
  :effect (and
    (not (evaluate comp122))
    (evaluate comp123)
    (all-on comp122)
  )
)
(:action evaluate-comp122-off-comp121
  :parameters ()
  :precondition (and
    (evaluate comp122)
    (not (rebooted comp122))
    (not (running comp121))
  )
  :effect (and
    (not (evaluate comp122))
    (evaluate comp123)
    (one-off comp122)
  )
)
(:action evaluate-comp123-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp123)
    (rebooted comp123)
  )
  :effect (and
    (not (evaluate comp123))
    (evaluate comp124)
  )
)
(:action evaluate-comp123-all-on
  :parameters ()
  :precondition (and
    (evaluate comp123)
    (not (rebooted comp123))
    (running comp60)
    (running comp70)
    (running comp122)
  )
  :effect (and
    (not (evaluate comp123))
    (evaluate comp124)
    (all-on comp123)
  )
)
(:action evaluate-comp123-off-comp60
  :parameters ()
  :precondition (and
    (evaluate comp123)
    (not (rebooted comp123))
    (not (running comp60))
  )
  :effect (and
    (not (evaluate comp123))
    (evaluate comp124)
    (one-off comp123)
  )
)
(:action evaluate-comp123-off-comp70
  :parameters ()
  :precondition (and
    (evaluate comp123)
    (not (rebooted comp123))
    (not (running comp70))
  )
  :effect (and
    (not (evaluate comp123))
    (evaluate comp124)
    (one-off comp123)
  )
)
(:action evaluate-comp123-off-comp122
  :parameters ()
  :precondition (and
    (evaluate comp123)
    (not (rebooted comp123))
    (not (running comp122))
  )
  :effect (and
    (not (evaluate comp123))
    (evaluate comp124)
    (one-off comp123)
  )
)
(:action evaluate-comp124-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp124)
    (rebooted comp124)
  )
  :effect (and
    (not (evaluate comp124))
    (evaluate comp125)
  )
)
(:action evaluate-comp124-all-on
  :parameters ()
  :precondition (and
    (evaluate comp124)
    (not (rebooted comp124))
    (running comp123)
  )
  :effect (and
    (not (evaluate comp124))
    (evaluate comp125)
    (all-on comp124)
  )
)
(:action evaluate-comp124-off-comp123
  :parameters ()
  :precondition (and
    (evaluate comp124)
    (not (rebooted comp124))
    (not (running comp123))
  )
  :effect (and
    (not (evaluate comp124))
    (evaluate comp125)
    (one-off comp124)
  )
)
(:action evaluate-comp125-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp125)
    (rebooted comp125)
  )
  :effect (and
    (not (evaluate comp125))
    (evaluate comp126)
  )
)
(:action evaluate-comp125-all-on
  :parameters ()
  :precondition (and
    (evaluate comp125)
    (not (rebooted comp125))
    (running comp124)
  )
  :effect (and
    (not (evaluate comp125))
    (evaluate comp126)
    (all-on comp125)
  )
)
(:action evaluate-comp125-off-comp124
  :parameters ()
  :precondition (and
    (evaluate comp125)
    (not (rebooted comp125))
    (not (running comp124))
  )
  :effect (and
    (not (evaluate comp125))
    (evaluate comp126)
    (one-off comp125)
  )
)
(:action evaluate-comp126-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp126)
    (rebooted comp126)
  )
  :effect (and
    (not (evaluate comp126))
    (evaluate comp127)
  )
)
(:action evaluate-comp126-all-on
  :parameters ()
  :precondition (and
    (evaluate comp126)
    (not (rebooted comp126))
    (running comp125)
  )
  :effect (and
    (not (evaluate comp126))
    (evaluate comp127)
    (all-on comp126)
  )
)
(:action evaluate-comp126-off-comp125
  :parameters ()
  :precondition (and
    (evaluate comp126)
    (not (rebooted comp126))
    (not (running comp125))
  )
  :effect (and
    (not (evaluate comp126))
    (evaluate comp127)
    (one-off comp126)
  )
)
(:action evaluate-comp127-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp127)
    (rebooted comp127)
  )
  :effect (and
    (not (evaluate comp127))
    (evaluate comp128)
  )
)
(:action evaluate-comp127-all-on
  :parameters ()
  :precondition (and
    (evaluate comp127)
    (not (rebooted comp127))
    (running comp44)
    (running comp126)
  )
  :effect (and
    (not (evaluate comp127))
    (evaluate comp128)
    (all-on comp127)
  )
)
(:action evaluate-comp127-off-comp44
  :parameters ()
  :precondition (and
    (evaluate comp127)
    (not (rebooted comp127))
    (not (running comp44))
  )
  :effect (and
    (not (evaluate comp127))
    (evaluate comp128)
    (one-off comp127)
  )
)
(:action evaluate-comp127-off-comp126
  :parameters ()
  :precondition (and
    (evaluate comp127)
    (not (rebooted comp127))
    (not (running comp126))
  )
  :effect (and
    (not (evaluate comp127))
    (evaluate comp128)
    (one-off comp127)
  )
)
(:action evaluate-comp128-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp128)
    (rebooted comp128)
  )
  :effect (and
    (not (evaluate comp128))
    (evaluate comp129)
  )
)
(:action evaluate-comp128-all-on
  :parameters ()
  :precondition (and
    (evaluate comp128)
    (not (rebooted comp128))
    (running comp127)
  )
  :effect (and
    (not (evaluate comp128))
    (evaluate comp129)
    (all-on comp128)
  )
)
(:action evaluate-comp128-off-comp127
  :parameters ()
  :precondition (and
    (evaluate comp128)
    (not (rebooted comp128))
    (not (running comp127))
  )
  :effect (and
    (not (evaluate comp128))
    (evaluate comp129)
    (one-off comp128)
  )
)
(:action evaluate-comp129-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp129)
    (rebooted comp129)
  )
  :effect (and
    (not (evaluate comp129))
    (evaluate comp130)
  )
)
(:action evaluate-comp129-all-on
  :parameters ()
  :precondition (and
    (evaluate comp129)
    (not (rebooted comp129))
    (running comp128)
  )
  :effect (and
    (not (evaluate comp129))
    (evaluate comp130)
    (all-on comp129)
  )
)
(:action evaluate-comp129-off-comp128
  :parameters ()
  :precondition (and
    (evaluate comp129)
    (not (rebooted comp129))
    (not (running comp128))
  )
  :effect (and
    (not (evaluate comp129))
    (evaluate comp130)
    (one-off comp129)
  )
)
(:action evaluate-comp130-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp130)
    (rebooted comp130)
  )
  :effect (and
    (not (evaluate comp130))
    (evaluate comp131)
  )
)
(:action evaluate-comp130-all-on
  :parameters ()
  :precondition (and
    (evaluate comp130)
    (not (rebooted comp130))
    (running comp89)
    (running comp129)
  )
  :effect (and
    (not (evaluate comp130))
    (evaluate comp131)
    (all-on comp130)
  )
)
(:action evaluate-comp130-off-comp89
  :parameters ()
  :precondition (and
    (evaluate comp130)
    (not (rebooted comp130))
    (not (running comp89))
  )
  :effect (and
    (not (evaluate comp130))
    (evaluate comp131)
    (one-off comp130)
  )
)
(:action evaluate-comp130-off-comp129
  :parameters ()
  :precondition (and
    (evaluate comp130)
    (not (rebooted comp130))
    (not (running comp129))
  )
  :effect (and
    (not (evaluate comp130))
    (evaluate comp131)
    (one-off comp130)
  )
)
(:action evaluate-comp131-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp131)
    (rebooted comp131)
  )
  :effect (and
    (not (evaluate comp131))
    (evaluate comp132)
  )
)
(:action evaluate-comp131-all-on
  :parameters ()
  :precondition (and
    (evaluate comp131)
    (not (rebooted comp131))
    (running comp102)
    (running comp130)
  )
  :effect (and
    (not (evaluate comp131))
    (evaluate comp132)
    (all-on comp131)
  )
)
(:action evaluate-comp131-off-comp102
  :parameters ()
  :precondition (and
    (evaluate comp131)
    (not (rebooted comp131))
    (not (running comp102))
  )
  :effect (and
    (not (evaluate comp131))
    (evaluate comp132)
    (one-off comp131)
  )
)
(:action evaluate-comp131-off-comp130
  :parameters ()
  :precondition (and
    (evaluate comp131)
    (not (rebooted comp131))
    (not (running comp130))
  )
  :effect (and
    (not (evaluate comp131))
    (evaluate comp132)
    (one-off comp131)
  )
)
(:action evaluate-comp132-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp132)
    (rebooted comp132)
  )
  :effect (and
    (not (evaluate comp132))
    (evaluate comp133)
  )
)
(:action evaluate-comp132-all-on
  :parameters ()
  :precondition (and
    (evaluate comp132)
    (not (rebooted comp132))
    (running comp131)
  )
  :effect (and
    (not (evaluate comp132))
    (evaluate comp133)
    (all-on comp132)
  )
)
(:action evaluate-comp132-off-comp131
  :parameters ()
  :precondition (and
    (evaluate comp132)
    (not (rebooted comp132))
    (not (running comp131))
  )
  :effect (and
    (not (evaluate comp132))
    (evaluate comp133)
    (one-off comp132)
  )
)
(:action evaluate-comp133-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp133)
    (rebooted comp133)
  )
  :effect (and
    (not (evaluate comp133))
    (evaluate comp134)
  )
)
(:action evaluate-comp133-all-on
  :parameters ()
  :precondition (and
    (evaluate comp133)
    (not (rebooted comp133))
    (running comp132)
    (running comp212)
  )
  :effect (and
    (not (evaluate comp133))
    (evaluate comp134)
    (all-on comp133)
  )
)
(:action evaluate-comp133-off-comp132
  :parameters ()
  :precondition (and
    (evaluate comp133)
    (not (rebooted comp133))
    (not (running comp132))
  )
  :effect (and
    (not (evaluate comp133))
    (evaluate comp134)
    (one-off comp133)
  )
)
(:action evaluate-comp133-off-comp212
  :parameters ()
  :precondition (and
    (evaluate comp133)
    (not (rebooted comp133))
    (not (running comp212))
  )
  :effect (and
    (not (evaluate comp133))
    (evaluate comp134)
    (one-off comp133)
  )
)
(:action evaluate-comp134-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp134)
    (rebooted comp134)
  )
  :effect (and
    (not (evaluate comp134))
    (evaluate comp135)
  )
)
(:action evaluate-comp134-all-on
  :parameters ()
  :precondition (and
    (evaluate comp134)
    (not (rebooted comp134))
    (running comp133)
    (running comp234)
  )
  :effect (and
    (not (evaluate comp134))
    (evaluate comp135)
    (all-on comp134)
  )
)
(:action evaluate-comp134-off-comp133
  :parameters ()
  :precondition (and
    (evaluate comp134)
    (not (rebooted comp134))
    (not (running comp133))
  )
  :effect (and
    (not (evaluate comp134))
    (evaluate comp135)
    (one-off comp134)
  )
)
(:action evaluate-comp134-off-comp234
  :parameters ()
  :precondition (and
    (evaluate comp134)
    (not (rebooted comp134))
    (not (running comp234))
  )
  :effect (and
    (not (evaluate comp134))
    (evaluate comp135)
    (one-off comp134)
  )
)
(:action evaluate-comp135-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp135)
    (rebooted comp135)
  )
  :effect (and
    (not (evaluate comp135))
    (evaluate comp136)
  )
)
(:action evaluate-comp135-all-on
  :parameters ()
  :precondition (and
    (evaluate comp135)
    (not (rebooted comp135))
    (running comp134)
  )
  :effect (and
    (not (evaluate comp135))
    (evaluate comp136)
    (all-on comp135)
  )
)
(:action evaluate-comp135-off-comp134
  :parameters ()
  :precondition (and
    (evaluate comp135)
    (not (rebooted comp135))
    (not (running comp134))
  )
  :effect (and
    (not (evaluate comp135))
    (evaluate comp136)
    (one-off comp135)
  )
)
(:action evaluate-comp136-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp136)
    (rebooted comp136)
  )
  :effect (and
    (not (evaluate comp136))
    (evaluate comp137)
  )
)
(:action evaluate-comp136-all-on
  :parameters ()
  :precondition (and
    (evaluate comp136)
    (not (rebooted comp136))
    (running comp135)
    (running comp160)
  )
  :effect (and
    (not (evaluate comp136))
    (evaluate comp137)
    (all-on comp136)
  )
)
(:action evaluate-comp136-off-comp135
  :parameters ()
  :precondition (and
    (evaluate comp136)
    (not (rebooted comp136))
    (not (running comp135))
  )
  :effect (and
    (not (evaluate comp136))
    (evaluate comp137)
    (one-off comp136)
  )
)
(:action evaluate-comp136-off-comp160
  :parameters ()
  :precondition (and
    (evaluate comp136)
    (not (rebooted comp136))
    (not (running comp160))
  )
  :effect (and
    (not (evaluate comp136))
    (evaluate comp137)
    (one-off comp136)
  )
)
(:action evaluate-comp137-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp137)
    (rebooted comp137)
  )
  :effect (and
    (not (evaluate comp137))
    (evaluate comp138)
  )
)
(:action evaluate-comp137-all-on
  :parameters ()
  :precondition (and
    (evaluate comp137)
    (not (rebooted comp137))
    (running comp136)
  )
  :effect (and
    (not (evaluate comp137))
    (evaluate comp138)
    (all-on comp137)
  )
)
(:action evaluate-comp137-off-comp136
  :parameters ()
  :precondition (and
    (evaluate comp137)
    (not (rebooted comp137))
    (not (running comp136))
  )
  :effect (and
    (not (evaluate comp137))
    (evaluate comp138)
    (one-off comp137)
  )
)
(:action evaluate-comp138-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp138)
    (rebooted comp138)
  )
  :effect (and
    (not (evaluate comp138))
    (evaluate comp139)
  )
)
(:action evaluate-comp138-all-on
  :parameters ()
  :precondition (and
    (evaluate comp138)
    (not (rebooted comp138))
    (running comp137)
  )
  :effect (and
    (not (evaluate comp138))
    (evaluate comp139)
    (all-on comp138)
  )
)
(:action evaluate-comp138-off-comp137
  :parameters ()
  :precondition (and
    (evaluate comp138)
    (not (rebooted comp138))
    (not (running comp137))
  )
  :effect (and
    (not (evaluate comp138))
    (evaluate comp139)
    (one-off comp138)
  )
)
(:action evaluate-comp139-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp139)
    (rebooted comp139)
  )
  :effect (and
    (not (evaluate comp139))
    (evaluate comp140)
  )
)
(:action evaluate-comp139-all-on
  :parameters ()
  :precondition (and
    (evaluate comp139)
    (not (rebooted comp139))
    (running comp138)
    (running comp208)
  )
  :effect (and
    (not (evaluate comp139))
    (evaluate comp140)
    (all-on comp139)
  )
)
(:action evaluate-comp139-off-comp138
  :parameters ()
  :precondition (and
    (evaluate comp139)
    (not (rebooted comp139))
    (not (running comp138))
  )
  :effect (and
    (not (evaluate comp139))
    (evaluate comp140)
    (one-off comp139)
  )
)
(:action evaluate-comp139-off-comp208
  :parameters ()
  :precondition (and
    (evaluate comp139)
    (not (rebooted comp139))
    (not (running comp208))
  )
  :effect (and
    (not (evaluate comp139))
    (evaluate comp140)
    (one-off comp139)
  )
)
(:action evaluate-comp140-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp140)
    (rebooted comp140)
  )
  :effect (and
    (not (evaluate comp140))
    (evaluate comp141)
  )
)
(:action evaluate-comp140-all-on
  :parameters ()
  :precondition (and
    (evaluate comp140)
    (not (rebooted comp140))
    (running comp98)
    (running comp139)
  )
  :effect (and
    (not (evaluate comp140))
    (evaluate comp141)
    (all-on comp140)
  )
)
(:action evaluate-comp140-off-comp98
  :parameters ()
  :precondition (and
    (evaluate comp140)
    (not (rebooted comp140))
    (not (running comp98))
  )
  :effect (and
    (not (evaluate comp140))
    (evaluate comp141)
    (one-off comp140)
  )
)
(:action evaluate-comp140-off-comp139
  :parameters ()
  :precondition (and
    (evaluate comp140)
    (not (rebooted comp140))
    (not (running comp139))
  )
  :effect (and
    (not (evaluate comp140))
    (evaluate comp141)
    (one-off comp140)
  )
)
(:action evaluate-comp141-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp141)
    (rebooted comp141)
  )
  :effect (and
    (not (evaluate comp141))
    (evaluate comp142)
  )
)
(:action evaluate-comp141-all-on
  :parameters ()
  :precondition (and
    (evaluate comp141)
    (not (rebooted comp141))
    (running comp140)
  )
  :effect (and
    (not (evaluate comp141))
    (evaluate comp142)
    (all-on comp141)
  )
)
(:action evaluate-comp141-off-comp140
  :parameters ()
  :precondition (and
    (evaluate comp141)
    (not (rebooted comp141))
    (not (running comp140))
  )
  :effect (and
    (not (evaluate comp141))
    (evaluate comp142)
    (one-off comp141)
  )
)
(:action evaluate-comp142-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp142)
    (rebooted comp142)
  )
  :effect (and
    (not (evaluate comp142))
    (evaluate comp143)
  )
)
(:action evaluate-comp142-all-on
  :parameters ()
  :precondition (and
    (evaluate comp142)
    (not (rebooted comp142))
    (running comp141)
    (running comp194)
  )
  :effect (and
    (not (evaluate comp142))
    (evaluate comp143)
    (all-on comp142)
  )
)
(:action evaluate-comp142-off-comp141
  :parameters ()
  :precondition (and
    (evaluate comp142)
    (not (rebooted comp142))
    (not (running comp141))
  )
  :effect (and
    (not (evaluate comp142))
    (evaluate comp143)
    (one-off comp142)
  )
)
(:action evaluate-comp142-off-comp194
  :parameters ()
  :precondition (and
    (evaluate comp142)
    (not (rebooted comp142))
    (not (running comp194))
  )
  :effect (and
    (not (evaluate comp142))
    (evaluate comp143)
    (one-off comp142)
  )
)
(:action evaluate-comp143-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp143)
    (rebooted comp143)
  )
  :effect (and
    (not (evaluate comp143))
    (evaluate comp144)
  )
)
(:action evaluate-comp143-all-on
  :parameters ()
  :precondition (and
    (evaluate comp143)
    (not (rebooted comp143))
    (running comp142)
  )
  :effect (and
    (not (evaluate comp143))
    (evaluate comp144)
    (all-on comp143)
  )
)
(:action evaluate-comp143-off-comp142
  :parameters ()
  :precondition (and
    (evaluate comp143)
    (not (rebooted comp143))
    (not (running comp142))
  )
  :effect (and
    (not (evaluate comp143))
    (evaluate comp144)
    (one-off comp143)
  )
)
(:action evaluate-comp144-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp144)
    (rebooted comp144)
  )
  :effect (and
    (not (evaluate comp144))
    (evaluate comp145)
  )
)
(:action evaluate-comp144-all-on
  :parameters ()
  :precondition (and
    (evaluate comp144)
    (not (rebooted comp144))
    (running comp143)
  )
  :effect (and
    (not (evaluate comp144))
    (evaluate comp145)
    (all-on comp144)
  )
)
(:action evaluate-comp144-off-comp143
  :parameters ()
  :precondition (and
    (evaluate comp144)
    (not (rebooted comp144))
    (not (running comp143))
  )
  :effect (and
    (not (evaluate comp144))
    (evaluate comp145)
    (one-off comp144)
  )
)
(:action evaluate-comp145-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp145)
    (rebooted comp145)
  )
  :effect (and
    (not (evaluate comp145))
    (evaluate comp146)
  )
)
(:action evaluate-comp145-all-on
  :parameters ()
  :precondition (and
    (evaluate comp145)
    (not (rebooted comp145))
    (running comp71)
    (running comp144)
  )
  :effect (and
    (not (evaluate comp145))
    (evaluate comp146)
    (all-on comp145)
  )
)
(:action evaluate-comp145-off-comp71
  :parameters ()
  :precondition (and
    (evaluate comp145)
    (not (rebooted comp145))
    (not (running comp71))
  )
  :effect (and
    (not (evaluate comp145))
    (evaluate comp146)
    (one-off comp145)
  )
)
(:action evaluate-comp145-off-comp144
  :parameters ()
  :precondition (and
    (evaluate comp145)
    (not (rebooted comp145))
    (not (running comp144))
  )
  :effect (and
    (not (evaluate comp145))
    (evaluate comp146)
    (one-off comp145)
  )
)
(:action evaluate-comp146-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp146)
    (rebooted comp146)
  )
  :effect (and
    (not (evaluate comp146))
    (evaluate comp147)
  )
)
(:action evaluate-comp146-all-on
  :parameters ()
  :precondition (and
    (evaluate comp146)
    (not (rebooted comp146))
    (running comp60)
    (running comp145)
    (running comp153)
  )
  :effect (and
    (not (evaluate comp146))
    (evaluate comp147)
    (all-on comp146)
  )
)
(:action evaluate-comp146-off-comp60
  :parameters ()
  :precondition (and
    (evaluate comp146)
    (not (rebooted comp146))
    (not (running comp60))
  )
  :effect (and
    (not (evaluate comp146))
    (evaluate comp147)
    (one-off comp146)
  )
)
(:action evaluate-comp146-off-comp145
  :parameters ()
  :precondition (and
    (evaluate comp146)
    (not (rebooted comp146))
    (not (running comp145))
  )
  :effect (and
    (not (evaluate comp146))
    (evaluate comp147)
    (one-off comp146)
  )
)
(:action evaluate-comp146-off-comp153
  :parameters ()
  :precondition (and
    (evaluate comp146)
    (not (rebooted comp146))
    (not (running comp153))
  )
  :effect (and
    (not (evaluate comp146))
    (evaluate comp147)
    (one-off comp146)
  )
)
(:action evaluate-comp147-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp147)
    (rebooted comp147)
  )
  :effect (and
    (not (evaluate comp147))
    (evaluate comp148)
  )
)
(:action evaluate-comp147-all-on
  :parameters ()
  :precondition (and
    (evaluate comp147)
    (not (rebooted comp147))
    (running comp146)
  )
  :effect (and
    (not (evaluate comp147))
    (evaluate comp148)
    (all-on comp147)
  )
)
(:action evaluate-comp147-off-comp146
  :parameters ()
  :precondition (and
    (evaluate comp147)
    (not (rebooted comp147))
    (not (running comp146))
  )
  :effect (and
    (not (evaluate comp147))
    (evaluate comp148)
    (one-off comp147)
  )
)
(:action evaluate-comp148-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp148)
    (rebooted comp148)
  )
  :effect (and
    (not (evaluate comp148))
    (evaluate comp149)
  )
)
(:action evaluate-comp148-all-on
  :parameters ()
  :precondition (and
    (evaluate comp148)
    (not (rebooted comp148))
    (running comp36)
    (running comp143)
    (running comp147)
  )
  :effect (and
    (not (evaluate comp148))
    (evaluate comp149)
    (all-on comp148)
  )
)
(:action evaluate-comp148-off-comp36
  :parameters ()
  :precondition (and
    (evaluate comp148)
    (not (rebooted comp148))
    (not (running comp36))
  )
  :effect (and
    (not (evaluate comp148))
    (evaluate comp149)
    (one-off comp148)
  )
)
(:action evaluate-comp148-off-comp143
  :parameters ()
  :precondition (and
    (evaluate comp148)
    (not (rebooted comp148))
    (not (running comp143))
  )
  :effect (and
    (not (evaluate comp148))
    (evaluate comp149)
    (one-off comp148)
  )
)
(:action evaluate-comp148-off-comp147
  :parameters ()
  :precondition (and
    (evaluate comp148)
    (not (rebooted comp148))
    (not (running comp147))
  )
  :effect (and
    (not (evaluate comp148))
    (evaluate comp149)
    (one-off comp148)
  )
)
(:action evaluate-comp149-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp149)
    (rebooted comp149)
  )
  :effect (and
    (not (evaluate comp149))
    (evaluate comp150)
  )
)
(:action evaluate-comp149-all-on
  :parameters ()
  :precondition (and
    (evaluate comp149)
    (not (rebooted comp149))
    (running comp148)
  )
  :effect (and
    (not (evaluate comp149))
    (evaluate comp150)
    (all-on comp149)
  )
)
(:action evaluate-comp149-off-comp148
  :parameters ()
  :precondition (and
    (evaluate comp149)
    (not (rebooted comp149))
    (not (running comp148))
  )
  :effect (and
    (not (evaluate comp149))
    (evaluate comp150)
    (one-off comp149)
  )
)
(:action evaluate-comp150-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp150)
    (rebooted comp150)
  )
  :effect (and
    (not (evaluate comp150))
    (evaluate comp151)
  )
)
(:action evaluate-comp150-all-on
  :parameters ()
  :precondition (and
    (evaluate comp150)
    (not (rebooted comp150))
    (running comp95)
    (running comp149)
  )
  :effect (and
    (not (evaluate comp150))
    (evaluate comp151)
    (all-on comp150)
  )
)
(:action evaluate-comp150-off-comp95
  :parameters ()
  :precondition (and
    (evaluate comp150)
    (not (rebooted comp150))
    (not (running comp95))
  )
  :effect (and
    (not (evaluate comp150))
    (evaluate comp151)
    (one-off comp150)
  )
)
(:action evaluate-comp150-off-comp149
  :parameters ()
  :precondition (and
    (evaluate comp150)
    (not (rebooted comp150))
    (not (running comp149))
  )
  :effect (and
    (not (evaluate comp150))
    (evaluate comp151)
    (one-off comp150)
  )
)
(:action evaluate-comp151-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp151)
    (rebooted comp151)
  )
  :effect (and
    (not (evaluate comp151))
    (evaluate comp152)
  )
)
(:action evaluate-comp151-all-on
  :parameters ()
  :precondition (and
    (evaluate comp151)
    (not (rebooted comp151))
    (running comp10)
    (running comp150)
  )
  :effect (and
    (not (evaluate comp151))
    (evaluate comp152)
    (all-on comp151)
  )
)
(:action evaluate-comp151-off-comp10
  :parameters ()
  :precondition (and
    (evaluate comp151)
    (not (rebooted comp151))
    (not (running comp10))
  )
  :effect (and
    (not (evaluate comp151))
    (evaluate comp152)
    (one-off comp151)
  )
)
(:action evaluate-comp151-off-comp150
  :parameters ()
  :precondition (and
    (evaluate comp151)
    (not (rebooted comp151))
    (not (running comp150))
  )
  :effect (and
    (not (evaluate comp151))
    (evaluate comp152)
    (one-off comp151)
  )
)
(:action evaluate-comp152-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp152)
    (rebooted comp152)
  )
  :effect (and
    (not (evaluate comp152))
    (evaluate comp153)
  )
)
(:action evaluate-comp152-all-on
  :parameters ()
  :precondition (and
    (evaluate comp152)
    (not (rebooted comp152))
    (running comp151)
  )
  :effect (and
    (not (evaluate comp152))
    (evaluate comp153)
    (all-on comp152)
  )
)
(:action evaluate-comp152-off-comp151
  :parameters ()
  :precondition (and
    (evaluate comp152)
    (not (rebooted comp152))
    (not (running comp151))
  )
  :effect (and
    (not (evaluate comp152))
    (evaluate comp153)
    (one-off comp152)
  )
)
(:action evaluate-comp153-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp153)
    (rebooted comp153)
  )
  :effect (and
    (not (evaluate comp153))
    (evaluate comp154)
  )
)
(:action evaluate-comp153-all-on
  :parameters ()
  :precondition (and
    (evaluate comp153)
    (not (rebooted comp153))
    (running comp152)
  )
  :effect (and
    (not (evaluate comp153))
    (evaluate comp154)
    (all-on comp153)
  )
)
(:action evaluate-comp153-off-comp152
  :parameters ()
  :precondition (and
    (evaluate comp153)
    (not (rebooted comp153))
    (not (running comp152))
  )
  :effect (and
    (not (evaluate comp153))
    (evaluate comp154)
    (one-off comp153)
  )
)
(:action evaluate-comp154-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp154)
    (rebooted comp154)
  )
  :effect (and
    (not (evaluate comp154))
    (evaluate comp155)
  )
)
(:action evaluate-comp154-all-on
  :parameters ()
  :precondition (and
    (evaluate comp154)
    (not (rebooted comp154))
    (running comp153)
  )
  :effect (and
    (not (evaluate comp154))
    (evaluate comp155)
    (all-on comp154)
  )
)
(:action evaluate-comp154-off-comp153
  :parameters ()
  :precondition (and
    (evaluate comp154)
    (not (rebooted comp154))
    (not (running comp153))
  )
  :effect (and
    (not (evaluate comp154))
    (evaluate comp155)
    (one-off comp154)
  )
)
(:action evaluate-comp155-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp155)
    (rebooted comp155)
  )
  :effect (and
    (not (evaluate comp155))
    (evaluate comp156)
  )
)
(:action evaluate-comp155-all-on
  :parameters ()
  :precondition (and
    (evaluate comp155)
    (not (rebooted comp155))
    (running comp154)
  )
  :effect (and
    (not (evaluate comp155))
    (evaluate comp156)
    (all-on comp155)
  )
)
(:action evaluate-comp155-off-comp154
  :parameters ()
  :precondition (and
    (evaluate comp155)
    (not (rebooted comp155))
    (not (running comp154))
  )
  :effect (and
    (not (evaluate comp155))
    (evaluate comp156)
    (one-off comp155)
  )
)
(:action evaluate-comp156-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp156)
    (rebooted comp156)
  )
  :effect (and
    (not (evaluate comp156))
    (evaluate comp157)
  )
)
(:action evaluate-comp156-all-on
  :parameters ()
  :precondition (and
    (evaluate comp156)
    (not (rebooted comp156))
    (running comp155)
  )
  :effect (and
    (not (evaluate comp156))
    (evaluate comp157)
    (all-on comp156)
  )
)
(:action evaluate-comp156-off-comp155
  :parameters ()
  :precondition (and
    (evaluate comp156)
    (not (rebooted comp156))
    (not (running comp155))
  )
  :effect (and
    (not (evaluate comp156))
    (evaluate comp157)
    (one-off comp156)
  )
)
(:action evaluate-comp157-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp157)
    (rebooted comp157)
  )
  :effect (and
    (not (evaluate comp157))
    (evaluate comp158)
  )
)
(:action evaluate-comp157-all-on
  :parameters ()
  :precondition (and
    (evaluate comp157)
    (not (rebooted comp157))
    (running comp156)
  )
  :effect (and
    (not (evaluate comp157))
    (evaluate comp158)
    (all-on comp157)
  )
)
(:action evaluate-comp157-off-comp156
  :parameters ()
  :precondition (and
    (evaluate comp157)
    (not (rebooted comp157))
    (not (running comp156))
  )
  :effect (and
    (not (evaluate comp157))
    (evaluate comp158)
    (one-off comp157)
  )
)
(:action evaluate-comp158-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp158)
    (rebooted comp158)
  )
  :effect (and
    (not (evaluate comp158))
    (evaluate comp159)
  )
)
(:action evaluate-comp158-all-on
  :parameters ()
  :precondition (and
    (evaluate comp158)
    (not (rebooted comp158))
    (running comp157)
    (running comp163)
  )
  :effect (and
    (not (evaluate comp158))
    (evaluate comp159)
    (all-on comp158)
  )
)
(:action evaluate-comp158-off-comp157
  :parameters ()
  :precondition (and
    (evaluate comp158)
    (not (rebooted comp158))
    (not (running comp157))
  )
  :effect (and
    (not (evaluate comp158))
    (evaluate comp159)
    (one-off comp158)
  )
)
(:action evaluate-comp158-off-comp163
  :parameters ()
  :precondition (and
    (evaluate comp158)
    (not (rebooted comp158))
    (not (running comp163))
  )
  :effect (and
    (not (evaluate comp158))
    (evaluate comp159)
    (one-off comp158)
  )
)
(:action evaluate-comp159-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp159)
    (rebooted comp159)
  )
  :effect (and
    (not (evaluate comp159))
    (evaluate comp160)
  )
)
(:action evaluate-comp159-all-on
  :parameters ()
  :precondition (and
    (evaluate comp159)
    (not (rebooted comp159))
    (running comp158)
    (running comp160)
  )
  :effect (and
    (not (evaluate comp159))
    (evaluate comp160)
    (all-on comp159)
  )
)
(:action evaluate-comp159-off-comp158
  :parameters ()
  :precondition (and
    (evaluate comp159)
    (not (rebooted comp159))
    (not (running comp158))
  )
  :effect (and
    (not (evaluate comp159))
    (evaluate comp160)
    (one-off comp159)
  )
)
(:action evaluate-comp159-off-comp160
  :parameters ()
  :precondition (and
    (evaluate comp159)
    (not (rebooted comp159))
    (not (running comp160))
  )
  :effect (and
    (not (evaluate comp159))
    (evaluate comp160)
    (one-off comp159)
  )
)
(:action evaluate-comp160-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp160)
    (rebooted comp160)
  )
  :effect (and
    (not (evaluate comp160))
    (evaluate comp161)
  )
)
(:action evaluate-comp160-all-on
  :parameters ()
  :precondition (and
    (evaluate comp160)
    (not (rebooted comp160))
    (running comp26)
    (running comp107)
    (running comp159)
  )
  :effect (and
    (not (evaluate comp160))
    (evaluate comp161)
    (all-on comp160)
  )
)
(:action evaluate-comp160-off-comp26
  :parameters ()
  :precondition (and
    (evaluate comp160)
    (not (rebooted comp160))
    (not (running comp26))
  )
  :effect (and
    (not (evaluate comp160))
    (evaluate comp161)
    (one-off comp160)
  )
)
(:action evaluate-comp160-off-comp107
  :parameters ()
  :precondition (and
    (evaluate comp160)
    (not (rebooted comp160))
    (not (running comp107))
  )
  :effect (and
    (not (evaluate comp160))
    (evaluate comp161)
    (one-off comp160)
  )
)
(:action evaluate-comp160-off-comp159
  :parameters ()
  :precondition (and
    (evaluate comp160)
    (not (rebooted comp160))
    (not (running comp159))
  )
  :effect (and
    (not (evaluate comp160))
    (evaluate comp161)
    (one-off comp160)
  )
)
(:action evaluate-comp161-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp161)
    (rebooted comp161)
  )
  :effect (and
    (not (evaluate comp161))
    (evaluate comp162)
  )
)
(:action evaluate-comp161-all-on
  :parameters ()
  :precondition (and
    (evaluate comp161)
    (not (rebooted comp161))
    (running comp160)
  )
  :effect (and
    (not (evaluate comp161))
    (evaluate comp162)
    (all-on comp161)
  )
)
(:action evaluate-comp161-off-comp160
  :parameters ()
  :precondition (and
    (evaluate comp161)
    (not (rebooted comp161))
    (not (running comp160))
  )
  :effect (and
    (not (evaluate comp161))
    (evaluate comp162)
    (one-off comp161)
  )
)
(:action evaluate-comp162-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp162)
    (rebooted comp162)
  )
  :effect (and
    (not (evaluate comp162))
    (evaluate comp163)
  )
)
(:action evaluate-comp162-all-on
  :parameters ()
  :precondition (and
    (evaluate comp162)
    (not (rebooted comp162))
    (running comp161)
  )
  :effect (and
    (not (evaluate comp162))
    (evaluate comp163)
    (all-on comp162)
  )
)
(:action evaluate-comp162-off-comp161
  :parameters ()
  :precondition (and
    (evaluate comp162)
    (not (rebooted comp162))
    (not (running comp161))
  )
  :effect (and
    (not (evaluate comp162))
    (evaluate comp163)
    (one-off comp162)
  )
)
(:action evaluate-comp163-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp163)
    (rebooted comp163)
  )
  :effect (and
    (not (evaluate comp163))
    (evaluate comp164)
  )
)
(:action evaluate-comp163-all-on
  :parameters ()
  :precondition (and
    (evaluate comp163)
    (not (rebooted comp163))
    (running comp162)
  )
  :effect (and
    (not (evaluate comp163))
    (evaluate comp164)
    (all-on comp163)
  )
)
(:action evaluate-comp163-off-comp162
  :parameters ()
  :precondition (and
    (evaluate comp163)
    (not (rebooted comp163))
    (not (running comp162))
  )
  :effect (and
    (not (evaluate comp163))
    (evaluate comp164)
    (one-off comp163)
  )
)
(:action evaluate-comp164-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp164)
    (rebooted comp164)
  )
  :effect (and
    (not (evaluate comp164))
    (evaluate comp165)
  )
)
(:action evaluate-comp164-all-on
  :parameters ()
  :precondition (and
    (evaluate comp164)
    (not (rebooted comp164))
    (running comp108)
    (running comp163)
    (running comp229)
  )
  :effect (and
    (not (evaluate comp164))
    (evaluate comp165)
    (all-on comp164)
  )
)
(:action evaluate-comp164-off-comp108
  :parameters ()
  :precondition (and
    (evaluate comp164)
    (not (rebooted comp164))
    (not (running comp108))
  )
  :effect (and
    (not (evaluate comp164))
    (evaluate comp165)
    (one-off comp164)
  )
)
(:action evaluate-comp164-off-comp163
  :parameters ()
  :precondition (and
    (evaluate comp164)
    (not (rebooted comp164))
    (not (running comp163))
  )
  :effect (and
    (not (evaluate comp164))
    (evaluate comp165)
    (one-off comp164)
  )
)
(:action evaluate-comp164-off-comp229
  :parameters ()
  :precondition (and
    (evaluate comp164)
    (not (rebooted comp164))
    (not (running comp229))
  )
  :effect (and
    (not (evaluate comp164))
    (evaluate comp165)
    (one-off comp164)
  )
)
(:action evaluate-comp165-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp165)
    (rebooted comp165)
  )
  :effect (and
    (not (evaluate comp165))
    (evaluate comp166)
  )
)
(:action evaluate-comp165-all-on
  :parameters ()
  :precondition (and
    (evaluate comp165)
    (not (rebooted comp165))
    (running comp164)
  )
  :effect (and
    (not (evaluate comp165))
    (evaluate comp166)
    (all-on comp165)
  )
)
(:action evaluate-comp165-off-comp164
  :parameters ()
  :precondition (and
    (evaluate comp165)
    (not (rebooted comp165))
    (not (running comp164))
  )
  :effect (and
    (not (evaluate comp165))
    (evaluate comp166)
    (one-off comp165)
  )
)
(:action evaluate-comp166-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp166)
    (rebooted comp166)
  )
  :effect (and
    (not (evaluate comp166))
    (evaluate comp167)
  )
)
(:action evaluate-comp166-all-on
  :parameters ()
  :precondition (and
    (evaluate comp166)
    (not (rebooted comp166))
    (running comp165)
    (running comp168)
  )
  :effect (and
    (not (evaluate comp166))
    (evaluate comp167)
    (all-on comp166)
  )
)
(:action evaluate-comp166-off-comp165
  :parameters ()
  :precondition (and
    (evaluate comp166)
    (not (rebooted comp166))
    (not (running comp165))
  )
  :effect (and
    (not (evaluate comp166))
    (evaluate comp167)
    (one-off comp166)
  )
)
(:action evaluate-comp166-off-comp168
  :parameters ()
  :precondition (and
    (evaluate comp166)
    (not (rebooted comp166))
    (not (running comp168))
  )
  :effect (and
    (not (evaluate comp166))
    (evaluate comp167)
    (one-off comp166)
  )
)
(:action evaluate-comp167-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp167)
    (rebooted comp167)
  )
  :effect (and
    (not (evaluate comp167))
    (evaluate comp168)
  )
)
(:action evaluate-comp167-all-on
  :parameters ()
  :precondition (and
    (evaluate comp167)
    (not (rebooted comp167))
    (running comp166)
    (running comp201)
  )
  :effect (and
    (not (evaluate comp167))
    (evaluate comp168)
    (all-on comp167)
  )
)
(:action evaluate-comp167-off-comp166
  :parameters ()
  :precondition (and
    (evaluate comp167)
    (not (rebooted comp167))
    (not (running comp166))
  )
  :effect (and
    (not (evaluate comp167))
    (evaluate comp168)
    (one-off comp167)
  )
)
(:action evaluate-comp167-off-comp201
  :parameters ()
  :precondition (and
    (evaluate comp167)
    (not (rebooted comp167))
    (not (running comp201))
  )
  :effect (and
    (not (evaluate comp167))
    (evaluate comp168)
    (one-off comp167)
  )
)
(:action evaluate-comp168-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp168)
    (rebooted comp168)
  )
  :effect (and
    (not (evaluate comp168))
    (evaluate comp169)
  )
)
(:action evaluate-comp168-all-on
  :parameters ()
  :precondition (and
    (evaluate comp168)
    (not (rebooted comp168))
    (running comp161)
    (running comp167)
  )
  :effect (and
    (not (evaluate comp168))
    (evaluate comp169)
    (all-on comp168)
  )
)
(:action evaluate-comp168-off-comp161
  :parameters ()
  :precondition (and
    (evaluate comp168)
    (not (rebooted comp168))
    (not (running comp161))
  )
  :effect (and
    (not (evaluate comp168))
    (evaluate comp169)
    (one-off comp168)
  )
)
(:action evaluate-comp168-off-comp167
  :parameters ()
  :precondition (and
    (evaluate comp168)
    (not (rebooted comp168))
    (not (running comp167))
  )
  :effect (and
    (not (evaluate comp168))
    (evaluate comp169)
    (one-off comp168)
  )
)
(:action evaluate-comp169-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp169)
    (rebooted comp169)
  )
  :effect (and
    (not (evaluate comp169))
    (evaluate comp170)
  )
)
(:action evaluate-comp169-all-on
  :parameters ()
  :precondition (and
    (evaluate comp169)
    (not (rebooted comp169))
    (running comp168)
  )
  :effect (and
    (not (evaluate comp169))
    (evaluate comp170)
    (all-on comp169)
  )
)
(:action evaluate-comp169-off-comp168
  :parameters ()
  :precondition (and
    (evaluate comp169)
    (not (rebooted comp169))
    (not (running comp168))
  )
  :effect (and
    (not (evaluate comp169))
    (evaluate comp170)
    (one-off comp169)
  )
)
(:action evaluate-comp170-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp170)
    (rebooted comp170)
  )
  :effect (and
    (not (evaluate comp170))
    (evaluate comp171)
  )
)
(:action evaluate-comp170-all-on
  :parameters ()
  :precondition (and
    (evaluate comp170)
    (not (rebooted comp170))
    (running comp114)
    (running comp169)
  )
  :effect (and
    (not (evaluate comp170))
    (evaluate comp171)
    (all-on comp170)
  )
)
(:action evaluate-comp170-off-comp114
  :parameters ()
  :precondition (and
    (evaluate comp170)
    (not (rebooted comp170))
    (not (running comp114))
  )
  :effect (and
    (not (evaluate comp170))
    (evaluate comp171)
    (one-off comp170)
  )
)
(:action evaluate-comp170-off-comp169
  :parameters ()
  :precondition (and
    (evaluate comp170)
    (not (rebooted comp170))
    (not (running comp169))
  )
  :effect (and
    (not (evaluate comp170))
    (evaluate comp171)
    (one-off comp170)
  )
)
(:action evaluate-comp171-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp171)
    (rebooted comp171)
  )
  :effect (and
    (not (evaluate comp171))
    (evaluate comp172)
  )
)
(:action evaluate-comp171-all-on
  :parameters ()
  :precondition (and
    (evaluate comp171)
    (not (rebooted comp171))
    (running comp170)
    (running comp225)
  )
  :effect (and
    (not (evaluate comp171))
    (evaluate comp172)
    (all-on comp171)
  )
)
(:action evaluate-comp171-off-comp170
  :parameters ()
  :precondition (and
    (evaluate comp171)
    (not (rebooted comp171))
    (not (running comp170))
  )
  :effect (and
    (not (evaluate comp171))
    (evaluate comp172)
    (one-off comp171)
  )
)
(:action evaluate-comp171-off-comp225
  :parameters ()
  :precondition (and
    (evaluate comp171)
    (not (rebooted comp171))
    (not (running comp225))
  )
  :effect (and
    (not (evaluate comp171))
    (evaluate comp172)
    (one-off comp171)
  )
)
(:action evaluate-comp172-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp172)
    (rebooted comp172)
  )
  :effect (and
    (not (evaluate comp172))
    (evaluate comp173)
  )
)
(:action evaluate-comp172-all-on
  :parameters ()
  :precondition (and
    (evaluate comp172)
    (not (rebooted comp172))
    (running comp171)
  )
  :effect (and
    (not (evaluate comp172))
    (evaluate comp173)
    (all-on comp172)
  )
)
(:action evaluate-comp172-off-comp171
  :parameters ()
  :precondition (and
    (evaluate comp172)
    (not (rebooted comp172))
    (not (running comp171))
  )
  :effect (and
    (not (evaluate comp172))
    (evaluate comp173)
    (one-off comp172)
  )
)
(:action evaluate-comp173-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp173)
    (rebooted comp173)
  )
  :effect (and
    (not (evaluate comp173))
    (evaluate comp174)
  )
)
(:action evaluate-comp173-all-on
  :parameters ()
  :precondition (and
    (evaluate comp173)
    (not (rebooted comp173))
    (running comp172)
  )
  :effect (and
    (not (evaluate comp173))
    (evaluate comp174)
    (all-on comp173)
  )
)
(:action evaluate-comp173-off-comp172
  :parameters ()
  :precondition (and
    (evaluate comp173)
    (not (rebooted comp173))
    (not (running comp172))
  )
  :effect (and
    (not (evaluate comp173))
    (evaluate comp174)
    (one-off comp173)
  )
)
(:action evaluate-comp174-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp174)
    (rebooted comp174)
  )
  :effect (and
    (not (evaluate comp174))
    (evaluate comp175)
  )
)
(:action evaluate-comp174-all-on
  :parameters ()
  :precondition (and
    (evaluate comp174)
    (not (rebooted comp174))
    (running comp173)
  )
  :effect (and
    (not (evaluate comp174))
    (evaluate comp175)
    (all-on comp174)
  )
)
(:action evaluate-comp174-off-comp173
  :parameters ()
  :precondition (and
    (evaluate comp174)
    (not (rebooted comp174))
    (not (running comp173))
  )
  :effect (and
    (not (evaluate comp174))
    (evaluate comp175)
    (one-off comp174)
  )
)
(:action evaluate-comp175-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp175)
    (rebooted comp175)
  )
  :effect (and
    (not (evaluate comp175))
    (evaluate comp176)
  )
)
(:action evaluate-comp175-all-on
  :parameters ()
  :precondition (and
    (evaluate comp175)
    (not (rebooted comp175))
    (running comp174)
  )
  :effect (and
    (not (evaluate comp175))
    (evaluate comp176)
    (all-on comp175)
  )
)
(:action evaluate-comp175-off-comp174
  :parameters ()
  :precondition (and
    (evaluate comp175)
    (not (rebooted comp175))
    (not (running comp174))
  )
  :effect (and
    (not (evaluate comp175))
    (evaluate comp176)
    (one-off comp175)
  )
)
(:action evaluate-comp176-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp176)
    (rebooted comp176)
  )
  :effect (and
    (not (evaluate comp176))
    (evaluate comp177)
  )
)
(:action evaluate-comp176-all-on
  :parameters ()
  :precondition (and
    (evaluate comp176)
    (not (rebooted comp176))
    (running comp175)
  )
  :effect (and
    (not (evaluate comp176))
    (evaluate comp177)
    (all-on comp176)
  )
)
(:action evaluate-comp176-off-comp175
  :parameters ()
  :precondition (and
    (evaluate comp176)
    (not (rebooted comp176))
    (not (running comp175))
  )
  :effect (and
    (not (evaluate comp176))
    (evaluate comp177)
    (one-off comp176)
  )
)
(:action evaluate-comp177-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp177)
    (rebooted comp177)
  )
  :effect (and
    (not (evaluate comp177))
    (evaluate comp178)
  )
)
(:action evaluate-comp177-all-on
  :parameters ()
  :precondition (and
    (evaluate comp177)
    (not (rebooted comp177))
    (running comp25)
    (running comp127)
    (running comp176)
  )
  :effect (and
    (not (evaluate comp177))
    (evaluate comp178)
    (all-on comp177)
  )
)
(:action evaluate-comp177-off-comp25
  :parameters ()
  :precondition (and
    (evaluate comp177)
    (not (rebooted comp177))
    (not (running comp25))
  )
  :effect (and
    (not (evaluate comp177))
    (evaluate comp178)
    (one-off comp177)
  )
)
(:action evaluate-comp177-off-comp127
  :parameters ()
  :precondition (and
    (evaluate comp177)
    (not (rebooted comp177))
    (not (running comp127))
  )
  :effect (and
    (not (evaluate comp177))
    (evaluate comp178)
    (one-off comp177)
  )
)
(:action evaluate-comp177-off-comp176
  :parameters ()
  :precondition (and
    (evaluate comp177)
    (not (rebooted comp177))
    (not (running comp176))
  )
  :effect (and
    (not (evaluate comp177))
    (evaluate comp178)
    (one-off comp177)
  )
)
(:action evaluate-comp178-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp178)
    (rebooted comp178)
  )
  :effect (and
    (not (evaluate comp178))
    (evaluate comp179)
  )
)
(:action evaluate-comp178-all-on
  :parameters ()
  :precondition (and
    (evaluate comp178)
    (not (rebooted comp178))
    (running comp177)
  )
  :effect (and
    (not (evaluate comp178))
    (evaluate comp179)
    (all-on comp178)
  )
)
(:action evaluate-comp178-off-comp177
  :parameters ()
  :precondition (and
    (evaluate comp178)
    (not (rebooted comp178))
    (not (running comp177))
  )
  :effect (and
    (not (evaluate comp178))
    (evaluate comp179)
    (one-off comp178)
  )
)
(:action evaluate-comp179-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp179)
    (rebooted comp179)
  )
  :effect (and
    (not (evaluate comp179))
    (evaluate comp180)
  )
)
(:action evaluate-comp179-all-on
  :parameters ()
  :precondition (and
    (evaluate comp179)
    (not (rebooted comp179))
    (running comp178)
  )
  :effect (and
    (not (evaluate comp179))
    (evaluate comp180)
    (all-on comp179)
  )
)
(:action evaluate-comp179-off-comp178
  :parameters ()
  :precondition (and
    (evaluate comp179)
    (not (rebooted comp179))
    (not (running comp178))
  )
  :effect (and
    (not (evaluate comp179))
    (evaluate comp180)
    (one-off comp179)
  )
)
(:action evaluate-comp180-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp180)
    (rebooted comp180)
  )
  :effect (and
    (not (evaluate comp180))
    (evaluate comp181)
  )
)
(:action evaluate-comp180-all-on
  :parameters ()
  :precondition (and
    (evaluate comp180)
    (not (rebooted comp180))
    (running comp179)
  )
  :effect (and
    (not (evaluate comp180))
    (evaluate comp181)
    (all-on comp180)
  )
)
(:action evaluate-comp180-off-comp179
  :parameters ()
  :precondition (and
    (evaluate comp180)
    (not (rebooted comp180))
    (not (running comp179))
  )
  :effect (and
    (not (evaluate comp180))
    (evaluate comp181)
    (one-off comp180)
  )
)
(:action evaluate-comp181-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp181)
    (rebooted comp181)
  )
  :effect (and
    (not (evaluate comp181))
    (evaluate comp182)
  )
)
(:action evaluate-comp181-all-on
  :parameters ()
  :precondition (and
    (evaluate comp181)
    (not (rebooted comp181))
    (running comp180)
  )
  :effect (and
    (not (evaluate comp181))
    (evaluate comp182)
    (all-on comp181)
  )
)
(:action evaluate-comp181-off-comp180
  :parameters ()
  :precondition (and
    (evaluate comp181)
    (not (rebooted comp181))
    (not (running comp180))
  )
  :effect (and
    (not (evaluate comp181))
    (evaluate comp182)
    (one-off comp181)
  )
)
(:action evaluate-comp182-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp182)
    (rebooted comp182)
  )
  :effect (and
    (not (evaluate comp182))
    (evaluate comp183)
  )
)
(:action evaluate-comp182-all-on
  :parameters ()
  :precondition (and
    (evaluate comp182)
    (not (rebooted comp182))
    (running comp181)
  )
  :effect (and
    (not (evaluate comp182))
    (evaluate comp183)
    (all-on comp182)
  )
)
(:action evaluate-comp182-off-comp181
  :parameters ()
  :precondition (and
    (evaluate comp182)
    (not (rebooted comp182))
    (not (running comp181))
  )
  :effect (and
    (not (evaluate comp182))
    (evaluate comp183)
    (one-off comp182)
  )
)
(:action evaluate-comp183-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp183)
    (rebooted comp183)
  )
  :effect (and
    (not (evaluate comp183))
    (evaluate comp184)
  )
)
(:action evaluate-comp183-all-on
  :parameters ()
  :precondition (and
    (evaluate comp183)
    (not (rebooted comp183))
    (running comp78)
    (running comp182)
  )
  :effect (and
    (not (evaluate comp183))
    (evaluate comp184)
    (all-on comp183)
  )
)
(:action evaluate-comp183-off-comp78
  :parameters ()
  :precondition (and
    (evaluate comp183)
    (not (rebooted comp183))
    (not (running comp78))
  )
  :effect (and
    (not (evaluate comp183))
    (evaluate comp184)
    (one-off comp183)
  )
)
(:action evaluate-comp183-off-comp182
  :parameters ()
  :precondition (and
    (evaluate comp183)
    (not (rebooted comp183))
    (not (running comp182))
  )
  :effect (and
    (not (evaluate comp183))
    (evaluate comp184)
    (one-off comp183)
  )
)
(:action evaluate-comp184-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp184)
    (rebooted comp184)
  )
  :effect (and
    (not (evaluate comp184))
    (evaluate comp185)
  )
)
(:action evaluate-comp184-all-on
  :parameters ()
  :precondition (and
    (evaluate comp184)
    (not (rebooted comp184))
    (running comp183)
  )
  :effect (and
    (not (evaluate comp184))
    (evaluate comp185)
    (all-on comp184)
  )
)
(:action evaluate-comp184-off-comp183
  :parameters ()
  :precondition (and
    (evaluate comp184)
    (not (rebooted comp184))
    (not (running comp183))
  )
  :effect (and
    (not (evaluate comp184))
    (evaluate comp185)
    (one-off comp184)
  )
)
(:action evaluate-comp185-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp185)
    (rebooted comp185)
  )
  :effect (and
    (not (evaluate comp185))
    (evaluate comp186)
  )
)
(:action evaluate-comp185-all-on
  :parameters ()
  :precondition (and
    (evaluate comp185)
    (not (rebooted comp185))
    (running comp184)
  )
  :effect (and
    (not (evaluate comp185))
    (evaluate comp186)
    (all-on comp185)
  )
)
(:action evaluate-comp185-off-comp184
  :parameters ()
  :precondition (and
    (evaluate comp185)
    (not (rebooted comp185))
    (not (running comp184))
  )
  :effect (and
    (not (evaluate comp185))
    (evaluate comp186)
    (one-off comp185)
  )
)
(:action evaluate-comp186-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp186)
    (rebooted comp186)
  )
  :effect (and
    (not (evaluate comp186))
    (evaluate comp187)
  )
)
(:action evaluate-comp186-all-on
  :parameters ()
  :precondition (and
    (evaluate comp186)
    (not (rebooted comp186))
    (running comp185)
  )
  :effect (and
    (not (evaluate comp186))
    (evaluate comp187)
    (all-on comp186)
  )
)
(:action evaluate-comp186-off-comp185
  :parameters ()
  :precondition (and
    (evaluate comp186)
    (not (rebooted comp186))
    (not (running comp185))
  )
  :effect (and
    (not (evaluate comp186))
    (evaluate comp187)
    (one-off comp186)
  )
)
(:action evaluate-comp187-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp187)
    (rebooted comp187)
  )
  :effect (and
    (not (evaluate comp187))
    (evaluate comp188)
  )
)
(:action evaluate-comp187-all-on
  :parameters ()
  :precondition (and
    (evaluate comp187)
    (not (rebooted comp187))
    (running comp186)
  )
  :effect (and
    (not (evaluate comp187))
    (evaluate comp188)
    (all-on comp187)
  )
)
(:action evaluate-comp187-off-comp186
  :parameters ()
  :precondition (and
    (evaluate comp187)
    (not (rebooted comp187))
    (not (running comp186))
  )
  :effect (and
    (not (evaluate comp187))
    (evaluate comp188)
    (one-off comp187)
  )
)
(:action evaluate-comp188-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp188)
    (rebooted comp188)
  )
  :effect (and
    (not (evaluate comp188))
    (evaluate comp189)
  )
)
(:action evaluate-comp188-all-on
  :parameters ()
  :precondition (and
    (evaluate comp188)
    (not (rebooted comp188))
    (running comp187)
  )
  :effect (and
    (not (evaluate comp188))
    (evaluate comp189)
    (all-on comp188)
  )
)
(:action evaluate-comp188-off-comp187
  :parameters ()
  :precondition (and
    (evaluate comp188)
    (not (rebooted comp188))
    (not (running comp187))
  )
  :effect (and
    (not (evaluate comp188))
    (evaluate comp189)
    (one-off comp188)
  )
)
(:action evaluate-comp189-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp189)
    (rebooted comp189)
  )
  :effect (and
    (not (evaluate comp189))
    (evaluate comp190)
  )
)
(:action evaluate-comp189-all-on
  :parameters ()
  :precondition (and
    (evaluate comp189)
    (not (rebooted comp189))
    (running comp188)
  )
  :effect (and
    (not (evaluate comp189))
    (evaluate comp190)
    (all-on comp189)
  )
)
(:action evaluate-comp189-off-comp188
  :parameters ()
  :precondition (and
    (evaluate comp189)
    (not (rebooted comp189))
    (not (running comp188))
  )
  :effect (and
    (not (evaluate comp189))
    (evaluate comp190)
    (one-off comp189)
  )
)
(:action evaluate-comp190-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp190)
    (rebooted comp190)
  )
  :effect (and
    (not (evaluate comp190))
    (evaluate comp191)
  )
)
(:action evaluate-comp190-all-on
  :parameters ()
  :precondition (and
    (evaluate comp190)
    (not (rebooted comp190))
    (running comp189)
  )
  :effect (and
    (not (evaluate comp190))
    (evaluate comp191)
    (all-on comp190)
  )
)
(:action evaluate-comp190-off-comp189
  :parameters ()
  :precondition (and
    (evaluate comp190)
    (not (rebooted comp190))
    (not (running comp189))
  )
  :effect (and
    (not (evaluate comp190))
    (evaluate comp191)
    (one-off comp190)
  )
)
(:action evaluate-comp191-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp191)
    (rebooted comp191)
  )
  :effect (and
    (not (evaluate comp191))
    (evaluate comp192)
  )
)
(:action evaluate-comp191-all-on
  :parameters ()
  :precondition (and
    (evaluate comp191)
    (not (rebooted comp191))
    (running comp190)
  )
  :effect (and
    (not (evaluate comp191))
    (evaluate comp192)
    (all-on comp191)
  )
)
(:action evaluate-comp191-off-comp190
  :parameters ()
  :precondition (and
    (evaluate comp191)
    (not (rebooted comp191))
    (not (running comp190))
  )
  :effect (and
    (not (evaluate comp191))
    (evaluate comp192)
    (one-off comp191)
  )
)
(:action evaluate-comp192-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp192)
    (rebooted comp192)
  )
  :effect (and
    (not (evaluate comp192))
    (evaluate comp193)
  )
)
(:action evaluate-comp192-all-on
  :parameters ()
  :precondition (and
    (evaluate comp192)
    (not (rebooted comp192))
    (running comp20)
    (running comp191)
  )
  :effect (and
    (not (evaluate comp192))
    (evaluate comp193)
    (all-on comp192)
  )
)
(:action evaluate-comp192-off-comp20
  :parameters ()
  :precondition (and
    (evaluate comp192)
    (not (rebooted comp192))
    (not (running comp20))
  )
  :effect (and
    (not (evaluate comp192))
    (evaluate comp193)
    (one-off comp192)
  )
)
(:action evaluate-comp192-off-comp191
  :parameters ()
  :precondition (and
    (evaluate comp192)
    (not (rebooted comp192))
    (not (running comp191))
  )
  :effect (and
    (not (evaluate comp192))
    (evaluate comp193)
    (one-off comp192)
  )
)
(:action evaluate-comp193-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp193)
    (rebooted comp193)
  )
  :effect (and
    (not (evaluate comp193))
    (evaluate comp194)
  )
)
(:action evaluate-comp193-all-on
  :parameters ()
  :precondition (and
    (evaluate comp193)
    (not (rebooted comp193))
    (running comp192)
  )
  :effect (and
    (not (evaluate comp193))
    (evaluate comp194)
    (all-on comp193)
  )
)
(:action evaluate-comp193-off-comp192
  :parameters ()
  :precondition (and
    (evaluate comp193)
    (not (rebooted comp193))
    (not (running comp192))
  )
  :effect (and
    (not (evaluate comp193))
    (evaluate comp194)
    (one-off comp193)
  )
)
(:action evaluate-comp194-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp194)
    (rebooted comp194)
  )
  :effect (and
    (not (evaluate comp194))
    (evaluate comp195)
  )
)
(:action evaluate-comp194-all-on
  :parameters ()
  :precondition (and
    (evaluate comp194)
    (not (rebooted comp194))
    (running comp193)
  )
  :effect (and
    (not (evaluate comp194))
    (evaluate comp195)
    (all-on comp194)
  )
)
(:action evaluate-comp194-off-comp193
  :parameters ()
  :precondition (and
    (evaluate comp194)
    (not (rebooted comp194))
    (not (running comp193))
  )
  :effect (and
    (not (evaluate comp194))
    (evaluate comp195)
    (one-off comp194)
  )
)
(:action evaluate-comp195-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp195)
    (rebooted comp195)
  )
  :effect (and
    (not (evaluate comp195))
    (evaluate comp196)
  )
)
(:action evaluate-comp195-all-on
  :parameters ()
  :precondition (and
    (evaluate comp195)
    (not (rebooted comp195))
    (running comp194)
  )
  :effect (and
    (not (evaluate comp195))
    (evaluate comp196)
    (all-on comp195)
  )
)
(:action evaluate-comp195-off-comp194
  :parameters ()
  :precondition (and
    (evaluate comp195)
    (not (rebooted comp195))
    (not (running comp194))
  )
  :effect (and
    (not (evaluate comp195))
    (evaluate comp196)
    (one-off comp195)
  )
)
(:action evaluate-comp196-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp196)
    (rebooted comp196)
  )
  :effect (and
    (not (evaluate comp196))
    (evaluate comp197)
  )
)
(:action evaluate-comp196-all-on
  :parameters ()
  :precondition (and
    (evaluate comp196)
    (not (rebooted comp196))
    (running comp195)
  )
  :effect (and
    (not (evaluate comp196))
    (evaluate comp197)
    (all-on comp196)
  )
)
(:action evaluate-comp196-off-comp195
  :parameters ()
  :precondition (and
    (evaluate comp196)
    (not (rebooted comp196))
    (not (running comp195))
  )
  :effect (and
    (not (evaluate comp196))
    (evaluate comp197)
    (one-off comp196)
  )
)
(:action evaluate-comp197-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp197)
    (rebooted comp197)
  )
  :effect (and
    (not (evaluate comp197))
    (evaluate comp198)
  )
)
(:action evaluate-comp197-all-on
  :parameters ()
  :precondition (and
    (evaluate comp197)
    (not (rebooted comp197))
    (running comp37)
    (running comp129)
    (running comp196)
  )
  :effect (and
    (not (evaluate comp197))
    (evaluate comp198)
    (all-on comp197)
  )
)
(:action evaluate-comp197-off-comp37
  :parameters ()
  :precondition (and
    (evaluate comp197)
    (not (rebooted comp197))
    (not (running comp37))
  )
  :effect (and
    (not (evaluate comp197))
    (evaluate comp198)
    (one-off comp197)
  )
)
(:action evaluate-comp197-off-comp129
  :parameters ()
  :precondition (and
    (evaluate comp197)
    (not (rebooted comp197))
    (not (running comp129))
  )
  :effect (and
    (not (evaluate comp197))
    (evaluate comp198)
    (one-off comp197)
  )
)
(:action evaluate-comp197-off-comp196
  :parameters ()
  :precondition (and
    (evaluate comp197)
    (not (rebooted comp197))
    (not (running comp196))
  )
  :effect (and
    (not (evaluate comp197))
    (evaluate comp198)
    (one-off comp197)
  )
)
(:action evaluate-comp198-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp198)
    (rebooted comp198)
  )
  :effect (and
    (not (evaluate comp198))
    (evaluate comp199)
  )
)
(:action evaluate-comp198-all-on
  :parameters ()
  :precondition (and
    (evaluate comp198)
    (not (rebooted comp198))
    (running comp197)
  )
  :effect (and
    (not (evaluate comp198))
    (evaluate comp199)
    (all-on comp198)
  )
)
(:action evaluate-comp198-off-comp197
  :parameters ()
  :precondition (and
    (evaluate comp198)
    (not (rebooted comp198))
    (not (running comp197))
  )
  :effect (and
    (not (evaluate comp198))
    (evaluate comp199)
    (one-off comp198)
  )
)
(:action evaluate-comp199-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp199)
    (rebooted comp199)
  )
  :effect (and
    (not (evaluate comp199))
    (evaluate comp200)
  )
)
(:action evaluate-comp199-all-on
  :parameters ()
  :precondition (and
    (evaluate comp199)
    (not (rebooted comp199))
    (running comp198)
  )
  :effect (and
    (not (evaluate comp199))
    (evaluate comp200)
    (all-on comp199)
  )
)
(:action evaluate-comp199-off-comp198
  :parameters ()
  :precondition (and
    (evaluate comp199)
    (not (rebooted comp199))
    (not (running comp198))
  )
  :effect (and
    (not (evaluate comp199))
    (evaluate comp200)
    (one-off comp199)
  )
)
(:action evaluate-comp200-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp200)
    (rebooted comp200)
  )
  :effect (and
    (not (evaluate comp200))
    (evaluate comp201)
  )
)
(:action evaluate-comp200-all-on
  :parameters ()
  :precondition (and
    (evaluate comp200)
    (not (rebooted comp200))
    (running comp199)
  )
  :effect (and
    (not (evaluate comp200))
    (evaluate comp201)
    (all-on comp200)
  )
)
(:action evaluate-comp200-off-comp199
  :parameters ()
  :precondition (and
    (evaluate comp200)
    (not (rebooted comp200))
    (not (running comp199))
  )
  :effect (and
    (not (evaluate comp200))
    (evaluate comp201)
    (one-off comp200)
  )
)
(:action evaluate-comp201-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp201)
    (rebooted comp201)
  )
  :effect (and
    (not (evaluate comp201))
    (evaluate comp202)
  )
)
(:action evaluate-comp201-all-on
  :parameters ()
  :precondition (and
    (evaluate comp201)
    (not (rebooted comp201))
    (running comp200)
  )
  :effect (and
    (not (evaluate comp201))
    (evaluate comp202)
    (all-on comp201)
  )
)
(:action evaluate-comp201-off-comp200
  :parameters ()
  :precondition (and
    (evaluate comp201)
    (not (rebooted comp201))
    (not (running comp200))
  )
  :effect (and
    (not (evaluate comp201))
    (evaluate comp202)
    (one-off comp201)
  )
)
(:action evaluate-comp202-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp202)
    (rebooted comp202)
  )
  :effect (and
    (not (evaluate comp202))
    (evaluate comp203)
  )
)
(:action evaluate-comp202-all-on
  :parameters ()
  :precondition (and
    (evaluate comp202)
    (not (rebooted comp202))
    (running comp201)
  )
  :effect (and
    (not (evaluate comp202))
    (evaluate comp203)
    (all-on comp202)
  )
)
(:action evaluate-comp202-off-comp201
  :parameters ()
  :precondition (and
    (evaluate comp202)
    (not (rebooted comp202))
    (not (running comp201))
  )
  :effect (and
    (not (evaluate comp202))
    (evaluate comp203)
    (one-off comp202)
  )
)
(:action evaluate-comp203-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp203)
    (rebooted comp203)
  )
  :effect (and
    (not (evaluate comp203))
    (evaluate comp204)
  )
)
(:action evaluate-comp203-all-on
  :parameters ()
  :precondition (and
    (evaluate comp203)
    (not (rebooted comp203))
    (running comp202)
  )
  :effect (and
    (not (evaluate comp203))
    (evaluate comp204)
    (all-on comp203)
  )
)
(:action evaluate-comp203-off-comp202
  :parameters ()
  :precondition (and
    (evaluate comp203)
    (not (rebooted comp203))
    (not (running comp202))
  )
  :effect (and
    (not (evaluate comp203))
    (evaluate comp204)
    (one-off comp203)
  )
)
(:action evaluate-comp204-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp204)
    (rebooted comp204)
  )
  :effect (and
    (not (evaluate comp204))
    (evaluate comp205)
  )
)
(:action evaluate-comp204-all-on
  :parameters ()
  :precondition (and
    (evaluate comp204)
    (not (rebooted comp204))
    (running comp162)
    (running comp203)
  )
  :effect (and
    (not (evaluate comp204))
    (evaluate comp205)
    (all-on comp204)
  )
)
(:action evaluate-comp204-off-comp162
  :parameters ()
  :precondition (and
    (evaluate comp204)
    (not (rebooted comp204))
    (not (running comp162))
  )
  :effect (and
    (not (evaluate comp204))
    (evaluate comp205)
    (one-off comp204)
  )
)
(:action evaluate-comp204-off-comp203
  :parameters ()
  :precondition (and
    (evaluate comp204)
    (not (rebooted comp204))
    (not (running comp203))
  )
  :effect (and
    (not (evaluate comp204))
    (evaluate comp205)
    (one-off comp204)
  )
)
(:action evaluate-comp205-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp205)
    (rebooted comp205)
  )
  :effect (and
    (not (evaluate comp205))
    (evaluate comp206)
  )
)
(:action evaluate-comp205-all-on
  :parameters ()
  :precondition (and
    (evaluate comp205)
    (not (rebooted comp205))
    (running comp204)
  )
  :effect (and
    (not (evaluate comp205))
    (evaluate comp206)
    (all-on comp205)
  )
)
(:action evaluate-comp205-off-comp204
  :parameters ()
  :precondition (and
    (evaluate comp205)
    (not (rebooted comp205))
    (not (running comp204))
  )
  :effect (and
    (not (evaluate comp205))
    (evaluate comp206)
    (one-off comp205)
  )
)
(:action evaluate-comp206-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp206)
    (rebooted comp206)
  )
  :effect (and
    (not (evaluate comp206))
    (evaluate comp207)
  )
)
(:action evaluate-comp206-all-on
  :parameters ()
  :precondition (and
    (evaluate comp206)
    (not (rebooted comp206))
    (running comp205)
  )
  :effect (and
    (not (evaluate comp206))
    (evaluate comp207)
    (all-on comp206)
  )
)
(:action evaluate-comp206-off-comp205
  :parameters ()
  :precondition (and
    (evaluate comp206)
    (not (rebooted comp206))
    (not (running comp205))
  )
  :effect (and
    (not (evaluate comp206))
    (evaluate comp207)
    (one-off comp206)
  )
)
(:action evaluate-comp207-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp207)
    (rebooted comp207)
  )
  :effect (and
    (not (evaluate comp207))
    (evaluate comp208)
  )
)
(:action evaluate-comp207-all-on
  :parameters ()
  :precondition (and
    (evaluate comp207)
    (not (rebooted comp207))
    (running comp206)
  )
  :effect (and
    (not (evaluate comp207))
    (evaluate comp208)
    (all-on comp207)
  )
)
(:action evaluate-comp207-off-comp206
  :parameters ()
  :precondition (and
    (evaluate comp207)
    (not (rebooted comp207))
    (not (running comp206))
  )
  :effect (and
    (not (evaluate comp207))
    (evaluate comp208)
    (one-off comp207)
  )
)
(:action evaluate-comp208-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp208)
    (rebooted comp208)
  )
  :effect (and
    (not (evaluate comp208))
    (evaluate comp209)
  )
)
(:action evaluate-comp208-all-on
  :parameters ()
  :precondition (and
    (evaluate comp208)
    (not (rebooted comp208))
    (running comp207)
  )
  :effect (and
    (not (evaluate comp208))
    (evaluate comp209)
    (all-on comp208)
  )
)
(:action evaluate-comp208-off-comp207
  :parameters ()
  :precondition (and
    (evaluate comp208)
    (not (rebooted comp208))
    (not (running comp207))
  )
  :effect (and
    (not (evaluate comp208))
    (evaluate comp209)
    (one-off comp208)
  )
)
(:action evaluate-comp209-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp209)
    (rebooted comp209)
  )
  :effect (and
    (not (evaluate comp209))
    (evaluate comp210)
  )
)
(:action evaluate-comp209-all-on
  :parameters ()
  :precondition (and
    (evaluate comp209)
    (not (rebooted comp209))
    (running comp144)
    (running comp208)
  )
  :effect (and
    (not (evaluate comp209))
    (evaluate comp210)
    (all-on comp209)
  )
)
(:action evaluate-comp209-off-comp144
  :parameters ()
  :precondition (and
    (evaluate comp209)
    (not (rebooted comp209))
    (not (running comp144))
  )
  :effect (and
    (not (evaluate comp209))
    (evaluate comp210)
    (one-off comp209)
  )
)
(:action evaluate-comp209-off-comp208
  :parameters ()
  :precondition (and
    (evaluate comp209)
    (not (rebooted comp209))
    (not (running comp208))
  )
  :effect (and
    (not (evaluate comp209))
    (evaluate comp210)
    (one-off comp209)
  )
)
(:action evaluate-comp210-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp210)
    (rebooted comp210)
  )
  :effect (and
    (not (evaluate comp210))
    (evaluate comp211)
  )
)
(:action evaluate-comp210-all-on
  :parameters ()
  :precondition (and
    (evaluate comp210)
    (not (rebooted comp210))
    (running comp209)
  )
  :effect (and
    (not (evaluate comp210))
    (evaluate comp211)
    (all-on comp210)
  )
)
(:action evaluate-comp210-off-comp209
  :parameters ()
  :precondition (and
    (evaluate comp210)
    (not (rebooted comp210))
    (not (running comp209))
  )
  :effect (and
    (not (evaluate comp210))
    (evaluate comp211)
    (one-off comp210)
  )
)
(:action evaluate-comp211-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp211)
    (rebooted comp211)
  )
  :effect (and
    (not (evaluate comp211))
    (evaluate comp212)
  )
)
(:action evaluate-comp211-all-on
  :parameters ()
  :precondition (and
    (evaluate comp211)
    (not (rebooted comp211))
    (running comp210)
  )
  :effect (and
    (not (evaluate comp211))
    (evaluate comp212)
    (all-on comp211)
  )
)
(:action evaluate-comp211-off-comp210
  :parameters ()
  :precondition (and
    (evaluate comp211)
    (not (rebooted comp211))
    (not (running comp210))
  )
  :effect (and
    (not (evaluate comp211))
    (evaluate comp212)
    (one-off comp211)
  )
)
(:action evaluate-comp212-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp212)
    (rebooted comp212)
  )
  :effect (and
    (not (evaluate comp212))
    (evaluate comp213)
  )
)
(:action evaluate-comp212-all-on
  :parameters ()
  :precondition (and
    (evaluate comp212)
    (not (rebooted comp212))
    (running comp211)
  )
  :effect (and
    (not (evaluate comp212))
    (evaluate comp213)
    (all-on comp212)
  )
)
(:action evaluate-comp212-off-comp211
  :parameters ()
  :precondition (and
    (evaluate comp212)
    (not (rebooted comp212))
    (not (running comp211))
  )
  :effect (and
    (not (evaluate comp212))
    (evaluate comp213)
    (one-off comp212)
  )
)
(:action evaluate-comp213-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp213)
    (rebooted comp213)
  )
  :effect (and
    (not (evaluate comp213))
    (evaluate comp214)
  )
)
(:action evaluate-comp213-all-on
  :parameters ()
  :precondition (and
    (evaluate comp213)
    (not (rebooted comp213))
    (running comp25)
    (running comp212)
  )
  :effect (and
    (not (evaluate comp213))
    (evaluate comp214)
    (all-on comp213)
  )
)
(:action evaluate-comp213-off-comp25
  :parameters ()
  :precondition (and
    (evaluate comp213)
    (not (rebooted comp213))
    (not (running comp25))
  )
  :effect (and
    (not (evaluate comp213))
    (evaluate comp214)
    (one-off comp213)
  )
)
(:action evaluate-comp213-off-comp212
  :parameters ()
  :precondition (and
    (evaluate comp213)
    (not (rebooted comp213))
    (not (running comp212))
  )
  :effect (and
    (not (evaluate comp213))
    (evaluate comp214)
    (one-off comp213)
  )
)
(:action evaluate-comp214-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp214)
    (rebooted comp214)
  )
  :effect (and
    (not (evaluate comp214))
    (evaluate comp215)
  )
)
(:action evaluate-comp214-all-on
  :parameters ()
  :precondition (and
    (evaluate comp214)
    (not (rebooted comp214))
    (running comp213)
  )
  :effect (and
    (not (evaluate comp214))
    (evaluate comp215)
    (all-on comp214)
  )
)
(:action evaluate-comp214-off-comp213
  :parameters ()
  :precondition (and
    (evaluate comp214)
    (not (rebooted comp214))
    (not (running comp213))
  )
  :effect (and
    (not (evaluate comp214))
    (evaluate comp215)
    (one-off comp214)
  )
)
(:action evaluate-comp215-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp215)
    (rebooted comp215)
  )
  :effect (and
    (not (evaluate comp215))
    (evaluate comp216)
  )
)
(:action evaluate-comp215-all-on
  :parameters ()
  :precondition (and
    (evaluate comp215)
    (not (rebooted comp215))
    (running comp73)
    (running comp214)
  )
  :effect (and
    (not (evaluate comp215))
    (evaluate comp216)
    (all-on comp215)
  )
)
(:action evaluate-comp215-off-comp73
  :parameters ()
  :precondition (and
    (evaluate comp215)
    (not (rebooted comp215))
    (not (running comp73))
  )
  :effect (and
    (not (evaluate comp215))
    (evaluate comp216)
    (one-off comp215)
  )
)
(:action evaluate-comp215-off-comp214
  :parameters ()
  :precondition (and
    (evaluate comp215)
    (not (rebooted comp215))
    (not (running comp214))
  )
  :effect (and
    (not (evaluate comp215))
    (evaluate comp216)
    (one-off comp215)
  )
)
(:action evaluate-comp216-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp216)
    (rebooted comp216)
  )
  :effect (and
    (not (evaluate comp216))
    (evaluate comp217)
  )
)
(:action evaluate-comp216-all-on
  :parameters ()
  :precondition (and
    (evaluate comp216)
    (not (rebooted comp216))
    (running comp215)
  )
  :effect (and
    (not (evaluate comp216))
    (evaluate comp217)
    (all-on comp216)
  )
)
(:action evaluate-comp216-off-comp215
  :parameters ()
  :precondition (and
    (evaluate comp216)
    (not (rebooted comp216))
    (not (running comp215))
  )
  :effect (and
    (not (evaluate comp216))
    (evaluate comp217)
    (one-off comp216)
  )
)
(:action evaluate-comp217-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp217)
    (rebooted comp217)
  )
  :effect (and
    (not (evaluate comp217))
    (evaluate comp218)
  )
)
(:action evaluate-comp217-all-on
  :parameters ()
  :precondition (and
    (evaluate comp217)
    (not (rebooted comp217))
    (running comp216)
  )
  :effect (and
    (not (evaluate comp217))
    (evaluate comp218)
    (all-on comp217)
  )
)
(:action evaluate-comp217-off-comp216
  :parameters ()
  :precondition (and
    (evaluate comp217)
    (not (rebooted comp217))
    (not (running comp216))
  )
  :effect (and
    (not (evaluate comp217))
    (evaluate comp218)
    (one-off comp217)
  )
)
(:action evaluate-comp218-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp218)
    (rebooted comp218)
  )
  :effect (and
    (not (evaluate comp218))
    (evaluate comp219)
  )
)
(:action evaluate-comp218-all-on
  :parameters ()
  :precondition (and
    (evaluate comp218)
    (not (rebooted comp218))
    (running comp88)
    (running comp185)
    (running comp217)
  )
  :effect (and
    (not (evaluate comp218))
    (evaluate comp219)
    (all-on comp218)
  )
)
(:action evaluate-comp218-off-comp88
  :parameters ()
  :precondition (and
    (evaluate comp218)
    (not (rebooted comp218))
    (not (running comp88))
  )
  :effect (and
    (not (evaluate comp218))
    (evaluate comp219)
    (one-off comp218)
  )
)
(:action evaluate-comp218-off-comp185
  :parameters ()
  :precondition (and
    (evaluate comp218)
    (not (rebooted comp218))
    (not (running comp185))
  )
  :effect (and
    (not (evaluate comp218))
    (evaluate comp219)
    (one-off comp218)
  )
)
(:action evaluate-comp218-off-comp217
  :parameters ()
  :precondition (and
    (evaluate comp218)
    (not (rebooted comp218))
    (not (running comp217))
  )
  :effect (and
    (not (evaluate comp218))
    (evaluate comp219)
    (one-off comp218)
  )
)
(:action evaluate-comp219-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp219)
    (rebooted comp219)
  )
  :effect (and
    (not (evaluate comp219))
    (evaluate comp220)
  )
)
(:action evaluate-comp219-all-on
  :parameters ()
  :precondition (and
    (evaluate comp219)
    (not (rebooted comp219))
    (running comp201)
    (running comp218)
  )
  :effect (and
    (not (evaluate comp219))
    (evaluate comp220)
    (all-on comp219)
  )
)
(:action evaluate-comp219-off-comp201
  :parameters ()
  :precondition (and
    (evaluate comp219)
    (not (rebooted comp219))
    (not (running comp201))
  )
  :effect (and
    (not (evaluate comp219))
    (evaluate comp220)
    (one-off comp219)
  )
)
(:action evaluate-comp219-off-comp218
  :parameters ()
  :precondition (and
    (evaluate comp219)
    (not (rebooted comp219))
    (not (running comp218))
  )
  :effect (and
    (not (evaluate comp219))
    (evaluate comp220)
    (one-off comp219)
  )
)
(:action evaluate-comp220-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp220)
    (rebooted comp220)
  )
  :effect (and
    (not (evaluate comp220))
    (evaluate comp221)
  )
)
(:action evaluate-comp220-all-on
  :parameters ()
  :precondition (and
    (evaluate comp220)
    (not (rebooted comp220))
    (running comp61)
    (running comp219)
  )
  :effect (and
    (not (evaluate comp220))
    (evaluate comp221)
    (all-on comp220)
  )
)
(:action evaluate-comp220-off-comp61
  :parameters ()
  :precondition (and
    (evaluate comp220)
    (not (rebooted comp220))
    (not (running comp61))
  )
  :effect (and
    (not (evaluate comp220))
    (evaluate comp221)
    (one-off comp220)
  )
)
(:action evaluate-comp220-off-comp219
  :parameters ()
  :precondition (and
    (evaluate comp220)
    (not (rebooted comp220))
    (not (running comp219))
  )
  :effect (and
    (not (evaluate comp220))
    (evaluate comp221)
    (one-off comp220)
  )
)
(:action evaluate-comp221-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp221)
    (rebooted comp221)
  )
  :effect (and
    (not (evaluate comp221))
    (evaluate comp222)
  )
)
(:action evaluate-comp221-all-on
  :parameters ()
  :precondition (and
    (evaluate comp221)
    (not (rebooted comp221))
    (running comp90)
    (running comp220)
  )
  :effect (and
    (not (evaluate comp221))
    (evaluate comp222)
    (all-on comp221)
  )
)
(:action evaluate-comp221-off-comp90
  :parameters ()
  :precondition (and
    (evaluate comp221)
    (not (rebooted comp221))
    (not (running comp90))
  )
  :effect (and
    (not (evaluate comp221))
    (evaluate comp222)
    (one-off comp221)
  )
)
(:action evaluate-comp221-off-comp220
  :parameters ()
  :precondition (and
    (evaluate comp221)
    (not (rebooted comp221))
    (not (running comp220))
  )
  :effect (and
    (not (evaluate comp221))
    (evaluate comp222)
    (one-off comp221)
  )
)
(:action evaluate-comp222-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp222)
    (rebooted comp222)
  )
  :effect (and
    (not (evaluate comp222))
    (evaluate comp223)
  )
)
(:action evaluate-comp222-all-on
  :parameters ()
  :precondition (and
    (evaluate comp222)
    (not (rebooted comp222))
    (running comp45)
    (running comp118)
    (running comp221)
  )
  :effect (and
    (not (evaluate comp222))
    (evaluate comp223)
    (all-on comp222)
  )
)
(:action evaluate-comp222-off-comp45
  :parameters ()
  :precondition (and
    (evaluate comp222)
    (not (rebooted comp222))
    (not (running comp45))
  )
  :effect (and
    (not (evaluate comp222))
    (evaluate comp223)
    (one-off comp222)
  )
)
(:action evaluate-comp222-off-comp118
  :parameters ()
  :precondition (and
    (evaluate comp222)
    (not (rebooted comp222))
    (not (running comp118))
  )
  :effect (and
    (not (evaluate comp222))
    (evaluate comp223)
    (one-off comp222)
  )
)
(:action evaluate-comp222-off-comp221
  :parameters ()
  :precondition (and
    (evaluate comp222)
    (not (rebooted comp222))
    (not (running comp221))
  )
  :effect (and
    (not (evaluate comp222))
    (evaluate comp223)
    (one-off comp222)
  )
)
(:action evaluate-comp223-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp223)
    (rebooted comp223)
  )
  :effect (and
    (not (evaluate comp223))
    (evaluate comp224)
  )
)
(:action evaluate-comp223-all-on
  :parameters ()
  :precondition (and
    (evaluate comp223)
    (not (rebooted comp223))
    (running comp222)
  )
  :effect (and
    (not (evaluate comp223))
    (evaluate comp224)
    (all-on comp223)
  )
)
(:action evaluate-comp223-off-comp222
  :parameters ()
  :precondition (and
    (evaluate comp223)
    (not (rebooted comp223))
    (not (running comp222))
  )
  :effect (and
    (not (evaluate comp223))
    (evaluate comp224)
    (one-off comp223)
  )
)
(:action evaluate-comp224-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp224)
    (rebooted comp224)
  )
  :effect (and
    (not (evaluate comp224))
    (evaluate comp225)
  )
)
(:action evaluate-comp224-all-on
  :parameters ()
  :precondition (and
    (evaluate comp224)
    (not (rebooted comp224))
    (running comp223)
  )
  :effect (and
    (not (evaluate comp224))
    (evaluate comp225)
    (all-on comp224)
  )
)
(:action evaluate-comp224-off-comp223
  :parameters ()
  :precondition (and
    (evaluate comp224)
    (not (rebooted comp224))
    (not (running comp223))
  )
  :effect (and
    (not (evaluate comp224))
    (evaluate comp225)
    (one-off comp224)
  )
)
(:action evaluate-comp225-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp225)
    (rebooted comp225)
  )
  :effect (and
    (not (evaluate comp225))
    (evaluate comp226)
  )
)
(:action evaluate-comp225-all-on
  :parameters ()
  :precondition (and
    (evaluate comp225)
    (not (rebooted comp225))
    (running comp65)
    (running comp224)
  )
  :effect (and
    (not (evaluate comp225))
    (evaluate comp226)
    (all-on comp225)
  )
)
(:action evaluate-comp225-off-comp65
  :parameters ()
  :precondition (and
    (evaluate comp225)
    (not (rebooted comp225))
    (not (running comp65))
  )
  :effect (and
    (not (evaluate comp225))
    (evaluate comp226)
    (one-off comp225)
  )
)
(:action evaluate-comp225-off-comp224
  :parameters ()
  :precondition (and
    (evaluate comp225)
    (not (rebooted comp225))
    (not (running comp224))
  )
  :effect (and
    (not (evaluate comp225))
    (evaluate comp226)
    (one-off comp225)
  )
)
(:action evaluate-comp226-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp226)
    (rebooted comp226)
  )
  :effect (and
    (not (evaluate comp226))
    (evaluate comp227)
  )
)
(:action evaluate-comp226-all-on
  :parameters ()
  :precondition (and
    (evaluate comp226)
    (not (rebooted comp226))
    (running comp174)
    (running comp225)
  )
  :effect (and
    (not (evaluate comp226))
    (evaluate comp227)
    (all-on comp226)
  )
)
(:action evaluate-comp226-off-comp174
  :parameters ()
  :precondition (and
    (evaluate comp226)
    (not (rebooted comp226))
    (not (running comp174))
  )
  :effect (and
    (not (evaluate comp226))
    (evaluate comp227)
    (one-off comp226)
  )
)
(:action evaluate-comp226-off-comp225
  :parameters ()
  :precondition (and
    (evaluate comp226)
    (not (rebooted comp226))
    (not (running comp225))
  )
  :effect (and
    (not (evaluate comp226))
    (evaluate comp227)
    (one-off comp226)
  )
)
(:action evaluate-comp227-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp227)
    (rebooted comp227)
  )
  :effect (and
    (not (evaluate comp227))
    (evaluate comp228)
  )
)
(:action evaluate-comp227-all-on
  :parameters ()
  :precondition (and
    (evaluate comp227)
    (not (rebooted comp227))
    (running comp226)
  )
  :effect (and
    (not (evaluate comp227))
    (evaluate comp228)
    (all-on comp227)
  )
)
(:action evaluate-comp227-off-comp226
  :parameters ()
  :precondition (and
    (evaluate comp227)
    (not (rebooted comp227))
    (not (running comp226))
  )
  :effect (and
    (not (evaluate comp227))
    (evaluate comp228)
    (one-off comp227)
  )
)
(:action evaluate-comp228-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp228)
    (rebooted comp228)
  )
  :effect (and
    (not (evaluate comp228))
    (evaluate comp229)
  )
)
(:action evaluate-comp228-all-on
  :parameters ()
  :precondition (and
    (evaluate comp228)
    (not (rebooted comp228))
    (running comp108)
    (running comp227)
  )
  :effect (and
    (not (evaluate comp228))
    (evaluate comp229)
    (all-on comp228)
  )
)
(:action evaluate-comp228-off-comp108
  :parameters ()
  :precondition (and
    (evaluate comp228)
    (not (rebooted comp228))
    (not (running comp108))
  )
  :effect (and
    (not (evaluate comp228))
    (evaluate comp229)
    (one-off comp228)
  )
)
(:action evaluate-comp228-off-comp227
  :parameters ()
  :precondition (and
    (evaluate comp228)
    (not (rebooted comp228))
    (not (running comp227))
  )
  :effect (and
    (not (evaluate comp228))
    (evaluate comp229)
    (one-off comp228)
  )
)
(:action evaluate-comp229-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp229)
    (rebooted comp229)
  )
  :effect (and
    (not (evaluate comp229))
    (evaluate comp230)
  )
)
(:action evaluate-comp229-all-on
  :parameters ()
  :precondition (and
    (evaluate comp229)
    (not (rebooted comp229))
    (running comp31)
    (running comp185)
    (running comp228)
  )
  :effect (and
    (not (evaluate comp229))
    (evaluate comp230)
    (all-on comp229)
  )
)
(:action evaluate-comp229-off-comp31
  :parameters ()
  :precondition (and
    (evaluate comp229)
    (not (rebooted comp229))
    (not (running comp31))
  )
  :effect (and
    (not (evaluate comp229))
    (evaluate comp230)
    (one-off comp229)
  )
)
(:action evaluate-comp229-off-comp185
  :parameters ()
  :precondition (and
    (evaluate comp229)
    (not (rebooted comp229))
    (not (running comp185))
  )
  :effect (and
    (not (evaluate comp229))
    (evaluate comp230)
    (one-off comp229)
  )
)
(:action evaluate-comp229-off-comp228
  :parameters ()
  :precondition (and
    (evaluate comp229)
    (not (rebooted comp229))
    (not (running comp228))
  )
  :effect (and
    (not (evaluate comp229))
    (evaluate comp230)
    (one-off comp229)
  )
)
(:action evaluate-comp230-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp230)
    (rebooted comp230)
  )
  :effect (and
    (not (evaluate comp230))
    (evaluate comp231)
  )
)
(:action evaluate-comp230-all-on
  :parameters ()
  :precondition (and
    (evaluate comp230)
    (not (rebooted comp230))
    (running comp229)
  )
  :effect (and
    (not (evaluate comp230))
    (evaluate comp231)
    (all-on comp230)
  )
)
(:action evaluate-comp230-off-comp229
  :parameters ()
  :precondition (and
    (evaluate comp230)
    (not (rebooted comp230))
    (not (running comp229))
  )
  :effect (and
    (not (evaluate comp230))
    (evaluate comp231)
    (one-off comp230)
  )
)
(:action evaluate-comp231-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp231)
    (rebooted comp231)
  )
  :effect (and
    (not (evaluate comp231))
    (evaluate comp232)
  )
)
(:action evaluate-comp231-all-on
  :parameters ()
  :precondition (and
    (evaluate comp231)
    (not (rebooted comp231))
    (running comp189)
    (running comp230)
    (running comp233)
  )
  :effect (and
    (not (evaluate comp231))
    (evaluate comp232)
    (all-on comp231)
  )
)
(:action evaluate-comp231-off-comp189
  :parameters ()
  :precondition (and
    (evaluate comp231)
    (not (rebooted comp231))
    (not (running comp189))
  )
  :effect (and
    (not (evaluate comp231))
    (evaluate comp232)
    (one-off comp231)
  )
)
(:action evaluate-comp231-off-comp230
  :parameters ()
  :precondition (and
    (evaluate comp231)
    (not (rebooted comp231))
    (not (running comp230))
  )
  :effect (and
    (not (evaluate comp231))
    (evaluate comp232)
    (one-off comp231)
  )
)
(:action evaluate-comp231-off-comp233
  :parameters ()
  :precondition (and
    (evaluate comp231)
    (not (rebooted comp231))
    (not (running comp233))
  )
  :effect (and
    (not (evaluate comp231))
    (evaluate comp232)
    (one-off comp231)
  )
)
(:action evaluate-comp232-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp232)
    (rebooted comp232)
  )
  :effect (and
    (not (evaluate comp232))
    (evaluate comp233)
  )
)
(:action evaluate-comp232-all-on
  :parameters ()
  :precondition (and
    (evaluate comp232)
    (not (rebooted comp232))
    (running comp231)
  )
  :effect (and
    (not (evaluate comp232))
    (evaluate comp233)
    (all-on comp232)
  )
)
(:action evaluate-comp232-off-comp231
  :parameters ()
  :precondition (and
    (evaluate comp232)
    (not (rebooted comp232))
    (not (running comp231))
  )
  :effect (and
    (not (evaluate comp232))
    (evaluate comp233)
    (one-off comp232)
  )
)
(:action evaluate-comp233-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp233)
    (rebooted comp233)
  )
  :effect (and
    (not (evaluate comp233))
    (evaluate comp234)
  )
)
(:action evaluate-comp233-all-on
  :parameters ()
  :precondition (and
    (evaluate comp233)
    (not (rebooted comp233))
    (running comp232)
  )
  :effect (and
    (not (evaluate comp233))
    (evaluate comp234)
    (all-on comp233)
  )
)
(:action evaluate-comp233-off-comp232
  :parameters ()
  :precondition (and
    (evaluate comp233)
    (not (rebooted comp233))
    (not (running comp232))
  )
  :effect (and
    (not (evaluate comp233))
    (evaluate comp234)
    (one-off comp233)
  )
)
(:action evaluate-comp234-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp234)
    (rebooted comp234)
  )
  :effect (and
    (not (evaluate comp234))
    (evaluate comp235)
  )
)
(:action evaluate-comp234-all-on
  :parameters ()
  :precondition (and
    (evaluate comp234)
    (not (rebooted comp234))
    (running comp233)
  )
  :effect (and
    (not (evaluate comp234))
    (evaluate comp235)
    (all-on comp234)
  )
)
(:action evaluate-comp234-off-comp233
  :parameters ()
  :precondition (and
    (evaluate comp234)
    (not (rebooted comp234))
    (not (running comp233))
  )
  :effect (and
    (not (evaluate comp234))
    (evaluate comp235)
    (one-off comp234)
  )
)
(:action evaluate-comp235-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp235)
    (rebooted comp235)
  )
  :effect (and
    (not (evaluate comp235))
    (evaluate comp236)
  )
)
(:action evaluate-comp235-all-on
  :parameters ()
  :precondition (and
    (evaluate comp235)
    (not (rebooted comp235))
    (running comp234)
  )
  :effect (and
    (not (evaluate comp235))
    (evaluate comp236)
    (all-on comp235)
  )
)
(:action evaluate-comp235-off-comp234
  :parameters ()
  :precondition (and
    (evaluate comp235)
    (not (rebooted comp235))
    (not (running comp234))
  )
  :effect (and
    (not (evaluate comp235))
    (evaluate comp236)
    (one-off comp235)
  )
)
(:action evaluate-comp236-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp236)
    (rebooted comp236)
  )
  :effect (and
    (not (evaluate comp236))
    (evaluate comp237)
  )
)
(:action evaluate-comp236-all-on
  :parameters ()
  :precondition (and
    (evaluate comp236)
    (not (rebooted comp236))
    (running comp235)
  )
  :effect (and
    (not (evaluate comp236))
    (evaluate comp237)
    (all-on comp236)
  )
)
(:action evaluate-comp236-off-comp235
  :parameters ()
  :precondition (and
    (evaluate comp236)
    (not (rebooted comp236))
    (not (running comp235))
  )
  :effect (and
    (not (evaluate comp236))
    (evaluate comp237)
    (one-off comp236)
  )
)
(:action evaluate-comp237-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp237)
    (rebooted comp237)
  )
  :effect (and
    (not (evaluate comp237))
    (evaluate comp238)
  )
)
(:action evaluate-comp237-all-on
  :parameters ()
  :precondition (and
    (evaluate comp237)
    (not (rebooted comp237))
    (running comp59)
    (running comp236)
  )
  :effect (and
    (not (evaluate comp237))
    (evaluate comp238)
    (all-on comp237)
  )
)
(:action evaluate-comp237-off-comp59
  :parameters ()
  :precondition (and
    (evaluate comp237)
    (not (rebooted comp237))
    (not (running comp59))
  )
  :effect (and
    (not (evaluate comp237))
    (evaluate comp238)
    (one-off comp237)
  )
)
(:action evaluate-comp237-off-comp236
  :parameters ()
  :precondition (and
    (evaluate comp237)
    (not (rebooted comp237))
    (not (running comp236))
  )
  :effect (and
    (not (evaluate comp237))
    (evaluate comp238)
    (one-off comp237)
  )
)
(:action evaluate-comp238-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp238)
    (rebooted comp238)
  )
  :effect (and
    (not (evaluate comp238))
    (evaluate comp239)
  )
)
(:action evaluate-comp238-all-on
  :parameters ()
  :precondition (and
    (evaluate comp238)
    (not (rebooted comp238))
    (running comp80)
    (running comp237)
  )
  :effect (and
    (not (evaluate comp238))
    (evaluate comp239)
    (all-on comp238)
  )
)
(:action evaluate-comp238-off-comp80
  :parameters ()
  :precondition (and
    (evaluate comp238)
    (not (rebooted comp238))
    (not (running comp80))
  )
  :effect (and
    (not (evaluate comp238))
    (evaluate comp239)
    (one-off comp238)
  )
)
(:action evaluate-comp238-off-comp237
  :parameters ()
  :precondition (and
    (evaluate comp238)
    (not (rebooted comp238))
    (not (running comp237))
  )
  :effect (and
    (not (evaluate comp238))
    (evaluate comp239)
    (one-off comp238)
  )
)
(:action evaluate-comp239-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp239)
    (rebooted comp239)
  )
  :effect (and
    (not (evaluate comp239))
    (update-status comp0)
  )
)
(:action evaluate-comp239-all-on
  :parameters ()
  :precondition (and
    (evaluate comp239)
    (not (rebooted comp239))
    (running comp238)
  )
  :effect (and
    (not (evaluate comp239))
    (update-status comp0)
    (all-on comp239)
  )
)
(:action evaluate-comp239-off-comp238
  :parameters ()
  :precondition (and
    (evaluate comp239)
    (not (rebooted comp239))
    (not (running comp238))
  )
  :effect (and
    (not (evaluate comp239))
    (update-status comp0)
    (one-off comp239)
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
    (update-status comp120)
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
    (update-status comp120)
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
    (update-status comp120)
    (not (one-off comp119))
    (probabilistic 1/4 (and (not (running comp119))))
  )
)
(:action update-status-comp120-rebooted
  :parameters ()
  :precondition (and
    (update-status comp120)
    (rebooted comp120)
  )
  :effect (and
    (not (update-status comp120))
    (update-status comp121)
    (not (rebooted comp120))
    (probabilistic 9/10 (and (running comp120)) 1/10 (and))
  )
)
(:action update-status-comp120-all-on
  :parameters ()
  :precondition (and
    (update-status comp120)
    (not (rebooted comp120))
    (all-on comp120)
  )
  :effect (and
    (not (update-status comp120))
    (update-status comp121)
    (not (all-on comp120))
    (probabilistic 1/20 (and (not (running comp120))))
  )
)
(:action update-status-comp120-one-off
  :parameters ()
  :precondition (and
    (update-status comp120)
    (not (rebooted comp120))
    (one-off comp120)
  )
  :effect (and
    (not (update-status comp120))
    (update-status comp121)
    (not (one-off comp120))
    (probabilistic 1/4 (and (not (running comp120))))
  )
)
(:action update-status-comp121-rebooted
  :parameters ()
  :precondition (and
    (update-status comp121)
    (rebooted comp121)
  )
  :effect (and
    (not (update-status comp121))
    (update-status comp122)
    (not (rebooted comp121))
    (probabilistic 9/10 (and (running comp121)) 1/10 (and))
  )
)
(:action update-status-comp121-all-on
  :parameters ()
  :precondition (and
    (update-status comp121)
    (not (rebooted comp121))
    (all-on comp121)
  )
  :effect (and
    (not (update-status comp121))
    (update-status comp122)
    (not (all-on comp121))
    (probabilistic 1/20 (and (not (running comp121))))
  )
)
(:action update-status-comp121-one-off
  :parameters ()
  :precondition (and
    (update-status comp121)
    (not (rebooted comp121))
    (one-off comp121)
  )
  :effect (and
    (not (update-status comp121))
    (update-status comp122)
    (not (one-off comp121))
    (probabilistic 1/4 (and (not (running comp121))))
  )
)
(:action update-status-comp122-rebooted
  :parameters ()
  :precondition (and
    (update-status comp122)
    (rebooted comp122)
  )
  :effect (and
    (not (update-status comp122))
    (update-status comp123)
    (not (rebooted comp122))
    (probabilistic 9/10 (and (running comp122)) 1/10 (and))
  )
)
(:action update-status-comp122-all-on
  :parameters ()
  :precondition (and
    (update-status comp122)
    (not (rebooted comp122))
    (all-on comp122)
  )
  :effect (and
    (not (update-status comp122))
    (update-status comp123)
    (not (all-on comp122))
    (probabilistic 1/20 (and (not (running comp122))))
  )
)
(:action update-status-comp122-one-off
  :parameters ()
  :precondition (and
    (update-status comp122)
    (not (rebooted comp122))
    (one-off comp122)
  )
  :effect (and
    (not (update-status comp122))
    (update-status comp123)
    (not (one-off comp122))
    (probabilistic 1/4 (and (not (running comp122))))
  )
)
(:action update-status-comp123-rebooted
  :parameters ()
  :precondition (and
    (update-status comp123)
    (rebooted comp123)
  )
  :effect (and
    (not (update-status comp123))
    (update-status comp124)
    (not (rebooted comp123))
    (probabilistic 9/10 (and (running comp123)) 1/10 (and))
  )
)
(:action update-status-comp123-all-on
  :parameters ()
  :precondition (and
    (update-status comp123)
    (not (rebooted comp123))
    (all-on comp123)
  )
  :effect (and
    (not (update-status comp123))
    (update-status comp124)
    (not (all-on comp123))
    (probabilistic 1/20 (and (not (running comp123))))
  )
)
(:action update-status-comp123-one-off
  :parameters ()
  :precondition (and
    (update-status comp123)
    (not (rebooted comp123))
    (one-off comp123)
  )
  :effect (and
    (not (update-status comp123))
    (update-status comp124)
    (not (one-off comp123))
    (probabilistic 1/4 (and (not (running comp123))))
  )
)
(:action update-status-comp124-rebooted
  :parameters ()
  :precondition (and
    (update-status comp124)
    (rebooted comp124)
  )
  :effect (and
    (not (update-status comp124))
    (update-status comp125)
    (not (rebooted comp124))
    (probabilistic 9/10 (and (running comp124)) 1/10 (and))
  )
)
(:action update-status-comp124-all-on
  :parameters ()
  :precondition (and
    (update-status comp124)
    (not (rebooted comp124))
    (all-on comp124)
  )
  :effect (and
    (not (update-status comp124))
    (update-status comp125)
    (not (all-on comp124))
    (probabilistic 1/20 (and (not (running comp124))))
  )
)
(:action update-status-comp124-one-off
  :parameters ()
  :precondition (and
    (update-status comp124)
    (not (rebooted comp124))
    (one-off comp124)
  )
  :effect (and
    (not (update-status comp124))
    (update-status comp125)
    (not (one-off comp124))
    (probabilistic 1/4 (and (not (running comp124))))
  )
)
(:action update-status-comp125-rebooted
  :parameters ()
  :precondition (and
    (update-status comp125)
    (rebooted comp125)
  )
  :effect (and
    (not (update-status comp125))
    (update-status comp126)
    (not (rebooted comp125))
    (probabilistic 9/10 (and (running comp125)) 1/10 (and))
  )
)
(:action update-status-comp125-all-on
  :parameters ()
  :precondition (and
    (update-status comp125)
    (not (rebooted comp125))
    (all-on comp125)
  )
  :effect (and
    (not (update-status comp125))
    (update-status comp126)
    (not (all-on comp125))
    (probabilistic 1/20 (and (not (running comp125))))
  )
)
(:action update-status-comp125-one-off
  :parameters ()
  :precondition (and
    (update-status comp125)
    (not (rebooted comp125))
    (one-off comp125)
  )
  :effect (and
    (not (update-status comp125))
    (update-status comp126)
    (not (one-off comp125))
    (probabilistic 1/4 (and (not (running comp125))))
  )
)
(:action update-status-comp126-rebooted
  :parameters ()
  :precondition (and
    (update-status comp126)
    (rebooted comp126)
  )
  :effect (and
    (not (update-status comp126))
    (update-status comp127)
    (not (rebooted comp126))
    (probabilistic 9/10 (and (running comp126)) 1/10 (and))
  )
)
(:action update-status-comp126-all-on
  :parameters ()
  :precondition (and
    (update-status comp126)
    (not (rebooted comp126))
    (all-on comp126)
  )
  :effect (and
    (not (update-status comp126))
    (update-status comp127)
    (not (all-on comp126))
    (probabilistic 1/20 (and (not (running comp126))))
  )
)
(:action update-status-comp126-one-off
  :parameters ()
  :precondition (and
    (update-status comp126)
    (not (rebooted comp126))
    (one-off comp126)
  )
  :effect (and
    (not (update-status comp126))
    (update-status comp127)
    (not (one-off comp126))
    (probabilistic 1/4 (and (not (running comp126))))
  )
)
(:action update-status-comp127-rebooted
  :parameters ()
  :precondition (and
    (update-status comp127)
    (rebooted comp127)
  )
  :effect (and
    (not (update-status comp127))
    (update-status comp128)
    (not (rebooted comp127))
    (probabilistic 9/10 (and (running comp127)) 1/10 (and))
  )
)
(:action update-status-comp127-all-on
  :parameters ()
  :precondition (and
    (update-status comp127)
    (not (rebooted comp127))
    (all-on comp127)
  )
  :effect (and
    (not (update-status comp127))
    (update-status comp128)
    (not (all-on comp127))
    (probabilistic 1/20 (and (not (running comp127))))
  )
)
(:action update-status-comp127-one-off
  :parameters ()
  :precondition (and
    (update-status comp127)
    (not (rebooted comp127))
    (one-off comp127)
  )
  :effect (and
    (not (update-status comp127))
    (update-status comp128)
    (not (one-off comp127))
    (probabilistic 1/4 (and (not (running comp127))))
  )
)
(:action update-status-comp128-rebooted
  :parameters ()
  :precondition (and
    (update-status comp128)
    (rebooted comp128)
  )
  :effect (and
    (not (update-status comp128))
    (update-status comp129)
    (not (rebooted comp128))
    (probabilistic 9/10 (and (running comp128)) 1/10 (and))
  )
)
(:action update-status-comp128-all-on
  :parameters ()
  :precondition (and
    (update-status comp128)
    (not (rebooted comp128))
    (all-on comp128)
  )
  :effect (and
    (not (update-status comp128))
    (update-status comp129)
    (not (all-on comp128))
    (probabilistic 1/20 (and (not (running comp128))))
  )
)
(:action update-status-comp128-one-off
  :parameters ()
  :precondition (and
    (update-status comp128)
    (not (rebooted comp128))
    (one-off comp128)
  )
  :effect (and
    (not (update-status comp128))
    (update-status comp129)
    (not (one-off comp128))
    (probabilistic 1/4 (and (not (running comp128))))
  )
)
(:action update-status-comp129-rebooted
  :parameters ()
  :precondition (and
    (update-status comp129)
    (rebooted comp129)
  )
  :effect (and
    (not (update-status comp129))
    (update-status comp130)
    (not (rebooted comp129))
    (probabilistic 9/10 (and (running comp129)) 1/10 (and))
  )
)
(:action update-status-comp129-all-on
  :parameters ()
  :precondition (and
    (update-status comp129)
    (not (rebooted comp129))
    (all-on comp129)
  )
  :effect (and
    (not (update-status comp129))
    (update-status comp130)
    (not (all-on comp129))
    (probabilistic 1/20 (and (not (running comp129))))
  )
)
(:action update-status-comp129-one-off
  :parameters ()
  :precondition (and
    (update-status comp129)
    (not (rebooted comp129))
    (one-off comp129)
  )
  :effect (and
    (not (update-status comp129))
    (update-status comp130)
    (not (one-off comp129))
    (probabilistic 1/4 (and (not (running comp129))))
  )
)
(:action update-status-comp130-rebooted
  :parameters ()
  :precondition (and
    (update-status comp130)
    (rebooted comp130)
  )
  :effect (and
    (not (update-status comp130))
    (update-status comp131)
    (not (rebooted comp130))
    (probabilistic 9/10 (and (running comp130)) 1/10 (and))
  )
)
(:action update-status-comp130-all-on
  :parameters ()
  :precondition (and
    (update-status comp130)
    (not (rebooted comp130))
    (all-on comp130)
  )
  :effect (and
    (not (update-status comp130))
    (update-status comp131)
    (not (all-on comp130))
    (probabilistic 1/20 (and (not (running comp130))))
  )
)
(:action update-status-comp130-one-off
  :parameters ()
  :precondition (and
    (update-status comp130)
    (not (rebooted comp130))
    (one-off comp130)
  )
  :effect (and
    (not (update-status comp130))
    (update-status comp131)
    (not (one-off comp130))
    (probabilistic 1/4 (and (not (running comp130))))
  )
)
(:action update-status-comp131-rebooted
  :parameters ()
  :precondition (and
    (update-status comp131)
    (rebooted comp131)
  )
  :effect (and
    (not (update-status comp131))
    (update-status comp132)
    (not (rebooted comp131))
    (probabilistic 9/10 (and (running comp131)) 1/10 (and))
  )
)
(:action update-status-comp131-all-on
  :parameters ()
  :precondition (and
    (update-status comp131)
    (not (rebooted comp131))
    (all-on comp131)
  )
  :effect (and
    (not (update-status comp131))
    (update-status comp132)
    (not (all-on comp131))
    (probabilistic 1/20 (and (not (running comp131))))
  )
)
(:action update-status-comp131-one-off
  :parameters ()
  :precondition (and
    (update-status comp131)
    (not (rebooted comp131))
    (one-off comp131)
  )
  :effect (and
    (not (update-status comp131))
    (update-status comp132)
    (not (one-off comp131))
    (probabilistic 1/4 (and (not (running comp131))))
  )
)
(:action update-status-comp132-rebooted
  :parameters ()
  :precondition (and
    (update-status comp132)
    (rebooted comp132)
  )
  :effect (and
    (not (update-status comp132))
    (update-status comp133)
    (not (rebooted comp132))
    (probabilistic 9/10 (and (running comp132)) 1/10 (and))
  )
)
(:action update-status-comp132-all-on
  :parameters ()
  :precondition (and
    (update-status comp132)
    (not (rebooted comp132))
    (all-on comp132)
  )
  :effect (and
    (not (update-status comp132))
    (update-status comp133)
    (not (all-on comp132))
    (probabilistic 1/20 (and (not (running comp132))))
  )
)
(:action update-status-comp132-one-off
  :parameters ()
  :precondition (and
    (update-status comp132)
    (not (rebooted comp132))
    (one-off comp132)
  )
  :effect (and
    (not (update-status comp132))
    (update-status comp133)
    (not (one-off comp132))
    (probabilistic 1/4 (and (not (running comp132))))
  )
)
(:action update-status-comp133-rebooted
  :parameters ()
  :precondition (and
    (update-status comp133)
    (rebooted comp133)
  )
  :effect (and
    (not (update-status comp133))
    (update-status comp134)
    (not (rebooted comp133))
    (probabilistic 9/10 (and (running comp133)) 1/10 (and))
  )
)
(:action update-status-comp133-all-on
  :parameters ()
  :precondition (and
    (update-status comp133)
    (not (rebooted comp133))
    (all-on comp133)
  )
  :effect (and
    (not (update-status comp133))
    (update-status comp134)
    (not (all-on comp133))
    (probabilistic 1/20 (and (not (running comp133))))
  )
)
(:action update-status-comp133-one-off
  :parameters ()
  :precondition (and
    (update-status comp133)
    (not (rebooted comp133))
    (one-off comp133)
  )
  :effect (and
    (not (update-status comp133))
    (update-status comp134)
    (not (one-off comp133))
    (probabilistic 1/4 (and (not (running comp133))))
  )
)
(:action update-status-comp134-rebooted
  :parameters ()
  :precondition (and
    (update-status comp134)
    (rebooted comp134)
  )
  :effect (and
    (not (update-status comp134))
    (update-status comp135)
    (not (rebooted comp134))
    (probabilistic 9/10 (and (running comp134)) 1/10 (and))
  )
)
(:action update-status-comp134-all-on
  :parameters ()
  :precondition (and
    (update-status comp134)
    (not (rebooted comp134))
    (all-on comp134)
  )
  :effect (and
    (not (update-status comp134))
    (update-status comp135)
    (not (all-on comp134))
    (probabilistic 1/20 (and (not (running comp134))))
  )
)
(:action update-status-comp134-one-off
  :parameters ()
  :precondition (and
    (update-status comp134)
    (not (rebooted comp134))
    (one-off comp134)
  )
  :effect (and
    (not (update-status comp134))
    (update-status comp135)
    (not (one-off comp134))
    (probabilistic 1/4 (and (not (running comp134))))
  )
)
(:action update-status-comp135-rebooted
  :parameters ()
  :precondition (and
    (update-status comp135)
    (rebooted comp135)
  )
  :effect (and
    (not (update-status comp135))
    (update-status comp136)
    (not (rebooted comp135))
    (probabilistic 9/10 (and (running comp135)) 1/10 (and))
  )
)
(:action update-status-comp135-all-on
  :parameters ()
  :precondition (and
    (update-status comp135)
    (not (rebooted comp135))
    (all-on comp135)
  )
  :effect (and
    (not (update-status comp135))
    (update-status comp136)
    (not (all-on comp135))
    (probabilistic 1/20 (and (not (running comp135))))
  )
)
(:action update-status-comp135-one-off
  :parameters ()
  :precondition (and
    (update-status comp135)
    (not (rebooted comp135))
    (one-off comp135)
  )
  :effect (and
    (not (update-status comp135))
    (update-status comp136)
    (not (one-off comp135))
    (probabilistic 1/4 (and (not (running comp135))))
  )
)
(:action update-status-comp136-rebooted
  :parameters ()
  :precondition (and
    (update-status comp136)
    (rebooted comp136)
  )
  :effect (and
    (not (update-status comp136))
    (update-status comp137)
    (not (rebooted comp136))
    (probabilistic 9/10 (and (running comp136)) 1/10 (and))
  )
)
(:action update-status-comp136-all-on
  :parameters ()
  :precondition (and
    (update-status comp136)
    (not (rebooted comp136))
    (all-on comp136)
  )
  :effect (and
    (not (update-status comp136))
    (update-status comp137)
    (not (all-on comp136))
    (probabilistic 1/20 (and (not (running comp136))))
  )
)
(:action update-status-comp136-one-off
  :parameters ()
  :precondition (and
    (update-status comp136)
    (not (rebooted comp136))
    (one-off comp136)
  )
  :effect (and
    (not (update-status comp136))
    (update-status comp137)
    (not (one-off comp136))
    (probabilistic 1/4 (and (not (running comp136))))
  )
)
(:action update-status-comp137-rebooted
  :parameters ()
  :precondition (and
    (update-status comp137)
    (rebooted comp137)
  )
  :effect (and
    (not (update-status comp137))
    (update-status comp138)
    (not (rebooted comp137))
    (probabilistic 9/10 (and (running comp137)) 1/10 (and))
  )
)
(:action update-status-comp137-all-on
  :parameters ()
  :precondition (and
    (update-status comp137)
    (not (rebooted comp137))
    (all-on comp137)
  )
  :effect (and
    (not (update-status comp137))
    (update-status comp138)
    (not (all-on comp137))
    (probabilistic 1/20 (and (not (running comp137))))
  )
)
(:action update-status-comp137-one-off
  :parameters ()
  :precondition (and
    (update-status comp137)
    (not (rebooted comp137))
    (one-off comp137)
  )
  :effect (and
    (not (update-status comp137))
    (update-status comp138)
    (not (one-off comp137))
    (probabilistic 1/4 (and (not (running comp137))))
  )
)
(:action update-status-comp138-rebooted
  :parameters ()
  :precondition (and
    (update-status comp138)
    (rebooted comp138)
  )
  :effect (and
    (not (update-status comp138))
    (update-status comp139)
    (not (rebooted comp138))
    (probabilistic 9/10 (and (running comp138)) 1/10 (and))
  )
)
(:action update-status-comp138-all-on
  :parameters ()
  :precondition (and
    (update-status comp138)
    (not (rebooted comp138))
    (all-on comp138)
  )
  :effect (and
    (not (update-status comp138))
    (update-status comp139)
    (not (all-on comp138))
    (probabilistic 1/20 (and (not (running comp138))))
  )
)
(:action update-status-comp138-one-off
  :parameters ()
  :precondition (and
    (update-status comp138)
    (not (rebooted comp138))
    (one-off comp138)
  )
  :effect (and
    (not (update-status comp138))
    (update-status comp139)
    (not (one-off comp138))
    (probabilistic 1/4 (and (not (running comp138))))
  )
)
(:action update-status-comp139-rebooted
  :parameters ()
  :precondition (and
    (update-status comp139)
    (rebooted comp139)
  )
  :effect (and
    (not (update-status comp139))
    (update-status comp140)
    (not (rebooted comp139))
    (probabilistic 9/10 (and (running comp139)) 1/10 (and))
  )
)
(:action update-status-comp139-all-on
  :parameters ()
  :precondition (and
    (update-status comp139)
    (not (rebooted comp139))
    (all-on comp139)
  )
  :effect (and
    (not (update-status comp139))
    (update-status comp140)
    (not (all-on comp139))
    (probabilistic 1/20 (and (not (running comp139))))
  )
)
(:action update-status-comp139-one-off
  :parameters ()
  :precondition (and
    (update-status comp139)
    (not (rebooted comp139))
    (one-off comp139)
  )
  :effect (and
    (not (update-status comp139))
    (update-status comp140)
    (not (one-off comp139))
    (probabilistic 1/4 (and (not (running comp139))))
  )
)
(:action update-status-comp140-rebooted
  :parameters ()
  :precondition (and
    (update-status comp140)
    (rebooted comp140)
  )
  :effect (and
    (not (update-status comp140))
    (update-status comp141)
    (not (rebooted comp140))
    (probabilistic 9/10 (and (running comp140)) 1/10 (and))
  )
)
(:action update-status-comp140-all-on
  :parameters ()
  :precondition (and
    (update-status comp140)
    (not (rebooted comp140))
    (all-on comp140)
  )
  :effect (and
    (not (update-status comp140))
    (update-status comp141)
    (not (all-on comp140))
    (probabilistic 1/20 (and (not (running comp140))))
  )
)
(:action update-status-comp140-one-off
  :parameters ()
  :precondition (and
    (update-status comp140)
    (not (rebooted comp140))
    (one-off comp140)
  )
  :effect (and
    (not (update-status comp140))
    (update-status comp141)
    (not (one-off comp140))
    (probabilistic 1/4 (and (not (running comp140))))
  )
)
(:action update-status-comp141-rebooted
  :parameters ()
  :precondition (and
    (update-status comp141)
    (rebooted comp141)
  )
  :effect (and
    (not (update-status comp141))
    (update-status comp142)
    (not (rebooted comp141))
    (probabilistic 9/10 (and (running comp141)) 1/10 (and))
  )
)
(:action update-status-comp141-all-on
  :parameters ()
  :precondition (and
    (update-status comp141)
    (not (rebooted comp141))
    (all-on comp141)
  )
  :effect (and
    (not (update-status comp141))
    (update-status comp142)
    (not (all-on comp141))
    (probabilistic 1/20 (and (not (running comp141))))
  )
)
(:action update-status-comp141-one-off
  :parameters ()
  :precondition (and
    (update-status comp141)
    (not (rebooted comp141))
    (one-off comp141)
  )
  :effect (and
    (not (update-status comp141))
    (update-status comp142)
    (not (one-off comp141))
    (probabilistic 1/4 (and (not (running comp141))))
  )
)
(:action update-status-comp142-rebooted
  :parameters ()
  :precondition (and
    (update-status comp142)
    (rebooted comp142)
  )
  :effect (and
    (not (update-status comp142))
    (update-status comp143)
    (not (rebooted comp142))
    (probabilistic 9/10 (and (running comp142)) 1/10 (and))
  )
)
(:action update-status-comp142-all-on
  :parameters ()
  :precondition (and
    (update-status comp142)
    (not (rebooted comp142))
    (all-on comp142)
  )
  :effect (and
    (not (update-status comp142))
    (update-status comp143)
    (not (all-on comp142))
    (probabilistic 1/20 (and (not (running comp142))))
  )
)
(:action update-status-comp142-one-off
  :parameters ()
  :precondition (and
    (update-status comp142)
    (not (rebooted comp142))
    (one-off comp142)
  )
  :effect (and
    (not (update-status comp142))
    (update-status comp143)
    (not (one-off comp142))
    (probabilistic 1/4 (and (not (running comp142))))
  )
)
(:action update-status-comp143-rebooted
  :parameters ()
  :precondition (and
    (update-status comp143)
    (rebooted comp143)
  )
  :effect (and
    (not (update-status comp143))
    (update-status comp144)
    (not (rebooted comp143))
    (probabilistic 9/10 (and (running comp143)) 1/10 (and))
  )
)
(:action update-status-comp143-all-on
  :parameters ()
  :precondition (and
    (update-status comp143)
    (not (rebooted comp143))
    (all-on comp143)
  )
  :effect (and
    (not (update-status comp143))
    (update-status comp144)
    (not (all-on comp143))
    (probabilistic 1/20 (and (not (running comp143))))
  )
)
(:action update-status-comp143-one-off
  :parameters ()
  :precondition (and
    (update-status comp143)
    (not (rebooted comp143))
    (one-off comp143)
  )
  :effect (and
    (not (update-status comp143))
    (update-status comp144)
    (not (one-off comp143))
    (probabilistic 1/4 (and (not (running comp143))))
  )
)
(:action update-status-comp144-rebooted
  :parameters ()
  :precondition (and
    (update-status comp144)
    (rebooted comp144)
  )
  :effect (and
    (not (update-status comp144))
    (update-status comp145)
    (not (rebooted comp144))
    (probabilistic 9/10 (and (running comp144)) 1/10 (and))
  )
)
(:action update-status-comp144-all-on
  :parameters ()
  :precondition (and
    (update-status comp144)
    (not (rebooted comp144))
    (all-on comp144)
  )
  :effect (and
    (not (update-status comp144))
    (update-status comp145)
    (not (all-on comp144))
    (probabilistic 1/20 (and (not (running comp144))))
  )
)
(:action update-status-comp144-one-off
  :parameters ()
  :precondition (and
    (update-status comp144)
    (not (rebooted comp144))
    (one-off comp144)
  )
  :effect (and
    (not (update-status comp144))
    (update-status comp145)
    (not (one-off comp144))
    (probabilistic 1/4 (and (not (running comp144))))
  )
)
(:action update-status-comp145-rebooted
  :parameters ()
  :precondition (and
    (update-status comp145)
    (rebooted comp145)
  )
  :effect (and
    (not (update-status comp145))
    (update-status comp146)
    (not (rebooted comp145))
    (probabilistic 9/10 (and (running comp145)) 1/10 (and))
  )
)
(:action update-status-comp145-all-on
  :parameters ()
  :precondition (and
    (update-status comp145)
    (not (rebooted comp145))
    (all-on comp145)
  )
  :effect (and
    (not (update-status comp145))
    (update-status comp146)
    (not (all-on comp145))
    (probabilistic 1/20 (and (not (running comp145))))
  )
)
(:action update-status-comp145-one-off
  :parameters ()
  :precondition (and
    (update-status comp145)
    (not (rebooted comp145))
    (one-off comp145)
  )
  :effect (and
    (not (update-status comp145))
    (update-status comp146)
    (not (one-off comp145))
    (probabilistic 1/4 (and (not (running comp145))))
  )
)
(:action update-status-comp146-rebooted
  :parameters ()
  :precondition (and
    (update-status comp146)
    (rebooted comp146)
  )
  :effect (and
    (not (update-status comp146))
    (update-status comp147)
    (not (rebooted comp146))
    (probabilistic 9/10 (and (running comp146)) 1/10 (and))
  )
)
(:action update-status-comp146-all-on
  :parameters ()
  :precondition (and
    (update-status comp146)
    (not (rebooted comp146))
    (all-on comp146)
  )
  :effect (and
    (not (update-status comp146))
    (update-status comp147)
    (not (all-on comp146))
    (probabilistic 1/20 (and (not (running comp146))))
  )
)
(:action update-status-comp146-one-off
  :parameters ()
  :precondition (and
    (update-status comp146)
    (not (rebooted comp146))
    (one-off comp146)
  )
  :effect (and
    (not (update-status comp146))
    (update-status comp147)
    (not (one-off comp146))
    (probabilistic 1/4 (and (not (running comp146))))
  )
)
(:action update-status-comp147-rebooted
  :parameters ()
  :precondition (and
    (update-status comp147)
    (rebooted comp147)
  )
  :effect (and
    (not (update-status comp147))
    (update-status comp148)
    (not (rebooted comp147))
    (probabilistic 9/10 (and (running comp147)) 1/10 (and))
  )
)
(:action update-status-comp147-all-on
  :parameters ()
  :precondition (and
    (update-status comp147)
    (not (rebooted comp147))
    (all-on comp147)
  )
  :effect (and
    (not (update-status comp147))
    (update-status comp148)
    (not (all-on comp147))
    (probabilistic 1/20 (and (not (running comp147))))
  )
)
(:action update-status-comp147-one-off
  :parameters ()
  :precondition (and
    (update-status comp147)
    (not (rebooted comp147))
    (one-off comp147)
  )
  :effect (and
    (not (update-status comp147))
    (update-status comp148)
    (not (one-off comp147))
    (probabilistic 1/4 (and (not (running comp147))))
  )
)
(:action update-status-comp148-rebooted
  :parameters ()
  :precondition (and
    (update-status comp148)
    (rebooted comp148)
  )
  :effect (and
    (not (update-status comp148))
    (update-status comp149)
    (not (rebooted comp148))
    (probabilistic 9/10 (and (running comp148)) 1/10 (and))
  )
)
(:action update-status-comp148-all-on
  :parameters ()
  :precondition (and
    (update-status comp148)
    (not (rebooted comp148))
    (all-on comp148)
  )
  :effect (and
    (not (update-status comp148))
    (update-status comp149)
    (not (all-on comp148))
    (probabilistic 1/20 (and (not (running comp148))))
  )
)
(:action update-status-comp148-one-off
  :parameters ()
  :precondition (and
    (update-status comp148)
    (not (rebooted comp148))
    (one-off comp148)
  )
  :effect (and
    (not (update-status comp148))
    (update-status comp149)
    (not (one-off comp148))
    (probabilistic 1/4 (and (not (running comp148))))
  )
)
(:action update-status-comp149-rebooted
  :parameters ()
  :precondition (and
    (update-status comp149)
    (rebooted comp149)
  )
  :effect (and
    (not (update-status comp149))
    (update-status comp150)
    (not (rebooted comp149))
    (probabilistic 9/10 (and (running comp149)) 1/10 (and))
  )
)
(:action update-status-comp149-all-on
  :parameters ()
  :precondition (and
    (update-status comp149)
    (not (rebooted comp149))
    (all-on comp149)
  )
  :effect (and
    (not (update-status comp149))
    (update-status comp150)
    (not (all-on comp149))
    (probabilistic 1/20 (and (not (running comp149))))
  )
)
(:action update-status-comp149-one-off
  :parameters ()
  :precondition (and
    (update-status comp149)
    (not (rebooted comp149))
    (one-off comp149)
  )
  :effect (and
    (not (update-status comp149))
    (update-status comp150)
    (not (one-off comp149))
    (probabilistic 1/4 (and (not (running comp149))))
  )
)
(:action update-status-comp150-rebooted
  :parameters ()
  :precondition (and
    (update-status comp150)
    (rebooted comp150)
  )
  :effect (and
    (not (update-status comp150))
    (update-status comp151)
    (not (rebooted comp150))
    (probabilistic 9/10 (and (running comp150)) 1/10 (and))
  )
)
(:action update-status-comp150-all-on
  :parameters ()
  :precondition (and
    (update-status comp150)
    (not (rebooted comp150))
    (all-on comp150)
  )
  :effect (and
    (not (update-status comp150))
    (update-status comp151)
    (not (all-on comp150))
    (probabilistic 1/20 (and (not (running comp150))))
  )
)
(:action update-status-comp150-one-off
  :parameters ()
  :precondition (and
    (update-status comp150)
    (not (rebooted comp150))
    (one-off comp150)
  )
  :effect (and
    (not (update-status comp150))
    (update-status comp151)
    (not (one-off comp150))
    (probabilistic 1/4 (and (not (running comp150))))
  )
)
(:action update-status-comp151-rebooted
  :parameters ()
  :precondition (and
    (update-status comp151)
    (rebooted comp151)
  )
  :effect (and
    (not (update-status comp151))
    (update-status comp152)
    (not (rebooted comp151))
    (probabilistic 9/10 (and (running comp151)) 1/10 (and))
  )
)
(:action update-status-comp151-all-on
  :parameters ()
  :precondition (and
    (update-status comp151)
    (not (rebooted comp151))
    (all-on comp151)
  )
  :effect (and
    (not (update-status comp151))
    (update-status comp152)
    (not (all-on comp151))
    (probabilistic 1/20 (and (not (running comp151))))
  )
)
(:action update-status-comp151-one-off
  :parameters ()
  :precondition (and
    (update-status comp151)
    (not (rebooted comp151))
    (one-off comp151)
  )
  :effect (and
    (not (update-status comp151))
    (update-status comp152)
    (not (one-off comp151))
    (probabilistic 1/4 (and (not (running comp151))))
  )
)
(:action update-status-comp152-rebooted
  :parameters ()
  :precondition (and
    (update-status comp152)
    (rebooted comp152)
  )
  :effect (and
    (not (update-status comp152))
    (update-status comp153)
    (not (rebooted comp152))
    (probabilistic 9/10 (and (running comp152)) 1/10 (and))
  )
)
(:action update-status-comp152-all-on
  :parameters ()
  :precondition (and
    (update-status comp152)
    (not (rebooted comp152))
    (all-on comp152)
  )
  :effect (and
    (not (update-status comp152))
    (update-status comp153)
    (not (all-on comp152))
    (probabilistic 1/20 (and (not (running comp152))))
  )
)
(:action update-status-comp152-one-off
  :parameters ()
  :precondition (and
    (update-status comp152)
    (not (rebooted comp152))
    (one-off comp152)
  )
  :effect (and
    (not (update-status comp152))
    (update-status comp153)
    (not (one-off comp152))
    (probabilistic 1/4 (and (not (running comp152))))
  )
)
(:action update-status-comp153-rebooted
  :parameters ()
  :precondition (and
    (update-status comp153)
    (rebooted comp153)
  )
  :effect (and
    (not (update-status comp153))
    (update-status comp154)
    (not (rebooted comp153))
    (probabilistic 9/10 (and (running comp153)) 1/10 (and))
  )
)
(:action update-status-comp153-all-on
  :parameters ()
  :precondition (and
    (update-status comp153)
    (not (rebooted comp153))
    (all-on comp153)
  )
  :effect (and
    (not (update-status comp153))
    (update-status comp154)
    (not (all-on comp153))
    (probabilistic 1/20 (and (not (running comp153))))
  )
)
(:action update-status-comp153-one-off
  :parameters ()
  :precondition (and
    (update-status comp153)
    (not (rebooted comp153))
    (one-off comp153)
  )
  :effect (and
    (not (update-status comp153))
    (update-status comp154)
    (not (one-off comp153))
    (probabilistic 1/4 (and (not (running comp153))))
  )
)
(:action update-status-comp154-rebooted
  :parameters ()
  :precondition (and
    (update-status comp154)
    (rebooted comp154)
  )
  :effect (and
    (not (update-status comp154))
    (update-status comp155)
    (not (rebooted comp154))
    (probabilistic 9/10 (and (running comp154)) 1/10 (and))
  )
)
(:action update-status-comp154-all-on
  :parameters ()
  :precondition (and
    (update-status comp154)
    (not (rebooted comp154))
    (all-on comp154)
  )
  :effect (and
    (not (update-status comp154))
    (update-status comp155)
    (not (all-on comp154))
    (probabilistic 1/20 (and (not (running comp154))))
  )
)
(:action update-status-comp154-one-off
  :parameters ()
  :precondition (and
    (update-status comp154)
    (not (rebooted comp154))
    (one-off comp154)
  )
  :effect (and
    (not (update-status comp154))
    (update-status comp155)
    (not (one-off comp154))
    (probabilistic 1/4 (and (not (running comp154))))
  )
)
(:action update-status-comp155-rebooted
  :parameters ()
  :precondition (and
    (update-status comp155)
    (rebooted comp155)
  )
  :effect (and
    (not (update-status comp155))
    (update-status comp156)
    (not (rebooted comp155))
    (probabilistic 9/10 (and (running comp155)) 1/10 (and))
  )
)
(:action update-status-comp155-all-on
  :parameters ()
  :precondition (and
    (update-status comp155)
    (not (rebooted comp155))
    (all-on comp155)
  )
  :effect (and
    (not (update-status comp155))
    (update-status comp156)
    (not (all-on comp155))
    (probabilistic 1/20 (and (not (running comp155))))
  )
)
(:action update-status-comp155-one-off
  :parameters ()
  :precondition (and
    (update-status comp155)
    (not (rebooted comp155))
    (one-off comp155)
  )
  :effect (and
    (not (update-status comp155))
    (update-status comp156)
    (not (one-off comp155))
    (probabilistic 1/4 (and (not (running comp155))))
  )
)
(:action update-status-comp156-rebooted
  :parameters ()
  :precondition (and
    (update-status comp156)
    (rebooted comp156)
  )
  :effect (and
    (not (update-status comp156))
    (update-status comp157)
    (not (rebooted comp156))
    (probabilistic 9/10 (and (running comp156)) 1/10 (and))
  )
)
(:action update-status-comp156-all-on
  :parameters ()
  :precondition (and
    (update-status comp156)
    (not (rebooted comp156))
    (all-on comp156)
  )
  :effect (and
    (not (update-status comp156))
    (update-status comp157)
    (not (all-on comp156))
    (probabilistic 1/20 (and (not (running comp156))))
  )
)
(:action update-status-comp156-one-off
  :parameters ()
  :precondition (and
    (update-status comp156)
    (not (rebooted comp156))
    (one-off comp156)
  )
  :effect (and
    (not (update-status comp156))
    (update-status comp157)
    (not (one-off comp156))
    (probabilistic 1/4 (and (not (running comp156))))
  )
)
(:action update-status-comp157-rebooted
  :parameters ()
  :precondition (and
    (update-status comp157)
    (rebooted comp157)
  )
  :effect (and
    (not (update-status comp157))
    (update-status comp158)
    (not (rebooted comp157))
    (probabilistic 9/10 (and (running comp157)) 1/10 (and))
  )
)
(:action update-status-comp157-all-on
  :parameters ()
  :precondition (and
    (update-status comp157)
    (not (rebooted comp157))
    (all-on comp157)
  )
  :effect (and
    (not (update-status comp157))
    (update-status comp158)
    (not (all-on comp157))
    (probabilistic 1/20 (and (not (running comp157))))
  )
)
(:action update-status-comp157-one-off
  :parameters ()
  :precondition (and
    (update-status comp157)
    (not (rebooted comp157))
    (one-off comp157)
  )
  :effect (and
    (not (update-status comp157))
    (update-status comp158)
    (not (one-off comp157))
    (probabilistic 1/4 (and (not (running comp157))))
  )
)
(:action update-status-comp158-rebooted
  :parameters ()
  :precondition (and
    (update-status comp158)
    (rebooted comp158)
  )
  :effect (and
    (not (update-status comp158))
    (update-status comp159)
    (not (rebooted comp158))
    (probabilistic 9/10 (and (running comp158)) 1/10 (and))
  )
)
(:action update-status-comp158-all-on
  :parameters ()
  :precondition (and
    (update-status comp158)
    (not (rebooted comp158))
    (all-on comp158)
  )
  :effect (and
    (not (update-status comp158))
    (update-status comp159)
    (not (all-on comp158))
    (probabilistic 1/20 (and (not (running comp158))))
  )
)
(:action update-status-comp158-one-off
  :parameters ()
  :precondition (and
    (update-status comp158)
    (not (rebooted comp158))
    (one-off comp158)
  )
  :effect (and
    (not (update-status comp158))
    (update-status comp159)
    (not (one-off comp158))
    (probabilistic 1/4 (and (not (running comp158))))
  )
)
(:action update-status-comp159-rebooted
  :parameters ()
  :precondition (and
    (update-status comp159)
    (rebooted comp159)
  )
  :effect (and
    (not (update-status comp159))
    (update-status comp160)
    (not (rebooted comp159))
    (probabilistic 9/10 (and (running comp159)) 1/10 (and))
  )
)
(:action update-status-comp159-all-on
  :parameters ()
  :precondition (and
    (update-status comp159)
    (not (rebooted comp159))
    (all-on comp159)
  )
  :effect (and
    (not (update-status comp159))
    (update-status comp160)
    (not (all-on comp159))
    (probabilistic 1/20 (and (not (running comp159))))
  )
)
(:action update-status-comp159-one-off
  :parameters ()
  :precondition (and
    (update-status comp159)
    (not (rebooted comp159))
    (one-off comp159)
  )
  :effect (and
    (not (update-status comp159))
    (update-status comp160)
    (not (one-off comp159))
    (probabilistic 1/4 (and (not (running comp159))))
  )
)
(:action update-status-comp160-rebooted
  :parameters ()
  :precondition (and
    (update-status comp160)
    (rebooted comp160)
  )
  :effect (and
    (not (update-status comp160))
    (update-status comp161)
    (not (rebooted comp160))
    (probabilistic 9/10 (and (running comp160)) 1/10 (and))
  )
)
(:action update-status-comp160-all-on
  :parameters ()
  :precondition (and
    (update-status comp160)
    (not (rebooted comp160))
    (all-on comp160)
  )
  :effect (and
    (not (update-status comp160))
    (update-status comp161)
    (not (all-on comp160))
    (probabilistic 1/20 (and (not (running comp160))))
  )
)
(:action update-status-comp160-one-off
  :parameters ()
  :precondition (and
    (update-status comp160)
    (not (rebooted comp160))
    (one-off comp160)
  )
  :effect (and
    (not (update-status comp160))
    (update-status comp161)
    (not (one-off comp160))
    (probabilistic 1/4 (and (not (running comp160))))
  )
)
(:action update-status-comp161-rebooted
  :parameters ()
  :precondition (and
    (update-status comp161)
    (rebooted comp161)
  )
  :effect (and
    (not (update-status comp161))
    (update-status comp162)
    (not (rebooted comp161))
    (probabilistic 9/10 (and (running comp161)) 1/10 (and))
  )
)
(:action update-status-comp161-all-on
  :parameters ()
  :precondition (and
    (update-status comp161)
    (not (rebooted comp161))
    (all-on comp161)
  )
  :effect (and
    (not (update-status comp161))
    (update-status comp162)
    (not (all-on comp161))
    (probabilistic 1/20 (and (not (running comp161))))
  )
)
(:action update-status-comp161-one-off
  :parameters ()
  :precondition (and
    (update-status comp161)
    (not (rebooted comp161))
    (one-off comp161)
  )
  :effect (and
    (not (update-status comp161))
    (update-status comp162)
    (not (one-off comp161))
    (probabilistic 1/4 (and (not (running comp161))))
  )
)
(:action update-status-comp162-rebooted
  :parameters ()
  :precondition (and
    (update-status comp162)
    (rebooted comp162)
  )
  :effect (and
    (not (update-status comp162))
    (update-status comp163)
    (not (rebooted comp162))
    (probabilistic 9/10 (and (running comp162)) 1/10 (and))
  )
)
(:action update-status-comp162-all-on
  :parameters ()
  :precondition (and
    (update-status comp162)
    (not (rebooted comp162))
    (all-on comp162)
  )
  :effect (and
    (not (update-status comp162))
    (update-status comp163)
    (not (all-on comp162))
    (probabilistic 1/20 (and (not (running comp162))))
  )
)
(:action update-status-comp162-one-off
  :parameters ()
  :precondition (and
    (update-status comp162)
    (not (rebooted comp162))
    (one-off comp162)
  )
  :effect (and
    (not (update-status comp162))
    (update-status comp163)
    (not (one-off comp162))
    (probabilistic 1/4 (and (not (running comp162))))
  )
)
(:action update-status-comp163-rebooted
  :parameters ()
  :precondition (and
    (update-status comp163)
    (rebooted comp163)
  )
  :effect (and
    (not (update-status comp163))
    (update-status comp164)
    (not (rebooted comp163))
    (probabilistic 9/10 (and (running comp163)) 1/10 (and))
  )
)
(:action update-status-comp163-all-on
  :parameters ()
  :precondition (and
    (update-status comp163)
    (not (rebooted comp163))
    (all-on comp163)
  )
  :effect (and
    (not (update-status comp163))
    (update-status comp164)
    (not (all-on comp163))
    (probabilistic 1/20 (and (not (running comp163))))
  )
)
(:action update-status-comp163-one-off
  :parameters ()
  :precondition (and
    (update-status comp163)
    (not (rebooted comp163))
    (one-off comp163)
  )
  :effect (and
    (not (update-status comp163))
    (update-status comp164)
    (not (one-off comp163))
    (probabilistic 1/4 (and (not (running comp163))))
  )
)
(:action update-status-comp164-rebooted
  :parameters ()
  :precondition (and
    (update-status comp164)
    (rebooted comp164)
  )
  :effect (and
    (not (update-status comp164))
    (update-status comp165)
    (not (rebooted comp164))
    (probabilistic 9/10 (and (running comp164)) 1/10 (and))
  )
)
(:action update-status-comp164-all-on
  :parameters ()
  :precondition (and
    (update-status comp164)
    (not (rebooted comp164))
    (all-on comp164)
  )
  :effect (and
    (not (update-status comp164))
    (update-status comp165)
    (not (all-on comp164))
    (probabilistic 1/20 (and (not (running comp164))))
  )
)
(:action update-status-comp164-one-off
  :parameters ()
  :precondition (and
    (update-status comp164)
    (not (rebooted comp164))
    (one-off comp164)
  )
  :effect (and
    (not (update-status comp164))
    (update-status comp165)
    (not (one-off comp164))
    (probabilistic 1/4 (and (not (running comp164))))
  )
)
(:action update-status-comp165-rebooted
  :parameters ()
  :precondition (and
    (update-status comp165)
    (rebooted comp165)
  )
  :effect (and
    (not (update-status comp165))
    (update-status comp166)
    (not (rebooted comp165))
    (probabilistic 9/10 (and (running comp165)) 1/10 (and))
  )
)
(:action update-status-comp165-all-on
  :parameters ()
  :precondition (and
    (update-status comp165)
    (not (rebooted comp165))
    (all-on comp165)
  )
  :effect (and
    (not (update-status comp165))
    (update-status comp166)
    (not (all-on comp165))
    (probabilistic 1/20 (and (not (running comp165))))
  )
)
(:action update-status-comp165-one-off
  :parameters ()
  :precondition (and
    (update-status comp165)
    (not (rebooted comp165))
    (one-off comp165)
  )
  :effect (and
    (not (update-status comp165))
    (update-status comp166)
    (not (one-off comp165))
    (probabilistic 1/4 (and (not (running comp165))))
  )
)
(:action update-status-comp166-rebooted
  :parameters ()
  :precondition (and
    (update-status comp166)
    (rebooted comp166)
  )
  :effect (and
    (not (update-status comp166))
    (update-status comp167)
    (not (rebooted comp166))
    (probabilistic 9/10 (and (running comp166)) 1/10 (and))
  )
)
(:action update-status-comp166-all-on
  :parameters ()
  :precondition (and
    (update-status comp166)
    (not (rebooted comp166))
    (all-on comp166)
  )
  :effect (and
    (not (update-status comp166))
    (update-status comp167)
    (not (all-on comp166))
    (probabilistic 1/20 (and (not (running comp166))))
  )
)
(:action update-status-comp166-one-off
  :parameters ()
  :precondition (and
    (update-status comp166)
    (not (rebooted comp166))
    (one-off comp166)
  )
  :effect (and
    (not (update-status comp166))
    (update-status comp167)
    (not (one-off comp166))
    (probabilistic 1/4 (and (not (running comp166))))
  )
)
(:action update-status-comp167-rebooted
  :parameters ()
  :precondition (and
    (update-status comp167)
    (rebooted comp167)
  )
  :effect (and
    (not (update-status comp167))
    (update-status comp168)
    (not (rebooted comp167))
    (probabilistic 9/10 (and (running comp167)) 1/10 (and))
  )
)
(:action update-status-comp167-all-on
  :parameters ()
  :precondition (and
    (update-status comp167)
    (not (rebooted comp167))
    (all-on comp167)
  )
  :effect (and
    (not (update-status comp167))
    (update-status comp168)
    (not (all-on comp167))
    (probabilistic 1/20 (and (not (running comp167))))
  )
)
(:action update-status-comp167-one-off
  :parameters ()
  :precondition (and
    (update-status comp167)
    (not (rebooted comp167))
    (one-off comp167)
  )
  :effect (and
    (not (update-status comp167))
    (update-status comp168)
    (not (one-off comp167))
    (probabilistic 1/4 (and (not (running comp167))))
  )
)
(:action update-status-comp168-rebooted
  :parameters ()
  :precondition (and
    (update-status comp168)
    (rebooted comp168)
  )
  :effect (and
    (not (update-status comp168))
    (update-status comp169)
    (not (rebooted comp168))
    (probabilistic 9/10 (and (running comp168)) 1/10 (and))
  )
)
(:action update-status-comp168-all-on
  :parameters ()
  :precondition (and
    (update-status comp168)
    (not (rebooted comp168))
    (all-on comp168)
  )
  :effect (and
    (not (update-status comp168))
    (update-status comp169)
    (not (all-on comp168))
    (probabilistic 1/20 (and (not (running comp168))))
  )
)
(:action update-status-comp168-one-off
  :parameters ()
  :precondition (and
    (update-status comp168)
    (not (rebooted comp168))
    (one-off comp168)
  )
  :effect (and
    (not (update-status comp168))
    (update-status comp169)
    (not (one-off comp168))
    (probabilistic 1/4 (and (not (running comp168))))
  )
)
(:action update-status-comp169-rebooted
  :parameters ()
  :precondition (and
    (update-status comp169)
    (rebooted comp169)
  )
  :effect (and
    (not (update-status comp169))
    (update-status comp170)
    (not (rebooted comp169))
    (probabilistic 9/10 (and (running comp169)) 1/10 (and))
  )
)
(:action update-status-comp169-all-on
  :parameters ()
  :precondition (and
    (update-status comp169)
    (not (rebooted comp169))
    (all-on comp169)
  )
  :effect (and
    (not (update-status comp169))
    (update-status comp170)
    (not (all-on comp169))
    (probabilistic 1/20 (and (not (running comp169))))
  )
)
(:action update-status-comp169-one-off
  :parameters ()
  :precondition (and
    (update-status comp169)
    (not (rebooted comp169))
    (one-off comp169)
  )
  :effect (and
    (not (update-status comp169))
    (update-status comp170)
    (not (one-off comp169))
    (probabilistic 1/4 (and (not (running comp169))))
  )
)
(:action update-status-comp170-rebooted
  :parameters ()
  :precondition (and
    (update-status comp170)
    (rebooted comp170)
  )
  :effect (and
    (not (update-status comp170))
    (update-status comp171)
    (not (rebooted comp170))
    (probabilistic 9/10 (and (running comp170)) 1/10 (and))
  )
)
(:action update-status-comp170-all-on
  :parameters ()
  :precondition (and
    (update-status comp170)
    (not (rebooted comp170))
    (all-on comp170)
  )
  :effect (and
    (not (update-status comp170))
    (update-status comp171)
    (not (all-on comp170))
    (probabilistic 1/20 (and (not (running comp170))))
  )
)
(:action update-status-comp170-one-off
  :parameters ()
  :precondition (and
    (update-status comp170)
    (not (rebooted comp170))
    (one-off comp170)
  )
  :effect (and
    (not (update-status comp170))
    (update-status comp171)
    (not (one-off comp170))
    (probabilistic 1/4 (and (not (running comp170))))
  )
)
(:action update-status-comp171-rebooted
  :parameters ()
  :precondition (and
    (update-status comp171)
    (rebooted comp171)
  )
  :effect (and
    (not (update-status comp171))
    (update-status comp172)
    (not (rebooted comp171))
    (probabilistic 9/10 (and (running comp171)) 1/10 (and))
  )
)
(:action update-status-comp171-all-on
  :parameters ()
  :precondition (and
    (update-status comp171)
    (not (rebooted comp171))
    (all-on comp171)
  )
  :effect (and
    (not (update-status comp171))
    (update-status comp172)
    (not (all-on comp171))
    (probabilistic 1/20 (and (not (running comp171))))
  )
)
(:action update-status-comp171-one-off
  :parameters ()
  :precondition (and
    (update-status comp171)
    (not (rebooted comp171))
    (one-off comp171)
  )
  :effect (and
    (not (update-status comp171))
    (update-status comp172)
    (not (one-off comp171))
    (probabilistic 1/4 (and (not (running comp171))))
  )
)
(:action update-status-comp172-rebooted
  :parameters ()
  :precondition (and
    (update-status comp172)
    (rebooted comp172)
  )
  :effect (and
    (not (update-status comp172))
    (update-status comp173)
    (not (rebooted comp172))
    (probabilistic 9/10 (and (running comp172)) 1/10 (and))
  )
)
(:action update-status-comp172-all-on
  :parameters ()
  :precondition (and
    (update-status comp172)
    (not (rebooted comp172))
    (all-on comp172)
  )
  :effect (and
    (not (update-status comp172))
    (update-status comp173)
    (not (all-on comp172))
    (probabilistic 1/20 (and (not (running comp172))))
  )
)
(:action update-status-comp172-one-off
  :parameters ()
  :precondition (and
    (update-status comp172)
    (not (rebooted comp172))
    (one-off comp172)
  )
  :effect (and
    (not (update-status comp172))
    (update-status comp173)
    (not (one-off comp172))
    (probabilistic 1/4 (and (not (running comp172))))
  )
)
(:action update-status-comp173-rebooted
  :parameters ()
  :precondition (and
    (update-status comp173)
    (rebooted comp173)
  )
  :effect (and
    (not (update-status comp173))
    (update-status comp174)
    (not (rebooted comp173))
    (probabilistic 9/10 (and (running comp173)) 1/10 (and))
  )
)
(:action update-status-comp173-all-on
  :parameters ()
  :precondition (and
    (update-status comp173)
    (not (rebooted comp173))
    (all-on comp173)
  )
  :effect (and
    (not (update-status comp173))
    (update-status comp174)
    (not (all-on comp173))
    (probabilistic 1/20 (and (not (running comp173))))
  )
)
(:action update-status-comp173-one-off
  :parameters ()
  :precondition (and
    (update-status comp173)
    (not (rebooted comp173))
    (one-off comp173)
  )
  :effect (and
    (not (update-status comp173))
    (update-status comp174)
    (not (one-off comp173))
    (probabilistic 1/4 (and (not (running comp173))))
  )
)
(:action update-status-comp174-rebooted
  :parameters ()
  :precondition (and
    (update-status comp174)
    (rebooted comp174)
  )
  :effect (and
    (not (update-status comp174))
    (update-status comp175)
    (not (rebooted comp174))
    (probabilistic 9/10 (and (running comp174)) 1/10 (and))
  )
)
(:action update-status-comp174-all-on
  :parameters ()
  :precondition (and
    (update-status comp174)
    (not (rebooted comp174))
    (all-on comp174)
  )
  :effect (and
    (not (update-status comp174))
    (update-status comp175)
    (not (all-on comp174))
    (probabilistic 1/20 (and (not (running comp174))))
  )
)
(:action update-status-comp174-one-off
  :parameters ()
  :precondition (and
    (update-status comp174)
    (not (rebooted comp174))
    (one-off comp174)
  )
  :effect (and
    (not (update-status comp174))
    (update-status comp175)
    (not (one-off comp174))
    (probabilistic 1/4 (and (not (running comp174))))
  )
)
(:action update-status-comp175-rebooted
  :parameters ()
  :precondition (and
    (update-status comp175)
    (rebooted comp175)
  )
  :effect (and
    (not (update-status comp175))
    (update-status comp176)
    (not (rebooted comp175))
    (probabilistic 9/10 (and (running comp175)) 1/10 (and))
  )
)
(:action update-status-comp175-all-on
  :parameters ()
  :precondition (and
    (update-status comp175)
    (not (rebooted comp175))
    (all-on comp175)
  )
  :effect (and
    (not (update-status comp175))
    (update-status comp176)
    (not (all-on comp175))
    (probabilistic 1/20 (and (not (running comp175))))
  )
)
(:action update-status-comp175-one-off
  :parameters ()
  :precondition (and
    (update-status comp175)
    (not (rebooted comp175))
    (one-off comp175)
  )
  :effect (and
    (not (update-status comp175))
    (update-status comp176)
    (not (one-off comp175))
    (probabilistic 1/4 (and (not (running comp175))))
  )
)
(:action update-status-comp176-rebooted
  :parameters ()
  :precondition (and
    (update-status comp176)
    (rebooted comp176)
  )
  :effect (and
    (not (update-status comp176))
    (update-status comp177)
    (not (rebooted comp176))
    (probabilistic 9/10 (and (running comp176)) 1/10 (and))
  )
)
(:action update-status-comp176-all-on
  :parameters ()
  :precondition (and
    (update-status comp176)
    (not (rebooted comp176))
    (all-on comp176)
  )
  :effect (and
    (not (update-status comp176))
    (update-status comp177)
    (not (all-on comp176))
    (probabilistic 1/20 (and (not (running comp176))))
  )
)
(:action update-status-comp176-one-off
  :parameters ()
  :precondition (and
    (update-status comp176)
    (not (rebooted comp176))
    (one-off comp176)
  )
  :effect (and
    (not (update-status comp176))
    (update-status comp177)
    (not (one-off comp176))
    (probabilistic 1/4 (and (not (running comp176))))
  )
)
(:action update-status-comp177-rebooted
  :parameters ()
  :precondition (and
    (update-status comp177)
    (rebooted comp177)
  )
  :effect (and
    (not (update-status comp177))
    (update-status comp178)
    (not (rebooted comp177))
    (probabilistic 9/10 (and (running comp177)) 1/10 (and))
  )
)
(:action update-status-comp177-all-on
  :parameters ()
  :precondition (and
    (update-status comp177)
    (not (rebooted comp177))
    (all-on comp177)
  )
  :effect (and
    (not (update-status comp177))
    (update-status comp178)
    (not (all-on comp177))
    (probabilistic 1/20 (and (not (running comp177))))
  )
)
(:action update-status-comp177-one-off
  :parameters ()
  :precondition (and
    (update-status comp177)
    (not (rebooted comp177))
    (one-off comp177)
  )
  :effect (and
    (not (update-status comp177))
    (update-status comp178)
    (not (one-off comp177))
    (probabilistic 1/4 (and (not (running comp177))))
  )
)
(:action update-status-comp178-rebooted
  :parameters ()
  :precondition (and
    (update-status comp178)
    (rebooted comp178)
  )
  :effect (and
    (not (update-status comp178))
    (update-status comp179)
    (not (rebooted comp178))
    (probabilistic 9/10 (and (running comp178)) 1/10 (and))
  )
)
(:action update-status-comp178-all-on
  :parameters ()
  :precondition (and
    (update-status comp178)
    (not (rebooted comp178))
    (all-on comp178)
  )
  :effect (and
    (not (update-status comp178))
    (update-status comp179)
    (not (all-on comp178))
    (probabilistic 1/20 (and (not (running comp178))))
  )
)
(:action update-status-comp178-one-off
  :parameters ()
  :precondition (and
    (update-status comp178)
    (not (rebooted comp178))
    (one-off comp178)
  )
  :effect (and
    (not (update-status comp178))
    (update-status comp179)
    (not (one-off comp178))
    (probabilistic 1/4 (and (not (running comp178))))
  )
)
(:action update-status-comp179-rebooted
  :parameters ()
  :precondition (and
    (update-status comp179)
    (rebooted comp179)
  )
  :effect (and
    (not (update-status comp179))
    (update-status comp180)
    (not (rebooted comp179))
    (probabilistic 9/10 (and (running comp179)) 1/10 (and))
  )
)
(:action update-status-comp179-all-on
  :parameters ()
  :precondition (and
    (update-status comp179)
    (not (rebooted comp179))
    (all-on comp179)
  )
  :effect (and
    (not (update-status comp179))
    (update-status comp180)
    (not (all-on comp179))
    (probabilistic 1/20 (and (not (running comp179))))
  )
)
(:action update-status-comp179-one-off
  :parameters ()
  :precondition (and
    (update-status comp179)
    (not (rebooted comp179))
    (one-off comp179)
  )
  :effect (and
    (not (update-status comp179))
    (update-status comp180)
    (not (one-off comp179))
    (probabilistic 1/4 (and (not (running comp179))))
  )
)
(:action update-status-comp180-rebooted
  :parameters ()
  :precondition (and
    (update-status comp180)
    (rebooted comp180)
  )
  :effect (and
    (not (update-status comp180))
    (update-status comp181)
    (not (rebooted comp180))
    (probabilistic 9/10 (and (running comp180)) 1/10 (and))
  )
)
(:action update-status-comp180-all-on
  :parameters ()
  :precondition (and
    (update-status comp180)
    (not (rebooted comp180))
    (all-on comp180)
  )
  :effect (and
    (not (update-status comp180))
    (update-status comp181)
    (not (all-on comp180))
    (probabilistic 1/20 (and (not (running comp180))))
  )
)
(:action update-status-comp180-one-off
  :parameters ()
  :precondition (and
    (update-status comp180)
    (not (rebooted comp180))
    (one-off comp180)
  )
  :effect (and
    (not (update-status comp180))
    (update-status comp181)
    (not (one-off comp180))
    (probabilistic 1/4 (and (not (running comp180))))
  )
)
(:action update-status-comp181-rebooted
  :parameters ()
  :precondition (and
    (update-status comp181)
    (rebooted comp181)
  )
  :effect (and
    (not (update-status comp181))
    (update-status comp182)
    (not (rebooted comp181))
    (probabilistic 9/10 (and (running comp181)) 1/10 (and))
  )
)
(:action update-status-comp181-all-on
  :parameters ()
  :precondition (and
    (update-status comp181)
    (not (rebooted comp181))
    (all-on comp181)
  )
  :effect (and
    (not (update-status comp181))
    (update-status comp182)
    (not (all-on comp181))
    (probabilistic 1/20 (and (not (running comp181))))
  )
)
(:action update-status-comp181-one-off
  :parameters ()
  :precondition (and
    (update-status comp181)
    (not (rebooted comp181))
    (one-off comp181)
  )
  :effect (and
    (not (update-status comp181))
    (update-status comp182)
    (not (one-off comp181))
    (probabilistic 1/4 (and (not (running comp181))))
  )
)
(:action update-status-comp182-rebooted
  :parameters ()
  :precondition (and
    (update-status comp182)
    (rebooted comp182)
  )
  :effect (and
    (not (update-status comp182))
    (update-status comp183)
    (not (rebooted comp182))
    (probabilistic 9/10 (and (running comp182)) 1/10 (and))
  )
)
(:action update-status-comp182-all-on
  :parameters ()
  :precondition (and
    (update-status comp182)
    (not (rebooted comp182))
    (all-on comp182)
  )
  :effect (and
    (not (update-status comp182))
    (update-status comp183)
    (not (all-on comp182))
    (probabilistic 1/20 (and (not (running comp182))))
  )
)
(:action update-status-comp182-one-off
  :parameters ()
  :precondition (and
    (update-status comp182)
    (not (rebooted comp182))
    (one-off comp182)
  )
  :effect (and
    (not (update-status comp182))
    (update-status comp183)
    (not (one-off comp182))
    (probabilistic 1/4 (and (not (running comp182))))
  )
)
(:action update-status-comp183-rebooted
  :parameters ()
  :precondition (and
    (update-status comp183)
    (rebooted comp183)
  )
  :effect (and
    (not (update-status comp183))
    (update-status comp184)
    (not (rebooted comp183))
    (probabilistic 9/10 (and (running comp183)) 1/10 (and))
  )
)
(:action update-status-comp183-all-on
  :parameters ()
  :precondition (and
    (update-status comp183)
    (not (rebooted comp183))
    (all-on comp183)
  )
  :effect (and
    (not (update-status comp183))
    (update-status comp184)
    (not (all-on comp183))
    (probabilistic 1/20 (and (not (running comp183))))
  )
)
(:action update-status-comp183-one-off
  :parameters ()
  :precondition (and
    (update-status comp183)
    (not (rebooted comp183))
    (one-off comp183)
  )
  :effect (and
    (not (update-status comp183))
    (update-status comp184)
    (not (one-off comp183))
    (probabilistic 1/4 (and (not (running comp183))))
  )
)
(:action update-status-comp184-rebooted
  :parameters ()
  :precondition (and
    (update-status comp184)
    (rebooted comp184)
  )
  :effect (and
    (not (update-status comp184))
    (update-status comp185)
    (not (rebooted comp184))
    (probabilistic 9/10 (and (running comp184)) 1/10 (and))
  )
)
(:action update-status-comp184-all-on
  :parameters ()
  :precondition (and
    (update-status comp184)
    (not (rebooted comp184))
    (all-on comp184)
  )
  :effect (and
    (not (update-status comp184))
    (update-status comp185)
    (not (all-on comp184))
    (probabilistic 1/20 (and (not (running comp184))))
  )
)
(:action update-status-comp184-one-off
  :parameters ()
  :precondition (and
    (update-status comp184)
    (not (rebooted comp184))
    (one-off comp184)
  )
  :effect (and
    (not (update-status comp184))
    (update-status comp185)
    (not (one-off comp184))
    (probabilistic 1/4 (and (not (running comp184))))
  )
)
(:action update-status-comp185-rebooted
  :parameters ()
  :precondition (and
    (update-status comp185)
    (rebooted comp185)
  )
  :effect (and
    (not (update-status comp185))
    (update-status comp186)
    (not (rebooted comp185))
    (probabilistic 9/10 (and (running comp185)) 1/10 (and))
  )
)
(:action update-status-comp185-all-on
  :parameters ()
  :precondition (and
    (update-status comp185)
    (not (rebooted comp185))
    (all-on comp185)
  )
  :effect (and
    (not (update-status comp185))
    (update-status comp186)
    (not (all-on comp185))
    (probabilistic 1/20 (and (not (running comp185))))
  )
)
(:action update-status-comp185-one-off
  :parameters ()
  :precondition (and
    (update-status comp185)
    (not (rebooted comp185))
    (one-off comp185)
  )
  :effect (and
    (not (update-status comp185))
    (update-status comp186)
    (not (one-off comp185))
    (probabilistic 1/4 (and (not (running comp185))))
  )
)
(:action update-status-comp186-rebooted
  :parameters ()
  :precondition (and
    (update-status comp186)
    (rebooted comp186)
  )
  :effect (and
    (not (update-status comp186))
    (update-status comp187)
    (not (rebooted comp186))
    (probabilistic 9/10 (and (running comp186)) 1/10 (and))
  )
)
(:action update-status-comp186-all-on
  :parameters ()
  :precondition (and
    (update-status comp186)
    (not (rebooted comp186))
    (all-on comp186)
  )
  :effect (and
    (not (update-status comp186))
    (update-status comp187)
    (not (all-on comp186))
    (probabilistic 1/20 (and (not (running comp186))))
  )
)
(:action update-status-comp186-one-off
  :parameters ()
  :precondition (and
    (update-status comp186)
    (not (rebooted comp186))
    (one-off comp186)
  )
  :effect (and
    (not (update-status comp186))
    (update-status comp187)
    (not (one-off comp186))
    (probabilistic 1/4 (and (not (running comp186))))
  )
)
(:action update-status-comp187-rebooted
  :parameters ()
  :precondition (and
    (update-status comp187)
    (rebooted comp187)
  )
  :effect (and
    (not (update-status comp187))
    (update-status comp188)
    (not (rebooted comp187))
    (probabilistic 9/10 (and (running comp187)) 1/10 (and))
  )
)
(:action update-status-comp187-all-on
  :parameters ()
  :precondition (and
    (update-status comp187)
    (not (rebooted comp187))
    (all-on comp187)
  )
  :effect (and
    (not (update-status comp187))
    (update-status comp188)
    (not (all-on comp187))
    (probabilistic 1/20 (and (not (running comp187))))
  )
)
(:action update-status-comp187-one-off
  :parameters ()
  :precondition (and
    (update-status comp187)
    (not (rebooted comp187))
    (one-off comp187)
  )
  :effect (and
    (not (update-status comp187))
    (update-status comp188)
    (not (one-off comp187))
    (probabilistic 1/4 (and (not (running comp187))))
  )
)
(:action update-status-comp188-rebooted
  :parameters ()
  :precondition (and
    (update-status comp188)
    (rebooted comp188)
  )
  :effect (and
    (not (update-status comp188))
    (update-status comp189)
    (not (rebooted comp188))
    (probabilistic 9/10 (and (running comp188)) 1/10 (and))
  )
)
(:action update-status-comp188-all-on
  :parameters ()
  :precondition (and
    (update-status comp188)
    (not (rebooted comp188))
    (all-on comp188)
  )
  :effect (and
    (not (update-status comp188))
    (update-status comp189)
    (not (all-on comp188))
    (probabilistic 1/20 (and (not (running comp188))))
  )
)
(:action update-status-comp188-one-off
  :parameters ()
  :precondition (and
    (update-status comp188)
    (not (rebooted comp188))
    (one-off comp188)
  )
  :effect (and
    (not (update-status comp188))
    (update-status comp189)
    (not (one-off comp188))
    (probabilistic 1/4 (and (not (running comp188))))
  )
)
(:action update-status-comp189-rebooted
  :parameters ()
  :precondition (and
    (update-status comp189)
    (rebooted comp189)
  )
  :effect (and
    (not (update-status comp189))
    (update-status comp190)
    (not (rebooted comp189))
    (probabilistic 9/10 (and (running comp189)) 1/10 (and))
  )
)
(:action update-status-comp189-all-on
  :parameters ()
  :precondition (and
    (update-status comp189)
    (not (rebooted comp189))
    (all-on comp189)
  )
  :effect (and
    (not (update-status comp189))
    (update-status comp190)
    (not (all-on comp189))
    (probabilistic 1/20 (and (not (running comp189))))
  )
)
(:action update-status-comp189-one-off
  :parameters ()
  :precondition (and
    (update-status comp189)
    (not (rebooted comp189))
    (one-off comp189)
  )
  :effect (and
    (not (update-status comp189))
    (update-status comp190)
    (not (one-off comp189))
    (probabilistic 1/4 (and (not (running comp189))))
  )
)
(:action update-status-comp190-rebooted
  :parameters ()
  :precondition (and
    (update-status comp190)
    (rebooted comp190)
  )
  :effect (and
    (not (update-status comp190))
    (update-status comp191)
    (not (rebooted comp190))
    (probabilistic 9/10 (and (running comp190)) 1/10 (and))
  )
)
(:action update-status-comp190-all-on
  :parameters ()
  :precondition (and
    (update-status comp190)
    (not (rebooted comp190))
    (all-on comp190)
  )
  :effect (and
    (not (update-status comp190))
    (update-status comp191)
    (not (all-on comp190))
    (probabilistic 1/20 (and (not (running comp190))))
  )
)
(:action update-status-comp190-one-off
  :parameters ()
  :precondition (and
    (update-status comp190)
    (not (rebooted comp190))
    (one-off comp190)
  )
  :effect (and
    (not (update-status comp190))
    (update-status comp191)
    (not (one-off comp190))
    (probabilistic 1/4 (and (not (running comp190))))
  )
)
(:action update-status-comp191-rebooted
  :parameters ()
  :precondition (and
    (update-status comp191)
    (rebooted comp191)
  )
  :effect (and
    (not (update-status comp191))
    (update-status comp192)
    (not (rebooted comp191))
    (probabilistic 9/10 (and (running comp191)) 1/10 (and))
  )
)
(:action update-status-comp191-all-on
  :parameters ()
  :precondition (and
    (update-status comp191)
    (not (rebooted comp191))
    (all-on comp191)
  )
  :effect (and
    (not (update-status comp191))
    (update-status comp192)
    (not (all-on comp191))
    (probabilistic 1/20 (and (not (running comp191))))
  )
)
(:action update-status-comp191-one-off
  :parameters ()
  :precondition (and
    (update-status comp191)
    (not (rebooted comp191))
    (one-off comp191)
  )
  :effect (and
    (not (update-status comp191))
    (update-status comp192)
    (not (one-off comp191))
    (probabilistic 1/4 (and (not (running comp191))))
  )
)
(:action update-status-comp192-rebooted
  :parameters ()
  :precondition (and
    (update-status comp192)
    (rebooted comp192)
  )
  :effect (and
    (not (update-status comp192))
    (update-status comp193)
    (not (rebooted comp192))
    (probabilistic 9/10 (and (running comp192)) 1/10 (and))
  )
)
(:action update-status-comp192-all-on
  :parameters ()
  :precondition (and
    (update-status comp192)
    (not (rebooted comp192))
    (all-on comp192)
  )
  :effect (and
    (not (update-status comp192))
    (update-status comp193)
    (not (all-on comp192))
    (probabilistic 1/20 (and (not (running comp192))))
  )
)
(:action update-status-comp192-one-off
  :parameters ()
  :precondition (and
    (update-status comp192)
    (not (rebooted comp192))
    (one-off comp192)
  )
  :effect (and
    (not (update-status comp192))
    (update-status comp193)
    (not (one-off comp192))
    (probabilistic 1/4 (and (not (running comp192))))
  )
)
(:action update-status-comp193-rebooted
  :parameters ()
  :precondition (and
    (update-status comp193)
    (rebooted comp193)
  )
  :effect (and
    (not (update-status comp193))
    (update-status comp194)
    (not (rebooted comp193))
    (probabilistic 9/10 (and (running comp193)) 1/10 (and))
  )
)
(:action update-status-comp193-all-on
  :parameters ()
  :precondition (and
    (update-status comp193)
    (not (rebooted comp193))
    (all-on comp193)
  )
  :effect (and
    (not (update-status comp193))
    (update-status comp194)
    (not (all-on comp193))
    (probabilistic 1/20 (and (not (running comp193))))
  )
)
(:action update-status-comp193-one-off
  :parameters ()
  :precondition (and
    (update-status comp193)
    (not (rebooted comp193))
    (one-off comp193)
  )
  :effect (and
    (not (update-status comp193))
    (update-status comp194)
    (not (one-off comp193))
    (probabilistic 1/4 (and (not (running comp193))))
  )
)
(:action update-status-comp194-rebooted
  :parameters ()
  :precondition (and
    (update-status comp194)
    (rebooted comp194)
  )
  :effect (and
    (not (update-status comp194))
    (update-status comp195)
    (not (rebooted comp194))
    (probabilistic 9/10 (and (running comp194)) 1/10 (and))
  )
)
(:action update-status-comp194-all-on
  :parameters ()
  :precondition (and
    (update-status comp194)
    (not (rebooted comp194))
    (all-on comp194)
  )
  :effect (and
    (not (update-status comp194))
    (update-status comp195)
    (not (all-on comp194))
    (probabilistic 1/20 (and (not (running comp194))))
  )
)
(:action update-status-comp194-one-off
  :parameters ()
  :precondition (and
    (update-status comp194)
    (not (rebooted comp194))
    (one-off comp194)
  )
  :effect (and
    (not (update-status comp194))
    (update-status comp195)
    (not (one-off comp194))
    (probabilistic 1/4 (and (not (running comp194))))
  )
)
(:action update-status-comp195-rebooted
  :parameters ()
  :precondition (and
    (update-status comp195)
    (rebooted comp195)
  )
  :effect (and
    (not (update-status comp195))
    (update-status comp196)
    (not (rebooted comp195))
    (probabilistic 9/10 (and (running comp195)) 1/10 (and))
  )
)
(:action update-status-comp195-all-on
  :parameters ()
  :precondition (and
    (update-status comp195)
    (not (rebooted comp195))
    (all-on comp195)
  )
  :effect (and
    (not (update-status comp195))
    (update-status comp196)
    (not (all-on comp195))
    (probabilistic 1/20 (and (not (running comp195))))
  )
)
(:action update-status-comp195-one-off
  :parameters ()
  :precondition (and
    (update-status comp195)
    (not (rebooted comp195))
    (one-off comp195)
  )
  :effect (and
    (not (update-status comp195))
    (update-status comp196)
    (not (one-off comp195))
    (probabilistic 1/4 (and (not (running comp195))))
  )
)
(:action update-status-comp196-rebooted
  :parameters ()
  :precondition (and
    (update-status comp196)
    (rebooted comp196)
  )
  :effect (and
    (not (update-status comp196))
    (update-status comp197)
    (not (rebooted comp196))
    (probabilistic 9/10 (and (running comp196)) 1/10 (and))
  )
)
(:action update-status-comp196-all-on
  :parameters ()
  :precondition (and
    (update-status comp196)
    (not (rebooted comp196))
    (all-on comp196)
  )
  :effect (and
    (not (update-status comp196))
    (update-status comp197)
    (not (all-on comp196))
    (probabilistic 1/20 (and (not (running comp196))))
  )
)
(:action update-status-comp196-one-off
  :parameters ()
  :precondition (and
    (update-status comp196)
    (not (rebooted comp196))
    (one-off comp196)
  )
  :effect (and
    (not (update-status comp196))
    (update-status comp197)
    (not (one-off comp196))
    (probabilistic 1/4 (and (not (running comp196))))
  )
)
(:action update-status-comp197-rebooted
  :parameters ()
  :precondition (and
    (update-status comp197)
    (rebooted comp197)
  )
  :effect (and
    (not (update-status comp197))
    (update-status comp198)
    (not (rebooted comp197))
    (probabilistic 9/10 (and (running comp197)) 1/10 (and))
  )
)
(:action update-status-comp197-all-on
  :parameters ()
  :precondition (and
    (update-status comp197)
    (not (rebooted comp197))
    (all-on comp197)
  )
  :effect (and
    (not (update-status comp197))
    (update-status comp198)
    (not (all-on comp197))
    (probabilistic 1/20 (and (not (running comp197))))
  )
)
(:action update-status-comp197-one-off
  :parameters ()
  :precondition (and
    (update-status comp197)
    (not (rebooted comp197))
    (one-off comp197)
  )
  :effect (and
    (not (update-status comp197))
    (update-status comp198)
    (not (one-off comp197))
    (probabilistic 1/4 (and (not (running comp197))))
  )
)
(:action update-status-comp198-rebooted
  :parameters ()
  :precondition (and
    (update-status comp198)
    (rebooted comp198)
  )
  :effect (and
    (not (update-status comp198))
    (update-status comp199)
    (not (rebooted comp198))
    (probabilistic 9/10 (and (running comp198)) 1/10 (and))
  )
)
(:action update-status-comp198-all-on
  :parameters ()
  :precondition (and
    (update-status comp198)
    (not (rebooted comp198))
    (all-on comp198)
  )
  :effect (and
    (not (update-status comp198))
    (update-status comp199)
    (not (all-on comp198))
    (probabilistic 1/20 (and (not (running comp198))))
  )
)
(:action update-status-comp198-one-off
  :parameters ()
  :precondition (and
    (update-status comp198)
    (not (rebooted comp198))
    (one-off comp198)
  )
  :effect (and
    (not (update-status comp198))
    (update-status comp199)
    (not (one-off comp198))
    (probabilistic 1/4 (and (not (running comp198))))
  )
)
(:action update-status-comp199-rebooted
  :parameters ()
  :precondition (and
    (update-status comp199)
    (rebooted comp199)
  )
  :effect (and
    (not (update-status comp199))
    (update-status comp200)
    (not (rebooted comp199))
    (probabilistic 9/10 (and (running comp199)) 1/10 (and))
  )
)
(:action update-status-comp199-all-on
  :parameters ()
  :precondition (and
    (update-status comp199)
    (not (rebooted comp199))
    (all-on comp199)
  )
  :effect (and
    (not (update-status comp199))
    (update-status comp200)
    (not (all-on comp199))
    (probabilistic 1/20 (and (not (running comp199))))
  )
)
(:action update-status-comp199-one-off
  :parameters ()
  :precondition (and
    (update-status comp199)
    (not (rebooted comp199))
    (one-off comp199)
  )
  :effect (and
    (not (update-status comp199))
    (update-status comp200)
    (not (one-off comp199))
    (probabilistic 1/4 (and (not (running comp199))))
  )
)
(:action update-status-comp200-rebooted
  :parameters ()
  :precondition (and
    (update-status comp200)
    (rebooted comp200)
  )
  :effect (and
    (not (update-status comp200))
    (update-status comp201)
    (not (rebooted comp200))
    (probabilistic 9/10 (and (running comp200)) 1/10 (and))
  )
)
(:action update-status-comp200-all-on
  :parameters ()
  :precondition (and
    (update-status comp200)
    (not (rebooted comp200))
    (all-on comp200)
  )
  :effect (and
    (not (update-status comp200))
    (update-status comp201)
    (not (all-on comp200))
    (probabilistic 1/20 (and (not (running comp200))))
  )
)
(:action update-status-comp200-one-off
  :parameters ()
  :precondition (and
    (update-status comp200)
    (not (rebooted comp200))
    (one-off comp200)
  )
  :effect (and
    (not (update-status comp200))
    (update-status comp201)
    (not (one-off comp200))
    (probabilistic 1/4 (and (not (running comp200))))
  )
)
(:action update-status-comp201-rebooted
  :parameters ()
  :precondition (and
    (update-status comp201)
    (rebooted comp201)
  )
  :effect (and
    (not (update-status comp201))
    (update-status comp202)
    (not (rebooted comp201))
    (probabilistic 9/10 (and (running comp201)) 1/10 (and))
  )
)
(:action update-status-comp201-all-on
  :parameters ()
  :precondition (and
    (update-status comp201)
    (not (rebooted comp201))
    (all-on comp201)
  )
  :effect (and
    (not (update-status comp201))
    (update-status comp202)
    (not (all-on comp201))
    (probabilistic 1/20 (and (not (running comp201))))
  )
)
(:action update-status-comp201-one-off
  :parameters ()
  :precondition (and
    (update-status comp201)
    (not (rebooted comp201))
    (one-off comp201)
  )
  :effect (and
    (not (update-status comp201))
    (update-status comp202)
    (not (one-off comp201))
    (probabilistic 1/4 (and (not (running comp201))))
  )
)
(:action update-status-comp202-rebooted
  :parameters ()
  :precondition (and
    (update-status comp202)
    (rebooted comp202)
  )
  :effect (and
    (not (update-status comp202))
    (update-status comp203)
    (not (rebooted comp202))
    (probabilistic 9/10 (and (running comp202)) 1/10 (and))
  )
)
(:action update-status-comp202-all-on
  :parameters ()
  :precondition (and
    (update-status comp202)
    (not (rebooted comp202))
    (all-on comp202)
  )
  :effect (and
    (not (update-status comp202))
    (update-status comp203)
    (not (all-on comp202))
    (probabilistic 1/20 (and (not (running comp202))))
  )
)
(:action update-status-comp202-one-off
  :parameters ()
  :precondition (and
    (update-status comp202)
    (not (rebooted comp202))
    (one-off comp202)
  )
  :effect (and
    (not (update-status comp202))
    (update-status comp203)
    (not (one-off comp202))
    (probabilistic 1/4 (and (not (running comp202))))
  )
)
(:action update-status-comp203-rebooted
  :parameters ()
  :precondition (and
    (update-status comp203)
    (rebooted comp203)
  )
  :effect (and
    (not (update-status comp203))
    (update-status comp204)
    (not (rebooted comp203))
    (probabilistic 9/10 (and (running comp203)) 1/10 (and))
  )
)
(:action update-status-comp203-all-on
  :parameters ()
  :precondition (and
    (update-status comp203)
    (not (rebooted comp203))
    (all-on comp203)
  )
  :effect (and
    (not (update-status comp203))
    (update-status comp204)
    (not (all-on comp203))
    (probabilistic 1/20 (and (not (running comp203))))
  )
)
(:action update-status-comp203-one-off
  :parameters ()
  :precondition (and
    (update-status comp203)
    (not (rebooted comp203))
    (one-off comp203)
  )
  :effect (and
    (not (update-status comp203))
    (update-status comp204)
    (not (one-off comp203))
    (probabilistic 1/4 (and (not (running comp203))))
  )
)
(:action update-status-comp204-rebooted
  :parameters ()
  :precondition (and
    (update-status comp204)
    (rebooted comp204)
  )
  :effect (and
    (not (update-status comp204))
    (update-status comp205)
    (not (rebooted comp204))
    (probabilistic 9/10 (and (running comp204)) 1/10 (and))
  )
)
(:action update-status-comp204-all-on
  :parameters ()
  :precondition (and
    (update-status comp204)
    (not (rebooted comp204))
    (all-on comp204)
  )
  :effect (and
    (not (update-status comp204))
    (update-status comp205)
    (not (all-on comp204))
    (probabilistic 1/20 (and (not (running comp204))))
  )
)
(:action update-status-comp204-one-off
  :parameters ()
  :precondition (and
    (update-status comp204)
    (not (rebooted comp204))
    (one-off comp204)
  )
  :effect (and
    (not (update-status comp204))
    (update-status comp205)
    (not (one-off comp204))
    (probabilistic 1/4 (and (not (running comp204))))
  )
)
(:action update-status-comp205-rebooted
  :parameters ()
  :precondition (and
    (update-status comp205)
    (rebooted comp205)
  )
  :effect (and
    (not (update-status comp205))
    (update-status comp206)
    (not (rebooted comp205))
    (probabilistic 9/10 (and (running comp205)) 1/10 (and))
  )
)
(:action update-status-comp205-all-on
  :parameters ()
  :precondition (and
    (update-status comp205)
    (not (rebooted comp205))
    (all-on comp205)
  )
  :effect (and
    (not (update-status comp205))
    (update-status comp206)
    (not (all-on comp205))
    (probabilistic 1/20 (and (not (running comp205))))
  )
)
(:action update-status-comp205-one-off
  :parameters ()
  :precondition (and
    (update-status comp205)
    (not (rebooted comp205))
    (one-off comp205)
  )
  :effect (and
    (not (update-status comp205))
    (update-status comp206)
    (not (one-off comp205))
    (probabilistic 1/4 (and (not (running comp205))))
  )
)
(:action update-status-comp206-rebooted
  :parameters ()
  :precondition (and
    (update-status comp206)
    (rebooted comp206)
  )
  :effect (and
    (not (update-status comp206))
    (update-status comp207)
    (not (rebooted comp206))
    (probabilistic 9/10 (and (running comp206)) 1/10 (and))
  )
)
(:action update-status-comp206-all-on
  :parameters ()
  :precondition (and
    (update-status comp206)
    (not (rebooted comp206))
    (all-on comp206)
  )
  :effect (and
    (not (update-status comp206))
    (update-status comp207)
    (not (all-on comp206))
    (probabilistic 1/20 (and (not (running comp206))))
  )
)
(:action update-status-comp206-one-off
  :parameters ()
  :precondition (and
    (update-status comp206)
    (not (rebooted comp206))
    (one-off comp206)
  )
  :effect (and
    (not (update-status comp206))
    (update-status comp207)
    (not (one-off comp206))
    (probabilistic 1/4 (and (not (running comp206))))
  )
)
(:action update-status-comp207-rebooted
  :parameters ()
  :precondition (and
    (update-status comp207)
    (rebooted comp207)
  )
  :effect (and
    (not (update-status comp207))
    (update-status comp208)
    (not (rebooted comp207))
    (probabilistic 9/10 (and (running comp207)) 1/10 (and))
  )
)
(:action update-status-comp207-all-on
  :parameters ()
  :precondition (and
    (update-status comp207)
    (not (rebooted comp207))
    (all-on comp207)
  )
  :effect (and
    (not (update-status comp207))
    (update-status comp208)
    (not (all-on comp207))
    (probabilistic 1/20 (and (not (running comp207))))
  )
)
(:action update-status-comp207-one-off
  :parameters ()
  :precondition (and
    (update-status comp207)
    (not (rebooted comp207))
    (one-off comp207)
  )
  :effect (and
    (not (update-status comp207))
    (update-status comp208)
    (not (one-off comp207))
    (probabilistic 1/4 (and (not (running comp207))))
  )
)
(:action update-status-comp208-rebooted
  :parameters ()
  :precondition (and
    (update-status comp208)
    (rebooted comp208)
  )
  :effect (and
    (not (update-status comp208))
    (update-status comp209)
    (not (rebooted comp208))
    (probabilistic 9/10 (and (running comp208)) 1/10 (and))
  )
)
(:action update-status-comp208-all-on
  :parameters ()
  :precondition (and
    (update-status comp208)
    (not (rebooted comp208))
    (all-on comp208)
  )
  :effect (and
    (not (update-status comp208))
    (update-status comp209)
    (not (all-on comp208))
    (probabilistic 1/20 (and (not (running comp208))))
  )
)
(:action update-status-comp208-one-off
  :parameters ()
  :precondition (and
    (update-status comp208)
    (not (rebooted comp208))
    (one-off comp208)
  )
  :effect (and
    (not (update-status comp208))
    (update-status comp209)
    (not (one-off comp208))
    (probabilistic 1/4 (and (not (running comp208))))
  )
)
(:action update-status-comp209-rebooted
  :parameters ()
  :precondition (and
    (update-status comp209)
    (rebooted comp209)
  )
  :effect (and
    (not (update-status comp209))
    (update-status comp210)
    (not (rebooted comp209))
    (probabilistic 9/10 (and (running comp209)) 1/10 (and))
  )
)
(:action update-status-comp209-all-on
  :parameters ()
  :precondition (and
    (update-status comp209)
    (not (rebooted comp209))
    (all-on comp209)
  )
  :effect (and
    (not (update-status comp209))
    (update-status comp210)
    (not (all-on comp209))
    (probabilistic 1/20 (and (not (running comp209))))
  )
)
(:action update-status-comp209-one-off
  :parameters ()
  :precondition (and
    (update-status comp209)
    (not (rebooted comp209))
    (one-off comp209)
  )
  :effect (and
    (not (update-status comp209))
    (update-status comp210)
    (not (one-off comp209))
    (probabilistic 1/4 (and (not (running comp209))))
  )
)
(:action update-status-comp210-rebooted
  :parameters ()
  :precondition (and
    (update-status comp210)
    (rebooted comp210)
  )
  :effect (and
    (not (update-status comp210))
    (update-status comp211)
    (not (rebooted comp210))
    (probabilistic 9/10 (and (running comp210)) 1/10 (and))
  )
)
(:action update-status-comp210-all-on
  :parameters ()
  :precondition (and
    (update-status comp210)
    (not (rebooted comp210))
    (all-on comp210)
  )
  :effect (and
    (not (update-status comp210))
    (update-status comp211)
    (not (all-on comp210))
    (probabilistic 1/20 (and (not (running comp210))))
  )
)
(:action update-status-comp210-one-off
  :parameters ()
  :precondition (and
    (update-status comp210)
    (not (rebooted comp210))
    (one-off comp210)
  )
  :effect (and
    (not (update-status comp210))
    (update-status comp211)
    (not (one-off comp210))
    (probabilistic 1/4 (and (not (running comp210))))
  )
)
(:action update-status-comp211-rebooted
  :parameters ()
  :precondition (and
    (update-status comp211)
    (rebooted comp211)
  )
  :effect (and
    (not (update-status comp211))
    (update-status comp212)
    (not (rebooted comp211))
    (probabilistic 9/10 (and (running comp211)) 1/10 (and))
  )
)
(:action update-status-comp211-all-on
  :parameters ()
  :precondition (and
    (update-status comp211)
    (not (rebooted comp211))
    (all-on comp211)
  )
  :effect (and
    (not (update-status comp211))
    (update-status comp212)
    (not (all-on comp211))
    (probabilistic 1/20 (and (not (running comp211))))
  )
)
(:action update-status-comp211-one-off
  :parameters ()
  :precondition (and
    (update-status comp211)
    (not (rebooted comp211))
    (one-off comp211)
  )
  :effect (and
    (not (update-status comp211))
    (update-status comp212)
    (not (one-off comp211))
    (probabilistic 1/4 (and (not (running comp211))))
  )
)
(:action update-status-comp212-rebooted
  :parameters ()
  :precondition (and
    (update-status comp212)
    (rebooted comp212)
  )
  :effect (and
    (not (update-status comp212))
    (update-status comp213)
    (not (rebooted comp212))
    (probabilistic 9/10 (and (running comp212)) 1/10 (and))
  )
)
(:action update-status-comp212-all-on
  :parameters ()
  :precondition (and
    (update-status comp212)
    (not (rebooted comp212))
    (all-on comp212)
  )
  :effect (and
    (not (update-status comp212))
    (update-status comp213)
    (not (all-on comp212))
    (probabilistic 1/20 (and (not (running comp212))))
  )
)
(:action update-status-comp212-one-off
  :parameters ()
  :precondition (and
    (update-status comp212)
    (not (rebooted comp212))
    (one-off comp212)
  )
  :effect (and
    (not (update-status comp212))
    (update-status comp213)
    (not (one-off comp212))
    (probabilistic 1/4 (and (not (running comp212))))
  )
)
(:action update-status-comp213-rebooted
  :parameters ()
  :precondition (and
    (update-status comp213)
    (rebooted comp213)
  )
  :effect (and
    (not (update-status comp213))
    (update-status comp214)
    (not (rebooted comp213))
    (probabilistic 9/10 (and (running comp213)) 1/10 (and))
  )
)
(:action update-status-comp213-all-on
  :parameters ()
  :precondition (and
    (update-status comp213)
    (not (rebooted comp213))
    (all-on comp213)
  )
  :effect (and
    (not (update-status comp213))
    (update-status comp214)
    (not (all-on comp213))
    (probabilistic 1/20 (and (not (running comp213))))
  )
)
(:action update-status-comp213-one-off
  :parameters ()
  :precondition (and
    (update-status comp213)
    (not (rebooted comp213))
    (one-off comp213)
  )
  :effect (and
    (not (update-status comp213))
    (update-status comp214)
    (not (one-off comp213))
    (probabilistic 1/4 (and (not (running comp213))))
  )
)
(:action update-status-comp214-rebooted
  :parameters ()
  :precondition (and
    (update-status comp214)
    (rebooted comp214)
  )
  :effect (and
    (not (update-status comp214))
    (update-status comp215)
    (not (rebooted comp214))
    (probabilistic 9/10 (and (running comp214)) 1/10 (and))
  )
)
(:action update-status-comp214-all-on
  :parameters ()
  :precondition (and
    (update-status comp214)
    (not (rebooted comp214))
    (all-on comp214)
  )
  :effect (and
    (not (update-status comp214))
    (update-status comp215)
    (not (all-on comp214))
    (probabilistic 1/20 (and (not (running comp214))))
  )
)
(:action update-status-comp214-one-off
  :parameters ()
  :precondition (and
    (update-status comp214)
    (not (rebooted comp214))
    (one-off comp214)
  )
  :effect (and
    (not (update-status comp214))
    (update-status comp215)
    (not (one-off comp214))
    (probabilistic 1/4 (and (not (running comp214))))
  )
)
(:action update-status-comp215-rebooted
  :parameters ()
  :precondition (and
    (update-status comp215)
    (rebooted comp215)
  )
  :effect (and
    (not (update-status comp215))
    (update-status comp216)
    (not (rebooted comp215))
    (probabilistic 9/10 (and (running comp215)) 1/10 (and))
  )
)
(:action update-status-comp215-all-on
  :parameters ()
  :precondition (and
    (update-status comp215)
    (not (rebooted comp215))
    (all-on comp215)
  )
  :effect (and
    (not (update-status comp215))
    (update-status comp216)
    (not (all-on comp215))
    (probabilistic 1/20 (and (not (running comp215))))
  )
)
(:action update-status-comp215-one-off
  :parameters ()
  :precondition (and
    (update-status comp215)
    (not (rebooted comp215))
    (one-off comp215)
  )
  :effect (and
    (not (update-status comp215))
    (update-status comp216)
    (not (one-off comp215))
    (probabilistic 1/4 (and (not (running comp215))))
  )
)
(:action update-status-comp216-rebooted
  :parameters ()
  :precondition (and
    (update-status comp216)
    (rebooted comp216)
  )
  :effect (and
    (not (update-status comp216))
    (update-status comp217)
    (not (rebooted comp216))
    (probabilistic 9/10 (and (running comp216)) 1/10 (and))
  )
)
(:action update-status-comp216-all-on
  :parameters ()
  :precondition (and
    (update-status comp216)
    (not (rebooted comp216))
    (all-on comp216)
  )
  :effect (and
    (not (update-status comp216))
    (update-status comp217)
    (not (all-on comp216))
    (probabilistic 1/20 (and (not (running comp216))))
  )
)
(:action update-status-comp216-one-off
  :parameters ()
  :precondition (and
    (update-status comp216)
    (not (rebooted comp216))
    (one-off comp216)
  )
  :effect (and
    (not (update-status comp216))
    (update-status comp217)
    (not (one-off comp216))
    (probabilistic 1/4 (and (not (running comp216))))
  )
)
(:action update-status-comp217-rebooted
  :parameters ()
  :precondition (and
    (update-status comp217)
    (rebooted comp217)
  )
  :effect (and
    (not (update-status comp217))
    (update-status comp218)
    (not (rebooted comp217))
    (probabilistic 9/10 (and (running comp217)) 1/10 (and))
  )
)
(:action update-status-comp217-all-on
  :parameters ()
  :precondition (and
    (update-status comp217)
    (not (rebooted comp217))
    (all-on comp217)
  )
  :effect (and
    (not (update-status comp217))
    (update-status comp218)
    (not (all-on comp217))
    (probabilistic 1/20 (and (not (running comp217))))
  )
)
(:action update-status-comp217-one-off
  :parameters ()
  :precondition (and
    (update-status comp217)
    (not (rebooted comp217))
    (one-off comp217)
  )
  :effect (and
    (not (update-status comp217))
    (update-status comp218)
    (not (one-off comp217))
    (probabilistic 1/4 (and (not (running comp217))))
  )
)
(:action update-status-comp218-rebooted
  :parameters ()
  :precondition (and
    (update-status comp218)
    (rebooted comp218)
  )
  :effect (and
    (not (update-status comp218))
    (update-status comp219)
    (not (rebooted comp218))
    (probabilistic 9/10 (and (running comp218)) 1/10 (and))
  )
)
(:action update-status-comp218-all-on
  :parameters ()
  :precondition (and
    (update-status comp218)
    (not (rebooted comp218))
    (all-on comp218)
  )
  :effect (and
    (not (update-status comp218))
    (update-status comp219)
    (not (all-on comp218))
    (probabilistic 1/20 (and (not (running comp218))))
  )
)
(:action update-status-comp218-one-off
  :parameters ()
  :precondition (and
    (update-status comp218)
    (not (rebooted comp218))
    (one-off comp218)
  )
  :effect (and
    (not (update-status comp218))
    (update-status comp219)
    (not (one-off comp218))
    (probabilistic 1/4 (and (not (running comp218))))
  )
)
(:action update-status-comp219-rebooted
  :parameters ()
  :precondition (and
    (update-status comp219)
    (rebooted comp219)
  )
  :effect (and
    (not (update-status comp219))
    (update-status comp220)
    (not (rebooted comp219))
    (probabilistic 9/10 (and (running comp219)) 1/10 (and))
  )
)
(:action update-status-comp219-all-on
  :parameters ()
  :precondition (and
    (update-status comp219)
    (not (rebooted comp219))
    (all-on comp219)
  )
  :effect (and
    (not (update-status comp219))
    (update-status comp220)
    (not (all-on comp219))
    (probabilistic 1/20 (and (not (running comp219))))
  )
)
(:action update-status-comp219-one-off
  :parameters ()
  :precondition (and
    (update-status comp219)
    (not (rebooted comp219))
    (one-off comp219)
  )
  :effect (and
    (not (update-status comp219))
    (update-status comp220)
    (not (one-off comp219))
    (probabilistic 1/4 (and (not (running comp219))))
  )
)
(:action update-status-comp220-rebooted
  :parameters ()
  :precondition (and
    (update-status comp220)
    (rebooted comp220)
  )
  :effect (and
    (not (update-status comp220))
    (update-status comp221)
    (not (rebooted comp220))
    (probabilistic 9/10 (and (running comp220)) 1/10 (and))
  )
)
(:action update-status-comp220-all-on
  :parameters ()
  :precondition (and
    (update-status comp220)
    (not (rebooted comp220))
    (all-on comp220)
  )
  :effect (and
    (not (update-status comp220))
    (update-status comp221)
    (not (all-on comp220))
    (probabilistic 1/20 (and (not (running comp220))))
  )
)
(:action update-status-comp220-one-off
  :parameters ()
  :precondition (and
    (update-status comp220)
    (not (rebooted comp220))
    (one-off comp220)
  )
  :effect (and
    (not (update-status comp220))
    (update-status comp221)
    (not (one-off comp220))
    (probabilistic 1/4 (and (not (running comp220))))
  )
)
(:action update-status-comp221-rebooted
  :parameters ()
  :precondition (and
    (update-status comp221)
    (rebooted comp221)
  )
  :effect (and
    (not (update-status comp221))
    (update-status comp222)
    (not (rebooted comp221))
    (probabilistic 9/10 (and (running comp221)) 1/10 (and))
  )
)
(:action update-status-comp221-all-on
  :parameters ()
  :precondition (and
    (update-status comp221)
    (not (rebooted comp221))
    (all-on comp221)
  )
  :effect (and
    (not (update-status comp221))
    (update-status comp222)
    (not (all-on comp221))
    (probabilistic 1/20 (and (not (running comp221))))
  )
)
(:action update-status-comp221-one-off
  :parameters ()
  :precondition (and
    (update-status comp221)
    (not (rebooted comp221))
    (one-off comp221)
  )
  :effect (and
    (not (update-status comp221))
    (update-status comp222)
    (not (one-off comp221))
    (probabilistic 1/4 (and (not (running comp221))))
  )
)
(:action update-status-comp222-rebooted
  :parameters ()
  :precondition (and
    (update-status comp222)
    (rebooted comp222)
  )
  :effect (and
    (not (update-status comp222))
    (update-status comp223)
    (not (rebooted comp222))
    (probabilistic 9/10 (and (running comp222)) 1/10 (and))
  )
)
(:action update-status-comp222-all-on
  :parameters ()
  :precondition (and
    (update-status comp222)
    (not (rebooted comp222))
    (all-on comp222)
  )
  :effect (and
    (not (update-status comp222))
    (update-status comp223)
    (not (all-on comp222))
    (probabilistic 1/20 (and (not (running comp222))))
  )
)
(:action update-status-comp222-one-off
  :parameters ()
  :precondition (and
    (update-status comp222)
    (not (rebooted comp222))
    (one-off comp222)
  )
  :effect (and
    (not (update-status comp222))
    (update-status comp223)
    (not (one-off comp222))
    (probabilistic 1/4 (and (not (running comp222))))
  )
)
(:action update-status-comp223-rebooted
  :parameters ()
  :precondition (and
    (update-status comp223)
    (rebooted comp223)
  )
  :effect (and
    (not (update-status comp223))
    (update-status comp224)
    (not (rebooted comp223))
    (probabilistic 9/10 (and (running comp223)) 1/10 (and))
  )
)
(:action update-status-comp223-all-on
  :parameters ()
  :precondition (and
    (update-status comp223)
    (not (rebooted comp223))
    (all-on comp223)
  )
  :effect (and
    (not (update-status comp223))
    (update-status comp224)
    (not (all-on comp223))
    (probabilistic 1/20 (and (not (running comp223))))
  )
)
(:action update-status-comp223-one-off
  :parameters ()
  :precondition (and
    (update-status comp223)
    (not (rebooted comp223))
    (one-off comp223)
  )
  :effect (and
    (not (update-status comp223))
    (update-status comp224)
    (not (one-off comp223))
    (probabilistic 1/4 (and (not (running comp223))))
  )
)
(:action update-status-comp224-rebooted
  :parameters ()
  :precondition (and
    (update-status comp224)
    (rebooted comp224)
  )
  :effect (and
    (not (update-status comp224))
    (update-status comp225)
    (not (rebooted comp224))
    (probabilistic 9/10 (and (running comp224)) 1/10 (and))
  )
)
(:action update-status-comp224-all-on
  :parameters ()
  :precondition (and
    (update-status comp224)
    (not (rebooted comp224))
    (all-on comp224)
  )
  :effect (and
    (not (update-status comp224))
    (update-status comp225)
    (not (all-on comp224))
    (probabilistic 1/20 (and (not (running comp224))))
  )
)
(:action update-status-comp224-one-off
  :parameters ()
  :precondition (and
    (update-status comp224)
    (not (rebooted comp224))
    (one-off comp224)
  )
  :effect (and
    (not (update-status comp224))
    (update-status comp225)
    (not (one-off comp224))
    (probabilistic 1/4 (and (not (running comp224))))
  )
)
(:action update-status-comp225-rebooted
  :parameters ()
  :precondition (and
    (update-status comp225)
    (rebooted comp225)
  )
  :effect (and
    (not (update-status comp225))
    (update-status comp226)
    (not (rebooted comp225))
    (probabilistic 9/10 (and (running comp225)) 1/10 (and))
  )
)
(:action update-status-comp225-all-on
  :parameters ()
  :precondition (and
    (update-status comp225)
    (not (rebooted comp225))
    (all-on comp225)
  )
  :effect (and
    (not (update-status comp225))
    (update-status comp226)
    (not (all-on comp225))
    (probabilistic 1/20 (and (not (running comp225))))
  )
)
(:action update-status-comp225-one-off
  :parameters ()
  :precondition (and
    (update-status comp225)
    (not (rebooted comp225))
    (one-off comp225)
  )
  :effect (and
    (not (update-status comp225))
    (update-status comp226)
    (not (one-off comp225))
    (probabilistic 1/4 (and (not (running comp225))))
  )
)
(:action update-status-comp226-rebooted
  :parameters ()
  :precondition (and
    (update-status comp226)
    (rebooted comp226)
  )
  :effect (and
    (not (update-status comp226))
    (update-status comp227)
    (not (rebooted comp226))
    (probabilistic 9/10 (and (running comp226)) 1/10 (and))
  )
)
(:action update-status-comp226-all-on
  :parameters ()
  :precondition (and
    (update-status comp226)
    (not (rebooted comp226))
    (all-on comp226)
  )
  :effect (and
    (not (update-status comp226))
    (update-status comp227)
    (not (all-on comp226))
    (probabilistic 1/20 (and (not (running comp226))))
  )
)
(:action update-status-comp226-one-off
  :parameters ()
  :precondition (and
    (update-status comp226)
    (not (rebooted comp226))
    (one-off comp226)
  )
  :effect (and
    (not (update-status comp226))
    (update-status comp227)
    (not (one-off comp226))
    (probabilistic 1/4 (and (not (running comp226))))
  )
)
(:action update-status-comp227-rebooted
  :parameters ()
  :precondition (and
    (update-status comp227)
    (rebooted comp227)
  )
  :effect (and
    (not (update-status comp227))
    (update-status comp228)
    (not (rebooted comp227))
    (probabilistic 9/10 (and (running comp227)) 1/10 (and))
  )
)
(:action update-status-comp227-all-on
  :parameters ()
  :precondition (and
    (update-status comp227)
    (not (rebooted comp227))
    (all-on comp227)
  )
  :effect (and
    (not (update-status comp227))
    (update-status comp228)
    (not (all-on comp227))
    (probabilistic 1/20 (and (not (running comp227))))
  )
)
(:action update-status-comp227-one-off
  :parameters ()
  :precondition (and
    (update-status comp227)
    (not (rebooted comp227))
    (one-off comp227)
  )
  :effect (and
    (not (update-status comp227))
    (update-status comp228)
    (not (one-off comp227))
    (probabilistic 1/4 (and (not (running comp227))))
  )
)
(:action update-status-comp228-rebooted
  :parameters ()
  :precondition (and
    (update-status comp228)
    (rebooted comp228)
  )
  :effect (and
    (not (update-status comp228))
    (update-status comp229)
    (not (rebooted comp228))
    (probabilistic 9/10 (and (running comp228)) 1/10 (and))
  )
)
(:action update-status-comp228-all-on
  :parameters ()
  :precondition (and
    (update-status comp228)
    (not (rebooted comp228))
    (all-on comp228)
  )
  :effect (and
    (not (update-status comp228))
    (update-status comp229)
    (not (all-on comp228))
    (probabilistic 1/20 (and (not (running comp228))))
  )
)
(:action update-status-comp228-one-off
  :parameters ()
  :precondition (and
    (update-status comp228)
    (not (rebooted comp228))
    (one-off comp228)
  )
  :effect (and
    (not (update-status comp228))
    (update-status comp229)
    (not (one-off comp228))
    (probabilistic 1/4 (and (not (running comp228))))
  )
)
(:action update-status-comp229-rebooted
  :parameters ()
  :precondition (and
    (update-status comp229)
    (rebooted comp229)
  )
  :effect (and
    (not (update-status comp229))
    (update-status comp230)
    (not (rebooted comp229))
    (probabilistic 9/10 (and (running comp229)) 1/10 (and))
  )
)
(:action update-status-comp229-all-on
  :parameters ()
  :precondition (and
    (update-status comp229)
    (not (rebooted comp229))
    (all-on comp229)
  )
  :effect (and
    (not (update-status comp229))
    (update-status comp230)
    (not (all-on comp229))
    (probabilistic 1/20 (and (not (running comp229))))
  )
)
(:action update-status-comp229-one-off
  :parameters ()
  :precondition (and
    (update-status comp229)
    (not (rebooted comp229))
    (one-off comp229)
  )
  :effect (and
    (not (update-status comp229))
    (update-status comp230)
    (not (one-off comp229))
    (probabilistic 1/4 (and (not (running comp229))))
  )
)
(:action update-status-comp230-rebooted
  :parameters ()
  :precondition (and
    (update-status comp230)
    (rebooted comp230)
  )
  :effect (and
    (not (update-status comp230))
    (update-status comp231)
    (not (rebooted comp230))
    (probabilistic 9/10 (and (running comp230)) 1/10 (and))
  )
)
(:action update-status-comp230-all-on
  :parameters ()
  :precondition (and
    (update-status comp230)
    (not (rebooted comp230))
    (all-on comp230)
  )
  :effect (and
    (not (update-status comp230))
    (update-status comp231)
    (not (all-on comp230))
    (probabilistic 1/20 (and (not (running comp230))))
  )
)
(:action update-status-comp230-one-off
  :parameters ()
  :precondition (and
    (update-status comp230)
    (not (rebooted comp230))
    (one-off comp230)
  )
  :effect (and
    (not (update-status comp230))
    (update-status comp231)
    (not (one-off comp230))
    (probabilistic 1/4 (and (not (running comp230))))
  )
)
(:action update-status-comp231-rebooted
  :parameters ()
  :precondition (and
    (update-status comp231)
    (rebooted comp231)
  )
  :effect (and
    (not (update-status comp231))
    (update-status comp232)
    (not (rebooted comp231))
    (probabilistic 9/10 (and (running comp231)) 1/10 (and))
  )
)
(:action update-status-comp231-all-on
  :parameters ()
  :precondition (and
    (update-status comp231)
    (not (rebooted comp231))
    (all-on comp231)
  )
  :effect (and
    (not (update-status comp231))
    (update-status comp232)
    (not (all-on comp231))
    (probabilistic 1/20 (and (not (running comp231))))
  )
)
(:action update-status-comp231-one-off
  :parameters ()
  :precondition (and
    (update-status comp231)
    (not (rebooted comp231))
    (one-off comp231)
  )
  :effect (and
    (not (update-status comp231))
    (update-status comp232)
    (not (one-off comp231))
    (probabilistic 1/4 (and (not (running comp231))))
  )
)
(:action update-status-comp232-rebooted
  :parameters ()
  :precondition (and
    (update-status comp232)
    (rebooted comp232)
  )
  :effect (and
    (not (update-status comp232))
    (update-status comp233)
    (not (rebooted comp232))
    (probabilistic 9/10 (and (running comp232)) 1/10 (and))
  )
)
(:action update-status-comp232-all-on
  :parameters ()
  :precondition (and
    (update-status comp232)
    (not (rebooted comp232))
    (all-on comp232)
  )
  :effect (and
    (not (update-status comp232))
    (update-status comp233)
    (not (all-on comp232))
    (probabilistic 1/20 (and (not (running comp232))))
  )
)
(:action update-status-comp232-one-off
  :parameters ()
  :precondition (and
    (update-status comp232)
    (not (rebooted comp232))
    (one-off comp232)
  )
  :effect (and
    (not (update-status comp232))
    (update-status comp233)
    (not (one-off comp232))
    (probabilistic 1/4 (and (not (running comp232))))
  )
)
(:action update-status-comp233-rebooted
  :parameters ()
  :precondition (and
    (update-status comp233)
    (rebooted comp233)
  )
  :effect (and
    (not (update-status comp233))
    (update-status comp234)
    (not (rebooted comp233))
    (probabilistic 9/10 (and (running comp233)) 1/10 (and))
  )
)
(:action update-status-comp233-all-on
  :parameters ()
  :precondition (and
    (update-status comp233)
    (not (rebooted comp233))
    (all-on comp233)
  )
  :effect (and
    (not (update-status comp233))
    (update-status comp234)
    (not (all-on comp233))
    (probabilistic 1/20 (and (not (running comp233))))
  )
)
(:action update-status-comp233-one-off
  :parameters ()
  :precondition (and
    (update-status comp233)
    (not (rebooted comp233))
    (one-off comp233)
  )
  :effect (and
    (not (update-status comp233))
    (update-status comp234)
    (not (one-off comp233))
    (probabilistic 1/4 (and (not (running comp233))))
  )
)
(:action update-status-comp234-rebooted
  :parameters ()
  :precondition (and
    (update-status comp234)
    (rebooted comp234)
  )
  :effect (and
    (not (update-status comp234))
    (update-status comp235)
    (not (rebooted comp234))
    (probabilistic 9/10 (and (running comp234)) 1/10 (and))
  )
)
(:action update-status-comp234-all-on
  :parameters ()
  :precondition (and
    (update-status comp234)
    (not (rebooted comp234))
    (all-on comp234)
  )
  :effect (and
    (not (update-status comp234))
    (update-status comp235)
    (not (all-on comp234))
    (probabilistic 1/20 (and (not (running comp234))))
  )
)
(:action update-status-comp234-one-off
  :parameters ()
  :precondition (and
    (update-status comp234)
    (not (rebooted comp234))
    (one-off comp234)
  )
  :effect (and
    (not (update-status comp234))
    (update-status comp235)
    (not (one-off comp234))
    (probabilistic 1/4 (and (not (running comp234))))
  )
)
(:action update-status-comp235-rebooted
  :parameters ()
  :precondition (and
    (update-status comp235)
    (rebooted comp235)
  )
  :effect (and
    (not (update-status comp235))
    (update-status comp236)
    (not (rebooted comp235))
    (probabilistic 9/10 (and (running comp235)) 1/10 (and))
  )
)
(:action update-status-comp235-all-on
  :parameters ()
  :precondition (and
    (update-status comp235)
    (not (rebooted comp235))
    (all-on comp235)
  )
  :effect (and
    (not (update-status comp235))
    (update-status comp236)
    (not (all-on comp235))
    (probabilistic 1/20 (and (not (running comp235))))
  )
)
(:action update-status-comp235-one-off
  :parameters ()
  :precondition (and
    (update-status comp235)
    (not (rebooted comp235))
    (one-off comp235)
  )
  :effect (and
    (not (update-status comp235))
    (update-status comp236)
    (not (one-off comp235))
    (probabilistic 1/4 (and (not (running comp235))))
  )
)
(:action update-status-comp236-rebooted
  :parameters ()
  :precondition (and
    (update-status comp236)
    (rebooted comp236)
  )
  :effect (and
    (not (update-status comp236))
    (update-status comp237)
    (not (rebooted comp236))
    (probabilistic 9/10 (and (running comp236)) 1/10 (and))
  )
)
(:action update-status-comp236-all-on
  :parameters ()
  :precondition (and
    (update-status comp236)
    (not (rebooted comp236))
    (all-on comp236)
  )
  :effect (and
    (not (update-status comp236))
    (update-status comp237)
    (not (all-on comp236))
    (probabilistic 1/20 (and (not (running comp236))))
  )
)
(:action update-status-comp236-one-off
  :parameters ()
  :precondition (and
    (update-status comp236)
    (not (rebooted comp236))
    (one-off comp236)
  )
  :effect (and
    (not (update-status comp236))
    (update-status comp237)
    (not (one-off comp236))
    (probabilistic 1/4 (and (not (running comp236))))
  )
)
(:action update-status-comp237-rebooted
  :parameters ()
  :precondition (and
    (update-status comp237)
    (rebooted comp237)
  )
  :effect (and
    (not (update-status comp237))
    (update-status comp238)
    (not (rebooted comp237))
    (probabilistic 9/10 (and (running comp237)) 1/10 (and))
  )
)
(:action update-status-comp237-all-on
  :parameters ()
  :precondition (and
    (update-status comp237)
    (not (rebooted comp237))
    (all-on comp237)
  )
  :effect (and
    (not (update-status comp237))
    (update-status comp238)
    (not (all-on comp237))
    (probabilistic 1/20 (and (not (running comp237))))
  )
)
(:action update-status-comp237-one-off
  :parameters ()
  :precondition (and
    (update-status comp237)
    (not (rebooted comp237))
    (one-off comp237)
  )
  :effect (and
    (not (update-status comp237))
    (update-status comp238)
    (not (one-off comp237))
    (probabilistic 1/4 (and (not (running comp237))))
  )
)
(:action update-status-comp238-rebooted
  :parameters ()
  :precondition (and
    (update-status comp238)
    (rebooted comp238)
  )
  :effect (and
    (not (update-status comp238))
    (update-status comp239)
    (not (rebooted comp238))
    (probabilistic 9/10 (and (running comp238)) 1/10 (and))
  )
)
(:action update-status-comp238-all-on
  :parameters ()
  :precondition (and
    (update-status comp238)
    (not (rebooted comp238))
    (all-on comp238)
  )
  :effect (and
    (not (update-status comp238))
    (update-status comp239)
    (not (all-on comp238))
    (probabilistic 1/20 (and (not (running comp238))))
  )
)
(:action update-status-comp238-one-off
  :parameters ()
  :precondition (and
    (update-status comp238)
    (not (rebooted comp238))
    (one-off comp238)
  )
  :effect (and
    (not (update-status comp238))
    (update-status comp239)
    (not (one-off comp238))
    (probabilistic 1/4 (and (not (running comp238))))
  )
)
(:action update-status-comp239-rebooted
  :parameters ()
  :precondition (and
    (update-status comp239)
    (rebooted comp239)
  )
  :effect (and
    (not (update-status comp239))
    (all-updated)
    (not (rebooted comp239))
    (probabilistic 9/10 (and (running comp239)) 1/10 (and))
  )
)
(:action update-status-comp239-all-on
  :parameters ()
  :precondition (and
    (update-status comp239)
    (not (rebooted comp239))
    (all-on comp239)
  )
  :effect (and
    (not (update-status comp239))
    (all-updated)
    (not (all-on comp239))
    (probabilistic 1/20 (and (not (running comp239))))
  )
)
(:action update-status-comp239-one-off
  :parameters ()
  :precondition (and
    (update-status comp239)
    (not (rebooted comp239))
    (one-off comp239)
  )
  :effect (and
    (not (update-status comp239))
    (all-updated)
    (not (one-off comp239))
    (probabilistic 1/4 (and (not (running comp239))))
  )
)
)