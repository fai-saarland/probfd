(define (domain sysadmin-nocount)
(:requirements :probabilistic-effects :negative-preconditions :typing)
(:types computer - object)
(:constants
  comp0 comp1 comp2 comp3 comp4 comp5 comp6 comp7 comp8 comp9 comp10 comp11 comp12 comp13 comp14 comp15 comp16 comp17 comp18 comp19 comp20 comp21 comp22 comp23 comp24 comp25 comp26 comp27 comp28 comp29 comp30 comp31 comp32 comp33 comp34 comp35 comp36 comp37 comp38 comp39 comp40 comp41 comp42 comp43 comp44 comp45 comp46 comp47 comp48 comp49 comp50 comp51 comp52 comp53 comp54 comp55 comp56 comp57 comp58 comp59 comp60 comp61 comp62 comp63 comp64 comp65 comp66 comp67 comp68 comp69 comp70 comp71 comp72 comp73 comp74 comp75 comp76 comp77 comp78 comp79 comp80 comp81 comp82 comp83 comp84 comp85 comp86 comp87 comp88 comp89 comp90 comp91 comp92 comp93 comp94 comp95 comp96 comp97 comp98 comp99 comp100 comp101 comp102 comp103 comp104 comp105 comp106 comp107 comp108 comp109 comp110 comp111 comp112 comp113 comp114 comp115 comp116 comp117 comp118 comp119 comp120 comp121 comp122 comp123 comp124 comp125 comp126 comp127 comp128 comp129 comp130 comp131 comp132 comp133 comp134 comp135 comp136 comp137 comp138 comp139 comp140 comp141 comp142 comp143 comp144 comp145 comp146 comp147 comp148 comp149 comp150 comp151 comp152 comp153 comp154 comp155 comp156 comp157 comp158 comp159 comp160 comp161 comp162 comp163 comp164 comp165 comp166 comp167 comp168 comp169 comp170 comp171 comp172 comp173 comp174 comp175 comp176 comp177 comp178 comp179 comp180 comp181 comp182 comp183 comp184 comp185 comp186 comp187 comp188 comp189 comp190 comp191 comp192 comp193 comp194 comp195 comp196 comp197 comp198 comp199 comp200 comp201 comp202 comp203 comp204 comp205 comp206 comp207 comp208 comp209 comp210 comp211 comp212 comp213 comp214 comp215 comp216 comp217 comp218 comp219 comp220 comp221 comp222 comp223 comp224 comp225 comp226 comp227 comp228 comp229 comp230 comp231 comp232 comp233 comp234 comp235 comp236 comp237 comp238 comp239 comp240 comp241 comp242 comp243 comp244 comp245 comp246 comp247 comp248 comp249 comp250 comp251 comp252 comp253 comp254 comp255 comp256 comp257 comp258 comp259 comp260 comp261 comp262 comp263 comp264 comp265 comp266 comp267 comp268 comp269 comp270 comp271 comp272 comp273 comp274 comp275 comp276 comp277 comp278 comp279 comp280 comp281 comp282 comp283 comp284 comp285 comp286 comp287 comp288 comp289 comp290 comp291 comp292 comp293 comp294 comp295 comp296 comp297 comp298 comp299 comp300 comp301 comp302 comp303 comp304 comp305 comp306 comp307 comp308 comp309 comp310 comp311 comp312 comp313 comp314 comp315 comp316 comp317 comp318 comp319 comp320 comp321 comp322 comp323 comp324 comp325 comp326 comp327 comp328 comp329 comp330 comp331 comp332 comp333 comp334 comp335 comp336 comp337 comp338 comp339 comp340 comp341 comp342 comp343 comp344 comp345 comp346 comp347 comp348 comp349 comp350 comp351 comp352 comp353 comp354 comp355 comp356 comp357 comp358 comp359 comp360 comp361 comp362 comp363 comp364 comp365 comp366 comp367 comp368 comp369 comp370 comp371 comp372 comp373 comp374 comp375 comp376 comp377 comp378 comp379 comp380 comp381 comp382 comp383 comp384 comp385 comp386 comp387 comp388 comp389 comp390 comp391 comp392 comp393 comp394 comp395 comp396 comp397 comp398 comp399 comp400 comp401 comp402 comp403 comp404 comp405 comp406 comp407 comp408 comp409 comp410 comp411 comp412 comp413 comp414 comp415 comp416 comp417 comp418 comp419 comp420 comp421 comp422 comp423 comp424 comp425 comp426 comp427 comp428 comp429 comp430 comp431 comp432 comp433 comp434 comp435 comp436 comp437 comp438 comp439 comp440 comp441 comp442 comp443 comp444 comp445 comp446 comp447 comp448 comp449 comp450 comp451 comp452 comp453 comp454 comp455 comp456 comp457 comp458 comp459 comp460 comp461 comp462 comp463 comp464 comp465 comp466 comp467 comp468 comp469 comp470 comp471 comp472 comp473 comp474 comp475 comp476 comp477 comp478 comp479 - computer
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
    (running comp479)
  )
  :effect (and
    (not (evaluate comp0))
    (evaluate comp1)
    (all-on comp0)
  )
)
(:action evaluate-comp0-off-comp479
  :parameters ()
  :precondition (and
    (evaluate comp0)
    (not (rebooted comp0))
    (not (running comp479))
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
    (running comp132)
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
(:action evaluate-comp2-off-comp132
  :parameters ()
  :precondition (and
    (evaluate comp2)
    (not (rebooted comp2))
    (not (running comp132))
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
    (running comp469)
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
(:action evaluate-comp4-off-comp469
  :parameters ()
  :precondition (and
    (evaluate comp4)
    (not (rebooted comp4))
    (not (running comp469))
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
    (running comp302)
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
(:action evaluate-comp5-off-comp302
  :parameters ()
  :precondition (and
    (evaluate comp5)
    (not (rebooted comp5))
    (not (running comp302))
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
    (running comp379)
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
(:action evaluate-comp6-off-comp379
  :parameters ()
  :precondition (and
    (evaluate comp6)
    (not (rebooted comp6))
    (not (running comp379))
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
    (running comp250)
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
(:action evaluate-comp8-off-comp250
  :parameters ()
  :precondition (and
    (evaluate comp8)
    (not (rebooted comp8))
    (not (running comp250))
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
    (running comp467)
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
(:action evaluate-comp9-off-comp467
  :parameters ()
  :precondition (and
    (evaluate comp9)
    (not (rebooted comp9))
    (not (running comp467))
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
    (running comp272)
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
(:action evaluate-comp19-off-comp272
  :parameters ()
  :precondition (and
    (evaluate comp19)
    (not (rebooted comp19))
    (not (running comp272))
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
    (running comp321)
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
(:action evaluate-comp22-off-comp321
  :parameters ()
  :precondition (and
    (evaluate comp22)
    (not (rebooted comp22))
    (not (running comp321))
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
    (running comp230)
    (running comp409)
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
(:action evaluate-comp23-off-comp230
  :parameters ()
  :precondition (and
    (evaluate comp23)
    (not (rebooted comp23))
    (not (running comp230))
  )
  :effect (and
    (not (evaluate comp23))
    (evaluate comp24)
    (one-off comp23)
  )
)
(:action evaluate-comp23-off-comp409
  :parameters ()
  :precondition (and
    (evaluate comp23)
    (not (rebooted comp23))
    (not (running comp409))
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
    (running comp401)
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
(:action evaluate-comp26-off-comp401
  :parameters ()
  :precondition (and
    (evaluate comp26)
    (not (rebooted comp26))
    (not (running comp401))
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
    (running comp455)
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
(:action evaluate-comp27-off-comp455
  :parameters ()
  :precondition (and
    (evaluate comp27)
    (not (rebooted comp27))
    (not (running comp455))
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
    (running comp159)
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
(:action evaluate-comp28-off-comp159
  :parameters ()
  :precondition (and
    (evaluate comp28)
    (not (rebooted comp28))
    (not (running comp159))
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
    (running comp146)
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
(:action evaluate-comp29-off-comp146
  :parameters ()
  :precondition (and
    (evaluate comp29)
    (not (rebooted comp29))
    (not (running comp146))
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
    (running comp238)
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
(:action evaluate-comp30-off-comp238
  :parameters ()
  :precondition (and
    (evaluate comp30)
    (not (rebooted comp30))
    (not (running comp238))
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
    (running comp88)
    (running comp305)
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
(:action evaluate-comp35-off-comp88
  :parameters ()
  :precondition (and
    (evaluate comp35)
    (not (rebooted comp35))
    (not (running comp88))
  )
  :effect (and
    (not (evaluate comp35))
    (evaluate comp36)
    (one-off comp35)
  )
)
(:action evaluate-comp35-off-comp305
  :parameters ()
  :precondition (and
    (evaluate comp35)
    (not (rebooted comp35))
    (not (running comp305))
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
    (running comp156)
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
(:action evaluate-comp36-off-comp156
  :parameters ()
  :precondition (and
    (evaluate comp36)
    (not (rebooted comp36))
    (not (running comp156))
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
    (running comp26)
    (running comp36)
  )
  :effect (and
    (not (evaluate comp37))
    (evaluate comp38)
    (all-on comp37)
  )
)
(:action evaluate-comp37-off-comp26
  :parameters ()
  :precondition (and
    (evaluate comp37)
    (not (rebooted comp37))
    (not (running comp26))
  )
  :effect (and
    (not (evaluate comp37))
    (evaluate comp38)
    (one-off comp37)
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
    (running comp141)
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
(:action evaluate-comp40-off-comp141
  :parameters ()
  :precondition (and
    (evaluate comp40)
    (not (rebooted comp40))
    (not (running comp141))
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
    (running comp138)
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
(:action evaluate-comp42-off-comp138
  :parameters ()
  :precondition (and
    (evaluate comp42)
    (not (rebooted comp42))
    (not (running comp138))
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
    (running comp348)
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
(:action evaluate-comp45-off-comp348
  :parameters ()
  :precondition (and
    (evaluate comp45)
    (not (rebooted comp45))
    (not (running comp348))
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
    (running comp13)
    (running comp50)
  )
  :effect (and
    (not (evaluate comp51))
    (evaluate comp52)
    (all-on comp51)
  )
)
(:action evaluate-comp51-off-comp13
  :parameters ()
  :precondition (and
    (evaluate comp51)
    (not (rebooted comp51))
    (not (running comp13))
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
    (running comp131)
    (running comp144)
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
(:action evaluate-comp52-off-comp131
  :parameters ()
  :precondition (and
    (evaluate comp52)
    (not (rebooted comp52))
    (not (running comp131))
  )
  :effect (and
    (not (evaluate comp52))
    (evaluate comp53)
    (one-off comp52)
  )
)
(:action evaluate-comp52-off-comp144
  :parameters ()
  :precondition (and
    (evaluate comp52)
    (not (rebooted comp52))
    (not (running comp144))
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
    (running comp206)
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
(:action evaluate-comp53-off-comp206
  :parameters ()
  :precondition (and
    (evaluate comp53)
    (not (rebooted comp53))
    (not (running comp206))
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
    (running comp55)
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
(:action evaluate-comp54-off-comp55
  :parameters ()
  :precondition (and
    (evaluate comp54)
    (not (rebooted comp54))
    (not (running comp55))
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
    (running comp229)
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
(:action evaluate-comp57-off-comp229
  :parameters ()
  :precondition (and
    (evaluate comp57)
    (not (rebooted comp57))
    (not (running comp229))
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
    (running comp42)
    (running comp43)
    (running comp58)
  )
  :effect (and
    (not (evaluate comp59))
    (evaluate comp60)
    (all-on comp59)
  )
)
(:action evaluate-comp59-off-comp42
  :parameters ()
  :precondition (and
    (evaluate comp59)
    (not (rebooted comp59))
    (not (running comp42))
  )
  :effect (and
    (not (evaluate comp59))
    (evaluate comp60)
    (one-off comp59)
  )
)
(:action evaluate-comp59-off-comp43
  :parameters ()
  :precondition (and
    (evaluate comp59)
    (not (rebooted comp59))
    (not (running comp43))
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
    (running comp62)
    (running comp262)
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
(:action evaluate-comp63-off-comp262
  :parameters ()
  :precondition (and
    (evaluate comp63)
    (not (rebooted comp63))
    (not (running comp262))
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
    (running comp386)
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
(:action evaluate-comp64-off-comp386
  :parameters ()
  :precondition (and
    (evaluate comp64)
    (not (rebooted comp64))
    (not (running comp386))
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
    (running comp369)
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
(:action evaluate-comp66-off-comp369
  :parameters ()
  :precondition (and
    (evaluate comp66)
    (not (rebooted comp66))
    (not (running comp369))
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
    (running comp243)
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
(:action evaluate-comp67-off-comp243
  :parameters ()
  :precondition (and
    (evaluate comp67)
    (not (rebooted comp67))
    (not (running comp243))
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
    (running comp77)
    (running comp415)
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
(:action evaluate-comp70-off-comp77
  :parameters ()
  :precondition (and
    (evaluate comp70)
    (not (rebooted comp70))
    (not (running comp77))
  )
  :effect (and
    (not (evaluate comp70))
    (evaluate comp71)
    (one-off comp70)
  )
)
(:action evaluate-comp70-off-comp415
  :parameters ()
  :precondition (and
    (evaluate comp70)
    (not (rebooted comp70))
    (not (running comp415))
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
    (running comp172)
    (running comp439)
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
(:action evaluate-comp71-off-comp172
  :parameters ()
  :precondition (and
    (evaluate comp71)
    (not (rebooted comp71))
    (not (running comp172))
  )
  :effect (and
    (not (evaluate comp71))
    (evaluate comp72)
    (one-off comp71)
  )
)
(:action evaluate-comp71-off-comp439
  :parameters ()
  :precondition (and
    (evaluate comp71)
    (not (rebooted comp71))
    (not (running comp439))
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
    (running comp240)
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
(:action evaluate-comp72-off-comp240
  :parameters ()
  :precondition (and
    (evaluate comp72)
    (not (rebooted comp72))
    (not (running comp240))
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
    (running comp431)
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
(:action evaluate-comp76-off-comp431
  :parameters ()
  :precondition (and
    (evaluate comp76)
    (not (rebooted comp76))
    (not (running comp431))
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
    (running comp77)
    (running comp171)
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
(:action evaluate-comp78-off-comp171
  :parameters ()
  :precondition (and
    (evaluate comp78)
    (not (rebooted comp78))
    (not (running comp171))
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
    (running comp203)
    (running comp219)
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
(:action evaluate-comp80-off-comp203
  :parameters ()
  :precondition (and
    (evaluate comp80)
    (not (rebooted comp80))
    (not (running comp203))
  )
  :effect (and
    (not (evaluate comp80))
    (evaluate comp81)
    (one-off comp80)
  )
)
(:action evaluate-comp80-off-comp219
  :parameters ()
  :precondition (and
    (evaluate comp80)
    (not (rebooted comp80))
    (not (running comp219))
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
    (running comp285)
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
(:action evaluate-comp85-off-comp285
  :parameters ()
  :precondition (and
    (evaluate comp85)
    (not (rebooted comp85))
    (not (running comp285))
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
    (running comp421)
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
(:action evaluate-comp87-off-comp421
  :parameters ()
  :precondition (and
    (evaluate comp87)
    (not (rebooted comp87))
    (not (running comp421))
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
    (running comp368)
    (running comp453)
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
(:action evaluate-comp90-off-comp368
  :parameters ()
  :precondition (and
    (evaluate comp90)
    (not (rebooted comp90))
    (not (running comp368))
  )
  :effect (and
    (not (evaluate comp90))
    (evaluate comp91)
    (one-off comp90)
  )
)
(:action evaluate-comp90-off-comp453
  :parameters ()
  :precondition (and
    (evaluate comp90)
    (not (rebooted comp90))
    (not (running comp453))
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
    (running comp291)
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
(:action evaluate-comp92-off-comp291
  :parameters ()
  :precondition (and
    (evaluate comp92)
    (not (rebooted comp92))
    (not (running comp291))
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
    (running comp437)
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
(:action evaluate-comp93-off-comp437
  :parameters ()
  :precondition (and
    (evaluate comp93)
    (not (rebooted comp93))
    (not (running comp437))
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
    (running comp319)
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
(:action evaluate-comp96-off-comp319
  :parameters ()
  :precondition (and
    (evaluate comp96)
    (not (rebooted comp96))
    (not (running comp319))
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
    (running comp188)
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
(:action evaluate-comp97-off-comp188
  :parameters ()
  :precondition (and
    (evaluate comp97)
    (not (rebooted comp97))
    (not (running comp188))
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
    (running comp90)
    (running comp97)
  )
  :effect (and
    (not (evaluate comp98))
    (evaluate comp99)
    (all-on comp98)
  )
)
(:action evaluate-comp98-off-comp90
  :parameters ()
  :precondition (and
    (evaluate comp98)
    (not (rebooted comp98))
    (not (running comp90))
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
    (running comp345)
    (running comp456)
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
(:action evaluate-comp101-off-comp345
  :parameters ()
  :precondition (and
    (evaluate comp101)
    (not (rebooted comp101))
    (not (running comp345))
  )
  :effect (and
    (not (evaluate comp101))
    (evaluate comp102)
    (one-off comp101)
  )
)
(:action evaluate-comp101-off-comp456
  :parameters ()
  :precondition (and
    (evaluate comp101)
    (not (rebooted comp101))
    (not (running comp456))
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
    (running comp101)
  )
  :effect (and
    (not (evaluate comp102))
    (evaluate comp103)
    (all-on comp102)
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
    (running comp161)
    (running comp372)
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
(:action evaluate-comp103-off-comp161
  :parameters ()
  :precondition (and
    (evaluate comp103)
    (not (rebooted comp103))
    (not (running comp161))
  )
  :effect (and
    (not (evaluate comp103))
    (evaluate comp104)
    (one-off comp103)
  )
)
(:action evaluate-comp103-off-comp372
  :parameters ()
  :precondition (and
    (evaluate comp103)
    (not (rebooted comp103))
    (not (running comp372))
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
    (running comp138)
    (running comp374)
    (running comp411)
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
(:action evaluate-comp107-off-comp138
  :parameters ()
  :precondition (and
    (evaluate comp107)
    (not (rebooted comp107))
    (not (running comp138))
  )
  :effect (and
    (not (evaluate comp107))
    (evaluate comp108)
    (one-off comp107)
  )
)
(:action evaluate-comp107-off-comp374
  :parameters ()
  :precondition (and
    (evaluate comp107)
    (not (rebooted comp107))
    (not (running comp374))
  )
  :effect (and
    (not (evaluate comp107))
    (evaluate comp108)
    (one-off comp107)
  )
)
(:action evaluate-comp107-off-comp411
  :parameters ()
  :precondition (and
    (evaluate comp107)
    (not (rebooted comp107))
    (not (running comp411))
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
    (running comp330)
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
(:action evaluate-comp109-off-comp330
  :parameters ()
  :precondition (and
    (evaluate comp109)
    (not (rebooted comp109))
    (not (running comp330))
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
    (running comp111)
    (running comp384)
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
(:action evaluate-comp112-off-comp384
  :parameters ()
  :precondition (and
    (evaluate comp112)
    (not (rebooted comp112))
    (not (running comp384))
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
    (running comp80)
    (running comp114)
    (running comp141)
  )
  :effect (and
    (not (evaluate comp115))
    (evaluate comp116)
    (all-on comp115)
  )
)
(:action evaluate-comp115-off-comp80
  :parameters ()
  :precondition (and
    (evaluate comp115)
    (not (rebooted comp115))
    (not (running comp80))
  )
  :effect (and
    (not (evaluate comp115))
    (evaluate comp116)
    (one-off comp115)
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
(:action evaluate-comp115-off-comp141
  :parameters ()
  :precondition (and
    (evaluate comp115)
    (not (rebooted comp115))
    (not (running comp141))
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
    (running comp3)
    (running comp121)
  )
  :effect (and
    (not (evaluate comp122))
    (evaluate comp123)
    (all-on comp122)
  )
)
(:action evaluate-comp122-off-comp3
  :parameters ()
  :precondition (and
    (evaluate comp122)
    (not (rebooted comp122))
    (not (running comp3))
  )
  :effect (and
    (not (evaluate comp122))
    (evaluate comp123)
    (one-off comp122)
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
    (running comp122)
  )
  :effect (and
    (not (evaluate comp123))
    (evaluate comp124)
    (all-on comp123)
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
    (running comp270)
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
(:action evaluate-comp126-off-comp270
  :parameters ()
  :precondition (and
    (evaluate comp126)
    (not (rebooted comp126))
    (not (running comp270))
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
    (running comp126)
    (running comp137)
  )
  :effect (and
    (not (evaluate comp127))
    (evaluate comp128)
    (all-on comp127)
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
(:action evaluate-comp127-off-comp137
  :parameters ()
  :precondition (and
    (evaluate comp127)
    (not (rebooted comp127))
    (not (running comp137))
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
    (running comp16)
    (running comp128)
    (running comp298)
  )
  :effect (and
    (not (evaluate comp129))
    (evaluate comp130)
    (all-on comp129)
  )
)
(:action evaluate-comp129-off-comp16
  :parameters ()
  :precondition (and
    (evaluate comp129)
    (not (rebooted comp129))
    (not (running comp16))
  )
  :effect (and
    (not (evaluate comp129))
    (evaluate comp130)
    (one-off comp129)
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
(:action evaluate-comp129-off-comp298
  :parameters ()
  :precondition (and
    (evaluate comp129)
    (not (rebooted comp129))
    (not (running comp298))
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
    (running comp129)
  )
  :effect (and
    (not (evaluate comp130))
    (evaluate comp131)
    (all-on comp130)
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
    (running comp130)
  )
  :effect (and
    (not (evaluate comp131))
    (evaluate comp132)
    (all-on comp131)
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
    (running comp163)
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
(:action evaluate-comp135-off-comp163
  :parameters ()
  :precondition (and
    (evaluate comp135)
    (not (rebooted comp135))
    (not (running comp163))
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
    (running comp189)
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
(:action evaluate-comp136-off-comp189
  :parameters ()
  :precondition (and
    (evaluate comp136)
    (not (rebooted comp136))
    (not (running comp189))
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
    (running comp213)
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
(:action evaluate-comp139-off-comp213
  :parameters ()
  :precondition (and
    (evaluate comp139)
    (not (rebooted comp139))
    (not (running comp213))
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
    (running comp139)
  )
  :effect (and
    (not (evaluate comp140))
    (evaluate comp141)
    (all-on comp140)
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
    (running comp108)
    (running comp140)
    (running comp350)
  )
  :effect (and
    (not (evaluate comp141))
    (evaluate comp142)
    (all-on comp141)
  )
)
(:action evaluate-comp141-off-comp108
  :parameters ()
  :precondition (and
    (evaluate comp141)
    (not (rebooted comp141))
    (not (running comp108))
  )
  :effect (and
    (not (evaluate comp141))
    (evaluate comp142)
    (one-off comp141)
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
(:action evaluate-comp141-off-comp350
  :parameters ()
  :precondition (and
    (evaluate comp141)
    (not (rebooted comp141))
    (not (running comp350))
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
    (running comp11)
    (running comp144)
    (running comp164)
  )
  :effect (and
    (not (evaluate comp145))
    (evaluate comp146)
    (all-on comp145)
  )
)
(:action evaluate-comp145-off-comp11
  :parameters ()
  :precondition (and
    (evaluate comp145)
    (not (rebooted comp145))
    (not (running comp11))
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
(:action evaluate-comp145-off-comp164
  :parameters ()
  :precondition (and
    (evaluate comp145)
    (not (rebooted comp145))
    (not (running comp164))
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
    (running comp145)
    (running comp165)
  )
  :effect (and
    (not (evaluate comp146))
    (evaluate comp147)
    (all-on comp146)
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
(:action evaluate-comp146-off-comp165
  :parameters ()
  :precondition (and
    (evaluate comp146)
    (not (rebooted comp146))
    (not (running comp165))
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
    (running comp147)
  )
  :effect (and
    (not (evaluate comp148))
    (evaluate comp149)
    (all-on comp148)
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
    (running comp149)
  )
  :effect (and
    (not (evaluate comp150))
    (evaluate comp151)
    (all-on comp150)
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
    (running comp150)
  )
  :effect (and
    (not (evaluate comp151))
    (evaluate comp152)
    (all-on comp151)
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
    (running comp289)
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
(:action evaluate-comp155-off-comp289
  :parameters ()
  :precondition (and
    (evaluate comp155)
    (not (rebooted comp155))
    (not (running comp289))
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
    (running comp471)
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
(:action evaluate-comp156-off-comp471
  :parameters ()
  :precondition (and
    (evaluate comp156)
    (not (rebooted comp156))
    (not (running comp471))
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
    (running comp101)
    (running comp156)
  )
  :effect (and
    (not (evaluate comp157))
    (evaluate comp158)
    (all-on comp157)
  )
)
(:action evaluate-comp157-off-comp101
  :parameters ()
  :precondition (and
    (evaluate comp157)
    (not (rebooted comp157))
    (not (running comp101))
  )
  :effect (and
    (not (evaluate comp157))
    (evaluate comp158)
    (one-off comp157)
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
    (running comp239)
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
(:action evaluate-comp158-off-comp239
  :parameters ()
  :precondition (and
    (evaluate comp158)
    (not (rebooted comp158))
    (not (running comp239))
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
    (running comp86)
    (running comp158)
  )
  :effect (and
    (not (evaluate comp159))
    (evaluate comp160)
    (all-on comp159)
  )
)
(:action evaluate-comp159-off-comp86
  :parameters ()
  :precondition (and
    (evaluate comp159)
    (not (rebooted comp159))
    (not (running comp86))
  )
  :effect (and
    (not (evaluate comp159))
    (evaluate comp160)
    (one-off comp159)
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
    (running comp159)
    (running comp184)
    (running comp312)
  )
  :effect (and
    (not (evaluate comp160))
    (evaluate comp161)
    (all-on comp160)
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
(:action evaluate-comp160-off-comp184
  :parameters ()
  :precondition (and
    (evaluate comp160)
    (not (rebooted comp160))
    (not (running comp184))
  )
  :effect (and
    (not (evaluate comp160))
    (evaluate comp161)
    (one-off comp160)
  )
)
(:action evaluate-comp160-off-comp312
  :parameters ()
  :precondition (and
    (evaluate comp160)
    (not (rebooted comp160))
    (not (running comp312))
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
    (running comp444)
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
(:action evaluate-comp162-off-comp444
  :parameters ()
  :precondition (and
    (evaluate comp162)
    (not (rebooted comp162))
    (not (running comp444))
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
    (running comp163)
  )
  :effect (and
    (not (evaluate comp164))
    (evaluate comp165)
    (all-on comp164)
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
    (running comp167)
  )
  :effect (and
    (not (evaluate comp168))
    (evaluate comp169)
    (all-on comp168)
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
    (running comp170)
    (running comp278)
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
(:action evaluate-comp169-off-comp170
  :parameters ()
  :precondition (and
    (evaluate comp169)
    (not (rebooted comp169))
    (not (running comp170))
  )
  :effect (and
    (not (evaluate comp169))
    (evaluate comp170)
    (one-off comp169)
  )
)
(:action evaluate-comp169-off-comp278
  :parameters ()
  :precondition (and
    (evaluate comp169)
    (not (rebooted comp169))
    (not (running comp278))
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
    (running comp169)
  )
  :effect (and
    (not (evaluate comp170))
    (evaluate comp171)
    (all-on comp170)
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
    (running comp11)
    (running comp171)
  )
  :effect (and
    (not (evaluate comp172))
    (evaluate comp173)
    (all-on comp172)
  )
)
(:action evaluate-comp172-off-comp11
  :parameters ()
  :precondition (and
    (evaluate comp172)
    (not (rebooted comp172))
    (not (running comp11))
  )
  :effect (and
    (not (evaluate comp172))
    (evaluate comp173)
    (one-off comp172)
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
    (running comp31)
    (running comp173)
  )
  :effect (and
    (not (evaluate comp174))
    (evaluate comp175)
    (all-on comp174)
  )
)
(:action evaluate-comp174-off-comp31
  :parameters ()
  :precondition (and
    (evaluate comp174)
    (not (rebooted comp174))
    (not (running comp31))
  )
  :effect (and
    (not (evaluate comp174))
    (evaluate comp175)
    (one-off comp174)
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
    (running comp33)
    (running comp176)
  )
  :effect (and
    (not (evaluate comp177))
    (evaluate comp178)
    (all-on comp177)
  )
)
(:action evaluate-comp177-off-comp33
  :parameters ()
  :precondition (and
    (evaluate comp177)
    (not (rebooted comp177))
    (not (running comp33))
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
    (running comp281)
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
(:action evaluate-comp181-off-comp281
  :parameters ()
  :precondition (and
    (evaluate comp181)
    (not (rebooted comp181))
    (not (running comp281))
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
    (running comp182)
    (running comp406)
  )
  :effect (and
    (not (evaluate comp183))
    (evaluate comp184)
    (all-on comp183)
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
(:action evaluate-comp183-off-comp406
  :parameters ()
  :precondition (and
    (evaluate comp183)
    (not (rebooted comp183))
    (not (running comp406))
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
    (running comp229)
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
(:action evaluate-comp185-off-comp229
  :parameters ()
  :precondition (and
    (evaluate comp185)
    (not (rebooted comp185))
    (not (running comp229))
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
    (running comp427)
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
(:action evaluate-comp188-off-comp427
  :parameters ()
  :precondition (and
    (evaluate comp188)
    (not (rebooted comp188))
    (not (running comp427))
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
    (running comp124)
    (running comp189)
  )
  :effect (and
    (not (evaluate comp190))
    (evaluate comp191)
    (all-on comp190)
  )
)
(:action evaluate-comp190-off-comp124
  :parameters ()
  :precondition (and
    (evaluate comp190)
    (not (rebooted comp190))
    (not (running comp124))
  )
  :effect (and
    (not (evaluate comp190))
    (evaluate comp191)
    (one-off comp190)
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
    (running comp191)
  )
  :effect (and
    (not (evaluate comp192))
    (evaluate comp193)
    (all-on comp192)
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
    (running comp200)
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
(:action evaluate-comp194-off-comp200
  :parameters ()
  :precondition (and
    (evaluate comp194)
    (not (rebooted comp194))
    (not (running comp200))
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
    (running comp237)
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
(:action evaluate-comp195-off-comp237
  :parameters ()
  :precondition (and
    (evaluate comp195)
    (not (rebooted comp195))
    (not (running comp237))
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
    (running comp196)
  )
  :effect (and
    (not (evaluate comp197))
    (evaluate comp198)
    (all-on comp197)
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
    (running comp41)
    (running comp197)
  )
  :effect (and
    (not (evaluate comp198))
    (evaluate comp199)
    (all-on comp198)
  )
)
(:action evaluate-comp198-off-comp41
  :parameters ()
  :precondition (and
    (evaluate comp198)
    (not (rebooted comp198))
    (not (running comp41))
  )
  :effect (and
    (not (evaluate comp198))
    (evaluate comp199)
    (one-off comp198)
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
    (running comp196)
    (running comp198)
  )
  :effect (and
    (not (evaluate comp199))
    (evaluate comp200)
    (all-on comp199)
  )
)
(:action evaluate-comp199-off-comp196
  :parameters ()
  :precondition (and
    (evaluate comp199)
    (not (rebooted comp199))
    (not (running comp196))
  )
  :effect (and
    (not (evaluate comp199))
    (evaluate comp200)
    (one-off comp199)
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
    (running comp133)
    (running comp200)
    (running comp232)
  )
  :effect (and
    (not (evaluate comp201))
    (evaluate comp202)
    (all-on comp201)
  )
)
(:action evaluate-comp201-off-comp133
  :parameters ()
  :precondition (and
    (evaluate comp201)
    (not (rebooted comp201))
    (not (running comp133))
  )
  :effect (and
    (not (evaluate comp201))
    (evaluate comp202)
    (one-off comp201)
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
(:action evaluate-comp201-off-comp232
  :parameters ()
  :precondition (and
    (evaluate comp201)
    (not (rebooted comp201))
    (not (running comp232))
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
    (running comp426)
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
(:action evaluate-comp203-off-comp426
  :parameters ()
  :precondition (and
    (evaluate comp203)
    (not (rebooted comp203))
    (not (running comp426))
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
    (running comp203)
    (running comp427)
  )
  :effect (and
    (not (evaluate comp204))
    (evaluate comp205)
    (all-on comp204)
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
(:action evaluate-comp204-off-comp427
  :parameters ()
  :precondition (and
    (evaluate comp204)
    (not (rebooted comp204))
    (not (running comp427))
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
    (running comp12)
    (running comp205)
    (running comp236)
    (running comp398)
  )
  :effect (and
    (not (evaluate comp206))
    (evaluate comp207)
    (all-on comp206)
  )
)
(:action evaluate-comp206-off-comp12
  :parameters ()
  :precondition (and
    (evaluate comp206)
    (not (rebooted comp206))
    (not (running comp12))
  )
  :effect (and
    (not (evaluate comp206))
    (evaluate comp207)
    (one-off comp206)
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
(:action evaluate-comp206-off-comp236
  :parameters ()
  :precondition (and
    (evaluate comp206)
    (not (rebooted comp206))
    (not (running comp236))
  )
  :effect (and
    (not (evaluate comp206))
    (evaluate comp207)
    (one-off comp206)
  )
)
(:action evaluate-comp206-off-comp398
  :parameters ()
  :precondition (and
    (evaluate comp206)
    (not (rebooted comp206))
    (not (running comp398))
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
    (running comp184)
    (running comp207)
  )
  :effect (and
    (not (evaluate comp208))
    (evaluate comp209)
    (all-on comp208)
  )
)
(:action evaluate-comp208-off-comp184
  :parameters ()
  :precondition (and
    (evaluate comp208)
    (not (rebooted comp208))
    (not (running comp184))
  )
  :effect (and
    (not (evaluate comp208))
    (evaluate comp209)
    (one-off comp208)
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
    (running comp208)
    (running comp387)
    (running comp457)
  )
  :effect (and
    (not (evaluate comp209))
    (evaluate comp210)
    (all-on comp209)
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
(:action evaluate-comp209-off-comp387
  :parameters ()
  :precondition (and
    (evaluate comp209)
    (not (rebooted comp209))
    (not (running comp387))
  )
  :effect (and
    (not (evaluate comp209))
    (evaluate comp210)
    (one-off comp209)
  )
)
(:action evaluate-comp209-off-comp457
  :parameters ()
  :precondition (and
    (evaluate comp209)
    (not (rebooted comp209))
    (not (running comp457))
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
    (running comp212)
    (running comp365)
  )
  :effect (and
    (not (evaluate comp213))
    (evaluate comp214)
    (all-on comp213)
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
(:action evaluate-comp213-off-comp365
  :parameters ()
  :precondition (and
    (evaluate comp213)
    (not (rebooted comp213))
    (not (running comp365))
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
    (running comp214)
  )
  :effect (and
    (not (evaluate comp215))
    (evaluate comp216)
    (all-on comp215)
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
    (running comp217)
    (running comp249)
  )
  :effect (and
    (not (evaluate comp218))
    (evaluate comp219)
    (all-on comp218)
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
(:action evaluate-comp218-off-comp249
  :parameters ()
  :precondition (and
    (evaluate comp218)
    (not (rebooted comp218))
    (not (running comp249))
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
    (running comp218)
  )
  :effect (and
    (not (evaluate comp219))
    (evaluate comp220)
    (all-on comp219)
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
    (running comp219)
  )
  :effect (and
    (not (evaluate comp220))
    (evaluate comp221)
    (all-on comp220)
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
    (running comp220)
  )
  :effect (and
    (not (evaluate comp221))
    (evaluate comp222)
    (all-on comp221)
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
    (running comp221)
  )
  :effect (and
    (not (evaluate comp222))
    (evaluate comp223)
    (all-on comp222)
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
    (running comp289)
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
(:action evaluate-comp223-off-comp289
  :parameters ()
  :precondition (and
    (evaluate comp223)
    (not (rebooted comp223))
    (not (running comp289))
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
    (running comp289)
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
(:action evaluate-comp224-off-comp289
  :parameters ()
  :precondition (and
    (evaluate comp224)
    (not (rebooted comp224))
    (not (running comp289))
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
    (running comp224)
  )
  :effect (and
    (not (evaluate comp225))
    (evaluate comp226)
    (all-on comp225)
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
    (running comp225)
  )
  :effect (and
    (not (evaluate comp226))
    (evaluate comp227)
    (all-on comp226)
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
    (running comp429)
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
(:action evaluate-comp227-off-comp429
  :parameters ()
  :precondition (and
    (evaluate comp227)
    (not (rebooted comp227))
    (not (running comp429))
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
    (running comp227)
  )
  :effect (and
    (not (evaluate comp228))
    (evaluate comp229)
    (all-on comp228)
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
    (running comp228)
  )
  :effect (and
    (not (evaluate comp229))
    (evaluate comp230)
    (all-on comp229)
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
    (running comp140)
    (running comp229)
  )
  :effect (and
    (not (evaluate comp230))
    (evaluate comp231)
    (all-on comp230)
  )
)
(:action evaluate-comp230-off-comp140
  :parameters ()
  :precondition (and
    (evaluate comp230)
    (not (rebooted comp230))
    (not (running comp140))
  )
  :effect (and
    (not (evaluate comp230))
    (evaluate comp231)
    (one-off comp230)
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
    (running comp230)
  )
  :effect (and
    (not (evaluate comp231))
    (evaluate comp232)
    (all-on comp231)
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
    (running comp339)
    (running comp414)
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
(:action evaluate-comp232-off-comp339
  :parameters ()
  :precondition (and
    (evaluate comp232)
    (not (rebooted comp232))
    (not (running comp339))
  )
  :effect (and
    (not (evaluate comp232))
    (evaluate comp233)
    (one-off comp232)
  )
)
(:action evaluate-comp232-off-comp414
  :parameters ()
  :precondition (and
    (evaluate comp232)
    (not (rebooted comp232))
    (not (running comp414))
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
    (running comp197)
    (running comp235)
    (running comp237)
  )
  :effect (and
    (not (evaluate comp236))
    (evaluate comp237)
    (all-on comp236)
  )
)
(:action evaluate-comp236-off-comp197
  :parameters ()
  :precondition (and
    (evaluate comp236)
    (not (rebooted comp236))
    (not (running comp197))
  )
  :effect (and
    (not (evaluate comp236))
    (evaluate comp237)
    (one-off comp236)
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
(:action evaluate-comp236-off-comp237
  :parameters ()
  :precondition (and
    (evaluate comp236)
    (not (rebooted comp236))
    (not (running comp237))
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
    (running comp236)
  )
  :effect (and
    (not (evaluate comp237))
    (evaluate comp238)
    (all-on comp237)
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
    (running comp237)
  )
  :effect (and
    (not (evaluate comp238))
    (evaluate comp239)
    (all-on comp238)
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
    (evaluate comp240)
  )
)
(:action evaluate-comp239-all-on
  :parameters ()
  :precondition (and
    (evaluate comp239)
    (not (rebooted comp239))
    (running comp43)
    (running comp238)
    (running comp257)
  )
  :effect (and
    (not (evaluate comp239))
    (evaluate comp240)
    (all-on comp239)
  )
)
(:action evaluate-comp239-off-comp43
  :parameters ()
  :precondition (and
    (evaluate comp239)
    (not (rebooted comp239))
    (not (running comp43))
  )
  :effect (and
    (not (evaluate comp239))
    (evaluate comp240)
    (one-off comp239)
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
    (evaluate comp240)
    (one-off comp239)
  )
)
(:action evaluate-comp239-off-comp257
  :parameters ()
  :precondition (and
    (evaluate comp239)
    (not (rebooted comp239))
    (not (running comp257))
  )
  :effect (and
    (not (evaluate comp239))
    (evaluate comp240)
    (one-off comp239)
  )
)
(:action evaluate-comp240-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp240)
    (rebooted comp240)
  )
  :effect (and
    (not (evaluate comp240))
    (evaluate comp241)
  )
)
(:action evaluate-comp240-all-on
  :parameters ()
  :precondition (and
    (evaluate comp240)
    (not (rebooted comp240))
    (running comp118)
    (running comp239)
  )
  :effect (and
    (not (evaluate comp240))
    (evaluate comp241)
    (all-on comp240)
  )
)
(:action evaluate-comp240-off-comp118
  :parameters ()
  :precondition (and
    (evaluate comp240)
    (not (rebooted comp240))
    (not (running comp118))
  )
  :effect (and
    (not (evaluate comp240))
    (evaluate comp241)
    (one-off comp240)
  )
)
(:action evaluate-comp240-off-comp239
  :parameters ()
  :precondition (and
    (evaluate comp240)
    (not (rebooted comp240))
    (not (running comp239))
  )
  :effect (and
    (not (evaluate comp240))
    (evaluate comp241)
    (one-off comp240)
  )
)
(:action evaluate-comp241-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp241)
    (rebooted comp241)
  )
  :effect (and
    (not (evaluate comp241))
    (evaluate comp242)
  )
)
(:action evaluate-comp241-all-on
  :parameters ()
  :precondition (and
    (evaluate comp241)
    (not (rebooted comp241))
    (running comp240)
  )
  :effect (and
    (not (evaluate comp241))
    (evaluate comp242)
    (all-on comp241)
  )
)
(:action evaluate-comp241-off-comp240
  :parameters ()
  :precondition (and
    (evaluate comp241)
    (not (rebooted comp241))
    (not (running comp240))
  )
  :effect (and
    (not (evaluate comp241))
    (evaluate comp242)
    (one-off comp241)
  )
)
(:action evaluate-comp242-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp242)
    (rebooted comp242)
  )
  :effect (and
    (not (evaluate comp242))
    (evaluate comp243)
  )
)
(:action evaluate-comp242-all-on
  :parameters ()
  :precondition (and
    (evaluate comp242)
    (not (rebooted comp242))
    (running comp241)
    (running comp256)
  )
  :effect (and
    (not (evaluate comp242))
    (evaluate comp243)
    (all-on comp242)
  )
)
(:action evaluate-comp242-off-comp241
  :parameters ()
  :precondition (and
    (evaluate comp242)
    (not (rebooted comp242))
    (not (running comp241))
  )
  :effect (and
    (not (evaluate comp242))
    (evaluate comp243)
    (one-off comp242)
  )
)
(:action evaluate-comp242-off-comp256
  :parameters ()
  :precondition (and
    (evaluate comp242)
    (not (rebooted comp242))
    (not (running comp256))
  )
  :effect (and
    (not (evaluate comp242))
    (evaluate comp243)
    (one-off comp242)
  )
)
(:action evaluate-comp243-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp243)
    (rebooted comp243)
  )
  :effect (and
    (not (evaluate comp243))
    (evaluate comp244)
  )
)
(:action evaluate-comp243-all-on
  :parameters ()
  :precondition (and
    (evaluate comp243)
    (not (rebooted comp243))
    (running comp189)
    (running comp227)
    (running comp242)
  )
  :effect (and
    (not (evaluate comp243))
    (evaluate comp244)
    (all-on comp243)
  )
)
(:action evaluate-comp243-off-comp189
  :parameters ()
  :precondition (and
    (evaluate comp243)
    (not (rebooted comp243))
    (not (running comp189))
  )
  :effect (and
    (not (evaluate comp243))
    (evaluate comp244)
    (one-off comp243)
  )
)
(:action evaluate-comp243-off-comp227
  :parameters ()
  :precondition (and
    (evaluate comp243)
    (not (rebooted comp243))
    (not (running comp227))
  )
  :effect (and
    (not (evaluate comp243))
    (evaluate comp244)
    (one-off comp243)
  )
)
(:action evaluate-comp243-off-comp242
  :parameters ()
  :precondition (and
    (evaluate comp243)
    (not (rebooted comp243))
    (not (running comp242))
  )
  :effect (and
    (not (evaluate comp243))
    (evaluate comp244)
    (one-off comp243)
  )
)
(:action evaluate-comp244-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp244)
    (rebooted comp244)
  )
  :effect (and
    (not (evaluate comp244))
    (evaluate comp245)
  )
)
(:action evaluate-comp244-all-on
  :parameters ()
  :precondition (and
    (evaluate comp244)
    (not (rebooted comp244))
    (running comp88)
    (running comp243)
  )
  :effect (and
    (not (evaluate comp244))
    (evaluate comp245)
    (all-on comp244)
  )
)
(:action evaluate-comp244-off-comp88
  :parameters ()
  :precondition (and
    (evaluate comp244)
    (not (rebooted comp244))
    (not (running comp88))
  )
  :effect (and
    (not (evaluate comp244))
    (evaluate comp245)
    (one-off comp244)
  )
)
(:action evaluate-comp244-off-comp243
  :parameters ()
  :precondition (and
    (evaluate comp244)
    (not (rebooted comp244))
    (not (running comp243))
  )
  :effect (and
    (not (evaluate comp244))
    (evaluate comp245)
    (one-off comp244)
  )
)
(:action evaluate-comp245-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp245)
    (rebooted comp245)
  )
  :effect (and
    (not (evaluate comp245))
    (evaluate comp246)
  )
)
(:action evaluate-comp245-all-on
  :parameters ()
  :precondition (and
    (evaluate comp245)
    (not (rebooted comp245))
    (running comp244)
  )
  :effect (and
    (not (evaluate comp245))
    (evaluate comp246)
    (all-on comp245)
  )
)
(:action evaluate-comp245-off-comp244
  :parameters ()
  :precondition (and
    (evaluate comp245)
    (not (rebooted comp245))
    (not (running comp244))
  )
  :effect (and
    (not (evaluate comp245))
    (evaluate comp246)
    (one-off comp245)
  )
)
(:action evaluate-comp246-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp246)
    (rebooted comp246)
  )
  :effect (and
    (not (evaluate comp246))
    (evaluate comp247)
  )
)
(:action evaluate-comp246-all-on
  :parameters ()
  :precondition (and
    (evaluate comp246)
    (not (rebooted comp246))
    (running comp7)
    (running comp143)
    (running comp171)
    (running comp245)
    (running comp291)
  )
  :effect (and
    (not (evaluate comp246))
    (evaluate comp247)
    (all-on comp246)
  )
)
(:action evaluate-comp246-off-comp7
  :parameters ()
  :precondition (and
    (evaluate comp246)
    (not (rebooted comp246))
    (not (running comp7))
  )
  :effect (and
    (not (evaluate comp246))
    (evaluate comp247)
    (one-off comp246)
  )
)
(:action evaluate-comp246-off-comp143
  :parameters ()
  :precondition (and
    (evaluate comp246)
    (not (rebooted comp246))
    (not (running comp143))
  )
  :effect (and
    (not (evaluate comp246))
    (evaluate comp247)
    (one-off comp246)
  )
)
(:action evaluate-comp246-off-comp171
  :parameters ()
  :precondition (and
    (evaluate comp246)
    (not (rebooted comp246))
    (not (running comp171))
  )
  :effect (and
    (not (evaluate comp246))
    (evaluate comp247)
    (one-off comp246)
  )
)
(:action evaluate-comp246-off-comp245
  :parameters ()
  :precondition (and
    (evaluate comp246)
    (not (rebooted comp246))
    (not (running comp245))
  )
  :effect (and
    (not (evaluate comp246))
    (evaluate comp247)
    (one-off comp246)
  )
)
(:action evaluate-comp246-off-comp291
  :parameters ()
  :precondition (and
    (evaluate comp246)
    (not (rebooted comp246))
    (not (running comp291))
  )
  :effect (and
    (not (evaluate comp246))
    (evaluate comp247)
    (one-off comp246)
  )
)
(:action evaluate-comp247-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp247)
    (rebooted comp247)
  )
  :effect (and
    (not (evaluate comp247))
    (evaluate comp248)
  )
)
(:action evaluate-comp247-all-on
  :parameters ()
  :precondition (and
    (evaluate comp247)
    (not (rebooted comp247))
    (running comp246)
  )
  :effect (and
    (not (evaluate comp247))
    (evaluate comp248)
    (all-on comp247)
  )
)
(:action evaluate-comp247-off-comp246
  :parameters ()
  :precondition (and
    (evaluate comp247)
    (not (rebooted comp247))
    (not (running comp246))
  )
  :effect (and
    (not (evaluate comp247))
    (evaluate comp248)
    (one-off comp247)
  )
)
(:action evaluate-comp248-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp248)
    (rebooted comp248)
  )
  :effect (and
    (not (evaluate comp248))
    (evaluate comp249)
  )
)
(:action evaluate-comp248-all-on
  :parameters ()
  :precondition (and
    (evaluate comp248)
    (not (rebooted comp248))
    (running comp247)
  )
  :effect (and
    (not (evaluate comp248))
    (evaluate comp249)
    (all-on comp248)
  )
)
(:action evaluate-comp248-off-comp247
  :parameters ()
  :precondition (and
    (evaluate comp248)
    (not (rebooted comp248))
    (not (running comp247))
  )
  :effect (and
    (not (evaluate comp248))
    (evaluate comp249)
    (one-off comp248)
  )
)
(:action evaluate-comp249-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp249)
    (rebooted comp249)
  )
  :effect (and
    (not (evaluate comp249))
    (evaluate comp250)
  )
)
(:action evaluate-comp249-all-on
  :parameters ()
  :precondition (and
    (evaluate comp249)
    (not (rebooted comp249))
    (running comp248)
  )
  :effect (and
    (not (evaluate comp249))
    (evaluate comp250)
    (all-on comp249)
  )
)
(:action evaluate-comp249-off-comp248
  :parameters ()
  :precondition (and
    (evaluate comp249)
    (not (rebooted comp249))
    (not (running comp248))
  )
  :effect (and
    (not (evaluate comp249))
    (evaluate comp250)
    (one-off comp249)
  )
)
(:action evaluate-comp250-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp250)
    (rebooted comp250)
  )
  :effect (and
    (not (evaluate comp250))
    (evaluate comp251)
  )
)
(:action evaluate-comp250-all-on
  :parameters ()
  :precondition (and
    (evaluate comp250)
    (not (rebooted comp250))
    (running comp249)
  )
  :effect (and
    (not (evaluate comp250))
    (evaluate comp251)
    (all-on comp250)
  )
)
(:action evaluate-comp250-off-comp249
  :parameters ()
  :precondition (and
    (evaluate comp250)
    (not (rebooted comp250))
    (not (running comp249))
  )
  :effect (and
    (not (evaluate comp250))
    (evaluate comp251)
    (one-off comp250)
  )
)
(:action evaluate-comp251-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp251)
    (rebooted comp251)
  )
  :effect (and
    (not (evaluate comp251))
    (evaluate comp252)
  )
)
(:action evaluate-comp251-all-on
  :parameters ()
  :precondition (and
    (evaluate comp251)
    (not (rebooted comp251))
    (running comp116)
    (running comp250)
  )
  :effect (and
    (not (evaluate comp251))
    (evaluate comp252)
    (all-on comp251)
  )
)
(:action evaluate-comp251-off-comp116
  :parameters ()
  :precondition (and
    (evaluate comp251)
    (not (rebooted comp251))
    (not (running comp116))
  )
  :effect (and
    (not (evaluate comp251))
    (evaluate comp252)
    (one-off comp251)
  )
)
(:action evaluate-comp251-off-comp250
  :parameters ()
  :precondition (and
    (evaluate comp251)
    (not (rebooted comp251))
    (not (running comp250))
  )
  :effect (and
    (not (evaluate comp251))
    (evaluate comp252)
    (one-off comp251)
  )
)
(:action evaluate-comp252-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp252)
    (rebooted comp252)
  )
  :effect (and
    (not (evaluate comp252))
    (evaluate comp253)
  )
)
(:action evaluate-comp252-all-on
  :parameters ()
  :precondition (and
    (evaluate comp252)
    (not (rebooted comp252))
    (running comp199)
    (running comp251)
  )
  :effect (and
    (not (evaluate comp252))
    (evaluate comp253)
    (all-on comp252)
  )
)
(:action evaluate-comp252-off-comp199
  :parameters ()
  :precondition (and
    (evaluate comp252)
    (not (rebooted comp252))
    (not (running comp199))
  )
  :effect (and
    (not (evaluate comp252))
    (evaluate comp253)
    (one-off comp252)
  )
)
(:action evaluate-comp252-off-comp251
  :parameters ()
  :precondition (and
    (evaluate comp252)
    (not (rebooted comp252))
    (not (running comp251))
  )
  :effect (and
    (not (evaluate comp252))
    (evaluate comp253)
    (one-off comp252)
  )
)
(:action evaluate-comp253-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp253)
    (rebooted comp253)
  )
  :effect (and
    (not (evaluate comp253))
    (evaluate comp254)
  )
)
(:action evaluate-comp253-all-on
  :parameters ()
  :precondition (and
    (evaluate comp253)
    (not (rebooted comp253))
    (running comp30)
    (running comp252)
  )
  :effect (and
    (not (evaluate comp253))
    (evaluate comp254)
    (all-on comp253)
  )
)
(:action evaluate-comp253-off-comp30
  :parameters ()
  :precondition (and
    (evaluate comp253)
    (not (rebooted comp253))
    (not (running comp30))
  )
  :effect (and
    (not (evaluate comp253))
    (evaluate comp254)
    (one-off comp253)
  )
)
(:action evaluate-comp253-off-comp252
  :parameters ()
  :precondition (and
    (evaluate comp253)
    (not (rebooted comp253))
    (not (running comp252))
  )
  :effect (and
    (not (evaluate comp253))
    (evaluate comp254)
    (one-off comp253)
  )
)
(:action evaluate-comp254-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp254)
    (rebooted comp254)
  )
  :effect (and
    (not (evaluate comp254))
    (evaluate comp255)
  )
)
(:action evaluate-comp254-all-on
  :parameters ()
  :precondition (and
    (evaluate comp254)
    (not (rebooted comp254))
    (running comp253)
  )
  :effect (and
    (not (evaluate comp254))
    (evaluate comp255)
    (all-on comp254)
  )
)
(:action evaluate-comp254-off-comp253
  :parameters ()
  :precondition (and
    (evaluate comp254)
    (not (rebooted comp254))
    (not (running comp253))
  )
  :effect (and
    (not (evaluate comp254))
    (evaluate comp255)
    (one-off comp254)
  )
)
(:action evaluate-comp255-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp255)
    (rebooted comp255)
  )
  :effect (and
    (not (evaluate comp255))
    (evaluate comp256)
  )
)
(:action evaluate-comp255-all-on
  :parameters ()
  :precondition (and
    (evaluate comp255)
    (not (rebooted comp255))
    (running comp33)
    (running comp254)
  )
  :effect (and
    (not (evaluate comp255))
    (evaluate comp256)
    (all-on comp255)
  )
)
(:action evaluate-comp255-off-comp33
  :parameters ()
  :precondition (and
    (evaluate comp255)
    (not (rebooted comp255))
    (not (running comp33))
  )
  :effect (and
    (not (evaluate comp255))
    (evaluate comp256)
    (one-off comp255)
  )
)
(:action evaluate-comp255-off-comp254
  :parameters ()
  :precondition (and
    (evaluate comp255)
    (not (rebooted comp255))
    (not (running comp254))
  )
  :effect (and
    (not (evaluate comp255))
    (evaluate comp256)
    (one-off comp255)
  )
)
(:action evaluate-comp256-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp256)
    (rebooted comp256)
  )
  :effect (and
    (not (evaluate comp256))
    (evaluate comp257)
  )
)
(:action evaluate-comp256-all-on
  :parameters ()
  :precondition (and
    (evaluate comp256)
    (not (rebooted comp256))
    (running comp255)
  )
  :effect (and
    (not (evaluate comp256))
    (evaluate comp257)
    (all-on comp256)
  )
)
(:action evaluate-comp256-off-comp255
  :parameters ()
  :precondition (and
    (evaluate comp256)
    (not (rebooted comp256))
    (not (running comp255))
  )
  :effect (and
    (not (evaluate comp256))
    (evaluate comp257)
    (one-off comp256)
  )
)
(:action evaluate-comp257-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp257)
    (rebooted comp257)
  )
  :effect (and
    (not (evaluate comp257))
    (evaluate comp258)
  )
)
(:action evaluate-comp257-all-on
  :parameters ()
  :precondition (and
    (evaluate comp257)
    (not (rebooted comp257))
    (running comp210)
    (running comp256)
  )
  :effect (and
    (not (evaluate comp257))
    (evaluate comp258)
    (all-on comp257)
  )
)
(:action evaluate-comp257-off-comp210
  :parameters ()
  :precondition (and
    (evaluate comp257)
    (not (rebooted comp257))
    (not (running comp210))
  )
  :effect (and
    (not (evaluate comp257))
    (evaluate comp258)
    (one-off comp257)
  )
)
(:action evaluate-comp257-off-comp256
  :parameters ()
  :precondition (and
    (evaluate comp257)
    (not (rebooted comp257))
    (not (running comp256))
  )
  :effect (and
    (not (evaluate comp257))
    (evaluate comp258)
    (one-off comp257)
  )
)
(:action evaluate-comp258-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp258)
    (rebooted comp258)
  )
  :effect (and
    (not (evaluate comp258))
    (evaluate comp259)
  )
)
(:action evaluate-comp258-all-on
  :parameters ()
  :precondition (and
    (evaluate comp258)
    (not (rebooted comp258))
    (running comp130)
    (running comp257)
  )
  :effect (and
    (not (evaluate comp258))
    (evaluate comp259)
    (all-on comp258)
  )
)
(:action evaluate-comp258-off-comp130
  :parameters ()
  :precondition (and
    (evaluate comp258)
    (not (rebooted comp258))
    (not (running comp130))
  )
  :effect (and
    (not (evaluate comp258))
    (evaluate comp259)
    (one-off comp258)
  )
)
(:action evaluate-comp258-off-comp257
  :parameters ()
  :precondition (and
    (evaluate comp258)
    (not (rebooted comp258))
    (not (running comp257))
  )
  :effect (and
    (not (evaluate comp258))
    (evaluate comp259)
    (one-off comp258)
  )
)
(:action evaluate-comp259-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp259)
    (rebooted comp259)
  )
  :effect (and
    (not (evaluate comp259))
    (evaluate comp260)
  )
)
(:action evaluate-comp259-all-on
  :parameters ()
  :precondition (and
    (evaluate comp259)
    (not (rebooted comp259))
    (running comp258)
  )
  :effect (and
    (not (evaluate comp259))
    (evaluate comp260)
    (all-on comp259)
  )
)
(:action evaluate-comp259-off-comp258
  :parameters ()
  :precondition (and
    (evaluate comp259)
    (not (rebooted comp259))
    (not (running comp258))
  )
  :effect (and
    (not (evaluate comp259))
    (evaluate comp260)
    (one-off comp259)
  )
)
(:action evaluate-comp260-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp260)
    (rebooted comp260)
  )
  :effect (and
    (not (evaluate comp260))
    (evaluate comp261)
  )
)
(:action evaluate-comp260-all-on
  :parameters ()
  :precondition (and
    (evaluate comp260)
    (not (rebooted comp260))
    (running comp259)
  )
  :effect (and
    (not (evaluate comp260))
    (evaluate comp261)
    (all-on comp260)
  )
)
(:action evaluate-comp260-off-comp259
  :parameters ()
  :precondition (and
    (evaluate comp260)
    (not (rebooted comp260))
    (not (running comp259))
  )
  :effect (and
    (not (evaluate comp260))
    (evaluate comp261)
    (one-off comp260)
  )
)
(:action evaluate-comp261-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp261)
    (rebooted comp261)
  )
  :effect (and
    (not (evaluate comp261))
    (evaluate comp262)
  )
)
(:action evaluate-comp261-all-on
  :parameters ()
  :precondition (and
    (evaluate comp261)
    (not (rebooted comp261))
    (running comp260)
    (running comp286)
  )
  :effect (and
    (not (evaluate comp261))
    (evaluate comp262)
    (all-on comp261)
  )
)
(:action evaluate-comp261-off-comp260
  :parameters ()
  :precondition (and
    (evaluate comp261)
    (not (rebooted comp261))
    (not (running comp260))
  )
  :effect (and
    (not (evaluate comp261))
    (evaluate comp262)
    (one-off comp261)
  )
)
(:action evaluate-comp261-off-comp286
  :parameters ()
  :precondition (and
    (evaluate comp261)
    (not (rebooted comp261))
    (not (running comp286))
  )
  :effect (and
    (not (evaluate comp261))
    (evaluate comp262)
    (one-off comp261)
  )
)
(:action evaluate-comp262-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp262)
    (rebooted comp262)
  )
  :effect (and
    (not (evaluate comp262))
    (evaluate comp263)
  )
)
(:action evaluate-comp262-all-on
  :parameters ()
  :precondition (and
    (evaluate comp262)
    (not (rebooted comp262))
    (running comp261)
    (running comp286)
  )
  :effect (and
    (not (evaluate comp262))
    (evaluate comp263)
    (all-on comp262)
  )
)
(:action evaluate-comp262-off-comp261
  :parameters ()
  :precondition (and
    (evaluate comp262)
    (not (rebooted comp262))
    (not (running comp261))
  )
  :effect (and
    (not (evaluate comp262))
    (evaluate comp263)
    (one-off comp262)
  )
)
(:action evaluate-comp262-off-comp286
  :parameters ()
  :precondition (and
    (evaluate comp262)
    (not (rebooted comp262))
    (not (running comp286))
  )
  :effect (and
    (not (evaluate comp262))
    (evaluate comp263)
    (one-off comp262)
  )
)
(:action evaluate-comp263-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp263)
    (rebooted comp263)
  )
  :effect (and
    (not (evaluate comp263))
    (evaluate comp264)
  )
)
(:action evaluate-comp263-all-on
  :parameters ()
  :precondition (and
    (evaluate comp263)
    (not (rebooted comp263))
    (running comp262)
  )
  :effect (and
    (not (evaluate comp263))
    (evaluate comp264)
    (all-on comp263)
  )
)
(:action evaluate-comp263-off-comp262
  :parameters ()
  :precondition (and
    (evaluate comp263)
    (not (rebooted comp263))
    (not (running comp262))
  )
  :effect (and
    (not (evaluate comp263))
    (evaluate comp264)
    (one-off comp263)
  )
)
(:action evaluate-comp264-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp264)
    (rebooted comp264)
  )
  :effect (and
    (not (evaluate comp264))
    (evaluate comp265)
  )
)
(:action evaluate-comp264-all-on
  :parameters ()
  :precondition (and
    (evaluate comp264)
    (not (rebooted comp264))
    (running comp263)
    (running comp366)
  )
  :effect (and
    (not (evaluate comp264))
    (evaluate comp265)
    (all-on comp264)
  )
)
(:action evaluate-comp264-off-comp263
  :parameters ()
  :precondition (and
    (evaluate comp264)
    (not (rebooted comp264))
    (not (running comp263))
  )
  :effect (and
    (not (evaluate comp264))
    (evaluate comp265)
    (one-off comp264)
  )
)
(:action evaluate-comp264-off-comp366
  :parameters ()
  :precondition (and
    (evaluate comp264)
    (not (rebooted comp264))
    (not (running comp366))
  )
  :effect (and
    (not (evaluate comp264))
    (evaluate comp265)
    (one-off comp264)
  )
)
(:action evaluate-comp265-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp265)
    (rebooted comp265)
  )
  :effect (and
    (not (evaluate comp265))
    (evaluate comp266)
  )
)
(:action evaluate-comp265-all-on
  :parameters ()
  :precondition (and
    (evaluate comp265)
    (not (rebooted comp265))
    (running comp48)
    (running comp195)
    (running comp264)
  )
  :effect (and
    (not (evaluate comp265))
    (evaluate comp266)
    (all-on comp265)
  )
)
(:action evaluate-comp265-off-comp48
  :parameters ()
  :precondition (and
    (evaluate comp265)
    (not (rebooted comp265))
    (not (running comp48))
  )
  :effect (and
    (not (evaluate comp265))
    (evaluate comp266)
    (one-off comp265)
  )
)
(:action evaluate-comp265-off-comp195
  :parameters ()
  :precondition (and
    (evaluate comp265)
    (not (rebooted comp265))
    (not (running comp195))
  )
  :effect (and
    (not (evaluate comp265))
    (evaluate comp266)
    (one-off comp265)
  )
)
(:action evaluate-comp265-off-comp264
  :parameters ()
  :precondition (and
    (evaluate comp265)
    (not (rebooted comp265))
    (not (running comp264))
  )
  :effect (and
    (not (evaluate comp265))
    (evaluate comp266)
    (one-off comp265)
  )
)
(:action evaluate-comp266-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp266)
    (rebooted comp266)
  )
  :effect (and
    (not (evaluate comp266))
    (evaluate comp267)
  )
)
(:action evaluate-comp266-all-on
  :parameters ()
  :precondition (and
    (evaluate comp266)
    (not (rebooted comp266))
    (running comp265)
  )
  :effect (and
    (not (evaluate comp266))
    (evaluate comp267)
    (all-on comp266)
  )
)
(:action evaluate-comp266-off-comp265
  :parameters ()
  :precondition (and
    (evaluate comp266)
    (not (rebooted comp266))
    (not (running comp265))
  )
  :effect (and
    (not (evaluate comp266))
    (evaluate comp267)
    (one-off comp266)
  )
)
(:action evaluate-comp267-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp267)
    (rebooted comp267)
  )
  :effect (and
    (not (evaluate comp267))
    (evaluate comp268)
  )
)
(:action evaluate-comp267-all-on
  :parameters ()
  :precondition (and
    (evaluate comp267)
    (not (rebooted comp267))
    (running comp266)
  )
  :effect (and
    (not (evaluate comp267))
    (evaluate comp268)
    (all-on comp267)
  )
)
(:action evaluate-comp267-off-comp266
  :parameters ()
  :precondition (and
    (evaluate comp267)
    (not (rebooted comp267))
    (not (running comp266))
  )
  :effect (and
    (not (evaluate comp267))
    (evaluate comp268)
    (one-off comp267)
  )
)
(:action evaluate-comp268-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp268)
    (rebooted comp268)
  )
  :effect (and
    (not (evaluate comp268))
    (evaluate comp269)
  )
)
(:action evaluate-comp268-all-on
  :parameters ()
  :precondition (and
    (evaluate comp268)
    (not (rebooted comp268))
    (running comp267)
  )
  :effect (and
    (not (evaluate comp268))
    (evaluate comp269)
    (all-on comp268)
  )
)
(:action evaluate-comp268-off-comp267
  :parameters ()
  :precondition (and
    (evaluate comp268)
    (not (rebooted comp268))
    (not (running comp267))
  )
  :effect (and
    (not (evaluate comp268))
    (evaluate comp269)
    (one-off comp268)
  )
)
(:action evaluate-comp269-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp269)
    (rebooted comp269)
  )
  :effect (and
    (not (evaluate comp269))
    (evaluate comp270)
  )
)
(:action evaluate-comp269-all-on
  :parameters ()
  :precondition (and
    (evaluate comp269)
    (not (rebooted comp269))
    (running comp268)
    (running comp352)
  )
  :effect (and
    (not (evaluate comp269))
    (evaluate comp270)
    (all-on comp269)
  )
)
(:action evaluate-comp269-off-comp268
  :parameters ()
  :precondition (and
    (evaluate comp269)
    (not (rebooted comp269))
    (not (running comp268))
  )
  :effect (and
    (not (evaluate comp269))
    (evaluate comp270)
    (one-off comp269)
  )
)
(:action evaluate-comp269-off-comp352
  :parameters ()
  :precondition (and
    (evaluate comp269)
    (not (rebooted comp269))
    (not (running comp352))
  )
  :effect (and
    (not (evaluate comp269))
    (evaluate comp270)
    (one-off comp269)
  )
)
(:action evaluate-comp270-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp270)
    (rebooted comp270)
  )
  :effect (and
    (not (evaluate comp270))
    (evaluate comp271)
  )
)
(:action evaluate-comp270-all-on
  :parameters ()
  :precondition (and
    (evaluate comp270)
    (not (rebooted comp270))
    (running comp269)
    (running comp315)
    (running comp431)
  )
  :effect (and
    (not (evaluate comp270))
    (evaluate comp271)
    (all-on comp270)
  )
)
(:action evaluate-comp270-off-comp269
  :parameters ()
  :precondition (and
    (evaluate comp270)
    (not (rebooted comp270))
    (not (running comp269))
  )
  :effect (and
    (not (evaluate comp270))
    (evaluate comp271)
    (one-off comp270)
  )
)
(:action evaluate-comp270-off-comp315
  :parameters ()
  :precondition (and
    (evaluate comp270)
    (not (rebooted comp270))
    (not (running comp315))
  )
  :effect (and
    (not (evaluate comp270))
    (evaluate comp271)
    (one-off comp270)
  )
)
(:action evaluate-comp270-off-comp431
  :parameters ()
  :precondition (and
    (evaluate comp270)
    (not (rebooted comp270))
    (not (running comp431))
  )
  :effect (and
    (not (evaluate comp270))
    (evaluate comp271)
    (one-off comp270)
  )
)
(:action evaluate-comp271-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp271)
    (rebooted comp271)
  )
  :effect (and
    (not (evaluate comp271))
    (evaluate comp272)
  )
)
(:action evaluate-comp271-all-on
  :parameters ()
  :precondition (and
    (evaluate comp271)
    (not (rebooted comp271))
    (running comp240)
    (running comp270)
  )
  :effect (and
    (not (evaluate comp271))
    (evaluate comp272)
    (all-on comp271)
  )
)
(:action evaluate-comp271-off-comp240
  :parameters ()
  :precondition (and
    (evaluate comp271)
    (not (rebooted comp271))
    (not (running comp240))
  )
  :effect (and
    (not (evaluate comp271))
    (evaluate comp272)
    (one-off comp271)
  )
)
(:action evaluate-comp271-off-comp270
  :parameters ()
  :precondition (and
    (evaluate comp271)
    (not (rebooted comp271))
    (not (running comp270))
  )
  :effect (and
    (not (evaluate comp271))
    (evaluate comp272)
    (one-off comp271)
  )
)
(:action evaluate-comp272-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp272)
    (rebooted comp272)
  )
  :effect (and
    (not (evaluate comp272))
    (evaluate comp273)
  )
)
(:action evaluate-comp272-all-on
  :parameters ()
  :precondition (and
    (evaluate comp272)
    (not (rebooted comp272))
    (running comp271)
  )
  :effect (and
    (not (evaluate comp272))
    (evaluate comp273)
    (all-on comp272)
  )
)
(:action evaluate-comp272-off-comp271
  :parameters ()
  :precondition (and
    (evaluate comp272)
    (not (rebooted comp272))
    (not (running comp271))
  )
  :effect (and
    (not (evaluate comp272))
    (evaluate comp273)
    (one-off comp272)
  )
)
(:action evaluate-comp273-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp273)
    (rebooted comp273)
  )
  :effect (and
    (not (evaluate comp273))
    (evaluate comp274)
  )
)
(:action evaluate-comp273-all-on
  :parameters ()
  :precondition (and
    (evaluate comp273)
    (not (rebooted comp273))
    (running comp272)
  )
  :effect (and
    (not (evaluate comp273))
    (evaluate comp274)
    (all-on comp273)
  )
)
(:action evaluate-comp273-off-comp272
  :parameters ()
  :precondition (and
    (evaluate comp273)
    (not (rebooted comp273))
    (not (running comp272))
  )
  :effect (and
    (not (evaluate comp273))
    (evaluate comp274)
    (one-off comp273)
  )
)
(:action evaluate-comp274-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp274)
    (rebooted comp274)
  )
  :effect (and
    (not (evaluate comp274))
    (evaluate comp275)
  )
)
(:action evaluate-comp274-all-on
  :parameters ()
  :precondition (and
    (evaluate comp274)
    (not (rebooted comp274))
    (running comp273)
  )
  :effect (and
    (not (evaluate comp274))
    (evaluate comp275)
    (all-on comp274)
  )
)
(:action evaluate-comp274-off-comp273
  :parameters ()
  :precondition (and
    (evaluate comp274)
    (not (rebooted comp274))
    (not (running comp273))
  )
  :effect (and
    (not (evaluate comp274))
    (evaluate comp275)
    (one-off comp274)
  )
)
(:action evaluate-comp275-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp275)
    (rebooted comp275)
  )
  :effect (and
    (not (evaluate comp275))
    (evaluate comp276)
  )
)
(:action evaluate-comp275-all-on
  :parameters ()
  :precondition (and
    (evaluate comp275)
    (not (rebooted comp275))
    (running comp274)
  )
  :effect (and
    (not (evaluate comp275))
    (evaluate comp276)
    (all-on comp275)
  )
)
(:action evaluate-comp275-off-comp274
  :parameters ()
  :precondition (and
    (evaluate comp275)
    (not (rebooted comp275))
    (not (running comp274))
  )
  :effect (and
    (not (evaluate comp275))
    (evaluate comp276)
    (one-off comp275)
  )
)
(:action evaluate-comp276-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp276)
    (rebooted comp276)
  )
  :effect (and
    (not (evaluate comp276))
    (evaluate comp277)
  )
)
(:action evaluate-comp276-all-on
  :parameters ()
  :precondition (and
    (evaluate comp276)
    (not (rebooted comp276))
    (running comp275)
  )
  :effect (and
    (not (evaluate comp276))
    (evaluate comp277)
    (all-on comp276)
  )
)
(:action evaluate-comp276-off-comp275
  :parameters ()
  :precondition (and
    (evaluate comp276)
    (not (rebooted comp276))
    (not (running comp275))
  )
  :effect (and
    (not (evaluate comp276))
    (evaluate comp277)
    (one-off comp276)
  )
)
(:action evaluate-comp277-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp277)
    (rebooted comp277)
  )
  :effect (and
    (not (evaluate comp277))
    (evaluate comp278)
  )
)
(:action evaluate-comp277-all-on
  :parameters ()
  :precondition (and
    (evaluate comp277)
    (not (rebooted comp277))
    (running comp276)
  )
  :effect (and
    (not (evaluate comp277))
    (evaluate comp278)
    (all-on comp277)
  )
)
(:action evaluate-comp277-off-comp276
  :parameters ()
  :precondition (and
    (evaluate comp277)
    (not (rebooted comp277))
    (not (running comp276))
  )
  :effect (and
    (not (evaluate comp277))
    (evaluate comp278)
    (one-off comp277)
  )
)
(:action evaluate-comp278-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp278)
    (rebooted comp278)
  )
  :effect (and
    (not (evaluate comp278))
    (evaluate comp279)
  )
)
(:action evaluate-comp278-all-on
  :parameters ()
  :precondition (and
    (evaluate comp278)
    (not (rebooted comp278))
    (running comp277)
  )
  :effect (and
    (not (evaluate comp278))
    (evaluate comp279)
    (all-on comp278)
  )
)
(:action evaluate-comp278-off-comp277
  :parameters ()
  :precondition (and
    (evaluate comp278)
    (not (rebooted comp278))
    (not (running comp277))
  )
  :effect (and
    (not (evaluate comp278))
    (evaluate comp279)
    (one-off comp278)
  )
)
(:action evaluate-comp279-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp279)
    (rebooted comp279)
  )
  :effect (and
    (not (evaluate comp279))
    (evaluate comp280)
  )
)
(:action evaluate-comp279-all-on
  :parameters ()
  :precondition (and
    (evaluate comp279)
    (not (rebooted comp279))
    (running comp278)
  )
  :effect (and
    (not (evaluate comp279))
    (evaluate comp280)
    (all-on comp279)
  )
)
(:action evaluate-comp279-off-comp278
  :parameters ()
  :precondition (and
    (evaluate comp279)
    (not (rebooted comp279))
    (not (running comp278))
  )
  :effect (and
    (not (evaluate comp279))
    (evaluate comp280)
    (one-off comp279)
  )
)
(:action evaluate-comp280-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp280)
    (rebooted comp280)
  )
  :effect (and
    (not (evaluate comp280))
    (evaluate comp281)
  )
)
(:action evaluate-comp280-all-on
  :parameters ()
  :precondition (and
    (evaluate comp280)
    (not (rebooted comp280))
    (running comp279)
  )
  :effect (and
    (not (evaluate comp280))
    (evaluate comp281)
    (all-on comp280)
  )
)
(:action evaluate-comp280-off-comp279
  :parameters ()
  :precondition (and
    (evaluate comp280)
    (not (rebooted comp280))
    (not (running comp279))
  )
  :effect (and
    (not (evaluate comp280))
    (evaluate comp281)
    (one-off comp280)
  )
)
(:action evaluate-comp281-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp281)
    (rebooted comp281)
  )
  :effect (and
    (not (evaluate comp281))
    (evaluate comp282)
  )
)
(:action evaluate-comp281-all-on
  :parameters ()
  :precondition (and
    (evaluate comp281)
    (not (rebooted comp281))
    (running comp19)
    (running comp280)
  )
  :effect (and
    (not (evaluate comp281))
    (evaluate comp282)
    (all-on comp281)
  )
)
(:action evaluate-comp281-off-comp19
  :parameters ()
  :precondition (and
    (evaluate comp281)
    (not (rebooted comp281))
    (not (running comp19))
  )
  :effect (and
    (not (evaluate comp281))
    (evaluate comp282)
    (one-off comp281)
  )
)
(:action evaluate-comp281-off-comp280
  :parameters ()
  :precondition (and
    (evaluate comp281)
    (not (rebooted comp281))
    (not (running comp280))
  )
  :effect (and
    (not (evaluate comp281))
    (evaluate comp282)
    (one-off comp281)
  )
)
(:action evaluate-comp282-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp282)
    (rebooted comp282)
  )
  :effect (and
    (not (evaluate comp282))
    (evaluate comp283)
  )
)
(:action evaluate-comp282-all-on
  :parameters ()
  :precondition (and
    (evaluate comp282)
    (not (rebooted comp282))
    (running comp281)
  )
  :effect (and
    (not (evaluate comp282))
    (evaluate comp283)
    (all-on comp282)
  )
)
(:action evaluate-comp282-off-comp281
  :parameters ()
  :precondition (and
    (evaluate comp282)
    (not (rebooted comp282))
    (not (running comp281))
  )
  :effect (and
    (not (evaluate comp282))
    (evaluate comp283)
    (one-off comp282)
  )
)
(:action evaluate-comp283-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp283)
    (rebooted comp283)
  )
  :effect (and
    (not (evaluate comp283))
    (evaluate comp284)
  )
)
(:action evaluate-comp283-all-on
  :parameters ()
  :precondition (and
    (evaluate comp283)
    (not (rebooted comp283))
    (running comp223)
    (running comp282)
  )
  :effect (and
    (not (evaluate comp283))
    (evaluate comp284)
    (all-on comp283)
  )
)
(:action evaluate-comp283-off-comp223
  :parameters ()
  :precondition (and
    (evaluate comp283)
    (not (rebooted comp283))
    (not (running comp223))
  )
  :effect (and
    (not (evaluate comp283))
    (evaluate comp284)
    (one-off comp283)
  )
)
(:action evaluate-comp283-off-comp282
  :parameters ()
  :precondition (and
    (evaluate comp283)
    (not (rebooted comp283))
    (not (running comp282))
  )
  :effect (and
    (not (evaluate comp283))
    (evaluate comp284)
    (one-off comp283)
  )
)
(:action evaluate-comp284-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp284)
    (rebooted comp284)
  )
  :effect (and
    (not (evaluate comp284))
    (evaluate comp285)
  )
)
(:action evaluate-comp284-all-on
  :parameters ()
  :precondition (and
    (evaluate comp284)
    (not (rebooted comp284))
    (running comp283)
  )
  :effect (and
    (not (evaluate comp284))
    (evaluate comp285)
    (all-on comp284)
  )
)
(:action evaluate-comp284-off-comp283
  :parameters ()
  :precondition (and
    (evaluate comp284)
    (not (rebooted comp284))
    (not (running comp283))
  )
  :effect (and
    (not (evaluate comp284))
    (evaluate comp285)
    (one-off comp284)
  )
)
(:action evaluate-comp285-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp285)
    (rebooted comp285)
  )
  :effect (and
    (not (evaluate comp285))
    (evaluate comp286)
  )
)
(:action evaluate-comp285-all-on
  :parameters ()
  :precondition (and
    (evaluate comp285)
    (not (rebooted comp285))
    (running comp280)
    (running comp284)
  )
  :effect (and
    (not (evaluate comp285))
    (evaluate comp286)
    (all-on comp285)
  )
)
(:action evaluate-comp285-off-comp280
  :parameters ()
  :precondition (and
    (evaluate comp285)
    (not (rebooted comp285))
    (not (running comp280))
  )
  :effect (and
    (not (evaluate comp285))
    (evaluate comp286)
    (one-off comp285)
  )
)
(:action evaluate-comp285-off-comp284
  :parameters ()
  :precondition (and
    (evaluate comp285)
    (not (rebooted comp285))
    (not (running comp284))
  )
  :effect (and
    (not (evaluate comp285))
    (evaluate comp286)
    (one-off comp285)
  )
)
(:action evaluate-comp286-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp286)
    (rebooted comp286)
  )
  :effect (and
    (not (evaluate comp286))
    (evaluate comp287)
  )
)
(:action evaluate-comp286-all-on
  :parameters ()
  :precondition (and
    (evaluate comp286)
    (not (rebooted comp286))
    (running comp285)
  )
  :effect (and
    (not (evaluate comp286))
    (evaluate comp287)
    (all-on comp286)
  )
)
(:action evaluate-comp286-off-comp285
  :parameters ()
  :precondition (and
    (evaluate comp286)
    (not (rebooted comp286))
    (not (running comp285))
  )
  :effect (and
    (not (evaluate comp286))
    (evaluate comp287)
    (one-off comp286)
  )
)
(:action evaluate-comp287-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp287)
    (rebooted comp287)
  )
  :effect (and
    (not (evaluate comp287))
    (evaluate comp288)
  )
)
(:action evaluate-comp287-all-on
  :parameters ()
  :precondition (and
    (evaluate comp287)
    (not (rebooted comp287))
    (running comp286)
    (running comp442)
  )
  :effect (and
    (not (evaluate comp287))
    (evaluate comp288)
    (all-on comp287)
  )
)
(:action evaluate-comp287-off-comp286
  :parameters ()
  :precondition (and
    (evaluate comp287)
    (not (rebooted comp287))
    (not (running comp286))
  )
  :effect (and
    (not (evaluate comp287))
    (evaluate comp288)
    (one-off comp287)
  )
)
(:action evaluate-comp287-off-comp442
  :parameters ()
  :precondition (and
    (evaluate comp287)
    (not (rebooted comp287))
    (not (running comp442))
  )
  :effect (and
    (not (evaluate comp287))
    (evaluate comp288)
    (one-off comp287)
  )
)
(:action evaluate-comp288-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp288)
    (rebooted comp288)
  )
  :effect (and
    (not (evaluate comp288))
    (evaluate comp289)
  )
)
(:action evaluate-comp288-all-on
  :parameters ()
  :precondition (and
    (evaluate comp288)
    (not (rebooted comp288))
    (running comp3)
    (running comp287)
  )
  :effect (and
    (not (evaluate comp288))
    (evaluate comp289)
    (all-on comp288)
  )
)
(:action evaluate-comp288-off-comp3
  :parameters ()
  :precondition (and
    (evaluate comp288)
    (not (rebooted comp288))
    (not (running comp3))
  )
  :effect (and
    (not (evaluate comp288))
    (evaluate comp289)
    (one-off comp288)
  )
)
(:action evaluate-comp288-off-comp287
  :parameters ()
  :precondition (and
    (evaluate comp288)
    (not (rebooted comp288))
    (not (running comp287))
  )
  :effect (and
    (not (evaluate comp288))
    (evaluate comp289)
    (one-off comp288)
  )
)
(:action evaluate-comp289-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp289)
    (rebooted comp289)
  )
  :effect (and
    (not (evaluate comp289))
    (evaluate comp290)
  )
)
(:action evaluate-comp289-all-on
  :parameters ()
  :precondition (and
    (evaluate comp289)
    (not (rebooted comp289))
    (running comp81)
    (running comp288)
    (running comp360)
  )
  :effect (and
    (not (evaluate comp289))
    (evaluate comp290)
    (all-on comp289)
  )
)
(:action evaluate-comp289-off-comp81
  :parameters ()
  :precondition (and
    (evaluate comp289)
    (not (rebooted comp289))
    (not (running comp81))
  )
  :effect (and
    (not (evaluate comp289))
    (evaluate comp290)
    (one-off comp289)
  )
)
(:action evaluate-comp289-off-comp288
  :parameters ()
  :precondition (and
    (evaluate comp289)
    (not (rebooted comp289))
    (not (running comp288))
  )
  :effect (and
    (not (evaluate comp289))
    (evaluate comp290)
    (one-off comp289)
  )
)
(:action evaluate-comp289-off-comp360
  :parameters ()
  :precondition (and
    (evaluate comp289)
    (not (rebooted comp289))
    (not (running comp360))
  )
  :effect (and
    (not (evaluate comp289))
    (evaluate comp290)
    (one-off comp289)
  )
)
(:action evaluate-comp290-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp290)
    (rebooted comp290)
  )
  :effect (and
    (not (evaluate comp290))
    (evaluate comp291)
  )
)
(:action evaluate-comp290-all-on
  :parameters ()
  :precondition (and
    (evaluate comp290)
    (not (rebooted comp290))
    (running comp76)
    (running comp289)
  )
  :effect (and
    (not (evaluate comp290))
    (evaluate comp291)
    (all-on comp290)
  )
)
(:action evaluate-comp290-off-comp76
  :parameters ()
  :precondition (and
    (evaluate comp290)
    (not (rebooted comp290))
    (not (running comp76))
  )
  :effect (and
    (not (evaluate comp290))
    (evaluate comp291)
    (one-off comp290)
  )
)
(:action evaluate-comp290-off-comp289
  :parameters ()
  :precondition (and
    (evaluate comp290)
    (not (rebooted comp290))
    (not (running comp289))
  )
  :effect (and
    (not (evaluate comp290))
    (evaluate comp291)
    (one-off comp290)
  )
)
(:action evaluate-comp291-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp291)
    (rebooted comp291)
  )
  :effect (and
    (not (evaluate comp291))
    (evaluate comp292)
  )
)
(:action evaluate-comp291-all-on
  :parameters ()
  :precondition (and
    (evaluate comp291)
    (not (rebooted comp291))
    (running comp290)
    (running comp405)
  )
  :effect (and
    (not (evaluate comp291))
    (evaluate comp292)
    (all-on comp291)
  )
)
(:action evaluate-comp291-off-comp290
  :parameters ()
  :precondition (and
    (evaluate comp291)
    (not (rebooted comp291))
    (not (running comp290))
  )
  :effect (and
    (not (evaluate comp291))
    (evaluate comp292)
    (one-off comp291)
  )
)
(:action evaluate-comp291-off-comp405
  :parameters ()
  :precondition (and
    (evaluate comp291)
    (not (rebooted comp291))
    (not (running comp405))
  )
  :effect (and
    (not (evaluate comp291))
    (evaluate comp292)
    (one-off comp291)
  )
)
(:action evaluate-comp292-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp292)
    (rebooted comp292)
  )
  :effect (and
    (not (evaluate comp292))
    (evaluate comp293)
  )
)
(:action evaluate-comp292-all-on
  :parameters ()
  :precondition (and
    (evaluate comp292)
    (not (rebooted comp292))
    (running comp29)
    (running comp183)
    (running comp291)
  )
  :effect (and
    (not (evaluate comp292))
    (evaluate comp293)
    (all-on comp292)
  )
)
(:action evaluate-comp292-off-comp29
  :parameters ()
  :precondition (and
    (evaluate comp292)
    (not (rebooted comp292))
    (not (running comp29))
  )
  :effect (and
    (not (evaluate comp292))
    (evaluate comp293)
    (one-off comp292)
  )
)
(:action evaluate-comp292-off-comp183
  :parameters ()
  :precondition (and
    (evaluate comp292)
    (not (rebooted comp292))
    (not (running comp183))
  )
  :effect (and
    (not (evaluate comp292))
    (evaluate comp293)
    (one-off comp292)
  )
)
(:action evaluate-comp292-off-comp291
  :parameters ()
  :precondition (and
    (evaluate comp292)
    (not (rebooted comp292))
    (not (running comp291))
  )
  :effect (and
    (not (evaluate comp292))
    (evaluate comp293)
    (one-off comp292)
  )
)
(:action evaluate-comp293-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp293)
    (rebooted comp293)
  )
  :effect (and
    (not (evaluate comp293))
    (evaluate comp294)
  )
)
(:action evaluate-comp293-all-on
  :parameters ()
  :precondition (and
    (evaluate comp293)
    (not (rebooted comp293))
    (running comp281)
    (running comp292)
  )
  :effect (and
    (not (evaluate comp293))
    (evaluate comp294)
    (all-on comp293)
  )
)
(:action evaluate-comp293-off-comp281
  :parameters ()
  :precondition (and
    (evaluate comp293)
    (not (rebooted comp293))
    (not (running comp281))
  )
  :effect (and
    (not (evaluate comp293))
    (evaluate comp294)
    (one-off comp293)
  )
)
(:action evaluate-comp293-off-comp292
  :parameters ()
  :precondition (and
    (evaluate comp293)
    (not (rebooted comp293))
    (not (running comp292))
  )
  :effect (and
    (not (evaluate comp293))
    (evaluate comp294)
    (one-off comp293)
  )
)
(:action evaluate-comp294-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp294)
    (rebooted comp294)
  )
  :effect (and
    (not (evaluate comp294))
    (evaluate comp295)
  )
)
(:action evaluate-comp294-all-on
  :parameters ()
  :precondition (and
    (evaluate comp294)
    (not (rebooted comp294))
    (running comp293)
  )
  :effect (and
    (not (evaluate comp294))
    (evaluate comp295)
    (all-on comp294)
  )
)
(:action evaluate-comp294-off-comp293
  :parameters ()
  :precondition (and
    (evaluate comp294)
    (not (rebooted comp294))
    (not (running comp293))
  )
  :effect (and
    (not (evaluate comp294))
    (evaluate comp295)
    (one-off comp294)
  )
)
(:action evaluate-comp295-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp295)
    (rebooted comp295)
  )
  :effect (and
    (not (evaluate comp295))
    (evaluate comp296)
  )
)
(:action evaluate-comp295-all-on
  :parameters ()
  :precondition (and
    (evaluate comp295)
    (not (rebooted comp295))
    (running comp294)
    (running comp359)
  )
  :effect (and
    (not (evaluate comp295))
    (evaluate comp296)
    (all-on comp295)
  )
)
(:action evaluate-comp295-off-comp294
  :parameters ()
  :precondition (and
    (evaluate comp295)
    (not (rebooted comp295))
    (not (running comp294))
  )
  :effect (and
    (not (evaluate comp295))
    (evaluate comp296)
    (one-off comp295)
  )
)
(:action evaluate-comp295-off-comp359
  :parameters ()
  :precondition (and
    (evaluate comp295)
    (not (rebooted comp295))
    (not (running comp359))
  )
  :effect (and
    (not (evaluate comp295))
    (evaluate comp296)
    (one-off comp295)
  )
)
(:action evaluate-comp296-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp296)
    (rebooted comp296)
  )
  :effect (and
    (not (evaluate comp296))
    (evaluate comp297)
  )
)
(:action evaluate-comp296-all-on
  :parameters ()
  :precondition (and
    (evaluate comp296)
    (not (rebooted comp296))
    (running comp221)
    (running comp295)
  )
  :effect (and
    (not (evaluate comp296))
    (evaluate comp297)
    (all-on comp296)
  )
)
(:action evaluate-comp296-off-comp221
  :parameters ()
  :precondition (and
    (evaluate comp296)
    (not (rebooted comp296))
    (not (running comp221))
  )
  :effect (and
    (not (evaluate comp296))
    (evaluate comp297)
    (one-off comp296)
  )
)
(:action evaluate-comp296-off-comp295
  :parameters ()
  :precondition (and
    (evaluate comp296)
    (not (rebooted comp296))
    (not (running comp295))
  )
  :effect (and
    (not (evaluate comp296))
    (evaluate comp297)
    (one-off comp296)
  )
)
(:action evaluate-comp297-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp297)
    (rebooted comp297)
  )
  :effect (and
    (not (evaluate comp297))
    (evaluate comp298)
  )
)
(:action evaluate-comp297-all-on
  :parameters ()
  :precondition (and
    (evaluate comp297)
    (not (rebooted comp297))
    (running comp296)
  )
  :effect (and
    (not (evaluate comp297))
    (evaluate comp298)
    (all-on comp297)
  )
)
(:action evaluate-comp297-off-comp296
  :parameters ()
  :precondition (and
    (evaluate comp297)
    (not (rebooted comp297))
    (not (running comp296))
  )
  :effect (and
    (not (evaluate comp297))
    (evaluate comp298)
    (one-off comp297)
  )
)
(:action evaluate-comp298-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp298)
    (rebooted comp298)
  )
  :effect (and
    (not (evaluate comp298))
    (evaluate comp299)
  )
)
(:action evaluate-comp298-all-on
  :parameters ()
  :precondition (and
    (evaluate comp298)
    (not (rebooted comp298))
    (running comp35)
    (running comp297)
  )
  :effect (and
    (not (evaluate comp298))
    (evaluate comp299)
    (all-on comp298)
  )
)
(:action evaluate-comp298-off-comp35
  :parameters ()
  :precondition (and
    (evaluate comp298)
    (not (rebooted comp298))
    (not (running comp35))
  )
  :effect (and
    (not (evaluate comp298))
    (evaluate comp299)
    (one-off comp298)
  )
)
(:action evaluate-comp298-off-comp297
  :parameters ()
  :precondition (and
    (evaluate comp298)
    (not (rebooted comp298))
    (not (running comp297))
  )
  :effect (and
    (not (evaluate comp298))
    (evaluate comp299)
    (one-off comp298)
  )
)
(:action evaluate-comp299-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp299)
    (rebooted comp299)
  )
  :effect (and
    (not (evaluate comp299))
    (evaluate comp300)
  )
)
(:action evaluate-comp299-all-on
  :parameters ()
  :precondition (and
    (evaluate comp299)
    (not (rebooted comp299))
    (running comp298)
  )
  :effect (and
    (not (evaluate comp299))
    (evaluate comp300)
    (all-on comp299)
  )
)
(:action evaluate-comp299-off-comp298
  :parameters ()
  :precondition (and
    (evaluate comp299)
    (not (rebooted comp299))
    (not (running comp298))
  )
  :effect (and
    (not (evaluate comp299))
    (evaluate comp300)
    (one-off comp299)
  )
)
(:action evaluate-comp300-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp300)
    (rebooted comp300)
  )
  :effect (and
    (not (evaluate comp300))
    (evaluate comp301)
  )
)
(:action evaluate-comp300-all-on
  :parameters ()
  :precondition (and
    (evaluate comp300)
    (not (rebooted comp300))
    (running comp299)
  )
  :effect (and
    (not (evaluate comp300))
    (evaluate comp301)
    (all-on comp300)
  )
)
(:action evaluate-comp300-off-comp299
  :parameters ()
  :precondition (and
    (evaluate comp300)
    (not (rebooted comp300))
    (not (running comp299))
  )
  :effect (and
    (not (evaluate comp300))
    (evaluate comp301)
    (one-off comp300)
  )
)
(:action evaluate-comp301-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp301)
    (rebooted comp301)
  )
  :effect (and
    (not (evaluate comp301))
    (evaluate comp302)
  )
)
(:action evaluate-comp301-all-on
  :parameters ()
  :precondition (and
    (evaluate comp301)
    (not (rebooted comp301))
    (running comp3)
    (running comp300)
  )
  :effect (and
    (not (evaluate comp301))
    (evaluate comp302)
    (all-on comp301)
  )
)
(:action evaluate-comp301-off-comp3
  :parameters ()
  :precondition (and
    (evaluate comp301)
    (not (rebooted comp301))
    (not (running comp3))
  )
  :effect (and
    (not (evaluate comp301))
    (evaluate comp302)
    (one-off comp301)
  )
)
(:action evaluate-comp301-off-comp300
  :parameters ()
  :precondition (and
    (evaluate comp301)
    (not (rebooted comp301))
    (not (running comp300))
  )
  :effect (and
    (not (evaluate comp301))
    (evaluate comp302)
    (one-off comp301)
  )
)
(:action evaluate-comp302-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp302)
    (rebooted comp302)
  )
  :effect (and
    (not (evaluate comp302))
    (evaluate comp303)
  )
)
(:action evaluate-comp302-all-on
  :parameters ()
  :precondition (and
    (evaluate comp302)
    (not (rebooted comp302))
    (running comp90)
    (running comp301)
    (running comp329)
  )
  :effect (and
    (not (evaluate comp302))
    (evaluate comp303)
    (all-on comp302)
  )
)
(:action evaluate-comp302-off-comp90
  :parameters ()
  :precondition (and
    (evaluate comp302)
    (not (rebooted comp302))
    (not (running comp90))
  )
  :effect (and
    (not (evaluate comp302))
    (evaluate comp303)
    (one-off comp302)
  )
)
(:action evaluate-comp302-off-comp301
  :parameters ()
  :precondition (and
    (evaluate comp302)
    (not (rebooted comp302))
    (not (running comp301))
  )
  :effect (and
    (not (evaluate comp302))
    (evaluate comp303)
    (one-off comp302)
  )
)
(:action evaluate-comp302-off-comp329
  :parameters ()
  :precondition (and
    (evaluate comp302)
    (not (rebooted comp302))
    (not (running comp329))
  )
  :effect (and
    (not (evaluate comp302))
    (evaluate comp303)
    (one-off comp302)
  )
)
(:action evaluate-comp303-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp303)
    (rebooted comp303)
  )
  :effect (and
    (not (evaluate comp303))
    (evaluate comp304)
  )
)
(:action evaluate-comp303-all-on
  :parameters ()
  :precondition (and
    (evaluate comp303)
    (not (rebooted comp303))
    (running comp302)
    (running comp308)
  )
  :effect (and
    (not (evaluate comp303))
    (evaluate comp304)
    (all-on comp303)
  )
)
(:action evaluate-comp303-off-comp302
  :parameters ()
  :precondition (and
    (evaluate comp303)
    (not (rebooted comp303))
    (not (running comp302))
  )
  :effect (and
    (not (evaluate comp303))
    (evaluate comp304)
    (one-off comp303)
  )
)
(:action evaluate-comp303-off-comp308
  :parameters ()
  :precondition (and
    (evaluate comp303)
    (not (rebooted comp303))
    (not (running comp308))
  )
  :effect (and
    (not (evaluate comp303))
    (evaluate comp304)
    (one-off comp303)
  )
)
(:action evaluate-comp304-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp304)
    (rebooted comp304)
  )
  :effect (and
    (not (evaluate comp304))
    (evaluate comp305)
  )
)
(:action evaluate-comp304-all-on
  :parameters ()
  :precondition (and
    (evaluate comp304)
    (not (rebooted comp304))
    (running comp303)
  )
  :effect (and
    (not (evaluate comp304))
    (evaluate comp305)
    (all-on comp304)
  )
)
(:action evaluate-comp304-off-comp303
  :parameters ()
  :precondition (and
    (evaluate comp304)
    (not (rebooted comp304))
    (not (running comp303))
  )
  :effect (and
    (not (evaluate comp304))
    (evaluate comp305)
    (one-off comp304)
  )
)
(:action evaluate-comp305-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp305)
    (rebooted comp305)
  )
  :effect (and
    (not (evaluate comp305))
    (evaluate comp306)
  )
)
(:action evaluate-comp305-all-on
  :parameters ()
  :precondition (and
    (evaluate comp305)
    (not (rebooted comp305))
    (running comp304)
  )
  :effect (and
    (not (evaluate comp305))
    (evaluate comp306)
    (all-on comp305)
  )
)
(:action evaluate-comp305-off-comp304
  :parameters ()
  :precondition (and
    (evaluate comp305)
    (not (rebooted comp305))
    (not (running comp304))
  )
  :effect (and
    (not (evaluate comp305))
    (evaluate comp306)
    (one-off comp305)
  )
)
(:action evaluate-comp306-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp306)
    (rebooted comp306)
  )
  :effect (and
    (not (evaluate comp306))
    (evaluate comp307)
  )
)
(:action evaluate-comp306-all-on
  :parameters ()
  :precondition (and
    (evaluate comp306)
    (not (rebooted comp306))
    (running comp305)
  )
  :effect (and
    (not (evaluate comp306))
    (evaluate comp307)
    (all-on comp306)
  )
)
(:action evaluate-comp306-off-comp305
  :parameters ()
  :precondition (and
    (evaluate comp306)
    (not (rebooted comp306))
    (not (running comp305))
  )
  :effect (and
    (not (evaluate comp306))
    (evaluate comp307)
    (one-off comp306)
  )
)
(:action evaluate-comp307-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp307)
    (rebooted comp307)
  )
  :effect (and
    (not (evaluate comp307))
    (evaluate comp308)
  )
)
(:action evaluate-comp307-all-on
  :parameters ()
  :precondition (and
    (evaluate comp307)
    (not (rebooted comp307))
    (running comp306)
  )
  :effect (and
    (not (evaluate comp307))
    (evaluate comp308)
    (all-on comp307)
  )
)
(:action evaluate-comp307-off-comp306
  :parameters ()
  :precondition (and
    (evaluate comp307)
    (not (rebooted comp307))
    (not (running comp306))
  )
  :effect (and
    (not (evaluate comp307))
    (evaluate comp308)
    (one-off comp307)
  )
)
(:action evaluate-comp308-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp308)
    (rebooted comp308)
  )
  :effect (and
    (not (evaluate comp308))
    (evaluate comp309)
  )
)
(:action evaluate-comp308-all-on
  :parameters ()
  :precondition (and
    (evaluate comp308)
    (not (rebooted comp308))
    (running comp307)
    (running comp466)
    (running comp467)
  )
  :effect (and
    (not (evaluate comp308))
    (evaluate comp309)
    (all-on comp308)
  )
)
(:action evaluate-comp308-off-comp307
  :parameters ()
  :precondition (and
    (evaluate comp308)
    (not (rebooted comp308))
    (not (running comp307))
  )
  :effect (and
    (not (evaluate comp308))
    (evaluate comp309)
    (one-off comp308)
  )
)
(:action evaluate-comp308-off-comp466
  :parameters ()
  :precondition (and
    (evaluate comp308)
    (not (rebooted comp308))
    (not (running comp466))
  )
  :effect (and
    (not (evaluate comp308))
    (evaluate comp309)
    (one-off comp308)
  )
)
(:action evaluate-comp308-off-comp467
  :parameters ()
  :precondition (and
    (evaluate comp308)
    (not (rebooted comp308))
    (not (running comp467))
  )
  :effect (and
    (not (evaluate comp308))
    (evaluate comp309)
    (one-off comp308)
  )
)
(:action evaluate-comp309-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp309)
    (rebooted comp309)
  )
  :effect (and
    (not (evaluate comp309))
    (evaluate comp310)
  )
)
(:action evaluate-comp309-all-on
  :parameters ()
  :precondition (and
    (evaluate comp309)
    (not (rebooted comp309))
    (running comp114)
    (running comp308)
  )
  :effect (and
    (not (evaluate comp309))
    (evaluate comp310)
    (all-on comp309)
  )
)
(:action evaluate-comp309-off-comp114
  :parameters ()
  :precondition (and
    (evaluate comp309)
    (not (rebooted comp309))
    (not (running comp114))
  )
  :effect (and
    (not (evaluate comp309))
    (evaluate comp310)
    (one-off comp309)
  )
)
(:action evaluate-comp309-off-comp308
  :parameters ()
  :precondition (and
    (evaluate comp309)
    (not (rebooted comp309))
    (not (running comp308))
  )
  :effect (and
    (not (evaluate comp309))
    (evaluate comp310)
    (one-off comp309)
  )
)
(:action evaluate-comp310-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp310)
    (rebooted comp310)
  )
  :effect (and
    (not (evaluate comp310))
    (evaluate comp311)
  )
)
(:action evaluate-comp310-all-on
  :parameters ()
  :precondition (and
    (evaluate comp310)
    (not (rebooted comp310))
    (running comp309)
  )
  :effect (and
    (not (evaluate comp310))
    (evaluate comp311)
    (all-on comp310)
  )
)
(:action evaluate-comp310-off-comp309
  :parameters ()
  :precondition (and
    (evaluate comp310)
    (not (rebooted comp310))
    (not (running comp309))
  )
  :effect (and
    (not (evaluate comp310))
    (evaluate comp311)
    (one-off comp310)
  )
)
(:action evaluate-comp311-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp311)
    (rebooted comp311)
  )
  :effect (and
    (not (evaluate comp311))
    (evaluate comp312)
  )
)
(:action evaluate-comp311-all-on
  :parameters ()
  :precondition (and
    (evaluate comp311)
    (not (rebooted comp311))
    (running comp310)
  )
  :effect (and
    (not (evaluate comp311))
    (evaluate comp312)
    (all-on comp311)
  )
)
(:action evaluate-comp311-off-comp310
  :parameters ()
  :precondition (and
    (evaluate comp311)
    (not (rebooted comp311))
    (not (running comp310))
  )
  :effect (and
    (not (evaluate comp311))
    (evaluate comp312)
    (one-off comp311)
  )
)
(:action evaluate-comp312-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp312)
    (rebooted comp312)
  )
  :effect (and
    (not (evaluate comp312))
    (evaluate comp313)
  )
)
(:action evaluate-comp312-all-on
  :parameters ()
  :precondition (and
    (evaluate comp312)
    (not (rebooted comp312))
    (running comp311)
  )
  :effect (and
    (not (evaluate comp312))
    (evaluate comp313)
    (all-on comp312)
  )
)
(:action evaluate-comp312-off-comp311
  :parameters ()
  :precondition (and
    (evaluate comp312)
    (not (rebooted comp312))
    (not (running comp311))
  )
  :effect (and
    (not (evaluate comp312))
    (evaluate comp313)
    (one-off comp312)
  )
)
(:action evaluate-comp313-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp313)
    (rebooted comp313)
  )
  :effect (and
    (not (evaluate comp313))
    (evaluate comp314)
  )
)
(:action evaluate-comp313-all-on
  :parameters ()
  :precondition (and
    (evaluate comp313)
    (not (rebooted comp313))
    (running comp312)
  )
  :effect (and
    (not (evaluate comp313))
    (evaluate comp314)
    (all-on comp313)
  )
)
(:action evaluate-comp313-off-comp312
  :parameters ()
  :precondition (and
    (evaluate comp313)
    (not (rebooted comp313))
    (not (running comp312))
  )
  :effect (and
    (not (evaluate comp313))
    (evaluate comp314)
    (one-off comp313)
  )
)
(:action evaluate-comp314-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp314)
    (rebooted comp314)
  )
  :effect (and
    (not (evaluate comp314))
    (evaluate comp315)
  )
)
(:action evaluate-comp314-all-on
  :parameters ()
  :precondition (and
    (evaluate comp314)
    (not (rebooted comp314))
    (running comp313)
  )
  :effect (and
    (not (evaluate comp314))
    (evaluate comp315)
    (all-on comp314)
  )
)
(:action evaluate-comp314-off-comp313
  :parameters ()
  :precondition (and
    (evaluate comp314)
    (not (rebooted comp314))
    (not (running comp313))
  )
  :effect (and
    (not (evaluate comp314))
    (evaluate comp315)
    (one-off comp314)
  )
)
(:action evaluate-comp315-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp315)
    (rebooted comp315)
  )
  :effect (and
    (not (evaluate comp315))
    (evaluate comp316)
  )
)
(:action evaluate-comp315-all-on
  :parameters ()
  :precondition (and
    (evaluate comp315)
    (not (rebooted comp315))
    (running comp314)
  )
  :effect (and
    (not (evaluate comp315))
    (evaluate comp316)
    (all-on comp315)
  )
)
(:action evaluate-comp315-off-comp314
  :parameters ()
  :precondition (and
    (evaluate comp315)
    (not (rebooted comp315))
    (not (running comp314))
  )
  :effect (and
    (not (evaluate comp315))
    (evaluate comp316)
    (one-off comp315)
  )
)
(:action evaluate-comp316-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp316)
    (rebooted comp316)
  )
  :effect (and
    (not (evaluate comp316))
    (evaluate comp317)
  )
)
(:action evaluate-comp316-all-on
  :parameters ()
  :precondition (and
    (evaluate comp316)
    (not (rebooted comp316))
    (running comp315)
  )
  :effect (and
    (not (evaluate comp316))
    (evaluate comp317)
    (all-on comp316)
  )
)
(:action evaluate-comp316-off-comp315
  :parameters ()
  :precondition (and
    (evaluate comp316)
    (not (rebooted comp316))
    (not (running comp315))
  )
  :effect (and
    (not (evaluate comp316))
    (evaluate comp317)
    (one-off comp316)
  )
)
(:action evaluate-comp317-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp317)
    (rebooted comp317)
  )
  :effect (and
    (not (evaluate comp317))
    (evaluate comp318)
  )
)
(:action evaluate-comp317-all-on
  :parameters ()
  :precondition (and
    (evaluate comp317)
    (not (rebooted comp317))
    (running comp316)
    (running comp378)
  )
  :effect (and
    (not (evaluate comp317))
    (evaluate comp318)
    (all-on comp317)
  )
)
(:action evaluate-comp317-off-comp316
  :parameters ()
  :precondition (and
    (evaluate comp317)
    (not (rebooted comp317))
    (not (running comp316))
  )
  :effect (and
    (not (evaluate comp317))
    (evaluate comp318)
    (one-off comp317)
  )
)
(:action evaluate-comp317-off-comp378
  :parameters ()
  :precondition (and
    (evaluate comp317)
    (not (rebooted comp317))
    (not (running comp378))
  )
  :effect (and
    (not (evaluate comp317))
    (evaluate comp318)
    (one-off comp317)
  )
)
(:action evaluate-comp318-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp318)
    (rebooted comp318)
  )
  :effect (and
    (not (evaluate comp318))
    (evaluate comp319)
  )
)
(:action evaluate-comp318-all-on
  :parameters ()
  :precondition (and
    (evaluate comp318)
    (not (rebooted comp318))
    (running comp317)
  )
  :effect (and
    (not (evaluate comp318))
    (evaluate comp319)
    (all-on comp318)
  )
)
(:action evaluate-comp318-off-comp317
  :parameters ()
  :precondition (and
    (evaluate comp318)
    (not (rebooted comp318))
    (not (running comp317))
  )
  :effect (and
    (not (evaluate comp318))
    (evaluate comp319)
    (one-off comp318)
  )
)
(:action evaluate-comp319-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp319)
    (rebooted comp319)
  )
  :effect (and
    (not (evaluate comp319))
    (evaluate comp320)
  )
)
(:action evaluate-comp319-all-on
  :parameters ()
  :precondition (and
    (evaluate comp319)
    (not (rebooted comp319))
    (running comp318)
  )
  :effect (and
    (not (evaluate comp319))
    (evaluate comp320)
    (all-on comp319)
  )
)
(:action evaluate-comp319-off-comp318
  :parameters ()
  :precondition (and
    (evaluate comp319)
    (not (rebooted comp319))
    (not (running comp318))
  )
  :effect (and
    (not (evaluate comp319))
    (evaluate comp320)
    (one-off comp319)
  )
)
(:action evaluate-comp320-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp320)
    (rebooted comp320)
  )
  :effect (and
    (not (evaluate comp320))
    (evaluate comp321)
  )
)
(:action evaluate-comp320-all-on
  :parameters ()
  :precondition (and
    (evaluate comp320)
    (not (rebooted comp320))
    (running comp319)
  )
  :effect (and
    (not (evaluate comp320))
    (evaluate comp321)
    (all-on comp320)
  )
)
(:action evaluate-comp320-off-comp319
  :parameters ()
  :precondition (and
    (evaluate comp320)
    (not (rebooted comp320))
    (not (running comp319))
  )
  :effect (and
    (not (evaluate comp320))
    (evaluate comp321)
    (one-off comp320)
  )
)
(:action evaluate-comp321-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp321)
    (rebooted comp321)
  )
  :effect (and
    (not (evaluate comp321))
    (evaluate comp322)
  )
)
(:action evaluate-comp321-all-on
  :parameters ()
  :precondition (and
    (evaluate comp321)
    (not (rebooted comp321))
    (running comp113)
    (running comp320)
  )
  :effect (and
    (not (evaluate comp321))
    (evaluate comp322)
    (all-on comp321)
  )
)
(:action evaluate-comp321-off-comp113
  :parameters ()
  :precondition (and
    (evaluate comp321)
    (not (rebooted comp321))
    (not (running comp113))
  )
  :effect (and
    (not (evaluate comp321))
    (evaluate comp322)
    (one-off comp321)
  )
)
(:action evaluate-comp321-off-comp320
  :parameters ()
  :precondition (and
    (evaluate comp321)
    (not (rebooted comp321))
    (not (running comp320))
  )
  :effect (and
    (not (evaluate comp321))
    (evaluate comp322)
    (one-off comp321)
  )
)
(:action evaluate-comp322-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp322)
    (rebooted comp322)
  )
  :effect (and
    (not (evaluate comp322))
    (evaluate comp323)
  )
)
(:action evaluate-comp322-all-on
  :parameters ()
  :precondition (and
    (evaluate comp322)
    (not (rebooted comp322))
    (running comp321)
  )
  :effect (and
    (not (evaluate comp322))
    (evaluate comp323)
    (all-on comp322)
  )
)
(:action evaluate-comp322-off-comp321
  :parameters ()
  :precondition (and
    (evaluate comp322)
    (not (rebooted comp322))
    (not (running comp321))
  )
  :effect (and
    (not (evaluate comp322))
    (evaluate comp323)
    (one-off comp322)
  )
)
(:action evaluate-comp323-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp323)
    (rebooted comp323)
  )
  :effect (and
    (not (evaluate comp323))
    (evaluate comp324)
  )
)
(:action evaluate-comp323-all-on
  :parameters ()
  :precondition (and
    (evaluate comp323)
    (not (rebooted comp323))
    (running comp275)
    (running comp322)
  )
  :effect (and
    (not (evaluate comp323))
    (evaluate comp324)
    (all-on comp323)
  )
)
(:action evaluate-comp323-off-comp275
  :parameters ()
  :precondition (and
    (evaluate comp323)
    (not (rebooted comp323))
    (not (running comp275))
  )
  :effect (and
    (not (evaluate comp323))
    (evaluate comp324)
    (one-off comp323)
  )
)
(:action evaluate-comp323-off-comp322
  :parameters ()
  :precondition (and
    (evaluate comp323)
    (not (rebooted comp323))
    (not (running comp322))
  )
  :effect (and
    (not (evaluate comp323))
    (evaluate comp324)
    (one-off comp323)
  )
)
(:action evaluate-comp324-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp324)
    (rebooted comp324)
  )
  :effect (and
    (not (evaluate comp324))
    (evaluate comp325)
  )
)
(:action evaluate-comp324-all-on
  :parameters ()
  :precondition (and
    (evaluate comp324)
    (not (rebooted comp324))
    (running comp323)
  )
  :effect (and
    (not (evaluate comp324))
    (evaluate comp325)
    (all-on comp324)
  )
)
(:action evaluate-comp324-off-comp323
  :parameters ()
  :precondition (and
    (evaluate comp324)
    (not (rebooted comp324))
    (not (running comp323))
  )
  :effect (and
    (not (evaluate comp324))
    (evaluate comp325)
    (one-off comp324)
  )
)
(:action evaluate-comp325-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp325)
    (rebooted comp325)
  )
  :effect (and
    (not (evaluate comp325))
    (evaluate comp326)
  )
)
(:action evaluate-comp325-all-on
  :parameters ()
  :precondition (and
    (evaluate comp325)
    (not (rebooted comp325))
    (running comp47)
    (running comp324)
  )
  :effect (and
    (not (evaluate comp325))
    (evaluate comp326)
    (all-on comp325)
  )
)
(:action evaluate-comp325-off-comp47
  :parameters ()
  :precondition (and
    (evaluate comp325)
    (not (rebooted comp325))
    (not (running comp47))
  )
  :effect (and
    (not (evaluate comp325))
    (evaluate comp326)
    (one-off comp325)
  )
)
(:action evaluate-comp325-off-comp324
  :parameters ()
  :precondition (and
    (evaluate comp325)
    (not (rebooted comp325))
    (not (running comp324))
  )
  :effect (and
    (not (evaluate comp325))
    (evaluate comp326)
    (one-off comp325)
  )
)
(:action evaluate-comp326-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp326)
    (rebooted comp326)
  )
  :effect (and
    (not (evaluate comp326))
    (evaluate comp327)
  )
)
(:action evaluate-comp326-all-on
  :parameters ()
  :precondition (and
    (evaluate comp326)
    (not (rebooted comp326))
    (running comp325)
  )
  :effect (and
    (not (evaluate comp326))
    (evaluate comp327)
    (all-on comp326)
  )
)
(:action evaluate-comp326-off-comp325
  :parameters ()
  :precondition (and
    (evaluate comp326)
    (not (rebooted comp326))
    (not (running comp325))
  )
  :effect (and
    (not (evaluate comp326))
    (evaluate comp327)
    (one-off comp326)
  )
)
(:action evaluate-comp327-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp327)
    (rebooted comp327)
  )
  :effect (and
    (not (evaluate comp327))
    (evaluate comp328)
  )
)
(:action evaluate-comp327-all-on
  :parameters ()
  :precondition (and
    (evaluate comp327)
    (not (rebooted comp327))
    (running comp326)
  )
  :effect (and
    (not (evaluate comp327))
    (evaluate comp328)
    (all-on comp327)
  )
)
(:action evaluate-comp327-off-comp326
  :parameters ()
  :precondition (and
    (evaluate comp327)
    (not (rebooted comp327))
    (not (running comp326))
  )
  :effect (and
    (not (evaluate comp327))
    (evaluate comp328)
    (one-off comp327)
  )
)
(:action evaluate-comp328-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp328)
    (rebooted comp328)
  )
  :effect (and
    (not (evaluate comp328))
    (evaluate comp329)
  )
)
(:action evaluate-comp328-all-on
  :parameters ()
  :precondition (and
    (evaluate comp328)
    (not (rebooted comp328))
    (running comp327)
    (running comp456)
  )
  :effect (and
    (not (evaluate comp328))
    (evaluate comp329)
    (all-on comp328)
  )
)
(:action evaluate-comp328-off-comp327
  :parameters ()
  :precondition (and
    (evaluate comp328)
    (not (rebooted comp328))
    (not (running comp327))
  )
  :effect (and
    (not (evaluate comp328))
    (evaluate comp329)
    (one-off comp328)
  )
)
(:action evaluate-comp328-off-comp456
  :parameters ()
  :precondition (and
    (evaluate comp328)
    (not (rebooted comp328))
    (not (running comp456))
  )
  :effect (and
    (not (evaluate comp328))
    (evaluate comp329)
    (one-off comp328)
  )
)
(:action evaluate-comp329-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp329)
    (rebooted comp329)
  )
  :effect (and
    (not (evaluate comp329))
    (evaluate comp330)
  )
)
(:action evaluate-comp329-all-on
  :parameters ()
  :precondition (and
    (evaluate comp329)
    (not (rebooted comp329))
    (running comp328)
  )
  :effect (and
    (not (evaluate comp329))
    (evaluate comp330)
    (all-on comp329)
  )
)
(:action evaluate-comp329-off-comp328
  :parameters ()
  :precondition (and
    (evaluate comp329)
    (not (rebooted comp329))
    (not (running comp328))
  )
  :effect (and
    (not (evaluate comp329))
    (evaluate comp330)
    (one-off comp329)
  )
)
(:action evaluate-comp330-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp330)
    (rebooted comp330)
  )
  :effect (and
    (not (evaluate comp330))
    (evaluate comp331)
  )
)
(:action evaluate-comp330-all-on
  :parameters ()
  :precondition (and
    (evaluate comp330)
    (not (rebooted comp330))
    (running comp329)
  )
  :effect (and
    (not (evaluate comp330))
    (evaluate comp331)
    (all-on comp330)
  )
)
(:action evaluate-comp330-off-comp329
  :parameters ()
  :precondition (and
    (evaluate comp330)
    (not (rebooted comp330))
    (not (running comp329))
  )
  :effect (and
    (not (evaluate comp330))
    (evaluate comp331)
    (one-off comp330)
  )
)
(:action evaluate-comp331-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp331)
    (rebooted comp331)
  )
  :effect (and
    (not (evaluate comp331))
    (evaluate comp332)
  )
)
(:action evaluate-comp331-all-on
  :parameters ()
  :precondition (and
    (evaluate comp331)
    (not (rebooted comp331))
    (running comp290)
    (running comp330)
  )
  :effect (and
    (not (evaluate comp331))
    (evaluate comp332)
    (all-on comp331)
  )
)
(:action evaluate-comp331-off-comp290
  :parameters ()
  :precondition (and
    (evaluate comp331)
    (not (rebooted comp331))
    (not (running comp290))
  )
  :effect (and
    (not (evaluate comp331))
    (evaluate comp332)
    (one-off comp331)
  )
)
(:action evaluate-comp331-off-comp330
  :parameters ()
  :precondition (and
    (evaluate comp331)
    (not (rebooted comp331))
    (not (running comp330))
  )
  :effect (and
    (not (evaluate comp331))
    (evaluate comp332)
    (one-off comp331)
  )
)
(:action evaluate-comp332-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp332)
    (rebooted comp332)
  )
  :effect (and
    (not (evaluate comp332))
    (evaluate comp333)
  )
)
(:action evaluate-comp332-all-on
  :parameters ()
  :precondition (and
    (evaluate comp332)
    (not (rebooted comp332))
    (running comp331)
  )
  :effect (and
    (not (evaluate comp332))
    (evaluate comp333)
    (all-on comp332)
  )
)
(:action evaluate-comp332-off-comp331
  :parameters ()
  :precondition (and
    (evaluate comp332)
    (not (rebooted comp332))
    (not (running comp331))
  )
  :effect (and
    (not (evaluate comp332))
    (evaluate comp333)
    (one-off comp332)
  )
)
(:action evaluate-comp333-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp333)
    (rebooted comp333)
  )
  :effect (and
    (not (evaluate comp333))
    (evaluate comp334)
  )
)
(:action evaluate-comp333-all-on
  :parameters ()
  :precondition (and
    (evaluate comp333)
    (not (rebooted comp333))
    (running comp332)
    (running comp478)
  )
  :effect (and
    (not (evaluate comp333))
    (evaluate comp334)
    (all-on comp333)
  )
)
(:action evaluate-comp333-off-comp332
  :parameters ()
  :precondition (and
    (evaluate comp333)
    (not (rebooted comp333))
    (not (running comp332))
  )
  :effect (and
    (not (evaluate comp333))
    (evaluate comp334)
    (one-off comp333)
  )
)
(:action evaluate-comp333-off-comp478
  :parameters ()
  :precondition (and
    (evaluate comp333)
    (not (rebooted comp333))
    (not (running comp478))
  )
  :effect (and
    (not (evaluate comp333))
    (evaluate comp334)
    (one-off comp333)
  )
)
(:action evaluate-comp334-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp334)
    (rebooted comp334)
  )
  :effect (and
    (not (evaluate comp334))
    (evaluate comp335)
  )
)
(:action evaluate-comp334-all-on
  :parameters ()
  :precondition (and
    (evaluate comp334)
    (not (rebooted comp334))
    (running comp333)
  )
  :effect (and
    (not (evaluate comp334))
    (evaluate comp335)
    (all-on comp334)
  )
)
(:action evaluate-comp334-off-comp333
  :parameters ()
  :precondition (and
    (evaluate comp334)
    (not (rebooted comp334))
    (not (running comp333))
  )
  :effect (and
    (not (evaluate comp334))
    (evaluate comp335)
    (one-off comp334)
  )
)
(:action evaluate-comp335-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp335)
    (rebooted comp335)
  )
  :effect (and
    (not (evaluate comp335))
    (evaluate comp336)
  )
)
(:action evaluate-comp335-all-on
  :parameters ()
  :precondition (and
    (evaluate comp335)
    (not (rebooted comp335))
    (running comp334)
  )
  :effect (and
    (not (evaluate comp335))
    (evaluate comp336)
    (all-on comp335)
  )
)
(:action evaluate-comp335-off-comp334
  :parameters ()
  :precondition (and
    (evaluate comp335)
    (not (rebooted comp335))
    (not (running comp334))
  )
  :effect (and
    (not (evaluate comp335))
    (evaluate comp336)
    (one-off comp335)
  )
)
(:action evaluate-comp336-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp336)
    (rebooted comp336)
  )
  :effect (and
    (not (evaluate comp336))
    (evaluate comp337)
  )
)
(:action evaluate-comp336-all-on
  :parameters ()
  :precondition (and
    (evaluate comp336)
    (not (rebooted comp336))
    (running comp335)
    (running comp371)
    (running comp459)
  )
  :effect (and
    (not (evaluate comp336))
    (evaluate comp337)
    (all-on comp336)
  )
)
(:action evaluate-comp336-off-comp335
  :parameters ()
  :precondition (and
    (evaluate comp336)
    (not (rebooted comp336))
    (not (running comp335))
  )
  :effect (and
    (not (evaluate comp336))
    (evaluate comp337)
    (one-off comp336)
  )
)
(:action evaluate-comp336-off-comp371
  :parameters ()
  :precondition (and
    (evaluate comp336)
    (not (rebooted comp336))
    (not (running comp371))
  )
  :effect (and
    (not (evaluate comp336))
    (evaluate comp337)
    (one-off comp336)
  )
)
(:action evaluate-comp336-off-comp459
  :parameters ()
  :precondition (and
    (evaluate comp336)
    (not (rebooted comp336))
    (not (running comp459))
  )
  :effect (and
    (not (evaluate comp336))
    (evaluate comp337)
    (one-off comp336)
  )
)
(:action evaluate-comp337-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp337)
    (rebooted comp337)
  )
  :effect (and
    (not (evaluate comp337))
    (evaluate comp338)
  )
)
(:action evaluate-comp337-all-on
  :parameters ()
  :precondition (and
    (evaluate comp337)
    (not (rebooted comp337))
    (running comp336)
  )
  :effect (and
    (not (evaluate comp337))
    (evaluate comp338)
    (all-on comp337)
  )
)
(:action evaluate-comp337-off-comp336
  :parameters ()
  :precondition (and
    (evaluate comp337)
    (not (rebooted comp337))
    (not (running comp336))
  )
  :effect (and
    (not (evaluate comp337))
    (evaluate comp338)
    (one-off comp337)
  )
)
(:action evaluate-comp338-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp338)
    (rebooted comp338)
  )
  :effect (and
    (not (evaluate comp338))
    (evaluate comp339)
  )
)
(:action evaluate-comp338-all-on
  :parameters ()
  :precondition (and
    (evaluate comp338)
    (not (rebooted comp338))
    (running comp337)
  )
  :effect (and
    (not (evaluate comp338))
    (evaluate comp339)
    (all-on comp338)
  )
)
(:action evaluate-comp338-off-comp337
  :parameters ()
  :precondition (and
    (evaluate comp338)
    (not (rebooted comp338))
    (not (running comp337))
  )
  :effect (and
    (not (evaluate comp338))
    (evaluate comp339)
    (one-off comp338)
  )
)
(:action evaluate-comp339-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp339)
    (rebooted comp339)
  )
  :effect (and
    (not (evaluate comp339))
    (evaluate comp340)
  )
)
(:action evaluate-comp339-all-on
  :parameters ()
  :precondition (and
    (evaluate comp339)
    (not (rebooted comp339))
    (running comp338)
  )
  :effect (and
    (not (evaluate comp339))
    (evaluate comp340)
    (all-on comp339)
  )
)
(:action evaluate-comp339-off-comp338
  :parameters ()
  :precondition (and
    (evaluate comp339)
    (not (rebooted comp339))
    (not (running comp338))
  )
  :effect (and
    (not (evaluate comp339))
    (evaluate comp340)
    (one-off comp339)
  )
)
(:action evaluate-comp340-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp340)
    (rebooted comp340)
  )
  :effect (and
    (not (evaluate comp340))
    (evaluate comp341)
  )
)
(:action evaluate-comp340-all-on
  :parameters ()
  :precondition (and
    (evaluate comp340)
    (not (rebooted comp340))
    (running comp339)
  )
  :effect (and
    (not (evaluate comp340))
    (evaluate comp341)
    (all-on comp340)
  )
)
(:action evaluate-comp340-off-comp339
  :parameters ()
  :precondition (and
    (evaluate comp340)
    (not (rebooted comp340))
    (not (running comp339))
  )
  :effect (and
    (not (evaluate comp340))
    (evaluate comp341)
    (one-off comp340)
  )
)
(:action evaluate-comp341-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp341)
    (rebooted comp341)
  )
  :effect (and
    (not (evaluate comp341))
    (evaluate comp342)
  )
)
(:action evaluate-comp341-all-on
  :parameters ()
  :precondition (and
    (evaluate comp341)
    (not (rebooted comp341))
    (running comp43)
    (running comp340)
  )
  :effect (and
    (not (evaluate comp341))
    (evaluate comp342)
    (all-on comp341)
  )
)
(:action evaluate-comp341-off-comp43
  :parameters ()
  :precondition (and
    (evaluate comp341)
    (not (rebooted comp341))
    (not (running comp43))
  )
  :effect (and
    (not (evaluate comp341))
    (evaluate comp342)
    (one-off comp341)
  )
)
(:action evaluate-comp341-off-comp340
  :parameters ()
  :precondition (and
    (evaluate comp341)
    (not (rebooted comp341))
    (not (running comp340))
  )
  :effect (and
    (not (evaluate comp341))
    (evaluate comp342)
    (one-off comp341)
  )
)
(:action evaluate-comp342-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp342)
    (rebooted comp342)
  )
  :effect (and
    (not (evaluate comp342))
    (evaluate comp343)
  )
)
(:action evaluate-comp342-all-on
  :parameters ()
  :precondition (and
    (evaluate comp342)
    (not (rebooted comp342))
    (running comp234)
    (running comp341)
    (running comp452)
  )
  :effect (and
    (not (evaluate comp342))
    (evaluate comp343)
    (all-on comp342)
  )
)
(:action evaluate-comp342-off-comp234
  :parameters ()
  :precondition (and
    (evaluate comp342)
    (not (rebooted comp342))
    (not (running comp234))
  )
  :effect (and
    (not (evaluate comp342))
    (evaluate comp343)
    (one-off comp342)
  )
)
(:action evaluate-comp342-off-comp341
  :parameters ()
  :precondition (and
    (evaluate comp342)
    (not (rebooted comp342))
    (not (running comp341))
  )
  :effect (and
    (not (evaluate comp342))
    (evaluate comp343)
    (one-off comp342)
  )
)
(:action evaluate-comp342-off-comp452
  :parameters ()
  :precondition (and
    (evaluate comp342)
    (not (rebooted comp342))
    (not (running comp452))
  )
  :effect (and
    (not (evaluate comp342))
    (evaluate comp343)
    (one-off comp342)
  )
)
(:action evaluate-comp343-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp343)
    (rebooted comp343)
  )
  :effect (and
    (not (evaluate comp343))
    (evaluate comp344)
  )
)
(:action evaluate-comp343-all-on
  :parameters ()
  :precondition (and
    (evaluate comp343)
    (not (rebooted comp343))
    (running comp342)
  )
  :effect (and
    (not (evaluate comp343))
    (evaluate comp344)
    (all-on comp343)
  )
)
(:action evaluate-comp343-off-comp342
  :parameters ()
  :precondition (and
    (evaluate comp343)
    (not (rebooted comp343))
    (not (running comp342))
  )
  :effect (and
    (not (evaluate comp343))
    (evaluate comp344)
    (one-off comp343)
  )
)
(:action evaluate-comp344-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp344)
    (rebooted comp344)
  )
  :effect (and
    (not (evaluate comp344))
    (evaluate comp345)
  )
)
(:action evaluate-comp344-all-on
  :parameters ()
  :precondition (and
    (evaluate comp344)
    (not (rebooted comp344))
    (running comp343)
  )
  :effect (and
    (not (evaluate comp344))
    (evaluate comp345)
    (all-on comp344)
  )
)
(:action evaluate-comp344-off-comp343
  :parameters ()
  :precondition (and
    (evaluate comp344)
    (not (rebooted comp344))
    (not (running comp343))
  )
  :effect (and
    (not (evaluate comp344))
    (evaluate comp345)
    (one-off comp344)
  )
)
(:action evaluate-comp345-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp345)
    (rebooted comp345)
  )
  :effect (and
    (not (evaluate comp345))
    (evaluate comp346)
  )
)
(:action evaluate-comp345-all-on
  :parameters ()
  :precondition (and
    (evaluate comp345)
    (not (rebooted comp345))
    (running comp344)
  )
  :effect (and
    (not (evaluate comp345))
    (evaluate comp346)
    (all-on comp345)
  )
)
(:action evaluate-comp345-off-comp344
  :parameters ()
  :precondition (and
    (evaluate comp345)
    (not (rebooted comp345))
    (not (running comp344))
  )
  :effect (and
    (not (evaluate comp345))
    (evaluate comp346)
    (one-off comp345)
  )
)
(:action evaluate-comp346-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp346)
    (rebooted comp346)
  )
  :effect (and
    (not (evaluate comp346))
    (evaluate comp347)
  )
)
(:action evaluate-comp346-all-on
  :parameters ()
  :precondition (and
    (evaluate comp346)
    (not (rebooted comp346))
    (running comp112)
    (running comp345)
  )
  :effect (and
    (not (evaluate comp346))
    (evaluate comp347)
    (all-on comp346)
  )
)
(:action evaluate-comp346-off-comp112
  :parameters ()
  :precondition (and
    (evaluate comp346)
    (not (rebooted comp346))
    (not (running comp112))
  )
  :effect (and
    (not (evaluate comp346))
    (evaluate comp347)
    (one-off comp346)
  )
)
(:action evaluate-comp346-off-comp345
  :parameters ()
  :precondition (and
    (evaluate comp346)
    (not (rebooted comp346))
    (not (running comp345))
  )
  :effect (and
    (not (evaluate comp346))
    (evaluate comp347)
    (one-off comp346)
  )
)
(:action evaluate-comp347-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp347)
    (rebooted comp347)
  )
  :effect (and
    (not (evaluate comp347))
    (evaluate comp348)
  )
)
(:action evaluate-comp347-all-on
  :parameters ()
  :precondition (and
    (evaluate comp347)
    (not (rebooted comp347))
    (running comp346)
  )
  :effect (and
    (not (evaluate comp347))
    (evaluate comp348)
    (all-on comp347)
  )
)
(:action evaluate-comp347-off-comp346
  :parameters ()
  :precondition (and
    (evaluate comp347)
    (not (rebooted comp347))
    (not (running comp346))
  )
  :effect (and
    (not (evaluate comp347))
    (evaluate comp348)
    (one-off comp347)
  )
)
(:action evaluate-comp348-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp348)
    (rebooted comp348)
  )
  :effect (and
    (not (evaluate comp348))
    (evaluate comp349)
  )
)
(:action evaluate-comp348-all-on
  :parameters ()
  :precondition (and
    (evaluate comp348)
    (not (rebooted comp348))
    (running comp347)
  )
  :effect (and
    (not (evaluate comp348))
    (evaluate comp349)
    (all-on comp348)
  )
)
(:action evaluate-comp348-off-comp347
  :parameters ()
  :precondition (and
    (evaluate comp348)
    (not (rebooted comp348))
    (not (running comp347))
  )
  :effect (and
    (not (evaluate comp348))
    (evaluate comp349)
    (one-off comp348)
  )
)
(:action evaluate-comp349-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp349)
    (rebooted comp349)
  )
  :effect (and
    (not (evaluate comp349))
    (evaluate comp350)
  )
)
(:action evaluate-comp349-all-on
  :parameters ()
  :precondition (and
    (evaluate comp349)
    (not (rebooted comp349))
    (running comp348)
  )
  :effect (and
    (not (evaluate comp349))
    (evaluate comp350)
    (all-on comp349)
  )
)
(:action evaluate-comp349-off-comp348
  :parameters ()
  :precondition (and
    (evaluate comp349)
    (not (rebooted comp349))
    (not (running comp348))
  )
  :effect (and
    (not (evaluate comp349))
    (evaluate comp350)
    (one-off comp349)
  )
)
(:action evaluate-comp350-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp350)
    (rebooted comp350)
  )
  :effect (and
    (not (evaluate comp350))
    (evaluate comp351)
  )
)
(:action evaluate-comp350-all-on
  :parameters ()
  :precondition (and
    (evaluate comp350)
    (not (rebooted comp350))
    (running comp349)
  )
  :effect (and
    (not (evaluate comp350))
    (evaluate comp351)
    (all-on comp350)
  )
)
(:action evaluate-comp350-off-comp349
  :parameters ()
  :precondition (and
    (evaluate comp350)
    (not (rebooted comp350))
    (not (running comp349))
  )
  :effect (and
    (not (evaluate comp350))
    (evaluate comp351)
    (one-off comp350)
  )
)
(:action evaluate-comp351-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp351)
    (rebooted comp351)
  )
  :effect (and
    (not (evaluate comp351))
    (evaluate comp352)
  )
)
(:action evaluate-comp351-all-on
  :parameters ()
  :precondition (and
    (evaluate comp351)
    (not (rebooted comp351))
    (running comp350)
  )
  :effect (and
    (not (evaluate comp351))
    (evaluate comp352)
    (all-on comp351)
  )
)
(:action evaluate-comp351-off-comp350
  :parameters ()
  :precondition (and
    (evaluate comp351)
    (not (rebooted comp351))
    (not (running comp350))
  )
  :effect (and
    (not (evaluate comp351))
    (evaluate comp352)
    (one-off comp351)
  )
)
(:action evaluate-comp352-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp352)
    (rebooted comp352)
  )
  :effect (and
    (not (evaluate comp352))
    (evaluate comp353)
  )
)
(:action evaluate-comp352-all-on
  :parameters ()
  :precondition (and
    (evaluate comp352)
    (not (rebooted comp352))
    (running comp47)
    (running comp351)
  )
  :effect (and
    (not (evaluate comp352))
    (evaluate comp353)
    (all-on comp352)
  )
)
(:action evaluate-comp352-off-comp47
  :parameters ()
  :precondition (and
    (evaluate comp352)
    (not (rebooted comp352))
    (not (running comp47))
  )
  :effect (and
    (not (evaluate comp352))
    (evaluate comp353)
    (one-off comp352)
  )
)
(:action evaluate-comp352-off-comp351
  :parameters ()
  :precondition (and
    (evaluate comp352)
    (not (rebooted comp352))
    (not (running comp351))
  )
  :effect (and
    (not (evaluate comp352))
    (evaluate comp353)
    (one-off comp352)
  )
)
(:action evaluate-comp353-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp353)
    (rebooted comp353)
  )
  :effect (and
    (not (evaluate comp353))
    (evaluate comp354)
  )
)
(:action evaluate-comp353-all-on
  :parameters ()
  :precondition (and
    (evaluate comp353)
    (not (rebooted comp353))
    (running comp352)
  )
  :effect (and
    (not (evaluate comp353))
    (evaluate comp354)
    (all-on comp353)
  )
)
(:action evaluate-comp353-off-comp352
  :parameters ()
  :precondition (and
    (evaluate comp353)
    (not (rebooted comp353))
    (not (running comp352))
  )
  :effect (and
    (not (evaluate comp353))
    (evaluate comp354)
    (one-off comp353)
  )
)
(:action evaluate-comp354-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp354)
    (rebooted comp354)
  )
  :effect (and
    (not (evaluate comp354))
    (evaluate comp355)
  )
)
(:action evaluate-comp354-all-on
  :parameters ()
  :precondition (and
    (evaluate comp354)
    (not (rebooted comp354))
    (running comp137)
    (running comp353)
  )
  :effect (and
    (not (evaluate comp354))
    (evaluate comp355)
    (all-on comp354)
  )
)
(:action evaluate-comp354-off-comp137
  :parameters ()
  :precondition (and
    (evaluate comp354)
    (not (rebooted comp354))
    (not (running comp137))
  )
  :effect (and
    (not (evaluate comp354))
    (evaluate comp355)
    (one-off comp354)
  )
)
(:action evaluate-comp354-off-comp353
  :parameters ()
  :precondition (and
    (evaluate comp354)
    (not (rebooted comp354))
    (not (running comp353))
  )
  :effect (and
    (not (evaluate comp354))
    (evaluate comp355)
    (one-off comp354)
  )
)
(:action evaluate-comp355-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp355)
    (rebooted comp355)
  )
  :effect (and
    (not (evaluate comp355))
    (evaluate comp356)
  )
)
(:action evaluate-comp355-all-on
  :parameters ()
  :precondition (and
    (evaluate comp355)
    (not (rebooted comp355))
    (running comp3)
    (running comp85)
    (running comp354)
  )
  :effect (and
    (not (evaluate comp355))
    (evaluate comp356)
    (all-on comp355)
  )
)
(:action evaluate-comp355-off-comp3
  :parameters ()
  :precondition (and
    (evaluate comp355)
    (not (rebooted comp355))
    (not (running comp3))
  )
  :effect (and
    (not (evaluate comp355))
    (evaluate comp356)
    (one-off comp355)
  )
)
(:action evaluate-comp355-off-comp85
  :parameters ()
  :precondition (and
    (evaluate comp355)
    (not (rebooted comp355))
    (not (running comp85))
  )
  :effect (and
    (not (evaluate comp355))
    (evaluate comp356)
    (one-off comp355)
  )
)
(:action evaluate-comp355-off-comp354
  :parameters ()
  :precondition (and
    (evaluate comp355)
    (not (rebooted comp355))
    (not (running comp354))
  )
  :effect (and
    (not (evaluate comp355))
    (evaluate comp356)
    (one-off comp355)
  )
)
(:action evaluate-comp356-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp356)
    (rebooted comp356)
  )
  :effect (and
    (not (evaluate comp356))
    (evaluate comp357)
  )
)
(:action evaluate-comp356-all-on
  :parameters ()
  :precondition (and
    (evaluate comp356)
    (not (rebooted comp356))
    (running comp355)
  )
  :effect (and
    (not (evaluate comp356))
    (evaluate comp357)
    (all-on comp356)
  )
)
(:action evaluate-comp356-off-comp355
  :parameters ()
  :precondition (and
    (evaluate comp356)
    (not (rebooted comp356))
    (not (running comp355))
  )
  :effect (and
    (not (evaluate comp356))
    (evaluate comp357)
    (one-off comp356)
  )
)
(:action evaluate-comp357-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp357)
    (rebooted comp357)
  )
  :effect (and
    (not (evaluate comp357))
    (evaluate comp358)
  )
)
(:action evaluate-comp357-all-on
  :parameters ()
  :precondition (and
    (evaluate comp357)
    (not (rebooted comp357))
    (running comp149)
    (running comp356)
  )
  :effect (and
    (not (evaluate comp357))
    (evaluate comp358)
    (all-on comp357)
  )
)
(:action evaluate-comp357-off-comp149
  :parameters ()
  :precondition (and
    (evaluate comp357)
    (not (rebooted comp357))
    (not (running comp149))
  )
  :effect (and
    (not (evaluate comp357))
    (evaluate comp358)
    (one-off comp357)
  )
)
(:action evaluate-comp357-off-comp356
  :parameters ()
  :precondition (and
    (evaluate comp357)
    (not (rebooted comp357))
    (not (running comp356))
  )
  :effect (and
    (not (evaluate comp357))
    (evaluate comp358)
    (one-off comp357)
  )
)
(:action evaluate-comp358-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp358)
    (rebooted comp358)
  )
  :effect (and
    (not (evaluate comp358))
    (evaluate comp359)
  )
)
(:action evaluate-comp358-all-on
  :parameters ()
  :precondition (and
    (evaluate comp358)
    (not (rebooted comp358))
    (running comp256)
    (running comp357)
  )
  :effect (and
    (not (evaluate comp358))
    (evaluate comp359)
    (all-on comp358)
  )
)
(:action evaluate-comp358-off-comp256
  :parameters ()
  :precondition (and
    (evaluate comp358)
    (not (rebooted comp358))
    (not (running comp256))
  )
  :effect (and
    (not (evaluate comp358))
    (evaluate comp359)
    (one-off comp358)
  )
)
(:action evaluate-comp358-off-comp357
  :parameters ()
  :precondition (and
    (evaluate comp358)
    (not (rebooted comp358))
    (not (running comp357))
  )
  :effect (and
    (not (evaluate comp358))
    (evaluate comp359)
    (one-off comp358)
  )
)
(:action evaluate-comp359-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp359)
    (rebooted comp359)
  )
  :effect (and
    (not (evaluate comp359))
    (evaluate comp360)
  )
)
(:action evaluate-comp359-all-on
  :parameters ()
  :precondition (and
    (evaluate comp359)
    (not (rebooted comp359))
    (running comp358)
    (running comp478)
  )
  :effect (and
    (not (evaluate comp359))
    (evaluate comp360)
    (all-on comp359)
  )
)
(:action evaluate-comp359-off-comp358
  :parameters ()
  :precondition (and
    (evaluate comp359)
    (not (rebooted comp359))
    (not (running comp358))
  )
  :effect (and
    (not (evaluate comp359))
    (evaluate comp360)
    (one-off comp359)
  )
)
(:action evaluate-comp359-off-comp478
  :parameters ()
  :precondition (and
    (evaluate comp359)
    (not (rebooted comp359))
    (not (running comp478))
  )
  :effect (and
    (not (evaluate comp359))
    (evaluate comp360)
    (one-off comp359)
  )
)
(:action evaluate-comp360-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp360)
    (rebooted comp360)
  )
  :effect (and
    (not (evaluate comp360))
    (evaluate comp361)
  )
)
(:action evaluate-comp360-all-on
  :parameters ()
  :precondition (and
    (evaluate comp360)
    (not (rebooted comp360))
    (running comp359)
  )
  :effect (and
    (not (evaluate comp360))
    (evaluate comp361)
    (all-on comp360)
  )
)
(:action evaluate-comp360-off-comp359
  :parameters ()
  :precondition (and
    (evaluate comp360)
    (not (rebooted comp360))
    (not (running comp359))
  )
  :effect (and
    (not (evaluate comp360))
    (evaluate comp361)
    (one-off comp360)
  )
)
(:action evaluate-comp361-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp361)
    (rebooted comp361)
  )
  :effect (and
    (not (evaluate comp361))
    (evaluate comp362)
  )
)
(:action evaluate-comp361-all-on
  :parameters ()
  :precondition (and
    (evaluate comp361)
    (not (rebooted comp361))
    (running comp271)
    (running comp360)
  )
  :effect (and
    (not (evaluate comp361))
    (evaluate comp362)
    (all-on comp361)
  )
)
(:action evaluate-comp361-off-comp271
  :parameters ()
  :precondition (and
    (evaluate comp361)
    (not (rebooted comp361))
    (not (running comp271))
  )
  :effect (and
    (not (evaluate comp361))
    (evaluate comp362)
    (one-off comp361)
  )
)
(:action evaluate-comp361-off-comp360
  :parameters ()
  :precondition (and
    (evaluate comp361)
    (not (rebooted comp361))
    (not (running comp360))
  )
  :effect (and
    (not (evaluate comp361))
    (evaluate comp362)
    (one-off comp361)
  )
)
(:action evaluate-comp362-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp362)
    (rebooted comp362)
  )
  :effect (and
    (not (evaluate comp362))
    (evaluate comp363)
  )
)
(:action evaluate-comp362-all-on
  :parameters ()
  :precondition (and
    (evaluate comp362)
    (not (rebooted comp362))
    (running comp361)
  )
  :effect (and
    (not (evaluate comp362))
    (evaluate comp363)
    (all-on comp362)
  )
)
(:action evaluate-comp362-off-comp361
  :parameters ()
  :precondition (and
    (evaluate comp362)
    (not (rebooted comp362))
    (not (running comp361))
  )
  :effect (and
    (not (evaluate comp362))
    (evaluate comp363)
    (one-off comp362)
  )
)
(:action evaluate-comp363-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp363)
    (rebooted comp363)
  )
  :effect (and
    (not (evaluate comp363))
    (evaluate comp364)
  )
)
(:action evaluate-comp363-all-on
  :parameters ()
  :precondition (and
    (evaluate comp363)
    (not (rebooted comp363))
    (running comp362)
  )
  :effect (and
    (not (evaluate comp363))
    (evaluate comp364)
    (all-on comp363)
  )
)
(:action evaluate-comp363-off-comp362
  :parameters ()
  :precondition (and
    (evaluate comp363)
    (not (rebooted comp363))
    (not (running comp362))
  )
  :effect (and
    (not (evaluate comp363))
    (evaluate comp364)
    (one-off comp363)
  )
)
(:action evaluate-comp364-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp364)
    (rebooted comp364)
  )
  :effect (and
    (not (evaluate comp364))
    (evaluate comp365)
  )
)
(:action evaluate-comp364-all-on
  :parameters ()
  :precondition (and
    (evaluate comp364)
    (not (rebooted comp364))
    (running comp198)
    (running comp363)
  )
  :effect (and
    (not (evaluate comp364))
    (evaluate comp365)
    (all-on comp364)
  )
)
(:action evaluate-comp364-off-comp198
  :parameters ()
  :precondition (and
    (evaluate comp364)
    (not (rebooted comp364))
    (not (running comp198))
  )
  :effect (and
    (not (evaluate comp364))
    (evaluate comp365)
    (one-off comp364)
  )
)
(:action evaluate-comp364-off-comp363
  :parameters ()
  :precondition (and
    (evaluate comp364)
    (not (rebooted comp364))
    (not (running comp363))
  )
  :effect (and
    (not (evaluate comp364))
    (evaluate comp365)
    (one-off comp364)
  )
)
(:action evaluate-comp365-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp365)
    (rebooted comp365)
  )
  :effect (and
    (not (evaluate comp365))
    (evaluate comp366)
  )
)
(:action evaluate-comp365-all-on
  :parameters ()
  :precondition (and
    (evaluate comp365)
    (not (rebooted comp365))
    (running comp364)
  )
  :effect (and
    (not (evaluate comp365))
    (evaluate comp366)
    (all-on comp365)
  )
)
(:action evaluate-comp365-off-comp364
  :parameters ()
  :precondition (and
    (evaluate comp365)
    (not (rebooted comp365))
    (not (running comp364))
  )
  :effect (and
    (not (evaluate comp365))
    (evaluate comp366)
    (one-off comp365)
  )
)
(:action evaluate-comp366-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp366)
    (rebooted comp366)
  )
  :effect (and
    (not (evaluate comp366))
    (evaluate comp367)
  )
)
(:action evaluate-comp366-all-on
  :parameters ()
  :precondition (and
    (evaluate comp366)
    (not (rebooted comp366))
    (running comp365)
  )
  :effect (and
    (not (evaluate comp366))
    (evaluate comp367)
    (all-on comp366)
  )
)
(:action evaluate-comp366-off-comp365
  :parameters ()
  :precondition (and
    (evaluate comp366)
    (not (rebooted comp366))
    (not (running comp365))
  )
  :effect (and
    (not (evaluate comp366))
    (evaluate comp367)
    (one-off comp366)
  )
)
(:action evaluate-comp367-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp367)
    (rebooted comp367)
  )
  :effect (and
    (not (evaluate comp367))
    (evaluate comp368)
  )
)
(:action evaluate-comp367-all-on
  :parameters ()
  :precondition (and
    (evaluate comp367)
    (not (rebooted comp367))
    (running comp366)
  )
  :effect (and
    (not (evaluate comp367))
    (evaluate comp368)
    (all-on comp367)
  )
)
(:action evaluate-comp367-off-comp366
  :parameters ()
  :precondition (and
    (evaluate comp367)
    (not (rebooted comp367))
    (not (running comp366))
  )
  :effect (and
    (not (evaluate comp367))
    (evaluate comp368)
    (one-off comp367)
  )
)
(:action evaluate-comp368-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp368)
    (rebooted comp368)
  )
  :effect (and
    (not (evaluate comp368))
    (evaluate comp369)
  )
)
(:action evaluate-comp368-all-on
  :parameters ()
  :precondition (and
    (evaluate comp368)
    (not (rebooted comp368))
    (running comp360)
    (running comp367)
  )
  :effect (and
    (not (evaluate comp368))
    (evaluate comp369)
    (all-on comp368)
  )
)
(:action evaluate-comp368-off-comp360
  :parameters ()
  :precondition (and
    (evaluate comp368)
    (not (rebooted comp368))
    (not (running comp360))
  )
  :effect (and
    (not (evaluate comp368))
    (evaluate comp369)
    (one-off comp368)
  )
)
(:action evaluate-comp368-off-comp367
  :parameters ()
  :precondition (and
    (evaluate comp368)
    (not (rebooted comp368))
    (not (running comp367))
  )
  :effect (and
    (not (evaluate comp368))
    (evaluate comp369)
    (one-off comp368)
  )
)
(:action evaluate-comp369-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp369)
    (rebooted comp369)
  )
  :effect (and
    (not (evaluate comp369))
    (evaluate comp370)
  )
)
(:action evaluate-comp369-all-on
  :parameters ()
  :precondition (and
    (evaluate comp369)
    (not (rebooted comp369))
    (running comp368)
  )
  :effect (and
    (not (evaluate comp369))
    (evaluate comp370)
    (all-on comp369)
  )
)
(:action evaluate-comp369-off-comp368
  :parameters ()
  :precondition (and
    (evaluate comp369)
    (not (rebooted comp369))
    (not (running comp368))
  )
  :effect (and
    (not (evaluate comp369))
    (evaluate comp370)
    (one-off comp369)
  )
)
(:action evaluate-comp370-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp370)
    (rebooted comp370)
  )
  :effect (and
    (not (evaluate comp370))
    (evaluate comp371)
  )
)
(:action evaluate-comp370-all-on
  :parameters ()
  :precondition (and
    (evaluate comp370)
    (not (rebooted comp370))
    (running comp27)
    (running comp369)
  )
  :effect (and
    (not (evaluate comp370))
    (evaluate comp371)
    (all-on comp370)
  )
)
(:action evaluate-comp370-off-comp27
  :parameters ()
  :precondition (and
    (evaluate comp370)
    (not (rebooted comp370))
    (not (running comp27))
  )
  :effect (and
    (not (evaluate comp370))
    (evaluate comp371)
    (one-off comp370)
  )
)
(:action evaluate-comp370-off-comp369
  :parameters ()
  :precondition (and
    (evaluate comp370)
    (not (rebooted comp370))
    (not (running comp369))
  )
  :effect (and
    (not (evaluate comp370))
    (evaluate comp371)
    (one-off comp370)
  )
)
(:action evaluate-comp371-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp371)
    (rebooted comp371)
  )
  :effect (and
    (not (evaluate comp371))
    (evaluate comp372)
  )
)
(:action evaluate-comp371-all-on
  :parameters ()
  :precondition (and
    (evaluate comp371)
    (not (rebooted comp371))
    (running comp370)
  )
  :effect (and
    (not (evaluate comp371))
    (evaluate comp372)
    (all-on comp371)
  )
)
(:action evaluate-comp371-off-comp370
  :parameters ()
  :precondition (and
    (evaluate comp371)
    (not (rebooted comp371))
    (not (running comp370))
  )
  :effect (and
    (not (evaluate comp371))
    (evaluate comp372)
    (one-off comp371)
  )
)
(:action evaluate-comp372-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp372)
    (rebooted comp372)
  )
  :effect (and
    (not (evaluate comp372))
    (evaluate comp373)
  )
)
(:action evaluate-comp372-all-on
  :parameters ()
  :precondition (and
    (evaluate comp372)
    (not (rebooted comp372))
    (running comp371)
  )
  :effect (and
    (not (evaluate comp372))
    (evaluate comp373)
    (all-on comp372)
  )
)
(:action evaluate-comp372-off-comp371
  :parameters ()
  :precondition (and
    (evaluate comp372)
    (not (rebooted comp372))
    (not (running comp371))
  )
  :effect (and
    (not (evaluate comp372))
    (evaluate comp373)
    (one-off comp372)
  )
)
(:action evaluate-comp373-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp373)
    (rebooted comp373)
  )
  :effect (and
    (not (evaluate comp373))
    (evaluate comp374)
  )
)
(:action evaluate-comp373-all-on
  :parameters ()
  :precondition (and
    (evaluate comp373)
    (not (rebooted comp373))
    (running comp334)
    (running comp372)
  )
  :effect (and
    (not (evaluate comp373))
    (evaluate comp374)
    (all-on comp373)
  )
)
(:action evaluate-comp373-off-comp334
  :parameters ()
  :precondition (and
    (evaluate comp373)
    (not (rebooted comp373))
    (not (running comp334))
  )
  :effect (and
    (not (evaluate comp373))
    (evaluate comp374)
    (one-off comp373)
  )
)
(:action evaluate-comp373-off-comp372
  :parameters ()
  :precondition (and
    (evaluate comp373)
    (not (rebooted comp373))
    (not (running comp372))
  )
  :effect (and
    (not (evaluate comp373))
    (evaluate comp374)
    (one-off comp373)
  )
)
(:action evaluate-comp374-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp374)
    (rebooted comp374)
  )
  :effect (and
    (not (evaluate comp374))
    (evaluate comp375)
  )
)
(:action evaluate-comp374-all-on
  :parameters ()
  :precondition (and
    (evaluate comp374)
    (not (rebooted comp374))
    (running comp373)
  )
  :effect (and
    (not (evaluate comp374))
    (evaluate comp375)
    (all-on comp374)
  )
)
(:action evaluate-comp374-off-comp373
  :parameters ()
  :precondition (and
    (evaluate comp374)
    (not (rebooted comp374))
    (not (running comp373))
  )
  :effect (and
    (not (evaluate comp374))
    (evaluate comp375)
    (one-off comp374)
  )
)
(:action evaluate-comp375-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp375)
    (rebooted comp375)
  )
  :effect (and
    (not (evaluate comp375))
    (evaluate comp376)
  )
)
(:action evaluate-comp375-all-on
  :parameters ()
  :precondition (and
    (evaluate comp375)
    (not (rebooted comp375))
    (running comp374)
    (running comp402)
  )
  :effect (and
    (not (evaluate comp375))
    (evaluate comp376)
    (all-on comp375)
  )
)
(:action evaluate-comp375-off-comp374
  :parameters ()
  :precondition (and
    (evaluate comp375)
    (not (rebooted comp375))
    (not (running comp374))
  )
  :effect (and
    (not (evaluate comp375))
    (evaluate comp376)
    (one-off comp375)
  )
)
(:action evaluate-comp375-off-comp402
  :parameters ()
  :precondition (and
    (evaluate comp375)
    (not (rebooted comp375))
    (not (running comp402))
  )
  :effect (and
    (not (evaluate comp375))
    (evaluate comp376)
    (one-off comp375)
  )
)
(:action evaluate-comp376-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp376)
    (rebooted comp376)
  )
  :effect (and
    (not (evaluate comp376))
    (evaluate comp377)
  )
)
(:action evaluate-comp376-all-on
  :parameters ()
  :precondition (and
    (evaluate comp376)
    (not (rebooted comp376))
    (running comp375)
  )
  :effect (and
    (not (evaluate comp376))
    (evaluate comp377)
    (all-on comp376)
  )
)
(:action evaluate-comp376-off-comp375
  :parameters ()
  :precondition (and
    (evaluate comp376)
    (not (rebooted comp376))
    (not (running comp375))
  )
  :effect (and
    (not (evaluate comp376))
    (evaluate comp377)
    (one-off comp376)
  )
)
(:action evaluate-comp377-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp377)
    (rebooted comp377)
  )
  :effect (and
    (not (evaluate comp377))
    (evaluate comp378)
  )
)
(:action evaluate-comp377-all-on
  :parameters ()
  :precondition (and
    (evaluate comp377)
    (not (rebooted comp377))
    (running comp353)
    (running comp376)
  )
  :effect (and
    (not (evaluate comp377))
    (evaluate comp378)
    (all-on comp377)
  )
)
(:action evaluate-comp377-off-comp353
  :parameters ()
  :precondition (and
    (evaluate comp377)
    (not (rebooted comp377))
    (not (running comp353))
  )
  :effect (and
    (not (evaluate comp377))
    (evaluate comp378)
    (one-off comp377)
  )
)
(:action evaluate-comp377-off-comp376
  :parameters ()
  :precondition (and
    (evaluate comp377)
    (not (rebooted comp377))
    (not (running comp376))
  )
  :effect (and
    (not (evaluate comp377))
    (evaluate comp378)
    (one-off comp377)
  )
)
(:action evaluate-comp378-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp378)
    (rebooted comp378)
  )
  :effect (and
    (not (evaluate comp378))
    (evaluate comp379)
  )
)
(:action evaluate-comp378-all-on
  :parameters ()
  :precondition (and
    (evaluate comp378)
    (not (rebooted comp378))
    (running comp377)
  )
  :effect (and
    (not (evaluate comp378))
    (evaluate comp379)
    (all-on comp378)
  )
)
(:action evaluate-comp378-off-comp377
  :parameters ()
  :precondition (and
    (evaluate comp378)
    (not (rebooted comp378))
    (not (running comp377))
  )
  :effect (and
    (not (evaluate comp378))
    (evaluate comp379)
    (one-off comp378)
  )
)
(:action evaluate-comp379-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp379)
    (rebooted comp379)
  )
  :effect (and
    (not (evaluate comp379))
    (evaluate comp380)
  )
)
(:action evaluate-comp379-all-on
  :parameters ()
  :precondition (and
    (evaluate comp379)
    (not (rebooted comp379))
    (running comp378)
  )
  :effect (and
    (not (evaluate comp379))
    (evaluate comp380)
    (all-on comp379)
  )
)
(:action evaluate-comp379-off-comp378
  :parameters ()
  :precondition (and
    (evaluate comp379)
    (not (rebooted comp379))
    (not (running comp378))
  )
  :effect (and
    (not (evaluate comp379))
    (evaluate comp380)
    (one-off comp379)
  )
)
(:action evaluate-comp380-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp380)
    (rebooted comp380)
  )
  :effect (and
    (not (evaluate comp380))
    (evaluate comp381)
  )
)
(:action evaluate-comp380-all-on
  :parameters ()
  :precondition (and
    (evaluate comp380)
    (not (rebooted comp380))
    (running comp379)
  )
  :effect (and
    (not (evaluate comp380))
    (evaluate comp381)
    (all-on comp380)
  )
)
(:action evaluate-comp380-off-comp379
  :parameters ()
  :precondition (and
    (evaluate comp380)
    (not (rebooted comp380))
    (not (running comp379))
  )
  :effect (and
    (not (evaluate comp380))
    (evaluate comp381)
    (one-off comp380)
  )
)
(:action evaluate-comp381-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp381)
    (rebooted comp381)
  )
  :effect (and
    (not (evaluate comp381))
    (evaluate comp382)
  )
)
(:action evaluate-comp381-all-on
  :parameters ()
  :precondition (and
    (evaluate comp381)
    (not (rebooted comp381))
    (running comp376)
    (running comp380)
  )
  :effect (and
    (not (evaluate comp381))
    (evaluate comp382)
    (all-on comp381)
  )
)
(:action evaluate-comp381-off-comp376
  :parameters ()
  :precondition (and
    (evaluate comp381)
    (not (rebooted comp381))
    (not (running comp376))
  )
  :effect (and
    (not (evaluate comp381))
    (evaluate comp382)
    (one-off comp381)
  )
)
(:action evaluate-comp381-off-comp380
  :parameters ()
  :precondition (and
    (evaluate comp381)
    (not (rebooted comp381))
    (not (running comp380))
  )
  :effect (and
    (not (evaluate comp381))
    (evaluate comp382)
    (one-off comp381)
  )
)
(:action evaluate-comp382-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp382)
    (rebooted comp382)
  )
  :effect (and
    (not (evaluate comp382))
    (evaluate comp383)
  )
)
(:action evaluate-comp382-all-on
  :parameters ()
  :precondition (and
    (evaluate comp382)
    (not (rebooted comp382))
    (running comp381)
  )
  :effect (and
    (not (evaluate comp382))
    (evaluate comp383)
    (all-on comp382)
  )
)
(:action evaluate-comp382-off-comp381
  :parameters ()
  :precondition (and
    (evaluate comp382)
    (not (rebooted comp382))
    (not (running comp381))
  )
  :effect (and
    (not (evaluate comp382))
    (evaluate comp383)
    (one-off comp382)
  )
)
(:action evaluate-comp383-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp383)
    (rebooted comp383)
  )
  :effect (and
    (not (evaluate comp383))
    (evaluate comp384)
  )
)
(:action evaluate-comp383-all-on
  :parameters ()
  :precondition (and
    (evaluate comp383)
    (not (rebooted comp383))
    (running comp113)
    (running comp382)
  )
  :effect (and
    (not (evaluate comp383))
    (evaluate comp384)
    (all-on comp383)
  )
)
(:action evaluate-comp383-off-comp113
  :parameters ()
  :precondition (and
    (evaluate comp383)
    (not (rebooted comp383))
    (not (running comp113))
  )
  :effect (and
    (not (evaluate comp383))
    (evaluate comp384)
    (one-off comp383)
  )
)
(:action evaluate-comp383-off-comp382
  :parameters ()
  :precondition (and
    (evaluate comp383)
    (not (rebooted comp383))
    (not (running comp382))
  )
  :effect (and
    (not (evaluate comp383))
    (evaluate comp384)
    (one-off comp383)
  )
)
(:action evaluate-comp384-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp384)
    (rebooted comp384)
  )
  :effect (and
    (not (evaluate comp384))
    (evaluate comp385)
  )
)
(:action evaluate-comp384-all-on
  :parameters ()
  :precondition (and
    (evaluate comp384)
    (not (rebooted comp384))
    (running comp383)
  )
  :effect (and
    (not (evaluate comp384))
    (evaluate comp385)
    (all-on comp384)
  )
)
(:action evaluate-comp384-off-comp383
  :parameters ()
  :precondition (and
    (evaluate comp384)
    (not (rebooted comp384))
    (not (running comp383))
  )
  :effect (and
    (not (evaluate comp384))
    (evaluate comp385)
    (one-off comp384)
  )
)
(:action evaluate-comp385-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp385)
    (rebooted comp385)
  )
  :effect (and
    (not (evaluate comp385))
    (evaluate comp386)
  )
)
(:action evaluate-comp385-all-on
  :parameters ()
  :precondition (and
    (evaluate comp385)
    (not (rebooted comp385))
    (running comp384)
  )
  :effect (and
    (not (evaluate comp385))
    (evaluate comp386)
    (all-on comp385)
  )
)
(:action evaluate-comp385-off-comp384
  :parameters ()
  :precondition (and
    (evaluate comp385)
    (not (rebooted comp385))
    (not (running comp384))
  )
  :effect (and
    (not (evaluate comp385))
    (evaluate comp386)
    (one-off comp385)
  )
)
(:action evaluate-comp386-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp386)
    (rebooted comp386)
  )
  :effect (and
    (not (evaluate comp386))
    (evaluate comp387)
  )
)
(:action evaluate-comp386-all-on
  :parameters ()
  :precondition (and
    (evaluate comp386)
    (not (rebooted comp386))
    (running comp263)
    (running comp385)
  )
  :effect (and
    (not (evaluate comp386))
    (evaluate comp387)
    (all-on comp386)
  )
)
(:action evaluate-comp386-off-comp263
  :parameters ()
  :precondition (and
    (evaluate comp386)
    (not (rebooted comp386))
    (not (running comp263))
  )
  :effect (and
    (not (evaluate comp386))
    (evaluate comp387)
    (one-off comp386)
  )
)
(:action evaluate-comp386-off-comp385
  :parameters ()
  :precondition (and
    (evaluate comp386)
    (not (rebooted comp386))
    (not (running comp385))
  )
  :effect (and
    (not (evaluate comp386))
    (evaluate comp387)
    (one-off comp386)
  )
)
(:action evaluate-comp387-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp387)
    (rebooted comp387)
  )
  :effect (and
    (not (evaluate comp387))
    (evaluate comp388)
  )
)
(:action evaluate-comp387-all-on
  :parameters ()
  :precondition (and
    (evaluate comp387)
    (not (rebooted comp387))
    (running comp140)
    (running comp386)
  )
  :effect (and
    (not (evaluate comp387))
    (evaluate comp388)
    (all-on comp387)
  )
)
(:action evaluate-comp387-off-comp140
  :parameters ()
  :precondition (and
    (evaluate comp387)
    (not (rebooted comp387))
    (not (running comp140))
  )
  :effect (and
    (not (evaluate comp387))
    (evaluate comp388)
    (one-off comp387)
  )
)
(:action evaluate-comp387-off-comp386
  :parameters ()
  :precondition (and
    (evaluate comp387)
    (not (rebooted comp387))
    (not (running comp386))
  )
  :effect (and
    (not (evaluate comp387))
    (evaluate comp388)
    (one-off comp387)
  )
)
(:action evaluate-comp388-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp388)
    (rebooted comp388)
  )
  :effect (and
    (not (evaluate comp388))
    (evaluate comp389)
  )
)
(:action evaluate-comp388-all-on
  :parameters ()
  :precondition (and
    (evaluate comp388)
    (not (rebooted comp388))
    (running comp196)
    (running comp387)
  )
  :effect (and
    (not (evaluate comp388))
    (evaluate comp389)
    (all-on comp388)
  )
)
(:action evaluate-comp388-off-comp196
  :parameters ()
  :precondition (and
    (evaluate comp388)
    (not (rebooted comp388))
    (not (running comp196))
  )
  :effect (and
    (not (evaluate comp388))
    (evaluate comp389)
    (one-off comp388)
  )
)
(:action evaluate-comp388-off-comp387
  :parameters ()
  :precondition (and
    (evaluate comp388)
    (not (rebooted comp388))
    (not (running comp387))
  )
  :effect (and
    (not (evaluate comp388))
    (evaluate comp389)
    (one-off comp388)
  )
)
(:action evaluate-comp389-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp389)
    (rebooted comp389)
  )
  :effect (and
    (not (evaluate comp389))
    (evaluate comp390)
  )
)
(:action evaluate-comp389-all-on
  :parameters ()
  :precondition (and
    (evaluate comp389)
    (not (rebooted comp389))
    (running comp311)
    (running comp388)
  )
  :effect (and
    (not (evaluate comp389))
    (evaluate comp390)
    (all-on comp389)
  )
)
(:action evaluate-comp389-off-comp311
  :parameters ()
  :precondition (and
    (evaluate comp389)
    (not (rebooted comp389))
    (not (running comp311))
  )
  :effect (and
    (not (evaluate comp389))
    (evaluate comp390)
    (one-off comp389)
  )
)
(:action evaluate-comp389-off-comp388
  :parameters ()
  :precondition (and
    (evaluate comp389)
    (not (rebooted comp389))
    (not (running comp388))
  )
  :effect (and
    (not (evaluate comp389))
    (evaluate comp390)
    (one-off comp389)
  )
)
(:action evaluate-comp390-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp390)
    (rebooted comp390)
  )
  :effect (and
    (not (evaluate comp390))
    (evaluate comp391)
  )
)
(:action evaluate-comp390-all-on
  :parameters ()
  :precondition (and
    (evaluate comp390)
    (not (rebooted comp390))
    (running comp304)
    (running comp389)
  )
  :effect (and
    (not (evaluate comp390))
    (evaluate comp391)
    (all-on comp390)
  )
)
(:action evaluate-comp390-off-comp304
  :parameters ()
  :precondition (and
    (evaluate comp390)
    (not (rebooted comp390))
    (not (running comp304))
  )
  :effect (and
    (not (evaluate comp390))
    (evaluate comp391)
    (one-off comp390)
  )
)
(:action evaluate-comp390-off-comp389
  :parameters ()
  :precondition (and
    (evaluate comp390)
    (not (rebooted comp390))
    (not (running comp389))
  )
  :effect (and
    (not (evaluate comp390))
    (evaluate comp391)
    (one-off comp390)
  )
)
(:action evaluate-comp391-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp391)
    (rebooted comp391)
  )
  :effect (and
    (not (evaluate comp391))
    (evaluate comp392)
  )
)
(:action evaluate-comp391-all-on
  :parameters ()
  :precondition (and
    (evaluate comp391)
    (not (rebooted comp391))
    (running comp390)
  )
  :effect (and
    (not (evaluate comp391))
    (evaluate comp392)
    (all-on comp391)
  )
)
(:action evaluate-comp391-off-comp390
  :parameters ()
  :precondition (and
    (evaluate comp391)
    (not (rebooted comp391))
    (not (running comp390))
  )
  :effect (and
    (not (evaluate comp391))
    (evaluate comp392)
    (one-off comp391)
  )
)
(:action evaluate-comp392-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp392)
    (rebooted comp392)
  )
  :effect (and
    (not (evaluate comp392))
    (evaluate comp393)
  )
)
(:action evaluate-comp392-all-on
  :parameters ()
  :precondition (and
    (evaluate comp392)
    (not (rebooted comp392))
    (running comp43)
    (running comp391)
  )
  :effect (and
    (not (evaluate comp392))
    (evaluate comp393)
    (all-on comp392)
  )
)
(:action evaluate-comp392-off-comp43
  :parameters ()
  :precondition (and
    (evaluate comp392)
    (not (rebooted comp392))
    (not (running comp43))
  )
  :effect (and
    (not (evaluate comp392))
    (evaluate comp393)
    (one-off comp392)
  )
)
(:action evaluate-comp392-off-comp391
  :parameters ()
  :precondition (and
    (evaluate comp392)
    (not (rebooted comp392))
    (not (running comp391))
  )
  :effect (and
    (not (evaluate comp392))
    (evaluate comp393)
    (one-off comp392)
  )
)
(:action evaluate-comp393-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp393)
    (rebooted comp393)
  )
  :effect (and
    (not (evaluate comp393))
    (evaluate comp394)
  )
)
(:action evaluate-comp393-all-on
  :parameters ()
  :precondition (and
    (evaluate comp393)
    (not (rebooted comp393))
    (running comp392)
  )
  :effect (and
    (not (evaluate comp393))
    (evaluate comp394)
    (all-on comp393)
  )
)
(:action evaluate-comp393-off-comp392
  :parameters ()
  :precondition (and
    (evaluate comp393)
    (not (rebooted comp393))
    (not (running comp392))
  )
  :effect (and
    (not (evaluate comp393))
    (evaluate comp394)
    (one-off comp393)
  )
)
(:action evaluate-comp394-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp394)
    (rebooted comp394)
  )
  :effect (and
    (not (evaluate comp394))
    (evaluate comp395)
  )
)
(:action evaluate-comp394-all-on
  :parameters ()
  :precondition (and
    (evaluate comp394)
    (not (rebooted comp394))
    (running comp393)
  )
  :effect (and
    (not (evaluate comp394))
    (evaluate comp395)
    (all-on comp394)
  )
)
(:action evaluate-comp394-off-comp393
  :parameters ()
  :precondition (and
    (evaluate comp394)
    (not (rebooted comp394))
    (not (running comp393))
  )
  :effect (and
    (not (evaluate comp394))
    (evaluate comp395)
    (one-off comp394)
  )
)
(:action evaluate-comp395-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp395)
    (rebooted comp395)
  )
  :effect (and
    (not (evaluate comp395))
    (evaluate comp396)
  )
)
(:action evaluate-comp395-all-on
  :parameters ()
  :precondition (and
    (evaluate comp395)
    (not (rebooted comp395))
    (running comp394)
  )
  :effect (and
    (not (evaluate comp395))
    (evaluate comp396)
    (all-on comp395)
  )
)
(:action evaluate-comp395-off-comp394
  :parameters ()
  :precondition (and
    (evaluate comp395)
    (not (rebooted comp395))
    (not (running comp394))
  )
  :effect (and
    (not (evaluate comp395))
    (evaluate comp396)
    (one-off comp395)
  )
)
(:action evaluate-comp396-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp396)
    (rebooted comp396)
  )
  :effect (and
    (not (evaluate comp396))
    (evaluate comp397)
  )
)
(:action evaluate-comp396-all-on
  :parameters ()
  :precondition (and
    (evaluate comp396)
    (not (rebooted comp396))
    (running comp23)
    (running comp395)
  )
  :effect (and
    (not (evaluate comp396))
    (evaluate comp397)
    (all-on comp396)
  )
)
(:action evaluate-comp396-off-comp23
  :parameters ()
  :precondition (and
    (evaluate comp396)
    (not (rebooted comp396))
    (not (running comp23))
  )
  :effect (and
    (not (evaluate comp396))
    (evaluate comp397)
    (one-off comp396)
  )
)
(:action evaluate-comp396-off-comp395
  :parameters ()
  :precondition (and
    (evaluate comp396)
    (not (rebooted comp396))
    (not (running comp395))
  )
  :effect (and
    (not (evaluate comp396))
    (evaluate comp397)
    (one-off comp396)
  )
)
(:action evaluate-comp397-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp397)
    (rebooted comp397)
  )
  :effect (and
    (not (evaluate comp397))
    (evaluate comp398)
  )
)
(:action evaluate-comp397-all-on
  :parameters ()
  :precondition (and
    (evaluate comp397)
    (not (rebooted comp397))
    (running comp369)
    (running comp396)
  )
  :effect (and
    (not (evaluate comp397))
    (evaluate comp398)
    (all-on comp397)
  )
)
(:action evaluate-comp397-off-comp369
  :parameters ()
  :precondition (and
    (evaluate comp397)
    (not (rebooted comp397))
    (not (running comp369))
  )
  :effect (and
    (not (evaluate comp397))
    (evaluate comp398)
    (one-off comp397)
  )
)
(:action evaluate-comp397-off-comp396
  :parameters ()
  :precondition (and
    (evaluate comp397)
    (not (rebooted comp397))
    (not (running comp396))
  )
  :effect (and
    (not (evaluate comp397))
    (evaluate comp398)
    (one-off comp397)
  )
)
(:action evaluate-comp398-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp398)
    (rebooted comp398)
  )
  :effect (and
    (not (evaluate comp398))
    (evaluate comp399)
  )
)
(:action evaluate-comp398-all-on
  :parameters ()
  :precondition (and
    (evaluate comp398)
    (not (rebooted comp398))
    (running comp161)
    (running comp397)
  )
  :effect (and
    (not (evaluate comp398))
    (evaluate comp399)
    (all-on comp398)
  )
)
(:action evaluate-comp398-off-comp161
  :parameters ()
  :precondition (and
    (evaluate comp398)
    (not (rebooted comp398))
    (not (running comp161))
  )
  :effect (and
    (not (evaluate comp398))
    (evaluate comp399)
    (one-off comp398)
  )
)
(:action evaluate-comp398-off-comp397
  :parameters ()
  :precondition (and
    (evaluate comp398)
    (not (rebooted comp398))
    (not (running comp397))
  )
  :effect (and
    (not (evaluate comp398))
    (evaluate comp399)
    (one-off comp398)
  )
)
(:action evaluate-comp399-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp399)
    (rebooted comp399)
  )
  :effect (and
    (not (evaluate comp399))
    (evaluate comp400)
  )
)
(:action evaluate-comp399-all-on
  :parameters ()
  :precondition (and
    (evaluate comp399)
    (not (rebooted comp399))
    (running comp33)
    (running comp306)
    (running comp398)
  )
  :effect (and
    (not (evaluate comp399))
    (evaluate comp400)
    (all-on comp399)
  )
)
(:action evaluate-comp399-off-comp33
  :parameters ()
  :precondition (and
    (evaluate comp399)
    (not (rebooted comp399))
    (not (running comp33))
  )
  :effect (and
    (not (evaluate comp399))
    (evaluate comp400)
    (one-off comp399)
  )
)
(:action evaluate-comp399-off-comp306
  :parameters ()
  :precondition (and
    (evaluate comp399)
    (not (rebooted comp399))
    (not (running comp306))
  )
  :effect (and
    (not (evaluate comp399))
    (evaluate comp400)
    (one-off comp399)
  )
)
(:action evaluate-comp399-off-comp398
  :parameters ()
  :precondition (and
    (evaluate comp399)
    (not (rebooted comp399))
    (not (running comp398))
  )
  :effect (and
    (not (evaluate comp399))
    (evaluate comp400)
    (one-off comp399)
  )
)
(:action evaluate-comp400-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp400)
    (rebooted comp400)
  )
  :effect (and
    (not (evaluate comp400))
    (evaluate comp401)
  )
)
(:action evaluate-comp400-all-on
  :parameters ()
  :precondition (and
    (evaluate comp400)
    (not (rebooted comp400))
    (running comp399)
  )
  :effect (and
    (not (evaluate comp400))
    (evaluate comp401)
    (all-on comp400)
  )
)
(:action evaluate-comp400-off-comp399
  :parameters ()
  :precondition (and
    (evaluate comp400)
    (not (rebooted comp400))
    (not (running comp399))
  )
  :effect (and
    (not (evaluate comp400))
    (evaluate comp401)
    (one-off comp400)
  )
)
(:action evaluate-comp401-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp401)
    (rebooted comp401)
  )
  :effect (and
    (not (evaluate comp401))
    (evaluate comp402)
  )
)
(:action evaluate-comp401-all-on
  :parameters ()
  :precondition (and
    (evaluate comp401)
    (not (rebooted comp401))
    (running comp198)
    (running comp400)
  )
  :effect (and
    (not (evaluate comp401))
    (evaluate comp402)
    (all-on comp401)
  )
)
(:action evaluate-comp401-off-comp198
  :parameters ()
  :precondition (and
    (evaluate comp401)
    (not (rebooted comp401))
    (not (running comp198))
  )
  :effect (and
    (not (evaluate comp401))
    (evaluate comp402)
    (one-off comp401)
  )
)
(:action evaluate-comp401-off-comp400
  :parameters ()
  :precondition (and
    (evaluate comp401)
    (not (rebooted comp401))
    (not (running comp400))
  )
  :effect (and
    (not (evaluate comp401))
    (evaluate comp402)
    (one-off comp401)
  )
)
(:action evaluate-comp402-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp402)
    (rebooted comp402)
  )
  :effect (and
    (not (evaluate comp402))
    (evaluate comp403)
  )
)
(:action evaluate-comp402-all-on
  :parameters ()
  :precondition (and
    (evaluate comp402)
    (not (rebooted comp402))
    (running comp401)
  )
  :effect (and
    (not (evaluate comp402))
    (evaluate comp403)
    (all-on comp402)
  )
)
(:action evaluate-comp402-off-comp401
  :parameters ()
  :precondition (and
    (evaluate comp402)
    (not (rebooted comp402))
    (not (running comp401))
  )
  :effect (and
    (not (evaluate comp402))
    (evaluate comp403)
    (one-off comp402)
  )
)
(:action evaluate-comp403-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp403)
    (rebooted comp403)
  )
  :effect (and
    (not (evaluate comp403))
    (evaluate comp404)
  )
)
(:action evaluate-comp403-all-on
  :parameters ()
  :precondition (and
    (evaluate comp403)
    (not (rebooted comp403))
    (running comp250)
    (running comp402)
  )
  :effect (and
    (not (evaluate comp403))
    (evaluate comp404)
    (all-on comp403)
  )
)
(:action evaluate-comp403-off-comp250
  :parameters ()
  :precondition (and
    (evaluate comp403)
    (not (rebooted comp403))
    (not (running comp250))
  )
  :effect (and
    (not (evaluate comp403))
    (evaluate comp404)
    (one-off comp403)
  )
)
(:action evaluate-comp403-off-comp402
  :parameters ()
  :precondition (and
    (evaluate comp403)
    (not (rebooted comp403))
    (not (running comp402))
  )
  :effect (and
    (not (evaluate comp403))
    (evaluate comp404)
    (one-off comp403)
  )
)
(:action evaluate-comp404-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp404)
    (rebooted comp404)
  )
  :effect (and
    (not (evaluate comp404))
    (evaluate comp405)
  )
)
(:action evaluate-comp404-all-on
  :parameters ()
  :precondition (and
    (evaluate comp404)
    (not (rebooted comp404))
    (running comp403)
  )
  :effect (and
    (not (evaluate comp404))
    (evaluate comp405)
    (all-on comp404)
  )
)
(:action evaluate-comp404-off-comp403
  :parameters ()
  :precondition (and
    (evaluate comp404)
    (not (rebooted comp404))
    (not (running comp403))
  )
  :effect (and
    (not (evaluate comp404))
    (evaluate comp405)
    (one-off comp404)
  )
)
(:action evaluate-comp405-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp405)
    (rebooted comp405)
  )
  :effect (and
    (not (evaluate comp405))
    (evaluate comp406)
  )
)
(:action evaluate-comp405-all-on
  :parameters ()
  :precondition (and
    (evaluate comp405)
    (not (rebooted comp405))
    (running comp404)
  )
  :effect (and
    (not (evaluate comp405))
    (evaluate comp406)
    (all-on comp405)
  )
)
(:action evaluate-comp405-off-comp404
  :parameters ()
  :precondition (and
    (evaluate comp405)
    (not (rebooted comp405))
    (not (running comp404))
  )
  :effect (and
    (not (evaluate comp405))
    (evaluate comp406)
    (one-off comp405)
  )
)
(:action evaluate-comp406-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp406)
    (rebooted comp406)
  )
  :effect (and
    (not (evaluate comp406))
    (evaluate comp407)
  )
)
(:action evaluate-comp406-all-on
  :parameters ()
  :precondition (and
    (evaluate comp406)
    (not (rebooted comp406))
    (running comp405)
  )
  :effect (and
    (not (evaluate comp406))
    (evaluate comp407)
    (all-on comp406)
  )
)
(:action evaluate-comp406-off-comp405
  :parameters ()
  :precondition (and
    (evaluate comp406)
    (not (rebooted comp406))
    (not (running comp405))
  )
  :effect (and
    (not (evaluate comp406))
    (evaluate comp407)
    (one-off comp406)
  )
)
(:action evaluate-comp407-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp407)
    (rebooted comp407)
  )
  :effect (and
    (not (evaluate comp407))
    (evaluate comp408)
  )
)
(:action evaluate-comp407-all-on
  :parameters ()
  :precondition (and
    (evaluate comp407)
    (not (rebooted comp407))
    (running comp406)
  )
  :effect (and
    (not (evaluate comp407))
    (evaluate comp408)
    (all-on comp407)
  )
)
(:action evaluate-comp407-off-comp406
  :parameters ()
  :precondition (and
    (evaluate comp407)
    (not (rebooted comp407))
    (not (running comp406))
  )
  :effect (and
    (not (evaluate comp407))
    (evaluate comp408)
    (one-off comp407)
  )
)
(:action evaluate-comp408-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp408)
    (rebooted comp408)
  )
  :effect (and
    (not (evaluate comp408))
    (evaluate comp409)
  )
)
(:action evaluate-comp408-all-on
  :parameters ()
  :precondition (and
    (evaluate comp408)
    (not (rebooted comp408))
    (running comp407)
  )
  :effect (and
    (not (evaluate comp408))
    (evaluate comp409)
    (all-on comp408)
  )
)
(:action evaluate-comp408-off-comp407
  :parameters ()
  :precondition (and
    (evaluate comp408)
    (not (rebooted comp408))
    (not (running comp407))
  )
  :effect (and
    (not (evaluate comp408))
    (evaluate comp409)
    (one-off comp408)
  )
)
(:action evaluate-comp409-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp409)
    (rebooted comp409)
  )
  :effect (and
    (not (evaluate comp409))
    (evaluate comp410)
  )
)
(:action evaluate-comp409-all-on
  :parameters ()
  :precondition (and
    (evaluate comp409)
    (not (rebooted comp409))
    (running comp408)
  )
  :effect (and
    (not (evaluate comp409))
    (evaluate comp410)
    (all-on comp409)
  )
)
(:action evaluate-comp409-off-comp408
  :parameters ()
  :precondition (and
    (evaluate comp409)
    (not (rebooted comp409))
    (not (running comp408))
  )
  :effect (and
    (not (evaluate comp409))
    (evaluate comp410)
    (one-off comp409)
  )
)
(:action evaluate-comp410-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp410)
    (rebooted comp410)
  )
  :effect (and
    (not (evaluate comp410))
    (evaluate comp411)
  )
)
(:action evaluate-comp410-all-on
  :parameters ()
  :precondition (and
    (evaluate comp410)
    (not (rebooted comp410))
    (running comp409)
  )
  :effect (and
    (not (evaluate comp410))
    (evaluate comp411)
    (all-on comp410)
  )
)
(:action evaluate-comp410-off-comp409
  :parameters ()
  :precondition (and
    (evaluate comp410)
    (not (rebooted comp410))
    (not (running comp409))
  )
  :effect (and
    (not (evaluate comp410))
    (evaluate comp411)
    (one-off comp410)
  )
)
(:action evaluate-comp411-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp411)
    (rebooted comp411)
  )
  :effect (and
    (not (evaluate comp411))
    (evaluate comp412)
  )
)
(:action evaluate-comp411-all-on
  :parameters ()
  :precondition (and
    (evaluate comp411)
    (not (rebooted comp411))
    (running comp410)
  )
  :effect (and
    (not (evaluate comp411))
    (evaluate comp412)
    (all-on comp411)
  )
)
(:action evaluate-comp411-off-comp410
  :parameters ()
  :precondition (and
    (evaluate comp411)
    (not (rebooted comp411))
    (not (running comp410))
  )
  :effect (and
    (not (evaluate comp411))
    (evaluate comp412)
    (one-off comp411)
  )
)
(:action evaluate-comp412-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp412)
    (rebooted comp412)
  )
  :effect (and
    (not (evaluate comp412))
    (evaluate comp413)
  )
)
(:action evaluate-comp412-all-on
  :parameters ()
  :precondition (and
    (evaluate comp412)
    (not (rebooted comp412))
    (running comp309)
    (running comp411)
  )
  :effect (and
    (not (evaluate comp412))
    (evaluate comp413)
    (all-on comp412)
  )
)
(:action evaluate-comp412-off-comp309
  :parameters ()
  :precondition (and
    (evaluate comp412)
    (not (rebooted comp412))
    (not (running comp309))
  )
  :effect (and
    (not (evaluate comp412))
    (evaluate comp413)
    (one-off comp412)
  )
)
(:action evaluate-comp412-off-comp411
  :parameters ()
  :precondition (and
    (evaluate comp412)
    (not (rebooted comp412))
    (not (running comp411))
  )
  :effect (and
    (not (evaluate comp412))
    (evaluate comp413)
    (one-off comp412)
  )
)
(:action evaluate-comp413-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp413)
    (rebooted comp413)
  )
  :effect (and
    (not (evaluate comp413))
    (evaluate comp414)
  )
)
(:action evaluate-comp413-all-on
  :parameters ()
  :precondition (and
    (evaluate comp413)
    (not (rebooted comp413))
    (running comp412)
  )
  :effect (and
    (not (evaluate comp413))
    (evaluate comp414)
    (all-on comp413)
  )
)
(:action evaluate-comp413-off-comp412
  :parameters ()
  :precondition (and
    (evaluate comp413)
    (not (rebooted comp413))
    (not (running comp412))
  )
  :effect (and
    (not (evaluate comp413))
    (evaluate comp414)
    (one-off comp413)
  )
)
(:action evaluate-comp414-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp414)
    (rebooted comp414)
  )
  :effect (and
    (not (evaluate comp414))
    (evaluate comp415)
  )
)
(:action evaluate-comp414-all-on
  :parameters ()
  :precondition (and
    (evaluate comp414)
    (not (rebooted comp414))
    (running comp413)
  )
  :effect (and
    (not (evaluate comp414))
    (evaluate comp415)
    (all-on comp414)
  )
)
(:action evaluate-comp414-off-comp413
  :parameters ()
  :precondition (and
    (evaluate comp414)
    (not (rebooted comp414))
    (not (running comp413))
  )
  :effect (and
    (not (evaluate comp414))
    (evaluate comp415)
    (one-off comp414)
  )
)
(:action evaluate-comp415-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp415)
    (rebooted comp415)
  )
  :effect (and
    (not (evaluate comp415))
    (evaluate comp416)
  )
)
(:action evaluate-comp415-all-on
  :parameters ()
  :precondition (and
    (evaluate comp415)
    (not (rebooted comp415))
    (running comp414)
  )
  :effect (and
    (not (evaluate comp415))
    (evaluate comp416)
    (all-on comp415)
  )
)
(:action evaluate-comp415-off-comp414
  :parameters ()
  :precondition (and
    (evaluate comp415)
    (not (rebooted comp415))
    (not (running comp414))
  )
  :effect (and
    (not (evaluate comp415))
    (evaluate comp416)
    (one-off comp415)
  )
)
(:action evaluate-comp416-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp416)
    (rebooted comp416)
  )
  :effect (and
    (not (evaluate comp416))
    (evaluate comp417)
  )
)
(:action evaluate-comp416-all-on
  :parameters ()
  :precondition (and
    (evaluate comp416)
    (not (rebooted comp416))
    (running comp42)
    (running comp415)
  )
  :effect (and
    (not (evaluate comp416))
    (evaluate comp417)
    (all-on comp416)
  )
)
(:action evaluate-comp416-off-comp42
  :parameters ()
  :precondition (and
    (evaluate comp416)
    (not (rebooted comp416))
    (not (running comp42))
  )
  :effect (and
    (not (evaluate comp416))
    (evaluate comp417)
    (one-off comp416)
  )
)
(:action evaluate-comp416-off-comp415
  :parameters ()
  :precondition (and
    (evaluate comp416)
    (not (rebooted comp416))
    (not (running comp415))
  )
  :effect (and
    (not (evaluate comp416))
    (evaluate comp417)
    (one-off comp416)
  )
)
(:action evaluate-comp417-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp417)
    (rebooted comp417)
  )
  :effect (and
    (not (evaluate comp417))
    (evaluate comp418)
  )
)
(:action evaluate-comp417-all-on
  :parameters ()
  :precondition (and
    (evaluate comp417)
    (not (rebooted comp417))
    (running comp416)
  )
  :effect (and
    (not (evaluate comp417))
    (evaluate comp418)
    (all-on comp417)
  )
)
(:action evaluate-comp417-off-comp416
  :parameters ()
  :precondition (and
    (evaluate comp417)
    (not (rebooted comp417))
    (not (running comp416))
  )
  :effect (and
    (not (evaluate comp417))
    (evaluate comp418)
    (one-off comp417)
  )
)
(:action evaluate-comp418-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp418)
    (rebooted comp418)
  )
  :effect (and
    (not (evaluate comp418))
    (evaluate comp419)
  )
)
(:action evaluate-comp418-all-on
  :parameters ()
  :precondition (and
    (evaluate comp418)
    (not (rebooted comp418))
    (running comp156)
    (running comp417)
    (running comp452)
  )
  :effect (and
    (not (evaluate comp418))
    (evaluate comp419)
    (all-on comp418)
  )
)
(:action evaluate-comp418-off-comp156
  :parameters ()
  :precondition (and
    (evaluate comp418)
    (not (rebooted comp418))
    (not (running comp156))
  )
  :effect (and
    (not (evaluate comp418))
    (evaluate comp419)
    (one-off comp418)
  )
)
(:action evaluate-comp418-off-comp417
  :parameters ()
  :precondition (and
    (evaluate comp418)
    (not (rebooted comp418))
    (not (running comp417))
  )
  :effect (and
    (not (evaluate comp418))
    (evaluate comp419)
    (one-off comp418)
  )
)
(:action evaluate-comp418-off-comp452
  :parameters ()
  :precondition (and
    (evaluate comp418)
    (not (rebooted comp418))
    (not (running comp452))
  )
  :effect (and
    (not (evaluate comp418))
    (evaluate comp419)
    (one-off comp418)
  )
)
(:action evaluate-comp419-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp419)
    (rebooted comp419)
  )
  :effect (and
    (not (evaluate comp419))
    (evaluate comp420)
  )
)
(:action evaluate-comp419-all-on
  :parameters ()
  :precondition (and
    (evaluate comp419)
    (not (rebooted comp419))
    (running comp139)
    (running comp418)
  )
  :effect (and
    (not (evaluate comp419))
    (evaluate comp420)
    (all-on comp419)
  )
)
(:action evaluate-comp419-off-comp139
  :parameters ()
  :precondition (and
    (evaluate comp419)
    (not (rebooted comp419))
    (not (running comp139))
  )
  :effect (and
    (not (evaluate comp419))
    (evaluate comp420)
    (one-off comp419)
  )
)
(:action evaluate-comp419-off-comp418
  :parameters ()
  :precondition (and
    (evaluate comp419)
    (not (rebooted comp419))
    (not (running comp418))
  )
  :effect (and
    (not (evaluate comp419))
    (evaluate comp420)
    (one-off comp419)
  )
)
(:action evaluate-comp420-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp420)
    (rebooted comp420)
  )
  :effect (and
    (not (evaluate comp420))
    (evaluate comp421)
  )
)
(:action evaluate-comp420-all-on
  :parameters ()
  :precondition (and
    (evaluate comp420)
    (not (rebooted comp420))
    (running comp419)
  )
  :effect (and
    (not (evaluate comp420))
    (evaluate comp421)
    (all-on comp420)
  )
)
(:action evaluate-comp420-off-comp419
  :parameters ()
  :precondition (and
    (evaluate comp420)
    (not (rebooted comp420))
    (not (running comp419))
  )
  :effect (and
    (not (evaluate comp420))
    (evaluate comp421)
    (one-off comp420)
  )
)
(:action evaluate-comp421-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp421)
    (rebooted comp421)
  )
  :effect (and
    (not (evaluate comp421))
    (evaluate comp422)
  )
)
(:action evaluate-comp421-all-on
  :parameters ()
  :precondition (and
    (evaluate comp421)
    (not (rebooted comp421))
    (running comp143)
    (running comp420)
  )
  :effect (and
    (not (evaluate comp421))
    (evaluate comp422)
    (all-on comp421)
  )
)
(:action evaluate-comp421-off-comp143
  :parameters ()
  :precondition (and
    (evaluate comp421)
    (not (rebooted comp421))
    (not (running comp143))
  )
  :effect (and
    (not (evaluate comp421))
    (evaluate comp422)
    (one-off comp421)
  )
)
(:action evaluate-comp421-off-comp420
  :parameters ()
  :precondition (and
    (evaluate comp421)
    (not (rebooted comp421))
    (not (running comp420))
  )
  :effect (and
    (not (evaluate comp421))
    (evaluate comp422)
    (one-off comp421)
  )
)
(:action evaluate-comp422-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp422)
    (rebooted comp422)
  )
  :effect (and
    (not (evaluate comp422))
    (evaluate comp423)
  )
)
(:action evaluate-comp422-all-on
  :parameters ()
  :precondition (and
    (evaluate comp422)
    (not (rebooted comp422))
    (running comp421)
  )
  :effect (and
    (not (evaluate comp422))
    (evaluate comp423)
    (all-on comp422)
  )
)
(:action evaluate-comp422-off-comp421
  :parameters ()
  :precondition (and
    (evaluate comp422)
    (not (rebooted comp422))
    (not (running comp421))
  )
  :effect (and
    (not (evaluate comp422))
    (evaluate comp423)
    (one-off comp422)
  )
)
(:action evaluate-comp423-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp423)
    (rebooted comp423)
  )
  :effect (and
    (not (evaluate comp423))
    (evaluate comp424)
  )
)
(:action evaluate-comp423-all-on
  :parameters ()
  :precondition (and
    (evaluate comp423)
    (not (rebooted comp423))
    (running comp45)
    (running comp422)
  )
  :effect (and
    (not (evaluate comp423))
    (evaluate comp424)
    (all-on comp423)
  )
)
(:action evaluate-comp423-off-comp45
  :parameters ()
  :precondition (and
    (evaluate comp423)
    (not (rebooted comp423))
    (not (running comp45))
  )
  :effect (and
    (not (evaluate comp423))
    (evaluate comp424)
    (one-off comp423)
  )
)
(:action evaluate-comp423-off-comp422
  :parameters ()
  :precondition (and
    (evaluate comp423)
    (not (rebooted comp423))
    (not (running comp422))
  )
  :effect (and
    (not (evaluate comp423))
    (evaluate comp424)
    (one-off comp423)
  )
)
(:action evaluate-comp424-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp424)
    (rebooted comp424)
  )
  :effect (and
    (not (evaluate comp424))
    (evaluate comp425)
  )
)
(:action evaluate-comp424-all-on
  :parameters ()
  :precondition (and
    (evaluate comp424)
    (not (rebooted comp424))
    (running comp164)
    (running comp368)
    (running comp423)
  )
  :effect (and
    (not (evaluate comp424))
    (evaluate comp425)
    (all-on comp424)
  )
)
(:action evaluate-comp424-off-comp164
  :parameters ()
  :precondition (and
    (evaluate comp424)
    (not (rebooted comp424))
    (not (running comp164))
  )
  :effect (and
    (not (evaluate comp424))
    (evaluate comp425)
    (one-off comp424)
  )
)
(:action evaluate-comp424-off-comp368
  :parameters ()
  :precondition (and
    (evaluate comp424)
    (not (rebooted comp424))
    (not (running comp368))
  )
  :effect (and
    (not (evaluate comp424))
    (evaluate comp425)
    (one-off comp424)
  )
)
(:action evaluate-comp424-off-comp423
  :parameters ()
  :precondition (and
    (evaluate comp424)
    (not (rebooted comp424))
    (not (running comp423))
  )
  :effect (and
    (not (evaluate comp424))
    (evaluate comp425)
    (one-off comp424)
  )
)
(:action evaluate-comp425-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp425)
    (rebooted comp425)
  )
  :effect (and
    (not (evaluate comp425))
    (evaluate comp426)
  )
)
(:action evaluate-comp425-all-on
  :parameters ()
  :precondition (and
    (evaluate comp425)
    (not (rebooted comp425))
    (running comp424)
  )
  :effect (and
    (not (evaluate comp425))
    (evaluate comp426)
    (all-on comp425)
  )
)
(:action evaluate-comp425-off-comp424
  :parameters ()
  :precondition (and
    (evaluate comp425)
    (not (rebooted comp425))
    (not (running comp424))
  )
  :effect (and
    (not (evaluate comp425))
    (evaluate comp426)
    (one-off comp425)
  )
)
(:action evaluate-comp426-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp426)
    (rebooted comp426)
  )
  :effect (and
    (not (evaluate comp426))
    (evaluate comp427)
  )
)
(:action evaluate-comp426-all-on
  :parameters ()
  :precondition (and
    (evaluate comp426)
    (not (rebooted comp426))
    (running comp425)
  )
  :effect (and
    (not (evaluate comp426))
    (evaluate comp427)
    (all-on comp426)
  )
)
(:action evaluate-comp426-off-comp425
  :parameters ()
  :precondition (and
    (evaluate comp426)
    (not (rebooted comp426))
    (not (running comp425))
  )
  :effect (and
    (not (evaluate comp426))
    (evaluate comp427)
    (one-off comp426)
  )
)
(:action evaluate-comp427-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp427)
    (rebooted comp427)
  )
  :effect (and
    (not (evaluate comp427))
    (evaluate comp428)
  )
)
(:action evaluate-comp427-all-on
  :parameters ()
  :precondition (and
    (evaluate comp427)
    (not (rebooted comp427))
    (running comp193)
    (running comp426)
    (running comp444)
  )
  :effect (and
    (not (evaluate comp427))
    (evaluate comp428)
    (all-on comp427)
  )
)
(:action evaluate-comp427-off-comp193
  :parameters ()
  :precondition (and
    (evaluate comp427)
    (not (rebooted comp427))
    (not (running comp193))
  )
  :effect (and
    (not (evaluate comp427))
    (evaluate comp428)
    (one-off comp427)
  )
)
(:action evaluate-comp427-off-comp426
  :parameters ()
  :precondition (and
    (evaluate comp427)
    (not (rebooted comp427))
    (not (running comp426))
  )
  :effect (and
    (not (evaluate comp427))
    (evaluate comp428)
    (one-off comp427)
  )
)
(:action evaluate-comp427-off-comp444
  :parameters ()
  :precondition (and
    (evaluate comp427)
    (not (rebooted comp427))
    (not (running comp444))
  )
  :effect (and
    (not (evaluate comp427))
    (evaluate comp428)
    (one-off comp427)
  )
)
(:action evaluate-comp428-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp428)
    (rebooted comp428)
  )
  :effect (and
    (not (evaluate comp428))
    (evaluate comp429)
  )
)
(:action evaluate-comp428-all-on
  :parameters ()
  :precondition (and
    (evaluate comp428)
    (not (rebooted comp428))
    (running comp427)
  )
  :effect (and
    (not (evaluate comp428))
    (evaluate comp429)
    (all-on comp428)
  )
)
(:action evaluate-comp428-off-comp427
  :parameters ()
  :precondition (and
    (evaluate comp428)
    (not (rebooted comp428))
    (not (running comp427))
  )
  :effect (and
    (not (evaluate comp428))
    (evaluate comp429)
    (one-off comp428)
  )
)
(:action evaluate-comp429-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp429)
    (rebooted comp429)
  )
  :effect (and
    (not (evaluate comp429))
    (evaluate comp430)
  )
)
(:action evaluate-comp429-all-on
  :parameters ()
  :precondition (and
    (evaluate comp429)
    (not (rebooted comp429))
    (running comp117)
    (running comp428)
  )
  :effect (and
    (not (evaluate comp429))
    (evaluate comp430)
    (all-on comp429)
  )
)
(:action evaluate-comp429-off-comp117
  :parameters ()
  :precondition (and
    (evaluate comp429)
    (not (rebooted comp429))
    (not (running comp117))
  )
  :effect (and
    (not (evaluate comp429))
    (evaluate comp430)
    (one-off comp429)
  )
)
(:action evaluate-comp429-off-comp428
  :parameters ()
  :precondition (and
    (evaluate comp429)
    (not (rebooted comp429))
    (not (running comp428))
  )
  :effect (and
    (not (evaluate comp429))
    (evaluate comp430)
    (one-off comp429)
  )
)
(:action evaluate-comp430-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp430)
    (rebooted comp430)
  )
  :effect (and
    (not (evaluate comp430))
    (evaluate comp431)
  )
)
(:action evaluate-comp430-all-on
  :parameters ()
  :precondition (and
    (evaluate comp430)
    (not (rebooted comp430))
    (running comp429)
  )
  :effect (and
    (not (evaluate comp430))
    (evaluate comp431)
    (all-on comp430)
  )
)
(:action evaluate-comp430-off-comp429
  :parameters ()
  :precondition (and
    (evaluate comp430)
    (not (rebooted comp430))
    (not (running comp429))
  )
  :effect (and
    (not (evaluate comp430))
    (evaluate comp431)
    (one-off comp430)
  )
)
(:action evaluate-comp431-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp431)
    (rebooted comp431)
  )
  :effect (and
    (not (evaluate comp431))
    (evaluate comp432)
  )
)
(:action evaluate-comp431-all-on
  :parameters ()
  :precondition (and
    (evaluate comp431)
    (not (rebooted comp431))
    (running comp430)
  )
  :effect (and
    (not (evaluate comp431))
    (evaluate comp432)
    (all-on comp431)
  )
)
(:action evaluate-comp431-off-comp430
  :parameters ()
  :precondition (and
    (evaluate comp431)
    (not (rebooted comp431))
    (not (running comp430))
  )
  :effect (and
    (not (evaluate comp431))
    (evaluate comp432)
    (one-off comp431)
  )
)
(:action evaluate-comp432-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp432)
    (rebooted comp432)
  )
  :effect (and
    (not (evaluate comp432))
    (evaluate comp433)
  )
)
(:action evaluate-comp432-all-on
  :parameters ()
  :precondition (and
    (evaluate comp432)
    (not (rebooted comp432))
    (running comp302)
    (running comp431)
  )
  :effect (and
    (not (evaluate comp432))
    (evaluate comp433)
    (all-on comp432)
  )
)
(:action evaluate-comp432-off-comp302
  :parameters ()
  :precondition (and
    (evaluate comp432)
    (not (rebooted comp432))
    (not (running comp302))
  )
  :effect (and
    (not (evaluate comp432))
    (evaluate comp433)
    (one-off comp432)
  )
)
(:action evaluate-comp432-off-comp431
  :parameters ()
  :precondition (and
    (evaluate comp432)
    (not (rebooted comp432))
    (not (running comp431))
  )
  :effect (and
    (not (evaluate comp432))
    (evaluate comp433)
    (one-off comp432)
  )
)
(:action evaluate-comp433-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp433)
    (rebooted comp433)
  )
  :effect (and
    (not (evaluate comp433))
    (evaluate comp434)
  )
)
(:action evaluate-comp433-all-on
  :parameters ()
  :precondition (and
    (evaluate comp433)
    (not (rebooted comp433))
    (running comp432)
  )
  :effect (and
    (not (evaluate comp433))
    (evaluate comp434)
    (all-on comp433)
  )
)
(:action evaluate-comp433-off-comp432
  :parameters ()
  :precondition (and
    (evaluate comp433)
    (not (rebooted comp433))
    (not (running comp432))
  )
  :effect (and
    (not (evaluate comp433))
    (evaluate comp434)
    (one-off comp433)
  )
)
(:action evaluate-comp434-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp434)
    (rebooted comp434)
  )
  :effect (and
    (not (evaluate comp434))
    (evaluate comp435)
  )
)
(:action evaluate-comp434-all-on
  :parameters ()
  :precondition (and
    (evaluate comp434)
    (not (rebooted comp434))
    (running comp139)
    (running comp433)
  )
  :effect (and
    (not (evaluate comp434))
    (evaluate comp435)
    (all-on comp434)
  )
)
(:action evaluate-comp434-off-comp139
  :parameters ()
  :precondition (and
    (evaluate comp434)
    (not (rebooted comp434))
    (not (running comp139))
  )
  :effect (and
    (not (evaluate comp434))
    (evaluate comp435)
    (one-off comp434)
  )
)
(:action evaluate-comp434-off-comp433
  :parameters ()
  :precondition (and
    (evaluate comp434)
    (not (rebooted comp434))
    (not (running comp433))
  )
  :effect (and
    (not (evaluate comp434))
    (evaluate comp435)
    (one-off comp434)
  )
)
(:action evaluate-comp435-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp435)
    (rebooted comp435)
  )
  :effect (and
    (not (evaluate comp435))
    (evaluate comp436)
  )
)
(:action evaluate-comp435-all-on
  :parameters ()
  :precondition (and
    (evaluate comp435)
    (not (rebooted comp435))
    (running comp434)
  )
  :effect (and
    (not (evaluate comp435))
    (evaluate comp436)
    (all-on comp435)
  )
)
(:action evaluate-comp435-off-comp434
  :parameters ()
  :precondition (and
    (evaluate comp435)
    (not (rebooted comp435))
    (not (running comp434))
  )
  :effect (and
    (not (evaluate comp435))
    (evaluate comp436)
    (one-off comp435)
  )
)
(:action evaluate-comp436-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp436)
    (rebooted comp436)
  )
  :effect (and
    (not (evaluate comp436))
    (evaluate comp437)
  )
)
(:action evaluate-comp436-all-on
  :parameters ()
  :precondition (and
    (evaluate comp436)
    (not (rebooted comp436))
    (running comp274)
    (running comp435)
  )
  :effect (and
    (not (evaluate comp436))
    (evaluate comp437)
    (all-on comp436)
  )
)
(:action evaluate-comp436-off-comp274
  :parameters ()
  :precondition (and
    (evaluate comp436)
    (not (rebooted comp436))
    (not (running comp274))
  )
  :effect (and
    (not (evaluate comp436))
    (evaluate comp437)
    (one-off comp436)
  )
)
(:action evaluate-comp436-off-comp435
  :parameters ()
  :precondition (and
    (evaluate comp436)
    (not (rebooted comp436))
    (not (running comp435))
  )
  :effect (and
    (not (evaluate comp436))
    (evaluate comp437)
    (one-off comp436)
  )
)
(:action evaluate-comp437-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp437)
    (rebooted comp437)
  )
  :effect (and
    (not (evaluate comp437))
    (evaluate comp438)
  )
)
(:action evaluate-comp437-all-on
  :parameters ()
  :precondition (and
    (evaluate comp437)
    (not (rebooted comp437))
    (running comp333)
    (running comp436)
  )
  :effect (and
    (not (evaluate comp437))
    (evaluate comp438)
    (all-on comp437)
  )
)
(:action evaluate-comp437-off-comp333
  :parameters ()
  :precondition (and
    (evaluate comp437)
    (not (rebooted comp437))
    (not (running comp333))
  )
  :effect (and
    (not (evaluate comp437))
    (evaluate comp438)
    (one-off comp437)
  )
)
(:action evaluate-comp437-off-comp436
  :parameters ()
  :precondition (and
    (evaluate comp437)
    (not (rebooted comp437))
    (not (running comp436))
  )
  :effect (and
    (not (evaluate comp437))
    (evaluate comp438)
    (one-off comp437)
  )
)
(:action evaluate-comp438-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp438)
    (rebooted comp438)
  )
  :effect (and
    (not (evaluate comp438))
    (evaluate comp439)
  )
)
(:action evaluate-comp438-all-on
  :parameters ()
  :precondition (and
    (evaluate comp438)
    (not (rebooted comp438))
    (running comp437)
  )
  :effect (and
    (not (evaluate comp438))
    (evaluate comp439)
    (all-on comp438)
  )
)
(:action evaluate-comp438-off-comp437
  :parameters ()
  :precondition (and
    (evaluate comp438)
    (not (rebooted comp438))
    (not (running comp437))
  )
  :effect (and
    (not (evaluate comp438))
    (evaluate comp439)
    (one-off comp438)
  )
)
(:action evaluate-comp439-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp439)
    (rebooted comp439)
  )
  :effect (and
    (not (evaluate comp439))
    (evaluate comp440)
  )
)
(:action evaluate-comp439-all-on
  :parameters ()
  :precondition (and
    (evaluate comp439)
    (not (rebooted comp439))
    (running comp438)
  )
  :effect (and
    (not (evaluate comp439))
    (evaluate comp440)
    (all-on comp439)
  )
)
(:action evaluate-comp439-off-comp438
  :parameters ()
  :precondition (and
    (evaluate comp439)
    (not (rebooted comp439))
    (not (running comp438))
  )
  :effect (and
    (not (evaluate comp439))
    (evaluate comp440)
    (one-off comp439)
  )
)
(:action evaluate-comp440-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp440)
    (rebooted comp440)
  )
  :effect (and
    (not (evaluate comp440))
    (evaluate comp441)
  )
)
(:action evaluate-comp440-all-on
  :parameters ()
  :precondition (and
    (evaluate comp440)
    (not (rebooted comp440))
    (running comp439)
  )
  :effect (and
    (not (evaluate comp440))
    (evaluate comp441)
    (all-on comp440)
  )
)
(:action evaluate-comp440-off-comp439
  :parameters ()
  :precondition (and
    (evaluate comp440)
    (not (rebooted comp440))
    (not (running comp439))
  )
  :effect (and
    (not (evaluate comp440))
    (evaluate comp441)
    (one-off comp440)
  )
)
(:action evaluate-comp441-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp441)
    (rebooted comp441)
  )
  :effect (and
    (not (evaluate comp441))
    (evaluate comp442)
  )
)
(:action evaluate-comp441-all-on
  :parameters ()
  :precondition (and
    (evaluate comp441)
    (not (rebooted comp441))
    (running comp440)
  )
  :effect (and
    (not (evaluate comp441))
    (evaluate comp442)
    (all-on comp441)
  )
)
(:action evaluate-comp441-off-comp440
  :parameters ()
  :precondition (and
    (evaluate comp441)
    (not (rebooted comp441))
    (not (running comp440))
  )
  :effect (and
    (not (evaluate comp441))
    (evaluate comp442)
    (one-off comp441)
  )
)
(:action evaluate-comp442-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp442)
    (rebooted comp442)
  )
  :effect (and
    (not (evaluate comp442))
    (evaluate comp443)
  )
)
(:action evaluate-comp442-all-on
  :parameters ()
  :precondition (and
    (evaluate comp442)
    (not (rebooted comp442))
    (running comp441)
  )
  :effect (and
    (not (evaluate comp442))
    (evaluate comp443)
    (all-on comp442)
  )
)
(:action evaluate-comp442-off-comp441
  :parameters ()
  :precondition (and
    (evaluate comp442)
    (not (rebooted comp442))
    (not (running comp441))
  )
  :effect (and
    (not (evaluate comp442))
    (evaluate comp443)
    (one-off comp442)
  )
)
(:action evaluate-comp443-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp443)
    (rebooted comp443)
  )
  :effect (and
    (not (evaluate comp443))
    (evaluate comp444)
  )
)
(:action evaluate-comp443-all-on
  :parameters ()
  :precondition (and
    (evaluate comp443)
    (not (rebooted comp443))
    (running comp442)
  )
  :effect (and
    (not (evaluate comp443))
    (evaluate comp444)
    (all-on comp443)
  )
)
(:action evaluate-comp443-off-comp442
  :parameters ()
  :precondition (and
    (evaluate comp443)
    (not (rebooted comp443))
    (not (running comp442))
  )
  :effect (and
    (not (evaluate comp443))
    (evaluate comp444)
    (one-off comp443)
  )
)
(:action evaluate-comp444-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp444)
    (rebooted comp444)
  )
  :effect (and
    (not (evaluate comp444))
    (evaluate comp445)
  )
)
(:action evaluate-comp444-all-on
  :parameters ()
  :precondition (and
    (evaluate comp444)
    (not (rebooted comp444))
    (running comp443)
  )
  :effect (and
    (not (evaluate comp444))
    (evaluate comp445)
    (all-on comp444)
  )
)
(:action evaluate-comp444-off-comp443
  :parameters ()
  :precondition (and
    (evaluate comp444)
    (not (rebooted comp444))
    (not (running comp443))
  )
  :effect (and
    (not (evaluate comp444))
    (evaluate comp445)
    (one-off comp444)
  )
)
(:action evaluate-comp445-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp445)
    (rebooted comp445)
  )
  :effect (and
    (not (evaluate comp445))
    (evaluate comp446)
  )
)
(:action evaluate-comp445-all-on
  :parameters ()
  :precondition (and
    (evaluate comp445)
    (not (rebooted comp445))
    (running comp444)
  )
  :effect (and
    (not (evaluate comp445))
    (evaluate comp446)
    (all-on comp445)
  )
)
(:action evaluate-comp445-off-comp444
  :parameters ()
  :precondition (and
    (evaluate comp445)
    (not (rebooted comp445))
    (not (running comp444))
  )
  :effect (and
    (not (evaluate comp445))
    (evaluate comp446)
    (one-off comp445)
  )
)
(:action evaluate-comp446-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp446)
    (rebooted comp446)
  )
  :effect (and
    (not (evaluate comp446))
    (evaluate comp447)
  )
)
(:action evaluate-comp446-all-on
  :parameters ()
  :precondition (and
    (evaluate comp446)
    (not (rebooted comp446))
    (running comp250)
    (running comp445)
    (running comp478)
  )
  :effect (and
    (not (evaluate comp446))
    (evaluate comp447)
    (all-on comp446)
  )
)
(:action evaluate-comp446-off-comp250
  :parameters ()
  :precondition (and
    (evaluate comp446)
    (not (rebooted comp446))
    (not (running comp250))
  )
  :effect (and
    (not (evaluate comp446))
    (evaluate comp447)
    (one-off comp446)
  )
)
(:action evaluate-comp446-off-comp445
  :parameters ()
  :precondition (and
    (evaluate comp446)
    (not (rebooted comp446))
    (not (running comp445))
  )
  :effect (and
    (not (evaluate comp446))
    (evaluate comp447)
    (one-off comp446)
  )
)
(:action evaluate-comp446-off-comp478
  :parameters ()
  :precondition (and
    (evaluate comp446)
    (not (rebooted comp446))
    (not (running comp478))
  )
  :effect (and
    (not (evaluate comp446))
    (evaluate comp447)
    (one-off comp446)
  )
)
(:action evaluate-comp447-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp447)
    (rebooted comp447)
  )
  :effect (and
    (not (evaluate comp447))
    (evaluate comp448)
  )
)
(:action evaluate-comp447-all-on
  :parameters ()
  :precondition (and
    (evaluate comp447)
    (not (rebooted comp447))
    (running comp446)
  )
  :effect (and
    (not (evaluate comp447))
    (evaluate comp448)
    (all-on comp447)
  )
)
(:action evaluate-comp447-off-comp446
  :parameters ()
  :precondition (and
    (evaluate comp447)
    (not (rebooted comp447))
    (not (running comp446))
  )
  :effect (and
    (not (evaluate comp447))
    (evaluate comp448)
    (one-off comp447)
  )
)
(:action evaluate-comp448-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp448)
    (rebooted comp448)
  )
  :effect (and
    (not (evaluate comp448))
    (evaluate comp449)
  )
)
(:action evaluate-comp448-all-on
  :parameters ()
  :precondition (and
    (evaluate comp448)
    (not (rebooted comp448))
    (running comp447)
  )
  :effect (and
    (not (evaluate comp448))
    (evaluate comp449)
    (all-on comp448)
  )
)
(:action evaluate-comp448-off-comp447
  :parameters ()
  :precondition (and
    (evaluate comp448)
    (not (rebooted comp448))
    (not (running comp447))
  )
  :effect (and
    (not (evaluate comp448))
    (evaluate comp449)
    (one-off comp448)
  )
)
(:action evaluate-comp449-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp449)
    (rebooted comp449)
  )
  :effect (and
    (not (evaluate comp449))
    (evaluate comp450)
  )
)
(:action evaluate-comp449-all-on
  :parameters ()
  :precondition (and
    (evaluate comp449)
    (not (rebooted comp449))
    (running comp410)
    (running comp448)
  )
  :effect (and
    (not (evaluate comp449))
    (evaluate comp450)
    (all-on comp449)
  )
)
(:action evaluate-comp449-off-comp410
  :parameters ()
  :precondition (and
    (evaluate comp449)
    (not (rebooted comp449))
    (not (running comp410))
  )
  :effect (and
    (not (evaluate comp449))
    (evaluate comp450)
    (one-off comp449)
  )
)
(:action evaluate-comp449-off-comp448
  :parameters ()
  :precondition (and
    (evaluate comp449)
    (not (rebooted comp449))
    (not (running comp448))
  )
  :effect (and
    (not (evaluate comp449))
    (evaluate comp450)
    (one-off comp449)
  )
)
(:action evaluate-comp450-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp450)
    (rebooted comp450)
  )
  :effect (and
    (not (evaluate comp450))
    (evaluate comp451)
  )
)
(:action evaluate-comp450-all-on
  :parameters ()
  :precondition (and
    (evaluate comp450)
    (not (rebooted comp450))
    (running comp449)
  )
  :effect (and
    (not (evaluate comp450))
    (evaluate comp451)
    (all-on comp450)
  )
)
(:action evaluate-comp450-off-comp449
  :parameters ()
  :precondition (and
    (evaluate comp450)
    (not (rebooted comp450))
    (not (running comp449))
  )
  :effect (and
    (not (evaluate comp450))
    (evaluate comp451)
    (one-off comp450)
  )
)
(:action evaluate-comp451-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp451)
    (rebooted comp451)
  )
  :effect (and
    (not (evaluate comp451))
    (evaluate comp452)
  )
)
(:action evaluate-comp451-all-on
  :parameters ()
  :precondition (and
    (evaluate comp451)
    (not (rebooted comp451))
    (running comp149)
    (running comp450)
  )
  :effect (and
    (not (evaluate comp451))
    (evaluate comp452)
    (all-on comp451)
  )
)
(:action evaluate-comp451-off-comp149
  :parameters ()
  :precondition (and
    (evaluate comp451)
    (not (rebooted comp451))
    (not (running comp149))
  )
  :effect (and
    (not (evaluate comp451))
    (evaluate comp452)
    (one-off comp451)
  )
)
(:action evaluate-comp451-off-comp450
  :parameters ()
  :precondition (and
    (evaluate comp451)
    (not (rebooted comp451))
    (not (running comp450))
  )
  :effect (and
    (not (evaluate comp451))
    (evaluate comp452)
    (one-off comp451)
  )
)
(:action evaluate-comp452-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp452)
    (rebooted comp452)
  )
  :effect (and
    (not (evaluate comp452))
    (evaluate comp453)
  )
)
(:action evaluate-comp452-all-on
  :parameters ()
  :precondition (and
    (evaluate comp452)
    (not (rebooted comp452))
    (running comp67)
    (running comp85)
    (running comp451)
  )
  :effect (and
    (not (evaluate comp452))
    (evaluate comp453)
    (all-on comp452)
  )
)
(:action evaluate-comp452-off-comp67
  :parameters ()
  :precondition (and
    (evaluate comp452)
    (not (rebooted comp452))
    (not (running comp67))
  )
  :effect (and
    (not (evaluate comp452))
    (evaluate comp453)
    (one-off comp452)
  )
)
(:action evaluate-comp452-off-comp85
  :parameters ()
  :precondition (and
    (evaluate comp452)
    (not (rebooted comp452))
    (not (running comp85))
  )
  :effect (and
    (not (evaluate comp452))
    (evaluate comp453)
    (one-off comp452)
  )
)
(:action evaluate-comp452-off-comp451
  :parameters ()
  :precondition (and
    (evaluate comp452)
    (not (rebooted comp452))
    (not (running comp451))
  )
  :effect (and
    (not (evaluate comp452))
    (evaluate comp453)
    (one-off comp452)
  )
)
(:action evaluate-comp453-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp453)
    (rebooted comp453)
  )
  :effect (and
    (not (evaluate comp453))
    (evaluate comp454)
  )
)
(:action evaluate-comp453-all-on
  :parameters ()
  :precondition (and
    (evaluate comp453)
    (not (rebooted comp453))
    (running comp452)
  )
  :effect (and
    (not (evaluate comp453))
    (evaluate comp454)
    (all-on comp453)
  )
)
(:action evaluate-comp453-off-comp452
  :parameters ()
  :precondition (and
    (evaluate comp453)
    (not (rebooted comp453))
    (not (running comp452))
  )
  :effect (and
    (not (evaluate comp453))
    (evaluate comp454)
    (one-off comp453)
  )
)
(:action evaluate-comp454-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp454)
    (rebooted comp454)
  )
  :effect (and
    (not (evaluate comp454))
    (evaluate comp455)
  )
)
(:action evaluate-comp454-all-on
  :parameters ()
  :precondition (and
    (evaluate comp454)
    (not (rebooted comp454))
    (running comp453)
  )
  :effect (and
    (not (evaluate comp454))
    (evaluate comp455)
    (all-on comp454)
  )
)
(:action evaluate-comp454-off-comp453
  :parameters ()
  :precondition (and
    (evaluate comp454)
    (not (rebooted comp454))
    (not (running comp453))
  )
  :effect (and
    (not (evaluate comp454))
    (evaluate comp455)
    (one-off comp454)
  )
)
(:action evaluate-comp455-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp455)
    (rebooted comp455)
  )
  :effect (and
    (not (evaluate comp455))
    (evaluate comp456)
  )
)
(:action evaluate-comp455-all-on
  :parameters ()
  :precondition (and
    (evaluate comp455)
    (not (rebooted comp455))
    (running comp454)
  )
  :effect (and
    (not (evaluate comp455))
    (evaluate comp456)
    (all-on comp455)
  )
)
(:action evaluate-comp455-off-comp454
  :parameters ()
  :precondition (and
    (evaluate comp455)
    (not (rebooted comp455))
    (not (running comp454))
  )
  :effect (and
    (not (evaluate comp455))
    (evaluate comp456)
    (one-off comp455)
  )
)
(:action evaluate-comp456-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp456)
    (rebooted comp456)
  )
  :effect (and
    (not (evaluate comp456))
    (evaluate comp457)
  )
)
(:action evaluate-comp456-all-on
  :parameters ()
  :precondition (and
    (evaluate comp456)
    (not (rebooted comp456))
    (running comp455)
  )
  :effect (and
    (not (evaluate comp456))
    (evaluate comp457)
    (all-on comp456)
  )
)
(:action evaluate-comp456-off-comp455
  :parameters ()
  :precondition (and
    (evaluate comp456)
    (not (rebooted comp456))
    (not (running comp455))
  )
  :effect (and
    (not (evaluate comp456))
    (evaluate comp457)
    (one-off comp456)
  )
)
(:action evaluate-comp457-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp457)
    (rebooted comp457)
  )
  :effect (and
    (not (evaluate comp457))
    (evaluate comp458)
  )
)
(:action evaluate-comp457-all-on
  :parameters ()
  :precondition (and
    (evaluate comp457)
    (not (rebooted comp457))
    (running comp39)
    (running comp456)
  )
  :effect (and
    (not (evaluate comp457))
    (evaluate comp458)
    (all-on comp457)
  )
)
(:action evaluate-comp457-off-comp39
  :parameters ()
  :precondition (and
    (evaluate comp457)
    (not (rebooted comp457))
    (not (running comp39))
  )
  :effect (and
    (not (evaluate comp457))
    (evaluate comp458)
    (one-off comp457)
  )
)
(:action evaluate-comp457-off-comp456
  :parameters ()
  :precondition (and
    (evaluate comp457)
    (not (rebooted comp457))
    (not (running comp456))
  )
  :effect (and
    (not (evaluate comp457))
    (evaluate comp458)
    (one-off comp457)
  )
)
(:action evaluate-comp458-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp458)
    (rebooted comp458)
  )
  :effect (and
    (not (evaluate comp458))
    (evaluate comp459)
  )
)
(:action evaluate-comp458-all-on
  :parameters ()
  :precondition (and
    (evaluate comp458)
    (not (rebooted comp458))
    (running comp357)
    (running comp457)
  )
  :effect (and
    (not (evaluate comp458))
    (evaluate comp459)
    (all-on comp458)
  )
)
(:action evaluate-comp458-off-comp357
  :parameters ()
  :precondition (and
    (evaluate comp458)
    (not (rebooted comp458))
    (not (running comp357))
  )
  :effect (and
    (not (evaluate comp458))
    (evaluate comp459)
    (one-off comp458)
  )
)
(:action evaluate-comp458-off-comp457
  :parameters ()
  :precondition (and
    (evaluate comp458)
    (not (rebooted comp458))
    (not (running comp457))
  )
  :effect (and
    (not (evaluate comp458))
    (evaluate comp459)
    (one-off comp458)
  )
)
(:action evaluate-comp459-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp459)
    (rebooted comp459)
  )
  :effect (and
    (not (evaluate comp459))
    (evaluate comp460)
  )
)
(:action evaluate-comp459-all-on
  :parameters ()
  :precondition (and
    (evaluate comp459)
    (not (rebooted comp459))
    (running comp159)
    (running comp458)
  )
  :effect (and
    (not (evaluate comp459))
    (evaluate comp460)
    (all-on comp459)
  )
)
(:action evaluate-comp459-off-comp159
  :parameters ()
  :precondition (and
    (evaluate comp459)
    (not (rebooted comp459))
    (not (running comp159))
  )
  :effect (and
    (not (evaluate comp459))
    (evaluate comp460)
    (one-off comp459)
  )
)
(:action evaluate-comp459-off-comp458
  :parameters ()
  :precondition (and
    (evaluate comp459)
    (not (rebooted comp459))
    (not (running comp458))
  )
  :effect (and
    (not (evaluate comp459))
    (evaluate comp460)
    (one-off comp459)
  )
)
(:action evaluate-comp460-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp460)
    (rebooted comp460)
  )
  :effect (and
    (not (evaluate comp460))
    (evaluate comp461)
  )
)
(:action evaluate-comp460-all-on
  :parameters ()
  :precondition (and
    (evaluate comp460)
    (not (rebooted comp460))
    (running comp459)
  )
  :effect (and
    (not (evaluate comp460))
    (evaluate comp461)
    (all-on comp460)
  )
)
(:action evaluate-comp460-off-comp459
  :parameters ()
  :precondition (and
    (evaluate comp460)
    (not (rebooted comp460))
    (not (running comp459))
  )
  :effect (and
    (not (evaluate comp460))
    (evaluate comp461)
    (one-off comp460)
  )
)
(:action evaluate-comp461-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp461)
    (rebooted comp461)
  )
  :effect (and
    (not (evaluate comp461))
    (evaluate comp462)
  )
)
(:action evaluate-comp461-all-on
  :parameters ()
  :precondition (and
    (evaluate comp461)
    (not (rebooted comp461))
    (running comp460)
  )
  :effect (and
    (not (evaluate comp461))
    (evaluate comp462)
    (all-on comp461)
  )
)
(:action evaluate-comp461-off-comp460
  :parameters ()
  :precondition (and
    (evaluate comp461)
    (not (rebooted comp461))
    (not (running comp460))
  )
  :effect (and
    (not (evaluate comp461))
    (evaluate comp462)
    (one-off comp461)
  )
)
(:action evaluate-comp462-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp462)
    (rebooted comp462)
  )
  :effect (and
    (not (evaluate comp462))
    (evaluate comp463)
  )
)
(:action evaluate-comp462-all-on
  :parameters ()
  :precondition (and
    (evaluate comp462)
    (not (rebooted comp462))
    (running comp461)
  )
  :effect (and
    (not (evaluate comp462))
    (evaluate comp463)
    (all-on comp462)
  )
)
(:action evaluate-comp462-off-comp461
  :parameters ()
  :precondition (and
    (evaluate comp462)
    (not (rebooted comp462))
    (not (running comp461))
  )
  :effect (and
    (not (evaluate comp462))
    (evaluate comp463)
    (one-off comp462)
  )
)
(:action evaluate-comp463-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp463)
    (rebooted comp463)
  )
  :effect (and
    (not (evaluate comp463))
    (evaluate comp464)
  )
)
(:action evaluate-comp463-all-on
  :parameters ()
  :precondition (and
    (evaluate comp463)
    (not (rebooted comp463))
    (running comp462)
  )
  :effect (and
    (not (evaluate comp463))
    (evaluate comp464)
    (all-on comp463)
  )
)
(:action evaluate-comp463-off-comp462
  :parameters ()
  :precondition (and
    (evaluate comp463)
    (not (rebooted comp463))
    (not (running comp462))
  )
  :effect (and
    (not (evaluate comp463))
    (evaluate comp464)
    (one-off comp463)
  )
)
(:action evaluate-comp464-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp464)
    (rebooted comp464)
  )
  :effect (and
    (not (evaluate comp464))
    (evaluate comp465)
  )
)
(:action evaluate-comp464-all-on
  :parameters ()
  :precondition (and
    (evaluate comp464)
    (not (rebooted comp464))
    (running comp72)
    (running comp463)
  )
  :effect (and
    (not (evaluate comp464))
    (evaluate comp465)
    (all-on comp464)
  )
)
(:action evaluate-comp464-off-comp72
  :parameters ()
  :precondition (and
    (evaluate comp464)
    (not (rebooted comp464))
    (not (running comp72))
  )
  :effect (and
    (not (evaluate comp464))
    (evaluate comp465)
    (one-off comp464)
  )
)
(:action evaluate-comp464-off-comp463
  :parameters ()
  :precondition (and
    (evaluate comp464)
    (not (rebooted comp464))
    (not (running comp463))
  )
  :effect (and
    (not (evaluate comp464))
    (evaluate comp465)
    (one-off comp464)
  )
)
(:action evaluate-comp465-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp465)
    (rebooted comp465)
  )
  :effect (and
    (not (evaluate comp465))
    (evaluate comp466)
  )
)
(:action evaluate-comp465-all-on
  :parameters ()
  :precondition (and
    (evaluate comp465)
    (not (rebooted comp465))
    (running comp464)
  )
  :effect (and
    (not (evaluate comp465))
    (evaluate comp466)
    (all-on comp465)
  )
)
(:action evaluate-comp465-off-comp464
  :parameters ()
  :precondition (and
    (evaluate comp465)
    (not (rebooted comp465))
    (not (running comp464))
  )
  :effect (and
    (not (evaluate comp465))
    (evaluate comp466)
    (one-off comp465)
  )
)
(:action evaluate-comp466-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp466)
    (rebooted comp466)
  )
  :effect (and
    (not (evaluate comp466))
    (evaluate comp467)
  )
)
(:action evaluate-comp466-all-on
  :parameters ()
  :precondition (and
    (evaluate comp466)
    (not (rebooted comp466))
    (running comp465)
  )
  :effect (and
    (not (evaluate comp466))
    (evaluate comp467)
    (all-on comp466)
  )
)
(:action evaluate-comp466-off-comp465
  :parameters ()
  :precondition (and
    (evaluate comp466)
    (not (rebooted comp466))
    (not (running comp465))
  )
  :effect (and
    (not (evaluate comp466))
    (evaluate comp467)
    (one-off comp466)
  )
)
(:action evaluate-comp467-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp467)
    (rebooted comp467)
  )
  :effect (and
    (not (evaluate comp467))
    (evaluate comp468)
  )
)
(:action evaluate-comp467-all-on
  :parameters ()
  :precondition (and
    (evaluate comp467)
    (not (rebooted comp467))
    (running comp466)
  )
  :effect (and
    (not (evaluate comp467))
    (evaluate comp468)
    (all-on comp467)
  )
)
(:action evaluate-comp467-off-comp466
  :parameters ()
  :precondition (and
    (evaluate comp467)
    (not (rebooted comp467))
    (not (running comp466))
  )
  :effect (and
    (not (evaluate comp467))
    (evaluate comp468)
    (one-off comp467)
  )
)
(:action evaluate-comp468-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp468)
    (rebooted comp468)
  )
  :effect (and
    (not (evaluate comp468))
    (evaluate comp469)
  )
)
(:action evaluate-comp468-all-on
  :parameters ()
  :precondition (and
    (evaluate comp468)
    (not (rebooted comp468))
    (running comp467)
  )
  :effect (and
    (not (evaluate comp468))
    (evaluate comp469)
    (all-on comp468)
  )
)
(:action evaluate-comp468-off-comp467
  :parameters ()
  :precondition (and
    (evaluate comp468)
    (not (rebooted comp468))
    (not (running comp467))
  )
  :effect (and
    (not (evaluate comp468))
    (evaluate comp469)
    (one-off comp468)
  )
)
(:action evaluate-comp469-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp469)
    (rebooted comp469)
  )
  :effect (and
    (not (evaluate comp469))
    (evaluate comp470)
  )
)
(:action evaluate-comp469-all-on
  :parameters ()
  :precondition (and
    (evaluate comp469)
    (not (rebooted comp469))
    (running comp135)
    (running comp468)
  )
  :effect (and
    (not (evaluate comp469))
    (evaluate comp470)
    (all-on comp469)
  )
)
(:action evaluate-comp469-off-comp135
  :parameters ()
  :precondition (and
    (evaluate comp469)
    (not (rebooted comp469))
    (not (running comp135))
  )
  :effect (and
    (not (evaluate comp469))
    (evaluate comp470)
    (one-off comp469)
  )
)
(:action evaluate-comp469-off-comp468
  :parameters ()
  :precondition (and
    (evaluate comp469)
    (not (rebooted comp469))
    (not (running comp468))
  )
  :effect (and
    (not (evaluate comp469))
    (evaluate comp470)
    (one-off comp469)
  )
)
(:action evaluate-comp470-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp470)
    (rebooted comp470)
  )
  :effect (and
    (not (evaluate comp470))
    (evaluate comp471)
  )
)
(:action evaluate-comp470-all-on
  :parameters ()
  :precondition (and
    (evaluate comp470)
    (not (rebooted comp470))
    (running comp308)
    (running comp469)
  )
  :effect (and
    (not (evaluate comp470))
    (evaluate comp471)
    (all-on comp470)
  )
)
(:action evaluate-comp470-off-comp308
  :parameters ()
  :precondition (and
    (evaluate comp470)
    (not (rebooted comp470))
    (not (running comp308))
  )
  :effect (and
    (not (evaluate comp470))
    (evaluate comp471)
    (one-off comp470)
  )
)
(:action evaluate-comp470-off-comp469
  :parameters ()
  :precondition (and
    (evaluate comp470)
    (not (rebooted comp470))
    (not (running comp469))
  )
  :effect (and
    (not (evaluate comp470))
    (evaluate comp471)
    (one-off comp470)
  )
)
(:action evaluate-comp471-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp471)
    (rebooted comp471)
  )
  :effect (and
    (not (evaluate comp471))
    (evaluate comp472)
  )
)
(:action evaluate-comp471-all-on
  :parameters ()
  :precondition (and
    (evaluate comp471)
    (not (rebooted comp471))
    (running comp470)
  )
  :effect (and
    (not (evaluate comp471))
    (evaluate comp472)
    (all-on comp471)
  )
)
(:action evaluate-comp471-off-comp470
  :parameters ()
  :precondition (and
    (evaluate comp471)
    (not (rebooted comp471))
    (not (running comp470))
  )
  :effect (and
    (not (evaluate comp471))
    (evaluate comp472)
    (one-off comp471)
  )
)
(:action evaluate-comp472-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp472)
    (rebooted comp472)
  )
  :effect (and
    (not (evaluate comp472))
    (evaluate comp473)
  )
)
(:action evaluate-comp472-all-on
  :parameters ()
  :precondition (and
    (evaluate comp472)
    (not (rebooted comp472))
    (running comp22)
    (running comp128)
    (running comp294)
    (running comp471)
  )
  :effect (and
    (not (evaluate comp472))
    (evaluate comp473)
    (all-on comp472)
  )
)
(:action evaluate-comp472-off-comp22
  :parameters ()
  :precondition (and
    (evaluate comp472)
    (not (rebooted comp472))
    (not (running comp22))
  )
  :effect (and
    (not (evaluate comp472))
    (evaluate comp473)
    (one-off comp472)
  )
)
(:action evaluate-comp472-off-comp128
  :parameters ()
  :precondition (and
    (evaluate comp472)
    (not (rebooted comp472))
    (not (running comp128))
  )
  :effect (and
    (not (evaluate comp472))
    (evaluate comp473)
    (one-off comp472)
  )
)
(:action evaluate-comp472-off-comp294
  :parameters ()
  :precondition (and
    (evaluate comp472)
    (not (rebooted comp472))
    (not (running comp294))
  )
  :effect (and
    (not (evaluate comp472))
    (evaluate comp473)
    (one-off comp472)
  )
)
(:action evaluate-comp472-off-comp471
  :parameters ()
  :precondition (and
    (evaluate comp472)
    (not (rebooted comp472))
    (not (running comp471))
  )
  :effect (and
    (not (evaluate comp472))
    (evaluate comp473)
    (one-off comp472)
  )
)
(:action evaluate-comp473-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp473)
    (rebooted comp473)
  )
  :effect (and
    (not (evaluate comp473))
    (evaluate comp474)
  )
)
(:action evaluate-comp473-all-on
  :parameters ()
  :precondition (and
    (evaluate comp473)
    (not (rebooted comp473))
    (running comp472)
  )
  :effect (and
    (not (evaluate comp473))
    (evaluate comp474)
    (all-on comp473)
  )
)
(:action evaluate-comp473-off-comp472
  :parameters ()
  :precondition (and
    (evaluate comp473)
    (not (rebooted comp473))
    (not (running comp472))
  )
  :effect (and
    (not (evaluate comp473))
    (evaluate comp474)
    (one-off comp473)
  )
)
(:action evaluate-comp474-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp474)
    (rebooted comp474)
  )
  :effect (and
    (not (evaluate comp474))
    (evaluate comp475)
  )
)
(:action evaluate-comp474-all-on
  :parameters ()
  :precondition (and
    (evaluate comp474)
    (not (rebooted comp474))
    (running comp343)
    (running comp473)
  )
  :effect (and
    (not (evaluate comp474))
    (evaluate comp475)
    (all-on comp474)
  )
)
(:action evaluate-comp474-off-comp343
  :parameters ()
  :precondition (and
    (evaluate comp474)
    (not (rebooted comp474))
    (not (running comp343))
  )
  :effect (and
    (not (evaluate comp474))
    (evaluate comp475)
    (one-off comp474)
  )
)
(:action evaluate-comp474-off-comp473
  :parameters ()
  :precondition (and
    (evaluate comp474)
    (not (rebooted comp474))
    (not (running comp473))
  )
  :effect (and
    (not (evaluate comp474))
    (evaluate comp475)
    (one-off comp474)
  )
)
(:action evaluate-comp475-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp475)
    (rebooted comp475)
  )
  :effect (and
    (not (evaluate comp475))
    (evaluate comp476)
  )
)
(:action evaluate-comp475-all-on
  :parameters ()
  :precondition (and
    (evaluate comp475)
    (not (rebooted comp475))
    (running comp474)
  )
  :effect (and
    (not (evaluate comp475))
    (evaluate comp476)
    (all-on comp475)
  )
)
(:action evaluate-comp475-off-comp474
  :parameters ()
  :precondition (and
    (evaluate comp475)
    (not (rebooted comp475))
    (not (running comp474))
  )
  :effect (and
    (not (evaluate comp475))
    (evaluate comp476)
    (one-off comp475)
  )
)
(:action evaluate-comp476-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp476)
    (rebooted comp476)
  )
  :effect (and
    (not (evaluate comp476))
    (evaluate comp477)
  )
)
(:action evaluate-comp476-all-on
  :parameters ()
  :precondition (and
    (evaluate comp476)
    (not (rebooted comp476))
    (running comp475)
  )
  :effect (and
    (not (evaluate comp476))
    (evaluate comp477)
    (all-on comp476)
  )
)
(:action evaluate-comp476-off-comp475
  :parameters ()
  :precondition (and
    (evaluate comp476)
    (not (rebooted comp476))
    (not (running comp475))
  )
  :effect (and
    (not (evaluate comp476))
    (evaluate comp477)
    (one-off comp476)
  )
)
(:action evaluate-comp477-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp477)
    (rebooted comp477)
  )
  :effect (and
    (not (evaluate comp477))
    (evaluate comp478)
  )
)
(:action evaluate-comp477-all-on
  :parameters ()
  :precondition (and
    (evaluate comp477)
    (not (rebooted comp477))
    (running comp476)
  )
  :effect (and
    (not (evaluate comp477))
    (evaluate comp478)
    (all-on comp477)
  )
)
(:action evaluate-comp477-off-comp476
  :parameters ()
  :precondition (and
    (evaluate comp477)
    (not (rebooted comp477))
    (not (running comp476))
  )
  :effect (and
    (not (evaluate comp477))
    (evaluate comp478)
    (one-off comp477)
  )
)
(:action evaluate-comp478-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp478)
    (rebooted comp478)
  )
  :effect (and
    (not (evaluate comp478))
    (evaluate comp479)
  )
)
(:action evaluate-comp478-all-on
  :parameters ()
  :precondition (and
    (evaluate comp478)
    (not (rebooted comp478))
    (running comp477)
  )
  :effect (and
    (not (evaluate comp478))
    (evaluate comp479)
    (all-on comp478)
  )
)
(:action evaluate-comp478-off-comp477
  :parameters ()
  :precondition (and
    (evaluate comp478)
    (not (rebooted comp478))
    (not (running comp477))
  )
  :effect (and
    (not (evaluate comp478))
    (evaluate comp479)
    (one-off comp478)
  )
)
(:action evaluate-comp479-rebooted
  :parameters ()
  :precondition (and
    (evaluate comp479)
    (rebooted comp479)
  )
  :effect (and
    (not (evaluate comp479))
    (update-status comp0)
  )
)
(:action evaluate-comp479-all-on
  :parameters ()
  :precondition (and
    (evaluate comp479)
    (not (rebooted comp479))
    (running comp478)
  )
  :effect (and
    (not (evaluate comp479))
    (update-status comp0)
    (all-on comp479)
  )
)
(:action evaluate-comp479-off-comp478
  :parameters ()
  :precondition (and
    (evaluate comp479)
    (not (rebooted comp479))
    (not (running comp478))
  )
  :effect (and
    (not (evaluate comp479))
    (update-status comp0)
    (one-off comp479)
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
    (update-status comp240)
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
    (update-status comp240)
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
    (update-status comp240)
    (not (one-off comp239))
    (probabilistic 1/4 (and (not (running comp239))))
  )
)
(:action update-status-comp240-rebooted
  :parameters ()
  :precondition (and
    (update-status comp240)
    (rebooted comp240)
  )
  :effect (and
    (not (update-status comp240))
    (update-status comp241)
    (not (rebooted comp240))
    (probabilistic 9/10 (and (running comp240)) 1/10 (and))
  )
)
(:action update-status-comp240-all-on
  :parameters ()
  :precondition (and
    (update-status comp240)
    (not (rebooted comp240))
    (all-on comp240)
  )
  :effect (and
    (not (update-status comp240))
    (update-status comp241)
    (not (all-on comp240))
    (probabilistic 1/20 (and (not (running comp240))))
  )
)
(:action update-status-comp240-one-off
  :parameters ()
  :precondition (and
    (update-status comp240)
    (not (rebooted comp240))
    (one-off comp240)
  )
  :effect (and
    (not (update-status comp240))
    (update-status comp241)
    (not (one-off comp240))
    (probabilistic 1/4 (and (not (running comp240))))
  )
)
(:action update-status-comp241-rebooted
  :parameters ()
  :precondition (and
    (update-status comp241)
    (rebooted comp241)
  )
  :effect (and
    (not (update-status comp241))
    (update-status comp242)
    (not (rebooted comp241))
    (probabilistic 9/10 (and (running comp241)) 1/10 (and))
  )
)
(:action update-status-comp241-all-on
  :parameters ()
  :precondition (and
    (update-status comp241)
    (not (rebooted comp241))
    (all-on comp241)
  )
  :effect (and
    (not (update-status comp241))
    (update-status comp242)
    (not (all-on comp241))
    (probabilistic 1/20 (and (not (running comp241))))
  )
)
(:action update-status-comp241-one-off
  :parameters ()
  :precondition (and
    (update-status comp241)
    (not (rebooted comp241))
    (one-off comp241)
  )
  :effect (and
    (not (update-status comp241))
    (update-status comp242)
    (not (one-off comp241))
    (probabilistic 1/4 (and (not (running comp241))))
  )
)
(:action update-status-comp242-rebooted
  :parameters ()
  :precondition (and
    (update-status comp242)
    (rebooted comp242)
  )
  :effect (and
    (not (update-status comp242))
    (update-status comp243)
    (not (rebooted comp242))
    (probabilistic 9/10 (and (running comp242)) 1/10 (and))
  )
)
(:action update-status-comp242-all-on
  :parameters ()
  :precondition (and
    (update-status comp242)
    (not (rebooted comp242))
    (all-on comp242)
  )
  :effect (and
    (not (update-status comp242))
    (update-status comp243)
    (not (all-on comp242))
    (probabilistic 1/20 (and (not (running comp242))))
  )
)
(:action update-status-comp242-one-off
  :parameters ()
  :precondition (and
    (update-status comp242)
    (not (rebooted comp242))
    (one-off comp242)
  )
  :effect (and
    (not (update-status comp242))
    (update-status comp243)
    (not (one-off comp242))
    (probabilistic 1/4 (and (not (running comp242))))
  )
)
(:action update-status-comp243-rebooted
  :parameters ()
  :precondition (and
    (update-status comp243)
    (rebooted comp243)
  )
  :effect (and
    (not (update-status comp243))
    (update-status comp244)
    (not (rebooted comp243))
    (probabilistic 9/10 (and (running comp243)) 1/10 (and))
  )
)
(:action update-status-comp243-all-on
  :parameters ()
  :precondition (and
    (update-status comp243)
    (not (rebooted comp243))
    (all-on comp243)
  )
  :effect (and
    (not (update-status comp243))
    (update-status comp244)
    (not (all-on comp243))
    (probabilistic 1/20 (and (not (running comp243))))
  )
)
(:action update-status-comp243-one-off
  :parameters ()
  :precondition (and
    (update-status comp243)
    (not (rebooted comp243))
    (one-off comp243)
  )
  :effect (and
    (not (update-status comp243))
    (update-status comp244)
    (not (one-off comp243))
    (probabilistic 1/4 (and (not (running comp243))))
  )
)
(:action update-status-comp244-rebooted
  :parameters ()
  :precondition (and
    (update-status comp244)
    (rebooted comp244)
  )
  :effect (and
    (not (update-status comp244))
    (update-status comp245)
    (not (rebooted comp244))
    (probabilistic 9/10 (and (running comp244)) 1/10 (and))
  )
)
(:action update-status-comp244-all-on
  :parameters ()
  :precondition (and
    (update-status comp244)
    (not (rebooted comp244))
    (all-on comp244)
  )
  :effect (and
    (not (update-status comp244))
    (update-status comp245)
    (not (all-on comp244))
    (probabilistic 1/20 (and (not (running comp244))))
  )
)
(:action update-status-comp244-one-off
  :parameters ()
  :precondition (and
    (update-status comp244)
    (not (rebooted comp244))
    (one-off comp244)
  )
  :effect (and
    (not (update-status comp244))
    (update-status comp245)
    (not (one-off comp244))
    (probabilistic 1/4 (and (not (running comp244))))
  )
)
(:action update-status-comp245-rebooted
  :parameters ()
  :precondition (and
    (update-status comp245)
    (rebooted comp245)
  )
  :effect (and
    (not (update-status comp245))
    (update-status comp246)
    (not (rebooted comp245))
    (probabilistic 9/10 (and (running comp245)) 1/10 (and))
  )
)
(:action update-status-comp245-all-on
  :parameters ()
  :precondition (and
    (update-status comp245)
    (not (rebooted comp245))
    (all-on comp245)
  )
  :effect (and
    (not (update-status comp245))
    (update-status comp246)
    (not (all-on comp245))
    (probabilistic 1/20 (and (not (running comp245))))
  )
)
(:action update-status-comp245-one-off
  :parameters ()
  :precondition (and
    (update-status comp245)
    (not (rebooted comp245))
    (one-off comp245)
  )
  :effect (and
    (not (update-status comp245))
    (update-status comp246)
    (not (one-off comp245))
    (probabilistic 1/4 (and (not (running comp245))))
  )
)
(:action update-status-comp246-rebooted
  :parameters ()
  :precondition (and
    (update-status comp246)
    (rebooted comp246)
  )
  :effect (and
    (not (update-status comp246))
    (update-status comp247)
    (not (rebooted comp246))
    (probabilistic 9/10 (and (running comp246)) 1/10 (and))
  )
)
(:action update-status-comp246-all-on
  :parameters ()
  :precondition (and
    (update-status comp246)
    (not (rebooted comp246))
    (all-on comp246)
  )
  :effect (and
    (not (update-status comp246))
    (update-status comp247)
    (not (all-on comp246))
    (probabilistic 1/20 (and (not (running comp246))))
  )
)
(:action update-status-comp246-one-off
  :parameters ()
  :precondition (and
    (update-status comp246)
    (not (rebooted comp246))
    (one-off comp246)
  )
  :effect (and
    (not (update-status comp246))
    (update-status comp247)
    (not (one-off comp246))
    (probabilistic 1/4 (and (not (running comp246))))
  )
)
(:action update-status-comp247-rebooted
  :parameters ()
  :precondition (and
    (update-status comp247)
    (rebooted comp247)
  )
  :effect (and
    (not (update-status comp247))
    (update-status comp248)
    (not (rebooted comp247))
    (probabilistic 9/10 (and (running comp247)) 1/10 (and))
  )
)
(:action update-status-comp247-all-on
  :parameters ()
  :precondition (and
    (update-status comp247)
    (not (rebooted comp247))
    (all-on comp247)
  )
  :effect (and
    (not (update-status comp247))
    (update-status comp248)
    (not (all-on comp247))
    (probabilistic 1/20 (and (not (running comp247))))
  )
)
(:action update-status-comp247-one-off
  :parameters ()
  :precondition (and
    (update-status comp247)
    (not (rebooted comp247))
    (one-off comp247)
  )
  :effect (and
    (not (update-status comp247))
    (update-status comp248)
    (not (one-off comp247))
    (probabilistic 1/4 (and (not (running comp247))))
  )
)
(:action update-status-comp248-rebooted
  :parameters ()
  :precondition (and
    (update-status comp248)
    (rebooted comp248)
  )
  :effect (and
    (not (update-status comp248))
    (update-status comp249)
    (not (rebooted comp248))
    (probabilistic 9/10 (and (running comp248)) 1/10 (and))
  )
)
(:action update-status-comp248-all-on
  :parameters ()
  :precondition (and
    (update-status comp248)
    (not (rebooted comp248))
    (all-on comp248)
  )
  :effect (and
    (not (update-status comp248))
    (update-status comp249)
    (not (all-on comp248))
    (probabilistic 1/20 (and (not (running comp248))))
  )
)
(:action update-status-comp248-one-off
  :parameters ()
  :precondition (and
    (update-status comp248)
    (not (rebooted comp248))
    (one-off comp248)
  )
  :effect (and
    (not (update-status comp248))
    (update-status comp249)
    (not (one-off comp248))
    (probabilistic 1/4 (and (not (running comp248))))
  )
)
(:action update-status-comp249-rebooted
  :parameters ()
  :precondition (and
    (update-status comp249)
    (rebooted comp249)
  )
  :effect (and
    (not (update-status comp249))
    (update-status comp250)
    (not (rebooted comp249))
    (probabilistic 9/10 (and (running comp249)) 1/10 (and))
  )
)
(:action update-status-comp249-all-on
  :parameters ()
  :precondition (and
    (update-status comp249)
    (not (rebooted comp249))
    (all-on comp249)
  )
  :effect (and
    (not (update-status comp249))
    (update-status comp250)
    (not (all-on comp249))
    (probabilistic 1/20 (and (not (running comp249))))
  )
)
(:action update-status-comp249-one-off
  :parameters ()
  :precondition (and
    (update-status comp249)
    (not (rebooted comp249))
    (one-off comp249)
  )
  :effect (and
    (not (update-status comp249))
    (update-status comp250)
    (not (one-off comp249))
    (probabilistic 1/4 (and (not (running comp249))))
  )
)
(:action update-status-comp250-rebooted
  :parameters ()
  :precondition (and
    (update-status comp250)
    (rebooted comp250)
  )
  :effect (and
    (not (update-status comp250))
    (update-status comp251)
    (not (rebooted comp250))
    (probabilistic 9/10 (and (running comp250)) 1/10 (and))
  )
)
(:action update-status-comp250-all-on
  :parameters ()
  :precondition (and
    (update-status comp250)
    (not (rebooted comp250))
    (all-on comp250)
  )
  :effect (and
    (not (update-status comp250))
    (update-status comp251)
    (not (all-on comp250))
    (probabilistic 1/20 (and (not (running comp250))))
  )
)
(:action update-status-comp250-one-off
  :parameters ()
  :precondition (and
    (update-status comp250)
    (not (rebooted comp250))
    (one-off comp250)
  )
  :effect (and
    (not (update-status comp250))
    (update-status comp251)
    (not (one-off comp250))
    (probabilistic 1/4 (and (not (running comp250))))
  )
)
(:action update-status-comp251-rebooted
  :parameters ()
  :precondition (and
    (update-status comp251)
    (rebooted comp251)
  )
  :effect (and
    (not (update-status comp251))
    (update-status comp252)
    (not (rebooted comp251))
    (probabilistic 9/10 (and (running comp251)) 1/10 (and))
  )
)
(:action update-status-comp251-all-on
  :parameters ()
  :precondition (and
    (update-status comp251)
    (not (rebooted comp251))
    (all-on comp251)
  )
  :effect (and
    (not (update-status comp251))
    (update-status comp252)
    (not (all-on comp251))
    (probabilistic 1/20 (and (not (running comp251))))
  )
)
(:action update-status-comp251-one-off
  :parameters ()
  :precondition (and
    (update-status comp251)
    (not (rebooted comp251))
    (one-off comp251)
  )
  :effect (and
    (not (update-status comp251))
    (update-status comp252)
    (not (one-off comp251))
    (probabilistic 1/4 (and (not (running comp251))))
  )
)
(:action update-status-comp252-rebooted
  :parameters ()
  :precondition (and
    (update-status comp252)
    (rebooted comp252)
  )
  :effect (and
    (not (update-status comp252))
    (update-status comp253)
    (not (rebooted comp252))
    (probabilistic 9/10 (and (running comp252)) 1/10 (and))
  )
)
(:action update-status-comp252-all-on
  :parameters ()
  :precondition (and
    (update-status comp252)
    (not (rebooted comp252))
    (all-on comp252)
  )
  :effect (and
    (not (update-status comp252))
    (update-status comp253)
    (not (all-on comp252))
    (probabilistic 1/20 (and (not (running comp252))))
  )
)
(:action update-status-comp252-one-off
  :parameters ()
  :precondition (and
    (update-status comp252)
    (not (rebooted comp252))
    (one-off comp252)
  )
  :effect (and
    (not (update-status comp252))
    (update-status comp253)
    (not (one-off comp252))
    (probabilistic 1/4 (and (not (running comp252))))
  )
)
(:action update-status-comp253-rebooted
  :parameters ()
  :precondition (and
    (update-status comp253)
    (rebooted comp253)
  )
  :effect (and
    (not (update-status comp253))
    (update-status comp254)
    (not (rebooted comp253))
    (probabilistic 9/10 (and (running comp253)) 1/10 (and))
  )
)
(:action update-status-comp253-all-on
  :parameters ()
  :precondition (and
    (update-status comp253)
    (not (rebooted comp253))
    (all-on comp253)
  )
  :effect (and
    (not (update-status comp253))
    (update-status comp254)
    (not (all-on comp253))
    (probabilistic 1/20 (and (not (running comp253))))
  )
)
(:action update-status-comp253-one-off
  :parameters ()
  :precondition (and
    (update-status comp253)
    (not (rebooted comp253))
    (one-off comp253)
  )
  :effect (and
    (not (update-status comp253))
    (update-status comp254)
    (not (one-off comp253))
    (probabilistic 1/4 (and (not (running comp253))))
  )
)
(:action update-status-comp254-rebooted
  :parameters ()
  :precondition (and
    (update-status comp254)
    (rebooted comp254)
  )
  :effect (and
    (not (update-status comp254))
    (update-status comp255)
    (not (rebooted comp254))
    (probabilistic 9/10 (and (running comp254)) 1/10 (and))
  )
)
(:action update-status-comp254-all-on
  :parameters ()
  :precondition (and
    (update-status comp254)
    (not (rebooted comp254))
    (all-on comp254)
  )
  :effect (and
    (not (update-status comp254))
    (update-status comp255)
    (not (all-on comp254))
    (probabilistic 1/20 (and (not (running comp254))))
  )
)
(:action update-status-comp254-one-off
  :parameters ()
  :precondition (and
    (update-status comp254)
    (not (rebooted comp254))
    (one-off comp254)
  )
  :effect (and
    (not (update-status comp254))
    (update-status comp255)
    (not (one-off comp254))
    (probabilistic 1/4 (and (not (running comp254))))
  )
)
(:action update-status-comp255-rebooted
  :parameters ()
  :precondition (and
    (update-status comp255)
    (rebooted comp255)
  )
  :effect (and
    (not (update-status comp255))
    (update-status comp256)
    (not (rebooted comp255))
    (probabilistic 9/10 (and (running comp255)) 1/10 (and))
  )
)
(:action update-status-comp255-all-on
  :parameters ()
  :precondition (and
    (update-status comp255)
    (not (rebooted comp255))
    (all-on comp255)
  )
  :effect (and
    (not (update-status comp255))
    (update-status comp256)
    (not (all-on comp255))
    (probabilistic 1/20 (and (not (running comp255))))
  )
)
(:action update-status-comp255-one-off
  :parameters ()
  :precondition (and
    (update-status comp255)
    (not (rebooted comp255))
    (one-off comp255)
  )
  :effect (and
    (not (update-status comp255))
    (update-status comp256)
    (not (one-off comp255))
    (probabilistic 1/4 (and (not (running comp255))))
  )
)
(:action update-status-comp256-rebooted
  :parameters ()
  :precondition (and
    (update-status comp256)
    (rebooted comp256)
  )
  :effect (and
    (not (update-status comp256))
    (update-status comp257)
    (not (rebooted comp256))
    (probabilistic 9/10 (and (running comp256)) 1/10 (and))
  )
)
(:action update-status-comp256-all-on
  :parameters ()
  :precondition (and
    (update-status comp256)
    (not (rebooted comp256))
    (all-on comp256)
  )
  :effect (and
    (not (update-status comp256))
    (update-status comp257)
    (not (all-on comp256))
    (probabilistic 1/20 (and (not (running comp256))))
  )
)
(:action update-status-comp256-one-off
  :parameters ()
  :precondition (and
    (update-status comp256)
    (not (rebooted comp256))
    (one-off comp256)
  )
  :effect (and
    (not (update-status comp256))
    (update-status comp257)
    (not (one-off comp256))
    (probabilistic 1/4 (and (not (running comp256))))
  )
)
(:action update-status-comp257-rebooted
  :parameters ()
  :precondition (and
    (update-status comp257)
    (rebooted comp257)
  )
  :effect (and
    (not (update-status comp257))
    (update-status comp258)
    (not (rebooted comp257))
    (probabilistic 9/10 (and (running comp257)) 1/10 (and))
  )
)
(:action update-status-comp257-all-on
  :parameters ()
  :precondition (and
    (update-status comp257)
    (not (rebooted comp257))
    (all-on comp257)
  )
  :effect (and
    (not (update-status comp257))
    (update-status comp258)
    (not (all-on comp257))
    (probabilistic 1/20 (and (not (running comp257))))
  )
)
(:action update-status-comp257-one-off
  :parameters ()
  :precondition (and
    (update-status comp257)
    (not (rebooted comp257))
    (one-off comp257)
  )
  :effect (and
    (not (update-status comp257))
    (update-status comp258)
    (not (one-off comp257))
    (probabilistic 1/4 (and (not (running comp257))))
  )
)
(:action update-status-comp258-rebooted
  :parameters ()
  :precondition (and
    (update-status comp258)
    (rebooted comp258)
  )
  :effect (and
    (not (update-status comp258))
    (update-status comp259)
    (not (rebooted comp258))
    (probabilistic 9/10 (and (running comp258)) 1/10 (and))
  )
)
(:action update-status-comp258-all-on
  :parameters ()
  :precondition (and
    (update-status comp258)
    (not (rebooted comp258))
    (all-on comp258)
  )
  :effect (and
    (not (update-status comp258))
    (update-status comp259)
    (not (all-on comp258))
    (probabilistic 1/20 (and (not (running comp258))))
  )
)
(:action update-status-comp258-one-off
  :parameters ()
  :precondition (and
    (update-status comp258)
    (not (rebooted comp258))
    (one-off comp258)
  )
  :effect (and
    (not (update-status comp258))
    (update-status comp259)
    (not (one-off comp258))
    (probabilistic 1/4 (and (not (running comp258))))
  )
)
(:action update-status-comp259-rebooted
  :parameters ()
  :precondition (and
    (update-status comp259)
    (rebooted comp259)
  )
  :effect (and
    (not (update-status comp259))
    (update-status comp260)
    (not (rebooted comp259))
    (probabilistic 9/10 (and (running comp259)) 1/10 (and))
  )
)
(:action update-status-comp259-all-on
  :parameters ()
  :precondition (and
    (update-status comp259)
    (not (rebooted comp259))
    (all-on comp259)
  )
  :effect (and
    (not (update-status comp259))
    (update-status comp260)
    (not (all-on comp259))
    (probabilistic 1/20 (and (not (running comp259))))
  )
)
(:action update-status-comp259-one-off
  :parameters ()
  :precondition (and
    (update-status comp259)
    (not (rebooted comp259))
    (one-off comp259)
  )
  :effect (and
    (not (update-status comp259))
    (update-status comp260)
    (not (one-off comp259))
    (probabilistic 1/4 (and (not (running comp259))))
  )
)
(:action update-status-comp260-rebooted
  :parameters ()
  :precondition (and
    (update-status comp260)
    (rebooted comp260)
  )
  :effect (and
    (not (update-status comp260))
    (update-status comp261)
    (not (rebooted comp260))
    (probabilistic 9/10 (and (running comp260)) 1/10 (and))
  )
)
(:action update-status-comp260-all-on
  :parameters ()
  :precondition (and
    (update-status comp260)
    (not (rebooted comp260))
    (all-on comp260)
  )
  :effect (and
    (not (update-status comp260))
    (update-status comp261)
    (not (all-on comp260))
    (probabilistic 1/20 (and (not (running comp260))))
  )
)
(:action update-status-comp260-one-off
  :parameters ()
  :precondition (and
    (update-status comp260)
    (not (rebooted comp260))
    (one-off comp260)
  )
  :effect (and
    (not (update-status comp260))
    (update-status comp261)
    (not (one-off comp260))
    (probabilistic 1/4 (and (not (running comp260))))
  )
)
(:action update-status-comp261-rebooted
  :parameters ()
  :precondition (and
    (update-status comp261)
    (rebooted comp261)
  )
  :effect (and
    (not (update-status comp261))
    (update-status comp262)
    (not (rebooted comp261))
    (probabilistic 9/10 (and (running comp261)) 1/10 (and))
  )
)
(:action update-status-comp261-all-on
  :parameters ()
  :precondition (and
    (update-status comp261)
    (not (rebooted comp261))
    (all-on comp261)
  )
  :effect (and
    (not (update-status comp261))
    (update-status comp262)
    (not (all-on comp261))
    (probabilistic 1/20 (and (not (running comp261))))
  )
)
(:action update-status-comp261-one-off
  :parameters ()
  :precondition (and
    (update-status comp261)
    (not (rebooted comp261))
    (one-off comp261)
  )
  :effect (and
    (not (update-status comp261))
    (update-status comp262)
    (not (one-off comp261))
    (probabilistic 1/4 (and (not (running comp261))))
  )
)
(:action update-status-comp262-rebooted
  :parameters ()
  :precondition (and
    (update-status comp262)
    (rebooted comp262)
  )
  :effect (and
    (not (update-status comp262))
    (update-status comp263)
    (not (rebooted comp262))
    (probabilistic 9/10 (and (running comp262)) 1/10 (and))
  )
)
(:action update-status-comp262-all-on
  :parameters ()
  :precondition (and
    (update-status comp262)
    (not (rebooted comp262))
    (all-on comp262)
  )
  :effect (and
    (not (update-status comp262))
    (update-status comp263)
    (not (all-on comp262))
    (probabilistic 1/20 (and (not (running comp262))))
  )
)
(:action update-status-comp262-one-off
  :parameters ()
  :precondition (and
    (update-status comp262)
    (not (rebooted comp262))
    (one-off comp262)
  )
  :effect (and
    (not (update-status comp262))
    (update-status comp263)
    (not (one-off comp262))
    (probabilistic 1/4 (and (not (running comp262))))
  )
)
(:action update-status-comp263-rebooted
  :parameters ()
  :precondition (and
    (update-status comp263)
    (rebooted comp263)
  )
  :effect (and
    (not (update-status comp263))
    (update-status comp264)
    (not (rebooted comp263))
    (probabilistic 9/10 (and (running comp263)) 1/10 (and))
  )
)
(:action update-status-comp263-all-on
  :parameters ()
  :precondition (and
    (update-status comp263)
    (not (rebooted comp263))
    (all-on comp263)
  )
  :effect (and
    (not (update-status comp263))
    (update-status comp264)
    (not (all-on comp263))
    (probabilistic 1/20 (and (not (running comp263))))
  )
)
(:action update-status-comp263-one-off
  :parameters ()
  :precondition (and
    (update-status comp263)
    (not (rebooted comp263))
    (one-off comp263)
  )
  :effect (and
    (not (update-status comp263))
    (update-status comp264)
    (not (one-off comp263))
    (probabilistic 1/4 (and (not (running comp263))))
  )
)
(:action update-status-comp264-rebooted
  :parameters ()
  :precondition (and
    (update-status comp264)
    (rebooted comp264)
  )
  :effect (and
    (not (update-status comp264))
    (update-status comp265)
    (not (rebooted comp264))
    (probabilistic 9/10 (and (running comp264)) 1/10 (and))
  )
)
(:action update-status-comp264-all-on
  :parameters ()
  :precondition (and
    (update-status comp264)
    (not (rebooted comp264))
    (all-on comp264)
  )
  :effect (and
    (not (update-status comp264))
    (update-status comp265)
    (not (all-on comp264))
    (probabilistic 1/20 (and (not (running comp264))))
  )
)
(:action update-status-comp264-one-off
  :parameters ()
  :precondition (and
    (update-status comp264)
    (not (rebooted comp264))
    (one-off comp264)
  )
  :effect (and
    (not (update-status comp264))
    (update-status comp265)
    (not (one-off comp264))
    (probabilistic 1/4 (and (not (running comp264))))
  )
)
(:action update-status-comp265-rebooted
  :parameters ()
  :precondition (and
    (update-status comp265)
    (rebooted comp265)
  )
  :effect (and
    (not (update-status comp265))
    (update-status comp266)
    (not (rebooted comp265))
    (probabilistic 9/10 (and (running comp265)) 1/10 (and))
  )
)
(:action update-status-comp265-all-on
  :parameters ()
  :precondition (and
    (update-status comp265)
    (not (rebooted comp265))
    (all-on comp265)
  )
  :effect (and
    (not (update-status comp265))
    (update-status comp266)
    (not (all-on comp265))
    (probabilistic 1/20 (and (not (running comp265))))
  )
)
(:action update-status-comp265-one-off
  :parameters ()
  :precondition (and
    (update-status comp265)
    (not (rebooted comp265))
    (one-off comp265)
  )
  :effect (and
    (not (update-status comp265))
    (update-status comp266)
    (not (one-off comp265))
    (probabilistic 1/4 (and (not (running comp265))))
  )
)
(:action update-status-comp266-rebooted
  :parameters ()
  :precondition (and
    (update-status comp266)
    (rebooted comp266)
  )
  :effect (and
    (not (update-status comp266))
    (update-status comp267)
    (not (rebooted comp266))
    (probabilistic 9/10 (and (running comp266)) 1/10 (and))
  )
)
(:action update-status-comp266-all-on
  :parameters ()
  :precondition (and
    (update-status comp266)
    (not (rebooted comp266))
    (all-on comp266)
  )
  :effect (and
    (not (update-status comp266))
    (update-status comp267)
    (not (all-on comp266))
    (probabilistic 1/20 (and (not (running comp266))))
  )
)
(:action update-status-comp266-one-off
  :parameters ()
  :precondition (and
    (update-status comp266)
    (not (rebooted comp266))
    (one-off comp266)
  )
  :effect (and
    (not (update-status comp266))
    (update-status comp267)
    (not (one-off comp266))
    (probabilistic 1/4 (and (not (running comp266))))
  )
)
(:action update-status-comp267-rebooted
  :parameters ()
  :precondition (and
    (update-status comp267)
    (rebooted comp267)
  )
  :effect (and
    (not (update-status comp267))
    (update-status comp268)
    (not (rebooted comp267))
    (probabilistic 9/10 (and (running comp267)) 1/10 (and))
  )
)
(:action update-status-comp267-all-on
  :parameters ()
  :precondition (and
    (update-status comp267)
    (not (rebooted comp267))
    (all-on comp267)
  )
  :effect (and
    (not (update-status comp267))
    (update-status comp268)
    (not (all-on comp267))
    (probabilistic 1/20 (and (not (running comp267))))
  )
)
(:action update-status-comp267-one-off
  :parameters ()
  :precondition (and
    (update-status comp267)
    (not (rebooted comp267))
    (one-off comp267)
  )
  :effect (and
    (not (update-status comp267))
    (update-status comp268)
    (not (one-off comp267))
    (probabilistic 1/4 (and (not (running comp267))))
  )
)
(:action update-status-comp268-rebooted
  :parameters ()
  :precondition (and
    (update-status comp268)
    (rebooted comp268)
  )
  :effect (and
    (not (update-status comp268))
    (update-status comp269)
    (not (rebooted comp268))
    (probabilistic 9/10 (and (running comp268)) 1/10 (and))
  )
)
(:action update-status-comp268-all-on
  :parameters ()
  :precondition (and
    (update-status comp268)
    (not (rebooted comp268))
    (all-on comp268)
  )
  :effect (and
    (not (update-status comp268))
    (update-status comp269)
    (not (all-on comp268))
    (probabilistic 1/20 (and (not (running comp268))))
  )
)
(:action update-status-comp268-one-off
  :parameters ()
  :precondition (and
    (update-status comp268)
    (not (rebooted comp268))
    (one-off comp268)
  )
  :effect (and
    (not (update-status comp268))
    (update-status comp269)
    (not (one-off comp268))
    (probabilistic 1/4 (and (not (running comp268))))
  )
)
(:action update-status-comp269-rebooted
  :parameters ()
  :precondition (and
    (update-status comp269)
    (rebooted comp269)
  )
  :effect (and
    (not (update-status comp269))
    (update-status comp270)
    (not (rebooted comp269))
    (probabilistic 9/10 (and (running comp269)) 1/10 (and))
  )
)
(:action update-status-comp269-all-on
  :parameters ()
  :precondition (and
    (update-status comp269)
    (not (rebooted comp269))
    (all-on comp269)
  )
  :effect (and
    (not (update-status comp269))
    (update-status comp270)
    (not (all-on comp269))
    (probabilistic 1/20 (and (not (running comp269))))
  )
)
(:action update-status-comp269-one-off
  :parameters ()
  :precondition (and
    (update-status comp269)
    (not (rebooted comp269))
    (one-off comp269)
  )
  :effect (and
    (not (update-status comp269))
    (update-status comp270)
    (not (one-off comp269))
    (probabilistic 1/4 (and (not (running comp269))))
  )
)
(:action update-status-comp270-rebooted
  :parameters ()
  :precondition (and
    (update-status comp270)
    (rebooted comp270)
  )
  :effect (and
    (not (update-status comp270))
    (update-status comp271)
    (not (rebooted comp270))
    (probabilistic 9/10 (and (running comp270)) 1/10 (and))
  )
)
(:action update-status-comp270-all-on
  :parameters ()
  :precondition (and
    (update-status comp270)
    (not (rebooted comp270))
    (all-on comp270)
  )
  :effect (and
    (not (update-status comp270))
    (update-status comp271)
    (not (all-on comp270))
    (probabilistic 1/20 (and (not (running comp270))))
  )
)
(:action update-status-comp270-one-off
  :parameters ()
  :precondition (and
    (update-status comp270)
    (not (rebooted comp270))
    (one-off comp270)
  )
  :effect (and
    (not (update-status comp270))
    (update-status comp271)
    (not (one-off comp270))
    (probabilistic 1/4 (and (not (running comp270))))
  )
)
(:action update-status-comp271-rebooted
  :parameters ()
  :precondition (and
    (update-status comp271)
    (rebooted comp271)
  )
  :effect (and
    (not (update-status comp271))
    (update-status comp272)
    (not (rebooted comp271))
    (probabilistic 9/10 (and (running comp271)) 1/10 (and))
  )
)
(:action update-status-comp271-all-on
  :parameters ()
  :precondition (and
    (update-status comp271)
    (not (rebooted comp271))
    (all-on comp271)
  )
  :effect (and
    (not (update-status comp271))
    (update-status comp272)
    (not (all-on comp271))
    (probabilistic 1/20 (and (not (running comp271))))
  )
)
(:action update-status-comp271-one-off
  :parameters ()
  :precondition (and
    (update-status comp271)
    (not (rebooted comp271))
    (one-off comp271)
  )
  :effect (and
    (not (update-status comp271))
    (update-status comp272)
    (not (one-off comp271))
    (probabilistic 1/4 (and (not (running comp271))))
  )
)
(:action update-status-comp272-rebooted
  :parameters ()
  :precondition (and
    (update-status comp272)
    (rebooted comp272)
  )
  :effect (and
    (not (update-status comp272))
    (update-status comp273)
    (not (rebooted comp272))
    (probabilistic 9/10 (and (running comp272)) 1/10 (and))
  )
)
(:action update-status-comp272-all-on
  :parameters ()
  :precondition (and
    (update-status comp272)
    (not (rebooted comp272))
    (all-on comp272)
  )
  :effect (and
    (not (update-status comp272))
    (update-status comp273)
    (not (all-on comp272))
    (probabilistic 1/20 (and (not (running comp272))))
  )
)
(:action update-status-comp272-one-off
  :parameters ()
  :precondition (and
    (update-status comp272)
    (not (rebooted comp272))
    (one-off comp272)
  )
  :effect (and
    (not (update-status comp272))
    (update-status comp273)
    (not (one-off comp272))
    (probabilistic 1/4 (and (not (running comp272))))
  )
)
(:action update-status-comp273-rebooted
  :parameters ()
  :precondition (and
    (update-status comp273)
    (rebooted comp273)
  )
  :effect (and
    (not (update-status comp273))
    (update-status comp274)
    (not (rebooted comp273))
    (probabilistic 9/10 (and (running comp273)) 1/10 (and))
  )
)
(:action update-status-comp273-all-on
  :parameters ()
  :precondition (and
    (update-status comp273)
    (not (rebooted comp273))
    (all-on comp273)
  )
  :effect (and
    (not (update-status comp273))
    (update-status comp274)
    (not (all-on comp273))
    (probabilistic 1/20 (and (not (running comp273))))
  )
)
(:action update-status-comp273-one-off
  :parameters ()
  :precondition (and
    (update-status comp273)
    (not (rebooted comp273))
    (one-off comp273)
  )
  :effect (and
    (not (update-status comp273))
    (update-status comp274)
    (not (one-off comp273))
    (probabilistic 1/4 (and (not (running comp273))))
  )
)
(:action update-status-comp274-rebooted
  :parameters ()
  :precondition (and
    (update-status comp274)
    (rebooted comp274)
  )
  :effect (and
    (not (update-status comp274))
    (update-status comp275)
    (not (rebooted comp274))
    (probabilistic 9/10 (and (running comp274)) 1/10 (and))
  )
)
(:action update-status-comp274-all-on
  :parameters ()
  :precondition (and
    (update-status comp274)
    (not (rebooted comp274))
    (all-on comp274)
  )
  :effect (and
    (not (update-status comp274))
    (update-status comp275)
    (not (all-on comp274))
    (probabilistic 1/20 (and (not (running comp274))))
  )
)
(:action update-status-comp274-one-off
  :parameters ()
  :precondition (and
    (update-status comp274)
    (not (rebooted comp274))
    (one-off comp274)
  )
  :effect (and
    (not (update-status comp274))
    (update-status comp275)
    (not (one-off comp274))
    (probabilistic 1/4 (and (not (running comp274))))
  )
)
(:action update-status-comp275-rebooted
  :parameters ()
  :precondition (and
    (update-status comp275)
    (rebooted comp275)
  )
  :effect (and
    (not (update-status comp275))
    (update-status comp276)
    (not (rebooted comp275))
    (probabilistic 9/10 (and (running comp275)) 1/10 (and))
  )
)
(:action update-status-comp275-all-on
  :parameters ()
  :precondition (and
    (update-status comp275)
    (not (rebooted comp275))
    (all-on comp275)
  )
  :effect (and
    (not (update-status comp275))
    (update-status comp276)
    (not (all-on comp275))
    (probabilistic 1/20 (and (not (running comp275))))
  )
)
(:action update-status-comp275-one-off
  :parameters ()
  :precondition (and
    (update-status comp275)
    (not (rebooted comp275))
    (one-off comp275)
  )
  :effect (and
    (not (update-status comp275))
    (update-status comp276)
    (not (one-off comp275))
    (probabilistic 1/4 (and (not (running comp275))))
  )
)
(:action update-status-comp276-rebooted
  :parameters ()
  :precondition (and
    (update-status comp276)
    (rebooted comp276)
  )
  :effect (and
    (not (update-status comp276))
    (update-status comp277)
    (not (rebooted comp276))
    (probabilistic 9/10 (and (running comp276)) 1/10 (and))
  )
)
(:action update-status-comp276-all-on
  :parameters ()
  :precondition (and
    (update-status comp276)
    (not (rebooted comp276))
    (all-on comp276)
  )
  :effect (and
    (not (update-status comp276))
    (update-status comp277)
    (not (all-on comp276))
    (probabilistic 1/20 (and (not (running comp276))))
  )
)
(:action update-status-comp276-one-off
  :parameters ()
  :precondition (and
    (update-status comp276)
    (not (rebooted comp276))
    (one-off comp276)
  )
  :effect (and
    (not (update-status comp276))
    (update-status comp277)
    (not (one-off comp276))
    (probabilistic 1/4 (and (not (running comp276))))
  )
)
(:action update-status-comp277-rebooted
  :parameters ()
  :precondition (and
    (update-status comp277)
    (rebooted comp277)
  )
  :effect (and
    (not (update-status comp277))
    (update-status comp278)
    (not (rebooted comp277))
    (probabilistic 9/10 (and (running comp277)) 1/10 (and))
  )
)
(:action update-status-comp277-all-on
  :parameters ()
  :precondition (and
    (update-status comp277)
    (not (rebooted comp277))
    (all-on comp277)
  )
  :effect (and
    (not (update-status comp277))
    (update-status comp278)
    (not (all-on comp277))
    (probabilistic 1/20 (and (not (running comp277))))
  )
)
(:action update-status-comp277-one-off
  :parameters ()
  :precondition (and
    (update-status comp277)
    (not (rebooted comp277))
    (one-off comp277)
  )
  :effect (and
    (not (update-status comp277))
    (update-status comp278)
    (not (one-off comp277))
    (probabilistic 1/4 (and (not (running comp277))))
  )
)
(:action update-status-comp278-rebooted
  :parameters ()
  :precondition (and
    (update-status comp278)
    (rebooted comp278)
  )
  :effect (and
    (not (update-status comp278))
    (update-status comp279)
    (not (rebooted comp278))
    (probabilistic 9/10 (and (running comp278)) 1/10 (and))
  )
)
(:action update-status-comp278-all-on
  :parameters ()
  :precondition (and
    (update-status comp278)
    (not (rebooted comp278))
    (all-on comp278)
  )
  :effect (and
    (not (update-status comp278))
    (update-status comp279)
    (not (all-on comp278))
    (probabilistic 1/20 (and (not (running comp278))))
  )
)
(:action update-status-comp278-one-off
  :parameters ()
  :precondition (and
    (update-status comp278)
    (not (rebooted comp278))
    (one-off comp278)
  )
  :effect (and
    (not (update-status comp278))
    (update-status comp279)
    (not (one-off comp278))
    (probabilistic 1/4 (and (not (running comp278))))
  )
)
(:action update-status-comp279-rebooted
  :parameters ()
  :precondition (and
    (update-status comp279)
    (rebooted comp279)
  )
  :effect (and
    (not (update-status comp279))
    (update-status comp280)
    (not (rebooted comp279))
    (probabilistic 9/10 (and (running comp279)) 1/10 (and))
  )
)
(:action update-status-comp279-all-on
  :parameters ()
  :precondition (and
    (update-status comp279)
    (not (rebooted comp279))
    (all-on comp279)
  )
  :effect (and
    (not (update-status comp279))
    (update-status comp280)
    (not (all-on comp279))
    (probabilistic 1/20 (and (not (running comp279))))
  )
)
(:action update-status-comp279-one-off
  :parameters ()
  :precondition (and
    (update-status comp279)
    (not (rebooted comp279))
    (one-off comp279)
  )
  :effect (and
    (not (update-status comp279))
    (update-status comp280)
    (not (one-off comp279))
    (probabilistic 1/4 (and (not (running comp279))))
  )
)
(:action update-status-comp280-rebooted
  :parameters ()
  :precondition (and
    (update-status comp280)
    (rebooted comp280)
  )
  :effect (and
    (not (update-status comp280))
    (update-status comp281)
    (not (rebooted comp280))
    (probabilistic 9/10 (and (running comp280)) 1/10 (and))
  )
)
(:action update-status-comp280-all-on
  :parameters ()
  :precondition (and
    (update-status comp280)
    (not (rebooted comp280))
    (all-on comp280)
  )
  :effect (and
    (not (update-status comp280))
    (update-status comp281)
    (not (all-on comp280))
    (probabilistic 1/20 (and (not (running comp280))))
  )
)
(:action update-status-comp280-one-off
  :parameters ()
  :precondition (and
    (update-status comp280)
    (not (rebooted comp280))
    (one-off comp280)
  )
  :effect (and
    (not (update-status comp280))
    (update-status comp281)
    (not (one-off comp280))
    (probabilistic 1/4 (and (not (running comp280))))
  )
)
(:action update-status-comp281-rebooted
  :parameters ()
  :precondition (and
    (update-status comp281)
    (rebooted comp281)
  )
  :effect (and
    (not (update-status comp281))
    (update-status comp282)
    (not (rebooted comp281))
    (probabilistic 9/10 (and (running comp281)) 1/10 (and))
  )
)
(:action update-status-comp281-all-on
  :parameters ()
  :precondition (and
    (update-status comp281)
    (not (rebooted comp281))
    (all-on comp281)
  )
  :effect (and
    (not (update-status comp281))
    (update-status comp282)
    (not (all-on comp281))
    (probabilistic 1/20 (and (not (running comp281))))
  )
)
(:action update-status-comp281-one-off
  :parameters ()
  :precondition (and
    (update-status comp281)
    (not (rebooted comp281))
    (one-off comp281)
  )
  :effect (and
    (not (update-status comp281))
    (update-status comp282)
    (not (one-off comp281))
    (probabilistic 1/4 (and (not (running comp281))))
  )
)
(:action update-status-comp282-rebooted
  :parameters ()
  :precondition (and
    (update-status comp282)
    (rebooted comp282)
  )
  :effect (and
    (not (update-status comp282))
    (update-status comp283)
    (not (rebooted comp282))
    (probabilistic 9/10 (and (running comp282)) 1/10 (and))
  )
)
(:action update-status-comp282-all-on
  :parameters ()
  :precondition (and
    (update-status comp282)
    (not (rebooted comp282))
    (all-on comp282)
  )
  :effect (and
    (not (update-status comp282))
    (update-status comp283)
    (not (all-on comp282))
    (probabilistic 1/20 (and (not (running comp282))))
  )
)
(:action update-status-comp282-one-off
  :parameters ()
  :precondition (and
    (update-status comp282)
    (not (rebooted comp282))
    (one-off comp282)
  )
  :effect (and
    (not (update-status comp282))
    (update-status comp283)
    (not (one-off comp282))
    (probabilistic 1/4 (and (not (running comp282))))
  )
)
(:action update-status-comp283-rebooted
  :parameters ()
  :precondition (and
    (update-status comp283)
    (rebooted comp283)
  )
  :effect (and
    (not (update-status comp283))
    (update-status comp284)
    (not (rebooted comp283))
    (probabilistic 9/10 (and (running comp283)) 1/10 (and))
  )
)
(:action update-status-comp283-all-on
  :parameters ()
  :precondition (and
    (update-status comp283)
    (not (rebooted comp283))
    (all-on comp283)
  )
  :effect (and
    (not (update-status comp283))
    (update-status comp284)
    (not (all-on comp283))
    (probabilistic 1/20 (and (not (running comp283))))
  )
)
(:action update-status-comp283-one-off
  :parameters ()
  :precondition (and
    (update-status comp283)
    (not (rebooted comp283))
    (one-off comp283)
  )
  :effect (and
    (not (update-status comp283))
    (update-status comp284)
    (not (one-off comp283))
    (probabilistic 1/4 (and (not (running comp283))))
  )
)
(:action update-status-comp284-rebooted
  :parameters ()
  :precondition (and
    (update-status comp284)
    (rebooted comp284)
  )
  :effect (and
    (not (update-status comp284))
    (update-status comp285)
    (not (rebooted comp284))
    (probabilistic 9/10 (and (running comp284)) 1/10 (and))
  )
)
(:action update-status-comp284-all-on
  :parameters ()
  :precondition (and
    (update-status comp284)
    (not (rebooted comp284))
    (all-on comp284)
  )
  :effect (and
    (not (update-status comp284))
    (update-status comp285)
    (not (all-on comp284))
    (probabilistic 1/20 (and (not (running comp284))))
  )
)
(:action update-status-comp284-one-off
  :parameters ()
  :precondition (and
    (update-status comp284)
    (not (rebooted comp284))
    (one-off comp284)
  )
  :effect (and
    (not (update-status comp284))
    (update-status comp285)
    (not (one-off comp284))
    (probabilistic 1/4 (and (not (running comp284))))
  )
)
(:action update-status-comp285-rebooted
  :parameters ()
  :precondition (and
    (update-status comp285)
    (rebooted comp285)
  )
  :effect (and
    (not (update-status comp285))
    (update-status comp286)
    (not (rebooted comp285))
    (probabilistic 9/10 (and (running comp285)) 1/10 (and))
  )
)
(:action update-status-comp285-all-on
  :parameters ()
  :precondition (and
    (update-status comp285)
    (not (rebooted comp285))
    (all-on comp285)
  )
  :effect (and
    (not (update-status comp285))
    (update-status comp286)
    (not (all-on comp285))
    (probabilistic 1/20 (and (not (running comp285))))
  )
)
(:action update-status-comp285-one-off
  :parameters ()
  :precondition (and
    (update-status comp285)
    (not (rebooted comp285))
    (one-off comp285)
  )
  :effect (and
    (not (update-status comp285))
    (update-status comp286)
    (not (one-off comp285))
    (probabilistic 1/4 (and (not (running comp285))))
  )
)
(:action update-status-comp286-rebooted
  :parameters ()
  :precondition (and
    (update-status comp286)
    (rebooted comp286)
  )
  :effect (and
    (not (update-status comp286))
    (update-status comp287)
    (not (rebooted comp286))
    (probabilistic 9/10 (and (running comp286)) 1/10 (and))
  )
)
(:action update-status-comp286-all-on
  :parameters ()
  :precondition (and
    (update-status comp286)
    (not (rebooted comp286))
    (all-on comp286)
  )
  :effect (and
    (not (update-status comp286))
    (update-status comp287)
    (not (all-on comp286))
    (probabilistic 1/20 (and (not (running comp286))))
  )
)
(:action update-status-comp286-one-off
  :parameters ()
  :precondition (and
    (update-status comp286)
    (not (rebooted comp286))
    (one-off comp286)
  )
  :effect (and
    (not (update-status comp286))
    (update-status comp287)
    (not (one-off comp286))
    (probabilistic 1/4 (and (not (running comp286))))
  )
)
(:action update-status-comp287-rebooted
  :parameters ()
  :precondition (and
    (update-status comp287)
    (rebooted comp287)
  )
  :effect (and
    (not (update-status comp287))
    (update-status comp288)
    (not (rebooted comp287))
    (probabilistic 9/10 (and (running comp287)) 1/10 (and))
  )
)
(:action update-status-comp287-all-on
  :parameters ()
  :precondition (and
    (update-status comp287)
    (not (rebooted comp287))
    (all-on comp287)
  )
  :effect (and
    (not (update-status comp287))
    (update-status comp288)
    (not (all-on comp287))
    (probabilistic 1/20 (and (not (running comp287))))
  )
)
(:action update-status-comp287-one-off
  :parameters ()
  :precondition (and
    (update-status comp287)
    (not (rebooted comp287))
    (one-off comp287)
  )
  :effect (and
    (not (update-status comp287))
    (update-status comp288)
    (not (one-off comp287))
    (probabilistic 1/4 (and (not (running comp287))))
  )
)
(:action update-status-comp288-rebooted
  :parameters ()
  :precondition (and
    (update-status comp288)
    (rebooted comp288)
  )
  :effect (and
    (not (update-status comp288))
    (update-status comp289)
    (not (rebooted comp288))
    (probabilistic 9/10 (and (running comp288)) 1/10 (and))
  )
)
(:action update-status-comp288-all-on
  :parameters ()
  :precondition (and
    (update-status comp288)
    (not (rebooted comp288))
    (all-on comp288)
  )
  :effect (and
    (not (update-status comp288))
    (update-status comp289)
    (not (all-on comp288))
    (probabilistic 1/20 (and (not (running comp288))))
  )
)
(:action update-status-comp288-one-off
  :parameters ()
  :precondition (and
    (update-status comp288)
    (not (rebooted comp288))
    (one-off comp288)
  )
  :effect (and
    (not (update-status comp288))
    (update-status comp289)
    (not (one-off comp288))
    (probabilistic 1/4 (and (not (running comp288))))
  )
)
(:action update-status-comp289-rebooted
  :parameters ()
  :precondition (and
    (update-status comp289)
    (rebooted comp289)
  )
  :effect (and
    (not (update-status comp289))
    (update-status comp290)
    (not (rebooted comp289))
    (probabilistic 9/10 (and (running comp289)) 1/10 (and))
  )
)
(:action update-status-comp289-all-on
  :parameters ()
  :precondition (and
    (update-status comp289)
    (not (rebooted comp289))
    (all-on comp289)
  )
  :effect (and
    (not (update-status comp289))
    (update-status comp290)
    (not (all-on comp289))
    (probabilistic 1/20 (and (not (running comp289))))
  )
)
(:action update-status-comp289-one-off
  :parameters ()
  :precondition (and
    (update-status comp289)
    (not (rebooted comp289))
    (one-off comp289)
  )
  :effect (and
    (not (update-status comp289))
    (update-status comp290)
    (not (one-off comp289))
    (probabilistic 1/4 (and (not (running comp289))))
  )
)
(:action update-status-comp290-rebooted
  :parameters ()
  :precondition (and
    (update-status comp290)
    (rebooted comp290)
  )
  :effect (and
    (not (update-status comp290))
    (update-status comp291)
    (not (rebooted comp290))
    (probabilistic 9/10 (and (running comp290)) 1/10 (and))
  )
)
(:action update-status-comp290-all-on
  :parameters ()
  :precondition (and
    (update-status comp290)
    (not (rebooted comp290))
    (all-on comp290)
  )
  :effect (and
    (not (update-status comp290))
    (update-status comp291)
    (not (all-on comp290))
    (probabilistic 1/20 (and (not (running comp290))))
  )
)
(:action update-status-comp290-one-off
  :parameters ()
  :precondition (and
    (update-status comp290)
    (not (rebooted comp290))
    (one-off comp290)
  )
  :effect (and
    (not (update-status comp290))
    (update-status comp291)
    (not (one-off comp290))
    (probabilistic 1/4 (and (not (running comp290))))
  )
)
(:action update-status-comp291-rebooted
  :parameters ()
  :precondition (and
    (update-status comp291)
    (rebooted comp291)
  )
  :effect (and
    (not (update-status comp291))
    (update-status comp292)
    (not (rebooted comp291))
    (probabilistic 9/10 (and (running comp291)) 1/10 (and))
  )
)
(:action update-status-comp291-all-on
  :parameters ()
  :precondition (and
    (update-status comp291)
    (not (rebooted comp291))
    (all-on comp291)
  )
  :effect (and
    (not (update-status comp291))
    (update-status comp292)
    (not (all-on comp291))
    (probabilistic 1/20 (and (not (running comp291))))
  )
)
(:action update-status-comp291-one-off
  :parameters ()
  :precondition (and
    (update-status comp291)
    (not (rebooted comp291))
    (one-off comp291)
  )
  :effect (and
    (not (update-status comp291))
    (update-status comp292)
    (not (one-off comp291))
    (probabilistic 1/4 (and (not (running comp291))))
  )
)
(:action update-status-comp292-rebooted
  :parameters ()
  :precondition (and
    (update-status comp292)
    (rebooted comp292)
  )
  :effect (and
    (not (update-status comp292))
    (update-status comp293)
    (not (rebooted comp292))
    (probabilistic 9/10 (and (running comp292)) 1/10 (and))
  )
)
(:action update-status-comp292-all-on
  :parameters ()
  :precondition (and
    (update-status comp292)
    (not (rebooted comp292))
    (all-on comp292)
  )
  :effect (and
    (not (update-status comp292))
    (update-status comp293)
    (not (all-on comp292))
    (probabilistic 1/20 (and (not (running comp292))))
  )
)
(:action update-status-comp292-one-off
  :parameters ()
  :precondition (and
    (update-status comp292)
    (not (rebooted comp292))
    (one-off comp292)
  )
  :effect (and
    (not (update-status comp292))
    (update-status comp293)
    (not (one-off comp292))
    (probabilistic 1/4 (and (not (running comp292))))
  )
)
(:action update-status-comp293-rebooted
  :parameters ()
  :precondition (and
    (update-status comp293)
    (rebooted comp293)
  )
  :effect (and
    (not (update-status comp293))
    (update-status comp294)
    (not (rebooted comp293))
    (probabilistic 9/10 (and (running comp293)) 1/10 (and))
  )
)
(:action update-status-comp293-all-on
  :parameters ()
  :precondition (and
    (update-status comp293)
    (not (rebooted comp293))
    (all-on comp293)
  )
  :effect (and
    (not (update-status comp293))
    (update-status comp294)
    (not (all-on comp293))
    (probabilistic 1/20 (and (not (running comp293))))
  )
)
(:action update-status-comp293-one-off
  :parameters ()
  :precondition (and
    (update-status comp293)
    (not (rebooted comp293))
    (one-off comp293)
  )
  :effect (and
    (not (update-status comp293))
    (update-status comp294)
    (not (one-off comp293))
    (probabilistic 1/4 (and (not (running comp293))))
  )
)
(:action update-status-comp294-rebooted
  :parameters ()
  :precondition (and
    (update-status comp294)
    (rebooted comp294)
  )
  :effect (and
    (not (update-status comp294))
    (update-status comp295)
    (not (rebooted comp294))
    (probabilistic 9/10 (and (running comp294)) 1/10 (and))
  )
)
(:action update-status-comp294-all-on
  :parameters ()
  :precondition (and
    (update-status comp294)
    (not (rebooted comp294))
    (all-on comp294)
  )
  :effect (and
    (not (update-status comp294))
    (update-status comp295)
    (not (all-on comp294))
    (probabilistic 1/20 (and (not (running comp294))))
  )
)
(:action update-status-comp294-one-off
  :parameters ()
  :precondition (and
    (update-status comp294)
    (not (rebooted comp294))
    (one-off comp294)
  )
  :effect (and
    (not (update-status comp294))
    (update-status comp295)
    (not (one-off comp294))
    (probabilistic 1/4 (and (not (running comp294))))
  )
)
(:action update-status-comp295-rebooted
  :parameters ()
  :precondition (and
    (update-status comp295)
    (rebooted comp295)
  )
  :effect (and
    (not (update-status comp295))
    (update-status comp296)
    (not (rebooted comp295))
    (probabilistic 9/10 (and (running comp295)) 1/10 (and))
  )
)
(:action update-status-comp295-all-on
  :parameters ()
  :precondition (and
    (update-status comp295)
    (not (rebooted comp295))
    (all-on comp295)
  )
  :effect (and
    (not (update-status comp295))
    (update-status comp296)
    (not (all-on comp295))
    (probabilistic 1/20 (and (not (running comp295))))
  )
)
(:action update-status-comp295-one-off
  :parameters ()
  :precondition (and
    (update-status comp295)
    (not (rebooted comp295))
    (one-off comp295)
  )
  :effect (and
    (not (update-status comp295))
    (update-status comp296)
    (not (one-off comp295))
    (probabilistic 1/4 (and (not (running comp295))))
  )
)
(:action update-status-comp296-rebooted
  :parameters ()
  :precondition (and
    (update-status comp296)
    (rebooted comp296)
  )
  :effect (and
    (not (update-status comp296))
    (update-status comp297)
    (not (rebooted comp296))
    (probabilistic 9/10 (and (running comp296)) 1/10 (and))
  )
)
(:action update-status-comp296-all-on
  :parameters ()
  :precondition (and
    (update-status comp296)
    (not (rebooted comp296))
    (all-on comp296)
  )
  :effect (and
    (not (update-status comp296))
    (update-status comp297)
    (not (all-on comp296))
    (probabilistic 1/20 (and (not (running comp296))))
  )
)
(:action update-status-comp296-one-off
  :parameters ()
  :precondition (and
    (update-status comp296)
    (not (rebooted comp296))
    (one-off comp296)
  )
  :effect (and
    (not (update-status comp296))
    (update-status comp297)
    (not (one-off comp296))
    (probabilistic 1/4 (and (not (running comp296))))
  )
)
(:action update-status-comp297-rebooted
  :parameters ()
  :precondition (and
    (update-status comp297)
    (rebooted comp297)
  )
  :effect (and
    (not (update-status comp297))
    (update-status comp298)
    (not (rebooted comp297))
    (probabilistic 9/10 (and (running comp297)) 1/10 (and))
  )
)
(:action update-status-comp297-all-on
  :parameters ()
  :precondition (and
    (update-status comp297)
    (not (rebooted comp297))
    (all-on comp297)
  )
  :effect (and
    (not (update-status comp297))
    (update-status comp298)
    (not (all-on comp297))
    (probabilistic 1/20 (and (not (running comp297))))
  )
)
(:action update-status-comp297-one-off
  :parameters ()
  :precondition (and
    (update-status comp297)
    (not (rebooted comp297))
    (one-off comp297)
  )
  :effect (and
    (not (update-status comp297))
    (update-status comp298)
    (not (one-off comp297))
    (probabilistic 1/4 (and (not (running comp297))))
  )
)
(:action update-status-comp298-rebooted
  :parameters ()
  :precondition (and
    (update-status comp298)
    (rebooted comp298)
  )
  :effect (and
    (not (update-status comp298))
    (update-status comp299)
    (not (rebooted comp298))
    (probabilistic 9/10 (and (running comp298)) 1/10 (and))
  )
)
(:action update-status-comp298-all-on
  :parameters ()
  :precondition (and
    (update-status comp298)
    (not (rebooted comp298))
    (all-on comp298)
  )
  :effect (and
    (not (update-status comp298))
    (update-status comp299)
    (not (all-on comp298))
    (probabilistic 1/20 (and (not (running comp298))))
  )
)
(:action update-status-comp298-one-off
  :parameters ()
  :precondition (and
    (update-status comp298)
    (not (rebooted comp298))
    (one-off comp298)
  )
  :effect (and
    (not (update-status comp298))
    (update-status comp299)
    (not (one-off comp298))
    (probabilistic 1/4 (and (not (running comp298))))
  )
)
(:action update-status-comp299-rebooted
  :parameters ()
  :precondition (and
    (update-status comp299)
    (rebooted comp299)
  )
  :effect (and
    (not (update-status comp299))
    (update-status comp300)
    (not (rebooted comp299))
    (probabilistic 9/10 (and (running comp299)) 1/10 (and))
  )
)
(:action update-status-comp299-all-on
  :parameters ()
  :precondition (and
    (update-status comp299)
    (not (rebooted comp299))
    (all-on comp299)
  )
  :effect (and
    (not (update-status comp299))
    (update-status comp300)
    (not (all-on comp299))
    (probabilistic 1/20 (and (not (running comp299))))
  )
)
(:action update-status-comp299-one-off
  :parameters ()
  :precondition (and
    (update-status comp299)
    (not (rebooted comp299))
    (one-off comp299)
  )
  :effect (and
    (not (update-status comp299))
    (update-status comp300)
    (not (one-off comp299))
    (probabilistic 1/4 (and (not (running comp299))))
  )
)
(:action update-status-comp300-rebooted
  :parameters ()
  :precondition (and
    (update-status comp300)
    (rebooted comp300)
  )
  :effect (and
    (not (update-status comp300))
    (update-status comp301)
    (not (rebooted comp300))
    (probabilistic 9/10 (and (running comp300)) 1/10 (and))
  )
)
(:action update-status-comp300-all-on
  :parameters ()
  :precondition (and
    (update-status comp300)
    (not (rebooted comp300))
    (all-on comp300)
  )
  :effect (and
    (not (update-status comp300))
    (update-status comp301)
    (not (all-on comp300))
    (probabilistic 1/20 (and (not (running comp300))))
  )
)
(:action update-status-comp300-one-off
  :parameters ()
  :precondition (and
    (update-status comp300)
    (not (rebooted comp300))
    (one-off comp300)
  )
  :effect (and
    (not (update-status comp300))
    (update-status comp301)
    (not (one-off comp300))
    (probabilistic 1/4 (and (not (running comp300))))
  )
)
(:action update-status-comp301-rebooted
  :parameters ()
  :precondition (and
    (update-status comp301)
    (rebooted comp301)
  )
  :effect (and
    (not (update-status comp301))
    (update-status comp302)
    (not (rebooted comp301))
    (probabilistic 9/10 (and (running comp301)) 1/10 (and))
  )
)
(:action update-status-comp301-all-on
  :parameters ()
  :precondition (and
    (update-status comp301)
    (not (rebooted comp301))
    (all-on comp301)
  )
  :effect (and
    (not (update-status comp301))
    (update-status comp302)
    (not (all-on comp301))
    (probabilistic 1/20 (and (not (running comp301))))
  )
)
(:action update-status-comp301-one-off
  :parameters ()
  :precondition (and
    (update-status comp301)
    (not (rebooted comp301))
    (one-off comp301)
  )
  :effect (and
    (not (update-status comp301))
    (update-status comp302)
    (not (one-off comp301))
    (probabilistic 1/4 (and (not (running comp301))))
  )
)
(:action update-status-comp302-rebooted
  :parameters ()
  :precondition (and
    (update-status comp302)
    (rebooted comp302)
  )
  :effect (and
    (not (update-status comp302))
    (update-status comp303)
    (not (rebooted comp302))
    (probabilistic 9/10 (and (running comp302)) 1/10 (and))
  )
)
(:action update-status-comp302-all-on
  :parameters ()
  :precondition (and
    (update-status comp302)
    (not (rebooted comp302))
    (all-on comp302)
  )
  :effect (and
    (not (update-status comp302))
    (update-status comp303)
    (not (all-on comp302))
    (probabilistic 1/20 (and (not (running comp302))))
  )
)
(:action update-status-comp302-one-off
  :parameters ()
  :precondition (and
    (update-status comp302)
    (not (rebooted comp302))
    (one-off comp302)
  )
  :effect (and
    (not (update-status comp302))
    (update-status comp303)
    (not (one-off comp302))
    (probabilistic 1/4 (and (not (running comp302))))
  )
)
(:action update-status-comp303-rebooted
  :parameters ()
  :precondition (and
    (update-status comp303)
    (rebooted comp303)
  )
  :effect (and
    (not (update-status comp303))
    (update-status comp304)
    (not (rebooted comp303))
    (probabilistic 9/10 (and (running comp303)) 1/10 (and))
  )
)
(:action update-status-comp303-all-on
  :parameters ()
  :precondition (and
    (update-status comp303)
    (not (rebooted comp303))
    (all-on comp303)
  )
  :effect (and
    (not (update-status comp303))
    (update-status comp304)
    (not (all-on comp303))
    (probabilistic 1/20 (and (not (running comp303))))
  )
)
(:action update-status-comp303-one-off
  :parameters ()
  :precondition (and
    (update-status comp303)
    (not (rebooted comp303))
    (one-off comp303)
  )
  :effect (and
    (not (update-status comp303))
    (update-status comp304)
    (not (one-off comp303))
    (probabilistic 1/4 (and (not (running comp303))))
  )
)
(:action update-status-comp304-rebooted
  :parameters ()
  :precondition (and
    (update-status comp304)
    (rebooted comp304)
  )
  :effect (and
    (not (update-status comp304))
    (update-status comp305)
    (not (rebooted comp304))
    (probabilistic 9/10 (and (running comp304)) 1/10 (and))
  )
)
(:action update-status-comp304-all-on
  :parameters ()
  :precondition (and
    (update-status comp304)
    (not (rebooted comp304))
    (all-on comp304)
  )
  :effect (and
    (not (update-status comp304))
    (update-status comp305)
    (not (all-on comp304))
    (probabilistic 1/20 (and (not (running comp304))))
  )
)
(:action update-status-comp304-one-off
  :parameters ()
  :precondition (and
    (update-status comp304)
    (not (rebooted comp304))
    (one-off comp304)
  )
  :effect (and
    (not (update-status comp304))
    (update-status comp305)
    (not (one-off comp304))
    (probabilistic 1/4 (and (not (running comp304))))
  )
)
(:action update-status-comp305-rebooted
  :parameters ()
  :precondition (and
    (update-status comp305)
    (rebooted comp305)
  )
  :effect (and
    (not (update-status comp305))
    (update-status comp306)
    (not (rebooted comp305))
    (probabilistic 9/10 (and (running comp305)) 1/10 (and))
  )
)
(:action update-status-comp305-all-on
  :parameters ()
  :precondition (and
    (update-status comp305)
    (not (rebooted comp305))
    (all-on comp305)
  )
  :effect (and
    (not (update-status comp305))
    (update-status comp306)
    (not (all-on comp305))
    (probabilistic 1/20 (and (not (running comp305))))
  )
)
(:action update-status-comp305-one-off
  :parameters ()
  :precondition (and
    (update-status comp305)
    (not (rebooted comp305))
    (one-off comp305)
  )
  :effect (and
    (not (update-status comp305))
    (update-status comp306)
    (not (one-off comp305))
    (probabilistic 1/4 (and (not (running comp305))))
  )
)
(:action update-status-comp306-rebooted
  :parameters ()
  :precondition (and
    (update-status comp306)
    (rebooted comp306)
  )
  :effect (and
    (not (update-status comp306))
    (update-status comp307)
    (not (rebooted comp306))
    (probabilistic 9/10 (and (running comp306)) 1/10 (and))
  )
)
(:action update-status-comp306-all-on
  :parameters ()
  :precondition (and
    (update-status comp306)
    (not (rebooted comp306))
    (all-on comp306)
  )
  :effect (and
    (not (update-status comp306))
    (update-status comp307)
    (not (all-on comp306))
    (probabilistic 1/20 (and (not (running comp306))))
  )
)
(:action update-status-comp306-one-off
  :parameters ()
  :precondition (and
    (update-status comp306)
    (not (rebooted comp306))
    (one-off comp306)
  )
  :effect (and
    (not (update-status comp306))
    (update-status comp307)
    (not (one-off comp306))
    (probabilistic 1/4 (and (not (running comp306))))
  )
)
(:action update-status-comp307-rebooted
  :parameters ()
  :precondition (and
    (update-status comp307)
    (rebooted comp307)
  )
  :effect (and
    (not (update-status comp307))
    (update-status comp308)
    (not (rebooted comp307))
    (probabilistic 9/10 (and (running comp307)) 1/10 (and))
  )
)
(:action update-status-comp307-all-on
  :parameters ()
  :precondition (and
    (update-status comp307)
    (not (rebooted comp307))
    (all-on comp307)
  )
  :effect (and
    (not (update-status comp307))
    (update-status comp308)
    (not (all-on comp307))
    (probabilistic 1/20 (and (not (running comp307))))
  )
)
(:action update-status-comp307-one-off
  :parameters ()
  :precondition (and
    (update-status comp307)
    (not (rebooted comp307))
    (one-off comp307)
  )
  :effect (and
    (not (update-status comp307))
    (update-status comp308)
    (not (one-off comp307))
    (probabilistic 1/4 (and (not (running comp307))))
  )
)
(:action update-status-comp308-rebooted
  :parameters ()
  :precondition (and
    (update-status comp308)
    (rebooted comp308)
  )
  :effect (and
    (not (update-status comp308))
    (update-status comp309)
    (not (rebooted comp308))
    (probabilistic 9/10 (and (running comp308)) 1/10 (and))
  )
)
(:action update-status-comp308-all-on
  :parameters ()
  :precondition (and
    (update-status comp308)
    (not (rebooted comp308))
    (all-on comp308)
  )
  :effect (and
    (not (update-status comp308))
    (update-status comp309)
    (not (all-on comp308))
    (probabilistic 1/20 (and (not (running comp308))))
  )
)
(:action update-status-comp308-one-off
  :parameters ()
  :precondition (and
    (update-status comp308)
    (not (rebooted comp308))
    (one-off comp308)
  )
  :effect (and
    (not (update-status comp308))
    (update-status comp309)
    (not (one-off comp308))
    (probabilistic 1/4 (and (not (running comp308))))
  )
)
(:action update-status-comp309-rebooted
  :parameters ()
  :precondition (and
    (update-status comp309)
    (rebooted comp309)
  )
  :effect (and
    (not (update-status comp309))
    (update-status comp310)
    (not (rebooted comp309))
    (probabilistic 9/10 (and (running comp309)) 1/10 (and))
  )
)
(:action update-status-comp309-all-on
  :parameters ()
  :precondition (and
    (update-status comp309)
    (not (rebooted comp309))
    (all-on comp309)
  )
  :effect (and
    (not (update-status comp309))
    (update-status comp310)
    (not (all-on comp309))
    (probabilistic 1/20 (and (not (running comp309))))
  )
)
(:action update-status-comp309-one-off
  :parameters ()
  :precondition (and
    (update-status comp309)
    (not (rebooted comp309))
    (one-off comp309)
  )
  :effect (and
    (not (update-status comp309))
    (update-status comp310)
    (not (one-off comp309))
    (probabilistic 1/4 (and (not (running comp309))))
  )
)
(:action update-status-comp310-rebooted
  :parameters ()
  :precondition (and
    (update-status comp310)
    (rebooted comp310)
  )
  :effect (and
    (not (update-status comp310))
    (update-status comp311)
    (not (rebooted comp310))
    (probabilistic 9/10 (and (running comp310)) 1/10 (and))
  )
)
(:action update-status-comp310-all-on
  :parameters ()
  :precondition (and
    (update-status comp310)
    (not (rebooted comp310))
    (all-on comp310)
  )
  :effect (and
    (not (update-status comp310))
    (update-status comp311)
    (not (all-on comp310))
    (probabilistic 1/20 (and (not (running comp310))))
  )
)
(:action update-status-comp310-one-off
  :parameters ()
  :precondition (and
    (update-status comp310)
    (not (rebooted comp310))
    (one-off comp310)
  )
  :effect (and
    (not (update-status comp310))
    (update-status comp311)
    (not (one-off comp310))
    (probabilistic 1/4 (and (not (running comp310))))
  )
)
(:action update-status-comp311-rebooted
  :parameters ()
  :precondition (and
    (update-status comp311)
    (rebooted comp311)
  )
  :effect (and
    (not (update-status comp311))
    (update-status comp312)
    (not (rebooted comp311))
    (probabilistic 9/10 (and (running comp311)) 1/10 (and))
  )
)
(:action update-status-comp311-all-on
  :parameters ()
  :precondition (and
    (update-status comp311)
    (not (rebooted comp311))
    (all-on comp311)
  )
  :effect (and
    (not (update-status comp311))
    (update-status comp312)
    (not (all-on comp311))
    (probabilistic 1/20 (and (not (running comp311))))
  )
)
(:action update-status-comp311-one-off
  :parameters ()
  :precondition (and
    (update-status comp311)
    (not (rebooted comp311))
    (one-off comp311)
  )
  :effect (and
    (not (update-status comp311))
    (update-status comp312)
    (not (one-off comp311))
    (probabilistic 1/4 (and (not (running comp311))))
  )
)
(:action update-status-comp312-rebooted
  :parameters ()
  :precondition (and
    (update-status comp312)
    (rebooted comp312)
  )
  :effect (and
    (not (update-status comp312))
    (update-status comp313)
    (not (rebooted comp312))
    (probabilistic 9/10 (and (running comp312)) 1/10 (and))
  )
)
(:action update-status-comp312-all-on
  :parameters ()
  :precondition (and
    (update-status comp312)
    (not (rebooted comp312))
    (all-on comp312)
  )
  :effect (and
    (not (update-status comp312))
    (update-status comp313)
    (not (all-on comp312))
    (probabilistic 1/20 (and (not (running comp312))))
  )
)
(:action update-status-comp312-one-off
  :parameters ()
  :precondition (and
    (update-status comp312)
    (not (rebooted comp312))
    (one-off comp312)
  )
  :effect (and
    (not (update-status comp312))
    (update-status comp313)
    (not (one-off comp312))
    (probabilistic 1/4 (and (not (running comp312))))
  )
)
(:action update-status-comp313-rebooted
  :parameters ()
  :precondition (and
    (update-status comp313)
    (rebooted comp313)
  )
  :effect (and
    (not (update-status comp313))
    (update-status comp314)
    (not (rebooted comp313))
    (probabilistic 9/10 (and (running comp313)) 1/10 (and))
  )
)
(:action update-status-comp313-all-on
  :parameters ()
  :precondition (and
    (update-status comp313)
    (not (rebooted comp313))
    (all-on comp313)
  )
  :effect (and
    (not (update-status comp313))
    (update-status comp314)
    (not (all-on comp313))
    (probabilistic 1/20 (and (not (running comp313))))
  )
)
(:action update-status-comp313-one-off
  :parameters ()
  :precondition (and
    (update-status comp313)
    (not (rebooted comp313))
    (one-off comp313)
  )
  :effect (and
    (not (update-status comp313))
    (update-status comp314)
    (not (one-off comp313))
    (probabilistic 1/4 (and (not (running comp313))))
  )
)
(:action update-status-comp314-rebooted
  :parameters ()
  :precondition (and
    (update-status comp314)
    (rebooted comp314)
  )
  :effect (and
    (not (update-status comp314))
    (update-status comp315)
    (not (rebooted comp314))
    (probabilistic 9/10 (and (running comp314)) 1/10 (and))
  )
)
(:action update-status-comp314-all-on
  :parameters ()
  :precondition (and
    (update-status comp314)
    (not (rebooted comp314))
    (all-on comp314)
  )
  :effect (and
    (not (update-status comp314))
    (update-status comp315)
    (not (all-on comp314))
    (probabilistic 1/20 (and (not (running comp314))))
  )
)
(:action update-status-comp314-one-off
  :parameters ()
  :precondition (and
    (update-status comp314)
    (not (rebooted comp314))
    (one-off comp314)
  )
  :effect (and
    (not (update-status comp314))
    (update-status comp315)
    (not (one-off comp314))
    (probabilistic 1/4 (and (not (running comp314))))
  )
)
(:action update-status-comp315-rebooted
  :parameters ()
  :precondition (and
    (update-status comp315)
    (rebooted comp315)
  )
  :effect (and
    (not (update-status comp315))
    (update-status comp316)
    (not (rebooted comp315))
    (probabilistic 9/10 (and (running comp315)) 1/10 (and))
  )
)
(:action update-status-comp315-all-on
  :parameters ()
  :precondition (and
    (update-status comp315)
    (not (rebooted comp315))
    (all-on comp315)
  )
  :effect (and
    (not (update-status comp315))
    (update-status comp316)
    (not (all-on comp315))
    (probabilistic 1/20 (and (not (running comp315))))
  )
)
(:action update-status-comp315-one-off
  :parameters ()
  :precondition (and
    (update-status comp315)
    (not (rebooted comp315))
    (one-off comp315)
  )
  :effect (and
    (not (update-status comp315))
    (update-status comp316)
    (not (one-off comp315))
    (probabilistic 1/4 (and (not (running comp315))))
  )
)
(:action update-status-comp316-rebooted
  :parameters ()
  :precondition (and
    (update-status comp316)
    (rebooted comp316)
  )
  :effect (and
    (not (update-status comp316))
    (update-status comp317)
    (not (rebooted comp316))
    (probabilistic 9/10 (and (running comp316)) 1/10 (and))
  )
)
(:action update-status-comp316-all-on
  :parameters ()
  :precondition (and
    (update-status comp316)
    (not (rebooted comp316))
    (all-on comp316)
  )
  :effect (and
    (not (update-status comp316))
    (update-status comp317)
    (not (all-on comp316))
    (probabilistic 1/20 (and (not (running comp316))))
  )
)
(:action update-status-comp316-one-off
  :parameters ()
  :precondition (and
    (update-status comp316)
    (not (rebooted comp316))
    (one-off comp316)
  )
  :effect (and
    (not (update-status comp316))
    (update-status comp317)
    (not (one-off comp316))
    (probabilistic 1/4 (and (not (running comp316))))
  )
)
(:action update-status-comp317-rebooted
  :parameters ()
  :precondition (and
    (update-status comp317)
    (rebooted comp317)
  )
  :effect (and
    (not (update-status comp317))
    (update-status comp318)
    (not (rebooted comp317))
    (probabilistic 9/10 (and (running comp317)) 1/10 (and))
  )
)
(:action update-status-comp317-all-on
  :parameters ()
  :precondition (and
    (update-status comp317)
    (not (rebooted comp317))
    (all-on comp317)
  )
  :effect (and
    (not (update-status comp317))
    (update-status comp318)
    (not (all-on comp317))
    (probabilistic 1/20 (and (not (running comp317))))
  )
)
(:action update-status-comp317-one-off
  :parameters ()
  :precondition (and
    (update-status comp317)
    (not (rebooted comp317))
    (one-off comp317)
  )
  :effect (and
    (not (update-status comp317))
    (update-status comp318)
    (not (one-off comp317))
    (probabilistic 1/4 (and (not (running comp317))))
  )
)
(:action update-status-comp318-rebooted
  :parameters ()
  :precondition (and
    (update-status comp318)
    (rebooted comp318)
  )
  :effect (and
    (not (update-status comp318))
    (update-status comp319)
    (not (rebooted comp318))
    (probabilistic 9/10 (and (running comp318)) 1/10 (and))
  )
)
(:action update-status-comp318-all-on
  :parameters ()
  :precondition (and
    (update-status comp318)
    (not (rebooted comp318))
    (all-on comp318)
  )
  :effect (and
    (not (update-status comp318))
    (update-status comp319)
    (not (all-on comp318))
    (probabilistic 1/20 (and (not (running comp318))))
  )
)
(:action update-status-comp318-one-off
  :parameters ()
  :precondition (and
    (update-status comp318)
    (not (rebooted comp318))
    (one-off comp318)
  )
  :effect (and
    (not (update-status comp318))
    (update-status comp319)
    (not (one-off comp318))
    (probabilistic 1/4 (and (not (running comp318))))
  )
)
(:action update-status-comp319-rebooted
  :parameters ()
  :precondition (and
    (update-status comp319)
    (rebooted comp319)
  )
  :effect (and
    (not (update-status comp319))
    (update-status comp320)
    (not (rebooted comp319))
    (probabilistic 9/10 (and (running comp319)) 1/10 (and))
  )
)
(:action update-status-comp319-all-on
  :parameters ()
  :precondition (and
    (update-status comp319)
    (not (rebooted comp319))
    (all-on comp319)
  )
  :effect (and
    (not (update-status comp319))
    (update-status comp320)
    (not (all-on comp319))
    (probabilistic 1/20 (and (not (running comp319))))
  )
)
(:action update-status-comp319-one-off
  :parameters ()
  :precondition (and
    (update-status comp319)
    (not (rebooted comp319))
    (one-off comp319)
  )
  :effect (and
    (not (update-status comp319))
    (update-status comp320)
    (not (one-off comp319))
    (probabilistic 1/4 (and (not (running comp319))))
  )
)
(:action update-status-comp320-rebooted
  :parameters ()
  :precondition (and
    (update-status comp320)
    (rebooted comp320)
  )
  :effect (and
    (not (update-status comp320))
    (update-status comp321)
    (not (rebooted comp320))
    (probabilistic 9/10 (and (running comp320)) 1/10 (and))
  )
)
(:action update-status-comp320-all-on
  :parameters ()
  :precondition (and
    (update-status comp320)
    (not (rebooted comp320))
    (all-on comp320)
  )
  :effect (and
    (not (update-status comp320))
    (update-status comp321)
    (not (all-on comp320))
    (probabilistic 1/20 (and (not (running comp320))))
  )
)
(:action update-status-comp320-one-off
  :parameters ()
  :precondition (and
    (update-status comp320)
    (not (rebooted comp320))
    (one-off comp320)
  )
  :effect (and
    (not (update-status comp320))
    (update-status comp321)
    (not (one-off comp320))
    (probabilistic 1/4 (and (not (running comp320))))
  )
)
(:action update-status-comp321-rebooted
  :parameters ()
  :precondition (and
    (update-status comp321)
    (rebooted comp321)
  )
  :effect (and
    (not (update-status comp321))
    (update-status comp322)
    (not (rebooted comp321))
    (probabilistic 9/10 (and (running comp321)) 1/10 (and))
  )
)
(:action update-status-comp321-all-on
  :parameters ()
  :precondition (and
    (update-status comp321)
    (not (rebooted comp321))
    (all-on comp321)
  )
  :effect (and
    (not (update-status comp321))
    (update-status comp322)
    (not (all-on comp321))
    (probabilistic 1/20 (and (not (running comp321))))
  )
)
(:action update-status-comp321-one-off
  :parameters ()
  :precondition (and
    (update-status comp321)
    (not (rebooted comp321))
    (one-off comp321)
  )
  :effect (and
    (not (update-status comp321))
    (update-status comp322)
    (not (one-off comp321))
    (probabilistic 1/4 (and (not (running comp321))))
  )
)
(:action update-status-comp322-rebooted
  :parameters ()
  :precondition (and
    (update-status comp322)
    (rebooted comp322)
  )
  :effect (and
    (not (update-status comp322))
    (update-status comp323)
    (not (rebooted comp322))
    (probabilistic 9/10 (and (running comp322)) 1/10 (and))
  )
)
(:action update-status-comp322-all-on
  :parameters ()
  :precondition (and
    (update-status comp322)
    (not (rebooted comp322))
    (all-on comp322)
  )
  :effect (and
    (not (update-status comp322))
    (update-status comp323)
    (not (all-on comp322))
    (probabilistic 1/20 (and (not (running comp322))))
  )
)
(:action update-status-comp322-one-off
  :parameters ()
  :precondition (and
    (update-status comp322)
    (not (rebooted comp322))
    (one-off comp322)
  )
  :effect (and
    (not (update-status comp322))
    (update-status comp323)
    (not (one-off comp322))
    (probabilistic 1/4 (and (not (running comp322))))
  )
)
(:action update-status-comp323-rebooted
  :parameters ()
  :precondition (and
    (update-status comp323)
    (rebooted comp323)
  )
  :effect (and
    (not (update-status comp323))
    (update-status comp324)
    (not (rebooted comp323))
    (probabilistic 9/10 (and (running comp323)) 1/10 (and))
  )
)
(:action update-status-comp323-all-on
  :parameters ()
  :precondition (and
    (update-status comp323)
    (not (rebooted comp323))
    (all-on comp323)
  )
  :effect (and
    (not (update-status comp323))
    (update-status comp324)
    (not (all-on comp323))
    (probabilistic 1/20 (and (not (running comp323))))
  )
)
(:action update-status-comp323-one-off
  :parameters ()
  :precondition (and
    (update-status comp323)
    (not (rebooted comp323))
    (one-off comp323)
  )
  :effect (and
    (not (update-status comp323))
    (update-status comp324)
    (not (one-off comp323))
    (probabilistic 1/4 (and (not (running comp323))))
  )
)
(:action update-status-comp324-rebooted
  :parameters ()
  :precondition (and
    (update-status comp324)
    (rebooted comp324)
  )
  :effect (and
    (not (update-status comp324))
    (update-status comp325)
    (not (rebooted comp324))
    (probabilistic 9/10 (and (running comp324)) 1/10 (and))
  )
)
(:action update-status-comp324-all-on
  :parameters ()
  :precondition (and
    (update-status comp324)
    (not (rebooted comp324))
    (all-on comp324)
  )
  :effect (and
    (not (update-status comp324))
    (update-status comp325)
    (not (all-on comp324))
    (probabilistic 1/20 (and (not (running comp324))))
  )
)
(:action update-status-comp324-one-off
  :parameters ()
  :precondition (and
    (update-status comp324)
    (not (rebooted comp324))
    (one-off comp324)
  )
  :effect (and
    (not (update-status comp324))
    (update-status comp325)
    (not (one-off comp324))
    (probabilistic 1/4 (and (not (running comp324))))
  )
)
(:action update-status-comp325-rebooted
  :parameters ()
  :precondition (and
    (update-status comp325)
    (rebooted comp325)
  )
  :effect (and
    (not (update-status comp325))
    (update-status comp326)
    (not (rebooted comp325))
    (probabilistic 9/10 (and (running comp325)) 1/10 (and))
  )
)
(:action update-status-comp325-all-on
  :parameters ()
  :precondition (and
    (update-status comp325)
    (not (rebooted comp325))
    (all-on comp325)
  )
  :effect (and
    (not (update-status comp325))
    (update-status comp326)
    (not (all-on comp325))
    (probabilistic 1/20 (and (not (running comp325))))
  )
)
(:action update-status-comp325-one-off
  :parameters ()
  :precondition (and
    (update-status comp325)
    (not (rebooted comp325))
    (one-off comp325)
  )
  :effect (and
    (not (update-status comp325))
    (update-status comp326)
    (not (one-off comp325))
    (probabilistic 1/4 (and (not (running comp325))))
  )
)
(:action update-status-comp326-rebooted
  :parameters ()
  :precondition (and
    (update-status comp326)
    (rebooted comp326)
  )
  :effect (and
    (not (update-status comp326))
    (update-status comp327)
    (not (rebooted comp326))
    (probabilistic 9/10 (and (running comp326)) 1/10 (and))
  )
)
(:action update-status-comp326-all-on
  :parameters ()
  :precondition (and
    (update-status comp326)
    (not (rebooted comp326))
    (all-on comp326)
  )
  :effect (and
    (not (update-status comp326))
    (update-status comp327)
    (not (all-on comp326))
    (probabilistic 1/20 (and (not (running comp326))))
  )
)
(:action update-status-comp326-one-off
  :parameters ()
  :precondition (and
    (update-status comp326)
    (not (rebooted comp326))
    (one-off comp326)
  )
  :effect (and
    (not (update-status comp326))
    (update-status comp327)
    (not (one-off comp326))
    (probabilistic 1/4 (and (not (running comp326))))
  )
)
(:action update-status-comp327-rebooted
  :parameters ()
  :precondition (and
    (update-status comp327)
    (rebooted comp327)
  )
  :effect (and
    (not (update-status comp327))
    (update-status comp328)
    (not (rebooted comp327))
    (probabilistic 9/10 (and (running comp327)) 1/10 (and))
  )
)
(:action update-status-comp327-all-on
  :parameters ()
  :precondition (and
    (update-status comp327)
    (not (rebooted comp327))
    (all-on comp327)
  )
  :effect (and
    (not (update-status comp327))
    (update-status comp328)
    (not (all-on comp327))
    (probabilistic 1/20 (and (not (running comp327))))
  )
)
(:action update-status-comp327-one-off
  :parameters ()
  :precondition (and
    (update-status comp327)
    (not (rebooted comp327))
    (one-off comp327)
  )
  :effect (and
    (not (update-status comp327))
    (update-status comp328)
    (not (one-off comp327))
    (probabilistic 1/4 (and (not (running comp327))))
  )
)
(:action update-status-comp328-rebooted
  :parameters ()
  :precondition (and
    (update-status comp328)
    (rebooted comp328)
  )
  :effect (and
    (not (update-status comp328))
    (update-status comp329)
    (not (rebooted comp328))
    (probabilistic 9/10 (and (running comp328)) 1/10 (and))
  )
)
(:action update-status-comp328-all-on
  :parameters ()
  :precondition (and
    (update-status comp328)
    (not (rebooted comp328))
    (all-on comp328)
  )
  :effect (and
    (not (update-status comp328))
    (update-status comp329)
    (not (all-on comp328))
    (probabilistic 1/20 (and (not (running comp328))))
  )
)
(:action update-status-comp328-one-off
  :parameters ()
  :precondition (and
    (update-status comp328)
    (not (rebooted comp328))
    (one-off comp328)
  )
  :effect (and
    (not (update-status comp328))
    (update-status comp329)
    (not (one-off comp328))
    (probabilistic 1/4 (and (not (running comp328))))
  )
)
(:action update-status-comp329-rebooted
  :parameters ()
  :precondition (and
    (update-status comp329)
    (rebooted comp329)
  )
  :effect (and
    (not (update-status comp329))
    (update-status comp330)
    (not (rebooted comp329))
    (probabilistic 9/10 (and (running comp329)) 1/10 (and))
  )
)
(:action update-status-comp329-all-on
  :parameters ()
  :precondition (and
    (update-status comp329)
    (not (rebooted comp329))
    (all-on comp329)
  )
  :effect (and
    (not (update-status comp329))
    (update-status comp330)
    (not (all-on comp329))
    (probabilistic 1/20 (and (not (running comp329))))
  )
)
(:action update-status-comp329-one-off
  :parameters ()
  :precondition (and
    (update-status comp329)
    (not (rebooted comp329))
    (one-off comp329)
  )
  :effect (and
    (not (update-status comp329))
    (update-status comp330)
    (not (one-off comp329))
    (probabilistic 1/4 (and (not (running comp329))))
  )
)
(:action update-status-comp330-rebooted
  :parameters ()
  :precondition (and
    (update-status comp330)
    (rebooted comp330)
  )
  :effect (and
    (not (update-status comp330))
    (update-status comp331)
    (not (rebooted comp330))
    (probabilistic 9/10 (and (running comp330)) 1/10 (and))
  )
)
(:action update-status-comp330-all-on
  :parameters ()
  :precondition (and
    (update-status comp330)
    (not (rebooted comp330))
    (all-on comp330)
  )
  :effect (and
    (not (update-status comp330))
    (update-status comp331)
    (not (all-on comp330))
    (probabilistic 1/20 (and (not (running comp330))))
  )
)
(:action update-status-comp330-one-off
  :parameters ()
  :precondition (and
    (update-status comp330)
    (not (rebooted comp330))
    (one-off comp330)
  )
  :effect (and
    (not (update-status comp330))
    (update-status comp331)
    (not (one-off comp330))
    (probabilistic 1/4 (and (not (running comp330))))
  )
)
(:action update-status-comp331-rebooted
  :parameters ()
  :precondition (and
    (update-status comp331)
    (rebooted comp331)
  )
  :effect (and
    (not (update-status comp331))
    (update-status comp332)
    (not (rebooted comp331))
    (probabilistic 9/10 (and (running comp331)) 1/10 (and))
  )
)
(:action update-status-comp331-all-on
  :parameters ()
  :precondition (and
    (update-status comp331)
    (not (rebooted comp331))
    (all-on comp331)
  )
  :effect (and
    (not (update-status comp331))
    (update-status comp332)
    (not (all-on comp331))
    (probabilistic 1/20 (and (not (running comp331))))
  )
)
(:action update-status-comp331-one-off
  :parameters ()
  :precondition (and
    (update-status comp331)
    (not (rebooted comp331))
    (one-off comp331)
  )
  :effect (and
    (not (update-status comp331))
    (update-status comp332)
    (not (one-off comp331))
    (probabilistic 1/4 (and (not (running comp331))))
  )
)
(:action update-status-comp332-rebooted
  :parameters ()
  :precondition (and
    (update-status comp332)
    (rebooted comp332)
  )
  :effect (and
    (not (update-status comp332))
    (update-status comp333)
    (not (rebooted comp332))
    (probabilistic 9/10 (and (running comp332)) 1/10 (and))
  )
)
(:action update-status-comp332-all-on
  :parameters ()
  :precondition (and
    (update-status comp332)
    (not (rebooted comp332))
    (all-on comp332)
  )
  :effect (and
    (not (update-status comp332))
    (update-status comp333)
    (not (all-on comp332))
    (probabilistic 1/20 (and (not (running comp332))))
  )
)
(:action update-status-comp332-one-off
  :parameters ()
  :precondition (and
    (update-status comp332)
    (not (rebooted comp332))
    (one-off comp332)
  )
  :effect (and
    (not (update-status comp332))
    (update-status comp333)
    (not (one-off comp332))
    (probabilistic 1/4 (and (not (running comp332))))
  )
)
(:action update-status-comp333-rebooted
  :parameters ()
  :precondition (and
    (update-status comp333)
    (rebooted comp333)
  )
  :effect (and
    (not (update-status comp333))
    (update-status comp334)
    (not (rebooted comp333))
    (probabilistic 9/10 (and (running comp333)) 1/10 (and))
  )
)
(:action update-status-comp333-all-on
  :parameters ()
  :precondition (and
    (update-status comp333)
    (not (rebooted comp333))
    (all-on comp333)
  )
  :effect (and
    (not (update-status comp333))
    (update-status comp334)
    (not (all-on comp333))
    (probabilistic 1/20 (and (not (running comp333))))
  )
)
(:action update-status-comp333-one-off
  :parameters ()
  :precondition (and
    (update-status comp333)
    (not (rebooted comp333))
    (one-off comp333)
  )
  :effect (and
    (not (update-status comp333))
    (update-status comp334)
    (not (one-off comp333))
    (probabilistic 1/4 (and (not (running comp333))))
  )
)
(:action update-status-comp334-rebooted
  :parameters ()
  :precondition (and
    (update-status comp334)
    (rebooted comp334)
  )
  :effect (and
    (not (update-status comp334))
    (update-status comp335)
    (not (rebooted comp334))
    (probabilistic 9/10 (and (running comp334)) 1/10 (and))
  )
)
(:action update-status-comp334-all-on
  :parameters ()
  :precondition (and
    (update-status comp334)
    (not (rebooted comp334))
    (all-on comp334)
  )
  :effect (and
    (not (update-status comp334))
    (update-status comp335)
    (not (all-on comp334))
    (probabilistic 1/20 (and (not (running comp334))))
  )
)
(:action update-status-comp334-one-off
  :parameters ()
  :precondition (and
    (update-status comp334)
    (not (rebooted comp334))
    (one-off comp334)
  )
  :effect (and
    (not (update-status comp334))
    (update-status comp335)
    (not (one-off comp334))
    (probabilistic 1/4 (and (not (running comp334))))
  )
)
(:action update-status-comp335-rebooted
  :parameters ()
  :precondition (and
    (update-status comp335)
    (rebooted comp335)
  )
  :effect (and
    (not (update-status comp335))
    (update-status comp336)
    (not (rebooted comp335))
    (probabilistic 9/10 (and (running comp335)) 1/10 (and))
  )
)
(:action update-status-comp335-all-on
  :parameters ()
  :precondition (and
    (update-status comp335)
    (not (rebooted comp335))
    (all-on comp335)
  )
  :effect (and
    (not (update-status comp335))
    (update-status comp336)
    (not (all-on comp335))
    (probabilistic 1/20 (and (not (running comp335))))
  )
)
(:action update-status-comp335-one-off
  :parameters ()
  :precondition (and
    (update-status comp335)
    (not (rebooted comp335))
    (one-off comp335)
  )
  :effect (and
    (not (update-status comp335))
    (update-status comp336)
    (not (one-off comp335))
    (probabilistic 1/4 (and (not (running comp335))))
  )
)
(:action update-status-comp336-rebooted
  :parameters ()
  :precondition (and
    (update-status comp336)
    (rebooted comp336)
  )
  :effect (and
    (not (update-status comp336))
    (update-status comp337)
    (not (rebooted comp336))
    (probabilistic 9/10 (and (running comp336)) 1/10 (and))
  )
)
(:action update-status-comp336-all-on
  :parameters ()
  :precondition (and
    (update-status comp336)
    (not (rebooted comp336))
    (all-on comp336)
  )
  :effect (and
    (not (update-status comp336))
    (update-status comp337)
    (not (all-on comp336))
    (probabilistic 1/20 (and (not (running comp336))))
  )
)
(:action update-status-comp336-one-off
  :parameters ()
  :precondition (and
    (update-status comp336)
    (not (rebooted comp336))
    (one-off comp336)
  )
  :effect (and
    (not (update-status comp336))
    (update-status comp337)
    (not (one-off comp336))
    (probabilistic 1/4 (and (not (running comp336))))
  )
)
(:action update-status-comp337-rebooted
  :parameters ()
  :precondition (and
    (update-status comp337)
    (rebooted comp337)
  )
  :effect (and
    (not (update-status comp337))
    (update-status comp338)
    (not (rebooted comp337))
    (probabilistic 9/10 (and (running comp337)) 1/10 (and))
  )
)
(:action update-status-comp337-all-on
  :parameters ()
  :precondition (and
    (update-status comp337)
    (not (rebooted comp337))
    (all-on comp337)
  )
  :effect (and
    (not (update-status comp337))
    (update-status comp338)
    (not (all-on comp337))
    (probabilistic 1/20 (and (not (running comp337))))
  )
)
(:action update-status-comp337-one-off
  :parameters ()
  :precondition (and
    (update-status comp337)
    (not (rebooted comp337))
    (one-off comp337)
  )
  :effect (and
    (not (update-status comp337))
    (update-status comp338)
    (not (one-off comp337))
    (probabilistic 1/4 (and (not (running comp337))))
  )
)
(:action update-status-comp338-rebooted
  :parameters ()
  :precondition (and
    (update-status comp338)
    (rebooted comp338)
  )
  :effect (and
    (not (update-status comp338))
    (update-status comp339)
    (not (rebooted comp338))
    (probabilistic 9/10 (and (running comp338)) 1/10 (and))
  )
)
(:action update-status-comp338-all-on
  :parameters ()
  :precondition (and
    (update-status comp338)
    (not (rebooted comp338))
    (all-on comp338)
  )
  :effect (and
    (not (update-status comp338))
    (update-status comp339)
    (not (all-on comp338))
    (probabilistic 1/20 (and (not (running comp338))))
  )
)
(:action update-status-comp338-one-off
  :parameters ()
  :precondition (and
    (update-status comp338)
    (not (rebooted comp338))
    (one-off comp338)
  )
  :effect (and
    (not (update-status comp338))
    (update-status comp339)
    (not (one-off comp338))
    (probabilistic 1/4 (and (not (running comp338))))
  )
)
(:action update-status-comp339-rebooted
  :parameters ()
  :precondition (and
    (update-status comp339)
    (rebooted comp339)
  )
  :effect (and
    (not (update-status comp339))
    (update-status comp340)
    (not (rebooted comp339))
    (probabilistic 9/10 (and (running comp339)) 1/10 (and))
  )
)
(:action update-status-comp339-all-on
  :parameters ()
  :precondition (and
    (update-status comp339)
    (not (rebooted comp339))
    (all-on comp339)
  )
  :effect (and
    (not (update-status comp339))
    (update-status comp340)
    (not (all-on comp339))
    (probabilistic 1/20 (and (not (running comp339))))
  )
)
(:action update-status-comp339-one-off
  :parameters ()
  :precondition (and
    (update-status comp339)
    (not (rebooted comp339))
    (one-off comp339)
  )
  :effect (and
    (not (update-status comp339))
    (update-status comp340)
    (not (one-off comp339))
    (probabilistic 1/4 (and (not (running comp339))))
  )
)
(:action update-status-comp340-rebooted
  :parameters ()
  :precondition (and
    (update-status comp340)
    (rebooted comp340)
  )
  :effect (and
    (not (update-status comp340))
    (update-status comp341)
    (not (rebooted comp340))
    (probabilistic 9/10 (and (running comp340)) 1/10 (and))
  )
)
(:action update-status-comp340-all-on
  :parameters ()
  :precondition (and
    (update-status comp340)
    (not (rebooted comp340))
    (all-on comp340)
  )
  :effect (and
    (not (update-status comp340))
    (update-status comp341)
    (not (all-on comp340))
    (probabilistic 1/20 (and (not (running comp340))))
  )
)
(:action update-status-comp340-one-off
  :parameters ()
  :precondition (and
    (update-status comp340)
    (not (rebooted comp340))
    (one-off comp340)
  )
  :effect (and
    (not (update-status comp340))
    (update-status comp341)
    (not (one-off comp340))
    (probabilistic 1/4 (and (not (running comp340))))
  )
)
(:action update-status-comp341-rebooted
  :parameters ()
  :precondition (and
    (update-status comp341)
    (rebooted comp341)
  )
  :effect (and
    (not (update-status comp341))
    (update-status comp342)
    (not (rebooted comp341))
    (probabilistic 9/10 (and (running comp341)) 1/10 (and))
  )
)
(:action update-status-comp341-all-on
  :parameters ()
  :precondition (and
    (update-status comp341)
    (not (rebooted comp341))
    (all-on comp341)
  )
  :effect (and
    (not (update-status comp341))
    (update-status comp342)
    (not (all-on comp341))
    (probabilistic 1/20 (and (not (running comp341))))
  )
)
(:action update-status-comp341-one-off
  :parameters ()
  :precondition (and
    (update-status comp341)
    (not (rebooted comp341))
    (one-off comp341)
  )
  :effect (and
    (not (update-status comp341))
    (update-status comp342)
    (not (one-off comp341))
    (probabilistic 1/4 (and (not (running comp341))))
  )
)
(:action update-status-comp342-rebooted
  :parameters ()
  :precondition (and
    (update-status comp342)
    (rebooted comp342)
  )
  :effect (and
    (not (update-status comp342))
    (update-status comp343)
    (not (rebooted comp342))
    (probabilistic 9/10 (and (running comp342)) 1/10 (and))
  )
)
(:action update-status-comp342-all-on
  :parameters ()
  :precondition (and
    (update-status comp342)
    (not (rebooted comp342))
    (all-on comp342)
  )
  :effect (and
    (not (update-status comp342))
    (update-status comp343)
    (not (all-on comp342))
    (probabilistic 1/20 (and (not (running comp342))))
  )
)
(:action update-status-comp342-one-off
  :parameters ()
  :precondition (and
    (update-status comp342)
    (not (rebooted comp342))
    (one-off comp342)
  )
  :effect (and
    (not (update-status comp342))
    (update-status comp343)
    (not (one-off comp342))
    (probabilistic 1/4 (and (not (running comp342))))
  )
)
(:action update-status-comp343-rebooted
  :parameters ()
  :precondition (and
    (update-status comp343)
    (rebooted comp343)
  )
  :effect (and
    (not (update-status comp343))
    (update-status comp344)
    (not (rebooted comp343))
    (probabilistic 9/10 (and (running comp343)) 1/10 (and))
  )
)
(:action update-status-comp343-all-on
  :parameters ()
  :precondition (and
    (update-status comp343)
    (not (rebooted comp343))
    (all-on comp343)
  )
  :effect (and
    (not (update-status comp343))
    (update-status comp344)
    (not (all-on comp343))
    (probabilistic 1/20 (and (not (running comp343))))
  )
)
(:action update-status-comp343-one-off
  :parameters ()
  :precondition (and
    (update-status comp343)
    (not (rebooted comp343))
    (one-off comp343)
  )
  :effect (and
    (not (update-status comp343))
    (update-status comp344)
    (not (one-off comp343))
    (probabilistic 1/4 (and (not (running comp343))))
  )
)
(:action update-status-comp344-rebooted
  :parameters ()
  :precondition (and
    (update-status comp344)
    (rebooted comp344)
  )
  :effect (and
    (not (update-status comp344))
    (update-status comp345)
    (not (rebooted comp344))
    (probabilistic 9/10 (and (running comp344)) 1/10 (and))
  )
)
(:action update-status-comp344-all-on
  :parameters ()
  :precondition (and
    (update-status comp344)
    (not (rebooted comp344))
    (all-on comp344)
  )
  :effect (and
    (not (update-status comp344))
    (update-status comp345)
    (not (all-on comp344))
    (probabilistic 1/20 (and (not (running comp344))))
  )
)
(:action update-status-comp344-one-off
  :parameters ()
  :precondition (and
    (update-status comp344)
    (not (rebooted comp344))
    (one-off comp344)
  )
  :effect (and
    (not (update-status comp344))
    (update-status comp345)
    (not (one-off comp344))
    (probabilistic 1/4 (and (not (running comp344))))
  )
)
(:action update-status-comp345-rebooted
  :parameters ()
  :precondition (and
    (update-status comp345)
    (rebooted comp345)
  )
  :effect (and
    (not (update-status comp345))
    (update-status comp346)
    (not (rebooted comp345))
    (probabilistic 9/10 (and (running comp345)) 1/10 (and))
  )
)
(:action update-status-comp345-all-on
  :parameters ()
  :precondition (and
    (update-status comp345)
    (not (rebooted comp345))
    (all-on comp345)
  )
  :effect (and
    (not (update-status comp345))
    (update-status comp346)
    (not (all-on comp345))
    (probabilistic 1/20 (and (not (running comp345))))
  )
)
(:action update-status-comp345-one-off
  :parameters ()
  :precondition (and
    (update-status comp345)
    (not (rebooted comp345))
    (one-off comp345)
  )
  :effect (and
    (not (update-status comp345))
    (update-status comp346)
    (not (one-off comp345))
    (probabilistic 1/4 (and (not (running comp345))))
  )
)
(:action update-status-comp346-rebooted
  :parameters ()
  :precondition (and
    (update-status comp346)
    (rebooted comp346)
  )
  :effect (and
    (not (update-status comp346))
    (update-status comp347)
    (not (rebooted comp346))
    (probabilistic 9/10 (and (running comp346)) 1/10 (and))
  )
)
(:action update-status-comp346-all-on
  :parameters ()
  :precondition (and
    (update-status comp346)
    (not (rebooted comp346))
    (all-on comp346)
  )
  :effect (and
    (not (update-status comp346))
    (update-status comp347)
    (not (all-on comp346))
    (probabilistic 1/20 (and (not (running comp346))))
  )
)
(:action update-status-comp346-one-off
  :parameters ()
  :precondition (and
    (update-status comp346)
    (not (rebooted comp346))
    (one-off comp346)
  )
  :effect (and
    (not (update-status comp346))
    (update-status comp347)
    (not (one-off comp346))
    (probabilistic 1/4 (and (not (running comp346))))
  )
)
(:action update-status-comp347-rebooted
  :parameters ()
  :precondition (and
    (update-status comp347)
    (rebooted comp347)
  )
  :effect (and
    (not (update-status comp347))
    (update-status comp348)
    (not (rebooted comp347))
    (probabilistic 9/10 (and (running comp347)) 1/10 (and))
  )
)
(:action update-status-comp347-all-on
  :parameters ()
  :precondition (and
    (update-status comp347)
    (not (rebooted comp347))
    (all-on comp347)
  )
  :effect (and
    (not (update-status comp347))
    (update-status comp348)
    (not (all-on comp347))
    (probabilistic 1/20 (and (not (running comp347))))
  )
)
(:action update-status-comp347-one-off
  :parameters ()
  :precondition (and
    (update-status comp347)
    (not (rebooted comp347))
    (one-off comp347)
  )
  :effect (and
    (not (update-status comp347))
    (update-status comp348)
    (not (one-off comp347))
    (probabilistic 1/4 (and (not (running comp347))))
  )
)
(:action update-status-comp348-rebooted
  :parameters ()
  :precondition (and
    (update-status comp348)
    (rebooted comp348)
  )
  :effect (and
    (not (update-status comp348))
    (update-status comp349)
    (not (rebooted comp348))
    (probabilistic 9/10 (and (running comp348)) 1/10 (and))
  )
)
(:action update-status-comp348-all-on
  :parameters ()
  :precondition (and
    (update-status comp348)
    (not (rebooted comp348))
    (all-on comp348)
  )
  :effect (and
    (not (update-status comp348))
    (update-status comp349)
    (not (all-on comp348))
    (probabilistic 1/20 (and (not (running comp348))))
  )
)
(:action update-status-comp348-one-off
  :parameters ()
  :precondition (and
    (update-status comp348)
    (not (rebooted comp348))
    (one-off comp348)
  )
  :effect (and
    (not (update-status comp348))
    (update-status comp349)
    (not (one-off comp348))
    (probabilistic 1/4 (and (not (running comp348))))
  )
)
(:action update-status-comp349-rebooted
  :parameters ()
  :precondition (and
    (update-status comp349)
    (rebooted comp349)
  )
  :effect (and
    (not (update-status comp349))
    (update-status comp350)
    (not (rebooted comp349))
    (probabilistic 9/10 (and (running comp349)) 1/10 (and))
  )
)
(:action update-status-comp349-all-on
  :parameters ()
  :precondition (and
    (update-status comp349)
    (not (rebooted comp349))
    (all-on comp349)
  )
  :effect (and
    (not (update-status comp349))
    (update-status comp350)
    (not (all-on comp349))
    (probabilistic 1/20 (and (not (running comp349))))
  )
)
(:action update-status-comp349-one-off
  :parameters ()
  :precondition (and
    (update-status comp349)
    (not (rebooted comp349))
    (one-off comp349)
  )
  :effect (and
    (not (update-status comp349))
    (update-status comp350)
    (not (one-off comp349))
    (probabilistic 1/4 (and (not (running comp349))))
  )
)
(:action update-status-comp350-rebooted
  :parameters ()
  :precondition (and
    (update-status comp350)
    (rebooted comp350)
  )
  :effect (and
    (not (update-status comp350))
    (update-status comp351)
    (not (rebooted comp350))
    (probabilistic 9/10 (and (running comp350)) 1/10 (and))
  )
)
(:action update-status-comp350-all-on
  :parameters ()
  :precondition (and
    (update-status comp350)
    (not (rebooted comp350))
    (all-on comp350)
  )
  :effect (and
    (not (update-status comp350))
    (update-status comp351)
    (not (all-on comp350))
    (probabilistic 1/20 (and (not (running comp350))))
  )
)
(:action update-status-comp350-one-off
  :parameters ()
  :precondition (and
    (update-status comp350)
    (not (rebooted comp350))
    (one-off comp350)
  )
  :effect (and
    (not (update-status comp350))
    (update-status comp351)
    (not (one-off comp350))
    (probabilistic 1/4 (and (not (running comp350))))
  )
)
(:action update-status-comp351-rebooted
  :parameters ()
  :precondition (and
    (update-status comp351)
    (rebooted comp351)
  )
  :effect (and
    (not (update-status comp351))
    (update-status comp352)
    (not (rebooted comp351))
    (probabilistic 9/10 (and (running comp351)) 1/10 (and))
  )
)
(:action update-status-comp351-all-on
  :parameters ()
  :precondition (and
    (update-status comp351)
    (not (rebooted comp351))
    (all-on comp351)
  )
  :effect (and
    (not (update-status comp351))
    (update-status comp352)
    (not (all-on comp351))
    (probabilistic 1/20 (and (not (running comp351))))
  )
)
(:action update-status-comp351-one-off
  :parameters ()
  :precondition (and
    (update-status comp351)
    (not (rebooted comp351))
    (one-off comp351)
  )
  :effect (and
    (not (update-status comp351))
    (update-status comp352)
    (not (one-off comp351))
    (probabilistic 1/4 (and (not (running comp351))))
  )
)
(:action update-status-comp352-rebooted
  :parameters ()
  :precondition (and
    (update-status comp352)
    (rebooted comp352)
  )
  :effect (and
    (not (update-status comp352))
    (update-status comp353)
    (not (rebooted comp352))
    (probabilistic 9/10 (and (running comp352)) 1/10 (and))
  )
)
(:action update-status-comp352-all-on
  :parameters ()
  :precondition (and
    (update-status comp352)
    (not (rebooted comp352))
    (all-on comp352)
  )
  :effect (and
    (not (update-status comp352))
    (update-status comp353)
    (not (all-on comp352))
    (probabilistic 1/20 (and (not (running comp352))))
  )
)
(:action update-status-comp352-one-off
  :parameters ()
  :precondition (and
    (update-status comp352)
    (not (rebooted comp352))
    (one-off comp352)
  )
  :effect (and
    (not (update-status comp352))
    (update-status comp353)
    (not (one-off comp352))
    (probabilistic 1/4 (and (not (running comp352))))
  )
)
(:action update-status-comp353-rebooted
  :parameters ()
  :precondition (and
    (update-status comp353)
    (rebooted comp353)
  )
  :effect (and
    (not (update-status comp353))
    (update-status comp354)
    (not (rebooted comp353))
    (probabilistic 9/10 (and (running comp353)) 1/10 (and))
  )
)
(:action update-status-comp353-all-on
  :parameters ()
  :precondition (and
    (update-status comp353)
    (not (rebooted comp353))
    (all-on comp353)
  )
  :effect (and
    (not (update-status comp353))
    (update-status comp354)
    (not (all-on comp353))
    (probabilistic 1/20 (and (not (running comp353))))
  )
)
(:action update-status-comp353-one-off
  :parameters ()
  :precondition (and
    (update-status comp353)
    (not (rebooted comp353))
    (one-off comp353)
  )
  :effect (and
    (not (update-status comp353))
    (update-status comp354)
    (not (one-off comp353))
    (probabilistic 1/4 (and (not (running comp353))))
  )
)
(:action update-status-comp354-rebooted
  :parameters ()
  :precondition (and
    (update-status comp354)
    (rebooted comp354)
  )
  :effect (and
    (not (update-status comp354))
    (update-status comp355)
    (not (rebooted comp354))
    (probabilistic 9/10 (and (running comp354)) 1/10 (and))
  )
)
(:action update-status-comp354-all-on
  :parameters ()
  :precondition (and
    (update-status comp354)
    (not (rebooted comp354))
    (all-on comp354)
  )
  :effect (and
    (not (update-status comp354))
    (update-status comp355)
    (not (all-on comp354))
    (probabilistic 1/20 (and (not (running comp354))))
  )
)
(:action update-status-comp354-one-off
  :parameters ()
  :precondition (and
    (update-status comp354)
    (not (rebooted comp354))
    (one-off comp354)
  )
  :effect (and
    (not (update-status comp354))
    (update-status comp355)
    (not (one-off comp354))
    (probabilistic 1/4 (and (not (running comp354))))
  )
)
(:action update-status-comp355-rebooted
  :parameters ()
  :precondition (and
    (update-status comp355)
    (rebooted comp355)
  )
  :effect (and
    (not (update-status comp355))
    (update-status comp356)
    (not (rebooted comp355))
    (probabilistic 9/10 (and (running comp355)) 1/10 (and))
  )
)
(:action update-status-comp355-all-on
  :parameters ()
  :precondition (and
    (update-status comp355)
    (not (rebooted comp355))
    (all-on comp355)
  )
  :effect (and
    (not (update-status comp355))
    (update-status comp356)
    (not (all-on comp355))
    (probabilistic 1/20 (and (not (running comp355))))
  )
)
(:action update-status-comp355-one-off
  :parameters ()
  :precondition (and
    (update-status comp355)
    (not (rebooted comp355))
    (one-off comp355)
  )
  :effect (and
    (not (update-status comp355))
    (update-status comp356)
    (not (one-off comp355))
    (probabilistic 1/4 (and (not (running comp355))))
  )
)
(:action update-status-comp356-rebooted
  :parameters ()
  :precondition (and
    (update-status comp356)
    (rebooted comp356)
  )
  :effect (and
    (not (update-status comp356))
    (update-status comp357)
    (not (rebooted comp356))
    (probabilistic 9/10 (and (running comp356)) 1/10 (and))
  )
)
(:action update-status-comp356-all-on
  :parameters ()
  :precondition (and
    (update-status comp356)
    (not (rebooted comp356))
    (all-on comp356)
  )
  :effect (and
    (not (update-status comp356))
    (update-status comp357)
    (not (all-on comp356))
    (probabilistic 1/20 (and (not (running comp356))))
  )
)
(:action update-status-comp356-one-off
  :parameters ()
  :precondition (and
    (update-status comp356)
    (not (rebooted comp356))
    (one-off comp356)
  )
  :effect (and
    (not (update-status comp356))
    (update-status comp357)
    (not (one-off comp356))
    (probabilistic 1/4 (and (not (running comp356))))
  )
)
(:action update-status-comp357-rebooted
  :parameters ()
  :precondition (and
    (update-status comp357)
    (rebooted comp357)
  )
  :effect (and
    (not (update-status comp357))
    (update-status comp358)
    (not (rebooted comp357))
    (probabilistic 9/10 (and (running comp357)) 1/10 (and))
  )
)
(:action update-status-comp357-all-on
  :parameters ()
  :precondition (and
    (update-status comp357)
    (not (rebooted comp357))
    (all-on comp357)
  )
  :effect (and
    (not (update-status comp357))
    (update-status comp358)
    (not (all-on comp357))
    (probabilistic 1/20 (and (not (running comp357))))
  )
)
(:action update-status-comp357-one-off
  :parameters ()
  :precondition (and
    (update-status comp357)
    (not (rebooted comp357))
    (one-off comp357)
  )
  :effect (and
    (not (update-status comp357))
    (update-status comp358)
    (not (one-off comp357))
    (probabilistic 1/4 (and (not (running comp357))))
  )
)
(:action update-status-comp358-rebooted
  :parameters ()
  :precondition (and
    (update-status comp358)
    (rebooted comp358)
  )
  :effect (and
    (not (update-status comp358))
    (update-status comp359)
    (not (rebooted comp358))
    (probabilistic 9/10 (and (running comp358)) 1/10 (and))
  )
)
(:action update-status-comp358-all-on
  :parameters ()
  :precondition (and
    (update-status comp358)
    (not (rebooted comp358))
    (all-on comp358)
  )
  :effect (and
    (not (update-status comp358))
    (update-status comp359)
    (not (all-on comp358))
    (probabilistic 1/20 (and (not (running comp358))))
  )
)
(:action update-status-comp358-one-off
  :parameters ()
  :precondition (and
    (update-status comp358)
    (not (rebooted comp358))
    (one-off comp358)
  )
  :effect (and
    (not (update-status comp358))
    (update-status comp359)
    (not (one-off comp358))
    (probabilistic 1/4 (and (not (running comp358))))
  )
)
(:action update-status-comp359-rebooted
  :parameters ()
  :precondition (and
    (update-status comp359)
    (rebooted comp359)
  )
  :effect (and
    (not (update-status comp359))
    (update-status comp360)
    (not (rebooted comp359))
    (probabilistic 9/10 (and (running comp359)) 1/10 (and))
  )
)
(:action update-status-comp359-all-on
  :parameters ()
  :precondition (and
    (update-status comp359)
    (not (rebooted comp359))
    (all-on comp359)
  )
  :effect (and
    (not (update-status comp359))
    (update-status comp360)
    (not (all-on comp359))
    (probabilistic 1/20 (and (not (running comp359))))
  )
)
(:action update-status-comp359-one-off
  :parameters ()
  :precondition (and
    (update-status comp359)
    (not (rebooted comp359))
    (one-off comp359)
  )
  :effect (and
    (not (update-status comp359))
    (update-status comp360)
    (not (one-off comp359))
    (probabilistic 1/4 (and (not (running comp359))))
  )
)
(:action update-status-comp360-rebooted
  :parameters ()
  :precondition (and
    (update-status comp360)
    (rebooted comp360)
  )
  :effect (and
    (not (update-status comp360))
    (update-status comp361)
    (not (rebooted comp360))
    (probabilistic 9/10 (and (running comp360)) 1/10 (and))
  )
)
(:action update-status-comp360-all-on
  :parameters ()
  :precondition (and
    (update-status comp360)
    (not (rebooted comp360))
    (all-on comp360)
  )
  :effect (and
    (not (update-status comp360))
    (update-status comp361)
    (not (all-on comp360))
    (probabilistic 1/20 (and (not (running comp360))))
  )
)
(:action update-status-comp360-one-off
  :parameters ()
  :precondition (and
    (update-status comp360)
    (not (rebooted comp360))
    (one-off comp360)
  )
  :effect (and
    (not (update-status comp360))
    (update-status comp361)
    (not (one-off comp360))
    (probabilistic 1/4 (and (not (running comp360))))
  )
)
(:action update-status-comp361-rebooted
  :parameters ()
  :precondition (and
    (update-status comp361)
    (rebooted comp361)
  )
  :effect (and
    (not (update-status comp361))
    (update-status comp362)
    (not (rebooted comp361))
    (probabilistic 9/10 (and (running comp361)) 1/10 (and))
  )
)
(:action update-status-comp361-all-on
  :parameters ()
  :precondition (and
    (update-status comp361)
    (not (rebooted comp361))
    (all-on comp361)
  )
  :effect (and
    (not (update-status comp361))
    (update-status comp362)
    (not (all-on comp361))
    (probabilistic 1/20 (and (not (running comp361))))
  )
)
(:action update-status-comp361-one-off
  :parameters ()
  :precondition (and
    (update-status comp361)
    (not (rebooted comp361))
    (one-off comp361)
  )
  :effect (and
    (not (update-status comp361))
    (update-status comp362)
    (not (one-off comp361))
    (probabilistic 1/4 (and (not (running comp361))))
  )
)
(:action update-status-comp362-rebooted
  :parameters ()
  :precondition (and
    (update-status comp362)
    (rebooted comp362)
  )
  :effect (and
    (not (update-status comp362))
    (update-status comp363)
    (not (rebooted comp362))
    (probabilistic 9/10 (and (running comp362)) 1/10 (and))
  )
)
(:action update-status-comp362-all-on
  :parameters ()
  :precondition (and
    (update-status comp362)
    (not (rebooted comp362))
    (all-on comp362)
  )
  :effect (and
    (not (update-status comp362))
    (update-status comp363)
    (not (all-on comp362))
    (probabilistic 1/20 (and (not (running comp362))))
  )
)
(:action update-status-comp362-one-off
  :parameters ()
  :precondition (and
    (update-status comp362)
    (not (rebooted comp362))
    (one-off comp362)
  )
  :effect (and
    (not (update-status comp362))
    (update-status comp363)
    (not (one-off comp362))
    (probabilistic 1/4 (and (not (running comp362))))
  )
)
(:action update-status-comp363-rebooted
  :parameters ()
  :precondition (and
    (update-status comp363)
    (rebooted comp363)
  )
  :effect (and
    (not (update-status comp363))
    (update-status comp364)
    (not (rebooted comp363))
    (probabilistic 9/10 (and (running comp363)) 1/10 (and))
  )
)
(:action update-status-comp363-all-on
  :parameters ()
  :precondition (and
    (update-status comp363)
    (not (rebooted comp363))
    (all-on comp363)
  )
  :effect (and
    (not (update-status comp363))
    (update-status comp364)
    (not (all-on comp363))
    (probabilistic 1/20 (and (not (running comp363))))
  )
)
(:action update-status-comp363-one-off
  :parameters ()
  :precondition (and
    (update-status comp363)
    (not (rebooted comp363))
    (one-off comp363)
  )
  :effect (and
    (not (update-status comp363))
    (update-status comp364)
    (not (one-off comp363))
    (probabilistic 1/4 (and (not (running comp363))))
  )
)
(:action update-status-comp364-rebooted
  :parameters ()
  :precondition (and
    (update-status comp364)
    (rebooted comp364)
  )
  :effect (and
    (not (update-status comp364))
    (update-status comp365)
    (not (rebooted comp364))
    (probabilistic 9/10 (and (running comp364)) 1/10 (and))
  )
)
(:action update-status-comp364-all-on
  :parameters ()
  :precondition (and
    (update-status comp364)
    (not (rebooted comp364))
    (all-on comp364)
  )
  :effect (and
    (not (update-status comp364))
    (update-status comp365)
    (not (all-on comp364))
    (probabilistic 1/20 (and (not (running comp364))))
  )
)
(:action update-status-comp364-one-off
  :parameters ()
  :precondition (and
    (update-status comp364)
    (not (rebooted comp364))
    (one-off comp364)
  )
  :effect (and
    (not (update-status comp364))
    (update-status comp365)
    (not (one-off comp364))
    (probabilistic 1/4 (and (not (running comp364))))
  )
)
(:action update-status-comp365-rebooted
  :parameters ()
  :precondition (and
    (update-status comp365)
    (rebooted comp365)
  )
  :effect (and
    (not (update-status comp365))
    (update-status comp366)
    (not (rebooted comp365))
    (probabilistic 9/10 (and (running comp365)) 1/10 (and))
  )
)
(:action update-status-comp365-all-on
  :parameters ()
  :precondition (and
    (update-status comp365)
    (not (rebooted comp365))
    (all-on comp365)
  )
  :effect (and
    (not (update-status comp365))
    (update-status comp366)
    (not (all-on comp365))
    (probabilistic 1/20 (and (not (running comp365))))
  )
)
(:action update-status-comp365-one-off
  :parameters ()
  :precondition (and
    (update-status comp365)
    (not (rebooted comp365))
    (one-off comp365)
  )
  :effect (and
    (not (update-status comp365))
    (update-status comp366)
    (not (one-off comp365))
    (probabilistic 1/4 (and (not (running comp365))))
  )
)
(:action update-status-comp366-rebooted
  :parameters ()
  :precondition (and
    (update-status comp366)
    (rebooted comp366)
  )
  :effect (and
    (not (update-status comp366))
    (update-status comp367)
    (not (rebooted comp366))
    (probabilistic 9/10 (and (running comp366)) 1/10 (and))
  )
)
(:action update-status-comp366-all-on
  :parameters ()
  :precondition (and
    (update-status comp366)
    (not (rebooted comp366))
    (all-on comp366)
  )
  :effect (and
    (not (update-status comp366))
    (update-status comp367)
    (not (all-on comp366))
    (probabilistic 1/20 (and (not (running comp366))))
  )
)
(:action update-status-comp366-one-off
  :parameters ()
  :precondition (and
    (update-status comp366)
    (not (rebooted comp366))
    (one-off comp366)
  )
  :effect (and
    (not (update-status comp366))
    (update-status comp367)
    (not (one-off comp366))
    (probabilistic 1/4 (and (not (running comp366))))
  )
)
(:action update-status-comp367-rebooted
  :parameters ()
  :precondition (and
    (update-status comp367)
    (rebooted comp367)
  )
  :effect (and
    (not (update-status comp367))
    (update-status comp368)
    (not (rebooted comp367))
    (probabilistic 9/10 (and (running comp367)) 1/10 (and))
  )
)
(:action update-status-comp367-all-on
  :parameters ()
  :precondition (and
    (update-status comp367)
    (not (rebooted comp367))
    (all-on comp367)
  )
  :effect (and
    (not (update-status comp367))
    (update-status comp368)
    (not (all-on comp367))
    (probabilistic 1/20 (and (not (running comp367))))
  )
)
(:action update-status-comp367-one-off
  :parameters ()
  :precondition (and
    (update-status comp367)
    (not (rebooted comp367))
    (one-off comp367)
  )
  :effect (and
    (not (update-status comp367))
    (update-status comp368)
    (not (one-off comp367))
    (probabilistic 1/4 (and (not (running comp367))))
  )
)
(:action update-status-comp368-rebooted
  :parameters ()
  :precondition (and
    (update-status comp368)
    (rebooted comp368)
  )
  :effect (and
    (not (update-status comp368))
    (update-status comp369)
    (not (rebooted comp368))
    (probabilistic 9/10 (and (running comp368)) 1/10 (and))
  )
)
(:action update-status-comp368-all-on
  :parameters ()
  :precondition (and
    (update-status comp368)
    (not (rebooted comp368))
    (all-on comp368)
  )
  :effect (and
    (not (update-status comp368))
    (update-status comp369)
    (not (all-on comp368))
    (probabilistic 1/20 (and (not (running comp368))))
  )
)
(:action update-status-comp368-one-off
  :parameters ()
  :precondition (and
    (update-status comp368)
    (not (rebooted comp368))
    (one-off comp368)
  )
  :effect (and
    (not (update-status comp368))
    (update-status comp369)
    (not (one-off comp368))
    (probabilistic 1/4 (and (not (running comp368))))
  )
)
(:action update-status-comp369-rebooted
  :parameters ()
  :precondition (and
    (update-status comp369)
    (rebooted comp369)
  )
  :effect (and
    (not (update-status comp369))
    (update-status comp370)
    (not (rebooted comp369))
    (probabilistic 9/10 (and (running comp369)) 1/10 (and))
  )
)
(:action update-status-comp369-all-on
  :parameters ()
  :precondition (and
    (update-status comp369)
    (not (rebooted comp369))
    (all-on comp369)
  )
  :effect (and
    (not (update-status comp369))
    (update-status comp370)
    (not (all-on comp369))
    (probabilistic 1/20 (and (not (running comp369))))
  )
)
(:action update-status-comp369-one-off
  :parameters ()
  :precondition (and
    (update-status comp369)
    (not (rebooted comp369))
    (one-off comp369)
  )
  :effect (and
    (not (update-status comp369))
    (update-status comp370)
    (not (one-off comp369))
    (probabilistic 1/4 (and (not (running comp369))))
  )
)
(:action update-status-comp370-rebooted
  :parameters ()
  :precondition (and
    (update-status comp370)
    (rebooted comp370)
  )
  :effect (and
    (not (update-status comp370))
    (update-status comp371)
    (not (rebooted comp370))
    (probabilistic 9/10 (and (running comp370)) 1/10 (and))
  )
)
(:action update-status-comp370-all-on
  :parameters ()
  :precondition (and
    (update-status comp370)
    (not (rebooted comp370))
    (all-on comp370)
  )
  :effect (and
    (not (update-status comp370))
    (update-status comp371)
    (not (all-on comp370))
    (probabilistic 1/20 (and (not (running comp370))))
  )
)
(:action update-status-comp370-one-off
  :parameters ()
  :precondition (and
    (update-status comp370)
    (not (rebooted comp370))
    (one-off comp370)
  )
  :effect (and
    (not (update-status comp370))
    (update-status comp371)
    (not (one-off comp370))
    (probabilistic 1/4 (and (not (running comp370))))
  )
)
(:action update-status-comp371-rebooted
  :parameters ()
  :precondition (and
    (update-status comp371)
    (rebooted comp371)
  )
  :effect (and
    (not (update-status comp371))
    (update-status comp372)
    (not (rebooted comp371))
    (probabilistic 9/10 (and (running comp371)) 1/10 (and))
  )
)
(:action update-status-comp371-all-on
  :parameters ()
  :precondition (and
    (update-status comp371)
    (not (rebooted comp371))
    (all-on comp371)
  )
  :effect (and
    (not (update-status comp371))
    (update-status comp372)
    (not (all-on comp371))
    (probabilistic 1/20 (and (not (running comp371))))
  )
)
(:action update-status-comp371-one-off
  :parameters ()
  :precondition (and
    (update-status comp371)
    (not (rebooted comp371))
    (one-off comp371)
  )
  :effect (and
    (not (update-status comp371))
    (update-status comp372)
    (not (one-off comp371))
    (probabilistic 1/4 (and (not (running comp371))))
  )
)
(:action update-status-comp372-rebooted
  :parameters ()
  :precondition (and
    (update-status comp372)
    (rebooted comp372)
  )
  :effect (and
    (not (update-status comp372))
    (update-status comp373)
    (not (rebooted comp372))
    (probabilistic 9/10 (and (running comp372)) 1/10 (and))
  )
)
(:action update-status-comp372-all-on
  :parameters ()
  :precondition (and
    (update-status comp372)
    (not (rebooted comp372))
    (all-on comp372)
  )
  :effect (and
    (not (update-status comp372))
    (update-status comp373)
    (not (all-on comp372))
    (probabilistic 1/20 (and (not (running comp372))))
  )
)
(:action update-status-comp372-one-off
  :parameters ()
  :precondition (and
    (update-status comp372)
    (not (rebooted comp372))
    (one-off comp372)
  )
  :effect (and
    (not (update-status comp372))
    (update-status comp373)
    (not (one-off comp372))
    (probabilistic 1/4 (and (not (running comp372))))
  )
)
(:action update-status-comp373-rebooted
  :parameters ()
  :precondition (and
    (update-status comp373)
    (rebooted comp373)
  )
  :effect (and
    (not (update-status comp373))
    (update-status comp374)
    (not (rebooted comp373))
    (probabilistic 9/10 (and (running comp373)) 1/10 (and))
  )
)
(:action update-status-comp373-all-on
  :parameters ()
  :precondition (and
    (update-status comp373)
    (not (rebooted comp373))
    (all-on comp373)
  )
  :effect (and
    (not (update-status comp373))
    (update-status comp374)
    (not (all-on comp373))
    (probabilistic 1/20 (and (not (running comp373))))
  )
)
(:action update-status-comp373-one-off
  :parameters ()
  :precondition (and
    (update-status comp373)
    (not (rebooted comp373))
    (one-off comp373)
  )
  :effect (and
    (not (update-status comp373))
    (update-status comp374)
    (not (one-off comp373))
    (probabilistic 1/4 (and (not (running comp373))))
  )
)
(:action update-status-comp374-rebooted
  :parameters ()
  :precondition (and
    (update-status comp374)
    (rebooted comp374)
  )
  :effect (and
    (not (update-status comp374))
    (update-status comp375)
    (not (rebooted comp374))
    (probabilistic 9/10 (and (running comp374)) 1/10 (and))
  )
)
(:action update-status-comp374-all-on
  :parameters ()
  :precondition (and
    (update-status comp374)
    (not (rebooted comp374))
    (all-on comp374)
  )
  :effect (and
    (not (update-status comp374))
    (update-status comp375)
    (not (all-on comp374))
    (probabilistic 1/20 (and (not (running comp374))))
  )
)
(:action update-status-comp374-one-off
  :parameters ()
  :precondition (and
    (update-status comp374)
    (not (rebooted comp374))
    (one-off comp374)
  )
  :effect (and
    (not (update-status comp374))
    (update-status comp375)
    (not (one-off comp374))
    (probabilistic 1/4 (and (not (running comp374))))
  )
)
(:action update-status-comp375-rebooted
  :parameters ()
  :precondition (and
    (update-status comp375)
    (rebooted comp375)
  )
  :effect (and
    (not (update-status comp375))
    (update-status comp376)
    (not (rebooted comp375))
    (probabilistic 9/10 (and (running comp375)) 1/10 (and))
  )
)
(:action update-status-comp375-all-on
  :parameters ()
  :precondition (and
    (update-status comp375)
    (not (rebooted comp375))
    (all-on comp375)
  )
  :effect (and
    (not (update-status comp375))
    (update-status comp376)
    (not (all-on comp375))
    (probabilistic 1/20 (and (not (running comp375))))
  )
)
(:action update-status-comp375-one-off
  :parameters ()
  :precondition (and
    (update-status comp375)
    (not (rebooted comp375))
    (one-off comp375)
  )
  :effect (and
    (not (update-status comp375))
    (update-status comp376)
    (not (one-off comp375))
    (probabilistic 1/4 (and (not (running comp375))))
  )
)
(:action update-status-comp376-rebooted
  :parameters ()
  :precondition (and
    (update-status comp376)
    (rebooted comp376)
  )
  :effect (and
    (not (update-status comp376))
    (update-status comp377)
    (not (rebooted comp376))
    (probabilistic 9/10 (and (running comp376)) 1/10 (and))
  )
)
(:action update-status-comp376-all-on
  :parameters ()
  :precondition (and
    (update-status comp376)
    (not (rebooted comp376))
    (all-on comp376)
  )
  :effect (and
    (not (update-status comp376))
    (update-status comp377)
    (not (all-on comp376))
    (probabilistic 1/20 (and (not (running comp376))))
  )
)
(:action update-status-comp376-one-off
  :parameters ()
  :precondition (and
    (update-status comp376)
    (not (rebooted comp376))
    (one-off comp376)
  )
  :effect (and
    (not (update-status comp376))
    (update-status comp377)
    (not (one-off comp376))
    (probabilistic 1/4 (and (not (running comp376))))
  )
)
(:action update-status-comp377-rebooted
  :parameters ()
  :precondition (and
    (update-status comp377)
    (rebooted comp377)
  )
  :effect (and
    (not (update-status comp377))
    (update-status comp378)
    (not (rebooted comp377))
    (probabilistic 9/10 (and (running comp377)) 1/10 (and))
  )
)
(:action update-status-comp377-all-on
  :parameters ()
  :precondition (and
    (update-status comp377)
    (not (rebooted comp377))
    (all-on comp377)
  )
  :effect (and
    (not (update-status comp377))
    (update-status comp378)
    (not (all-on comp377))
    (probabilistic 1/20 (and (not (running comp377))))
  )
)
(:action update-status-comp377-one-off
  :parameters ()
  :precondition (and
    (update-status comp377)
    (not (rebooted comp377))
    (one-off comp377)
  )
  :effect (and
    (not (update-status comp377))
    (update-status comp378)
    (not (one-off comp377))
    (probabilistic 1/4 (and (not (running comp377))))
  )
)
(:action update-status-comp378-rebooted
  :parameters ()
  :precondition (and
    (update-status comp378)
    (rebooted comp378)
  )
  :effect (and
    (not (update-status comp378))
    (update-status comp379)
    (not (rebooted comp378))
    (probabilistic 9/10 (and (running comp378)) 1/10 (and))
  )
)
(:action update-status-comp378-all-on
  :parameters ()
  :precondition (and
    (update-status comp378)
    (not (rebooted comp378))
    (all-on comp378)
  )
  :effect (and
    (not (update-status comp378))
    (update-status comp379)
    (not (all-on comp378))
    (probabilistic 1/20 (and (not (running comp378))))
  )
)
(:action update-status-comp378-one-off
  :parameters ()
  :precondition (and
    (update-status comp378)
    (not (rebooted comp378))
    (one-off comp378)
  )
  :effect (and
    (not (update-status comp378))
    (update-status comp379)
    (not (one-off comp378))
    (probabilistic 1/4 (and (not (running comp378))))
  )
)
(:action update-status-comp379-rebooted
  :parameters ()
  :precondition (and
    (update-status comp379)
    (rebooted comp379)
  )
  :effect (and
    (not (update-status comp379))
    (update-status comp380)
    (not (rebooted comp379))
    (probabilistic 9/10 (and (running comp379)) 1/10 (and))
  )
)
(:action update-status-comp379-all-on
  :parameters ()
  :precondition (and
    (update-status comp379)
    (not (rebooted comp379))
    (all-on comp379)
  )
  :effect (and
    (not (update-status comp379))
    (update-status comp380)
    (not (all-on comp379))
    (probabilistic 1/20 (and (not (running comp379))))
  )
)
(:action update-status-comp379-one-off
  :parameters ()
  :precondition (and
    (update-status comp379)
    (not (rebooted comp379))
    (one-off comp379)
  )
  :effect (and
    (not (update-status comp379))
    (update-status comp380)
    (not (one-off comp379))
    (probabilistic 1/4 (and (not (running comp379))))
  )
)
(:action update-status-comp380-rebooted
  :parameters ()
  :precondition (and
    (update-status comp380)
    (rebooted comp380)
  )
  :effect (and
    (not (update-status comp380))
    (update-status comp381)
    (not (rebooted comp380))
    (probabilistic 9/10 (and (running comp380)) 1/10 (and))
  )
)
(:action update-status-comp380-all-on
  :parameters ()
  :precondition (and
    (update-status comp380)
    (not (rebooted comp380))
    (all-on comp380)
  )
  :effect (and
    (not (update-status comp380))
    (update-status comp381)
    (not (all-on comp380))
    (probabilistic 1/20 (and (not (running comp380))))
  )
)
(:action update-status-comp380-one-off
  :parameters ()
  :precondition (and
    (update-status comp380)
    (not (rebooted comp380))
    (one-off comp380)
  )
  :effect (and
    (not (update-status comp380))
    (update-status comp381)
    (not (one-off comp380))
    (probabilistic 1/4 (and (not (running comp380))))
  )
)
(:action update-status-comp381-rebooted
  :parameters ()
  :precondition (and
    (update-status comp381)
    (rebooted comp381)
  )
  :effect (and
    (not (update-status comp381))
    (update-status comp382)
    (not (rebooted comp381))
    (probabilistic 9/10 (and (running comp381)) 1/10 (and))
  )
)
(:action update-status-comp381-all-on
  :parameters ()
  :precondition (and
    (update-status comp381)
    (not (rebooted comp381))
    (all-on comp381)
  )
  :effect (and
    (not (update-status comp381))
    (update-status comp382)
    (not (all-on comp381))
    (probabilistic 1/20 (and (not (running comp381))))
  )
)
(:action update-status-comp381-one-off
  :parameters ()
  :precondition (and
    (update-status comp381)
    (not (rebooted comp381))
    (one-off comp381)
  )
  :effect (and
    (not (update-status comp381))
    (update-status comp382)
    (not (one-off comp381))
    (probabilistic 1/4 (and (not (running comp381))))
  )
)
(:action update-status-comp382-rebooted
  :parameters ()
  :precondition (and
    (update-status comp382)
    (rebooted comp382)
  )
  :effect (and
    (not (update-status comp382))
    (update-status comp383)
    (not (rebooted comp382))
    (probabilistic 9/10 (and (running comp382)) 1/10 (and))
  )
)
(:action update-status-comp382-all-on
  :parameters ()
  :precondition (and
    (update-status comp382)
    (not (rebooted comp382))
    (all-on comp382)
  )
  :effect (and
    (not (update-status comp382))
    (update-status comp383)
    (not (all-on comp382))
    (probabilistic 1/20 (and (not (running comp382))))
  )
)
(:action update-status-comp382-one-off
  :parameters ()
  :precondition (and
    (update-status comp382)
    (not (rebooted comp382))
    (one-off comp382)
  )
  :effect (and
    (not (update-status comp382))
    (update-status comp383)
    (not (one-off comp382))
    (probabilistic 1/4 (and (not (running comp382))))
  )
)
(:action update-status-comp383-rebooted
  :parameters ()
  :precondition (and
    (update-status comp383)
    (rebooted comp383)
  )
  :effect (and
    (not (update-status comp383))
    (update-status comp384)
    (not (rebooted comp383))
    (probabilistic 9/10 (and (running comp383)) 1/10 (and))
  )
)
(:action update-status-comp383-all-on
  :parameters ()
  :precondition (and
    (update-status comp383)
    (not (rebooted comp383))
    (all-on comp383)
  )
  :effect (and
    (not (update-status comp383))
    (update-status comp384)
    (not (all-on comp383))
    (probabilistic 1/20 (and (not (running comp383))))
  )
)
(:action update-status-comp383-one-off
  :parameters ()
  :precondition (and
    (update-status comp383)
    (not (rebooted comp383))
    (one-off comp383)
  )
  :effect (and
    (not (update-status comp383))
    (update-status comp384)
    (not (one-off comp383))
    (probabilistic 1/4 (and (not (running comp383))))
  )
)
(:action update-status-comp384-rebooted
  :parameters ()
  :precondition (and
    (update-status comp384)
    (rebooted comp384)
  )
  :effect (and
    (not (update-status comp384))
    (update-status comp385)
    (not (rebooted comp384))
    (probabilistic 9/10 (and (running comp384)) 1/10 (and))
  )
)
(:action update-status-comp384-all-on
  :parameters ()
  :precondition (and
    (update-status comp384)
    (not (rebooted comp384))
    (all-on comp384)
  )
  :effect (and
    (not (update-status comp384))
    (update-status comp385)
    (not (all-on comp384))
    (probabilistic 1/20 (and (not (running comp384))))
  )
)
(:action update-status-comp384-one-off
  :parameters ()
  :precondition (and
    (update-status comp384)
    (not (rebooted comp384))
    (one-off comp384)
  )
  :effect (and
    (not (update-status comp384))
    (update-status comp385)
    (not (one-off comp384))
    (probabilistic 1/4 (and (not (running comp384))))
  )
)
(:action update-status-comp385-rebooted
  :parameters ()
  :precondition (and
    (update-status comp385)
    (rebooted comp385)
  )
  :effect (and
    (not (update-status comp385))
    (update-status comp386)
    (not (rebooted comp385))
    (probabilistic 9/10 (and (running comp385)) 1/10 (and))
  )
)
(:action update-status-comp385-all-on
  :parameters ()
  :precondition (and
    (update-status comp385)
    (not (rebooted comp385))
    (all-on comp385)
  )
  :effect (and
    (not (update-status comp385))
    (update-status comp386)
    (not (all-on comp385))
    (probabilistic 1/20 (and (not (running comp385))))
  )
)
(:action update-status-comp385-one-off
  :parameters ()
  :precondition (and
    (update-status comp385)
    (not (rebooted comp385))
    (one-off comp385)
  )
  :effect (and
    (not (update-status comp385))
    (update-status comp386)
    (not (one-off comp385))
    (probabilistic 1/4 (and (not (running comp385))))
  )
)
(:action update-status-comp386-rebooted
  :parameters ()
  :precondition (and
    (update-status comp386)
    (rebooted comp386)
  )
  :effect (and
    (not (update-status comp386))
    (update-status comp387)
    (not (rebooted comp386))
    (probabilistic 9/10 (and (running comp386)) 1/10 (and))
  )
)
(:action update-status-comp386-all-on
  :parameters ()
  :precondition (and
    (update-status comp386)
    (not (rebooted comp386))
    (all-on comp386)
  )
  :effect (and
    (not (update-status comp386))
    (update-status comp387)
    (not (all-on comp386))
    (probabilistic 1/20 (and (not (running comp386))))
  )
)
(:action update-status-comp386-one-off
  :parameters ()
  :precondition (and
    (update-status comp386)
    (not (rebooted comp386))
    (one-off comp386)
  )
  :effect (and
    (not (update-status comp386))
    (update-status comp387)
    (not (one-off comp386))
    (probabilistic 1/4 (and (not (running comp386))))
  )
)
(:action update-status-comp387-rebooted
  :parameters ()
  :precondition (and
    (update-status comp387)
    (rebooted comp387)
  )
  :effect (and
    (not (update-status comp387))
    (update-status comp388)
    (not (rebooted comp387))
    (probabilistic 9/10 (and (running comp387)) 1/10 (and))
  )
)
(:action update-status-comp387-all-on
  :parameters ()
  :precondition (and
    (update-status comp387)
    (not (rebooted comp387))
    (all-on comp387)
  )
  :effect (and
    (not (update-status comp387))
    (update-status comp388)
    (not (all-on comp387))
    (probabilistic 1/20 (and (not (running comp387))))
  )
)
(:action update-status-comp387-one-off
  :parameters ()
  :precondition (and
    (update-status comp387)
    (not (rebooted comp387))
    (one-off comp387)
  )
  :effect (and
    (not (update-status comp387))
    (update-status comp388)
    (not (one-off comp387))
    (probabilistic 1/4 (and (not (running comp387))))
  )
)
(:action update-status-comp388-rebooted
  :parameters ()
  :precondition (and
    (update-status comp388)
    (rebooted comp388)
  )
  :effect (and
    (not (update-status comp388))
    (update-status comp389)
    (not (rebooted comp388))
    (probabilistic 9/10 (and (running comp388)) 1/10 (and))
  )
)
(:action update-status-comp388-all-on
  :parameters ()
  :precondition (and
    (update-status comp388)
    (not (rebooted comp388))
    (all-on comp388)
  )
  :effect (and
    (not (update-status comp388))
    (update-status comp389)
    (not (all-on comp388))
    (probabilistic 1/20 (and (not (running comp388))))
  )
)
(:action update-status-comp388-one-off
  :parameters ()
  :precondition (and
    (update-status comp388)
    (not (rebooted comp388))
    (one-off comp388)
  )
  :effect (and
    (not (update-status comp388))
    (update-status comp389)
    (not (one-off comp388))
    (probabilistic 1/4 (and (not (running comp388))))
  )
)
(:action update-status-comp389-rebooted
  :parameters ()
  :precondition (and
    (update-status comp389)
    (rebooted comp389)
  )
  :effect (and
    (not (update-status comp389))
    (update-status comp390)
    (not (rebooted comp389))
    (probabilistic 9/10 (and (running comp389)) 1/10 (and))
  )
)
(:action update-status-comp389-all-on
  :parameters ()
  :precondition (and
    (update-status comp389)
    (not (rebooted comp389))
    (all-on comp389)
  )
  :effect (and
    (not (update-status comp389))
    (update-status comp390)
    (not (all-on comp389))
    (probabilistic 1/20 (and (not (running comp389))))
  )
)
(:action update-status-comp389-one-off
  :parameters ()
  :precondition (and
    (update-status comp389)
    (not (rebooted comp389))
    (one-off comp389)
  )
  :effect (and
    (not (update-status comp389))
    (update-status comp390)
    (not (one-off comp389))
    (probabilistic 1/4 (and (not (running comp389))))
  )
)
(:action update-status-comp390-rebooted
  :parameters ()
  :precondition (and
    (update-status comp390)
    (rebooted comp390)
  )
  :effect (and
    (not (update-status comp390))
    (update-status comp391)
    (not (rebooted comp390))
    (probabilistic 9/10 (and (running comp390)) 1/10 (and))
  )
)
(:action update-status-comp390-all-on
  :parameters ()
  :precondition (and
    (update-status comp390)
    (not (rebooted comp390))
    (all-on comp390)
  )
  :effect (and
    (not (update-status comp390))
    (update-status comp391)
    (not (all-on comp390))
    (probabilistic 1/20 (and (not (running comp390))))
  )
)
(:action update-status-comp390-one-off
  :parameters ()
  :precondition (and
    (update-status comp390)
    (not (rebooted comp390))
    (one-off comp390)
  )
  :effect (and
    (not (update-status comp390))
    (update-status comp391)
    (not (one-off comp390))
    (probabilistic 1/4 (and (not (running comp390))))
  )
)
(:action update-status-comp391-rebooted
  :parameters ()
  :precondition (and
    (update-status comp391)
    (rebooted comp391)
  )
  :effect (and
    (not (update-status comp391))
    (update-status comp392)
    (not (rebooted comp391))
    (probabilistic 9/10 (and (running comp391)) 1/10 (and))
  )
)
(:action update-status-comp391-all-on
  :parameters ()
  :precondition (and
    (update-status comp391)
    (not (rebooted comp391))
    (all-on comp391)
  )
  :effect (and
    (not (update-status comp391))
    (update-status comp392)
    (not (all-on comp391))
    (probabilistic 1/20 (and (not (running comp391))))
  )
)
(:action update-status-comp391-one-off
  :parameters ()
  :precondition (and
    (update-status comp391)
    (not (rebooted comp391))
    (one-off comp391)
  )
  :effect (and
    (not (update-status comp391))
    (update-status comp392)
    (not (one-off comp391))
    (probabilistic 1/4 (and (not (running comp391))))
  )
)
(:action update-status-comp392-rebooted
  :parameters ()
  :precondition (and
    (update-status comp392)
    (rebooted comp392)
  )
  :effect (and
    (not (update-status comp392))
    (update-status comp393)
    (not (rebooted comp392))
    (probabilistic 9/10 (and (running comp392)) 1/10 (and))
  )
)
(:action update-status-comp392-all-on
  :parameters ()
  :precondition (and
    (update-status comp392)
    (not (rebooted comp392))
    (all-on comp392)
  )
  :effect (and
    (not (update-status comp392))
    (update-status comp393)
    (not (all-on comp392))
    (probabilistic 1/20 (and (not (running comp392))))
  )
)
(:action update-status-comp392-one-off
  :parameters ()
  :precondition (and
    (update-status comp392)
    (not (rebooted comp392))
    (one-off comp392)
  )
  :effect (and
    (not (update-status comp392))
    (update-status comp393)
    (not (one-off comp392))
    (probabilistic 1/4 (and (not (running comp392))))
  )
)
(:action update-status-comp393-rebooted
  :parameters ()
  :precondition (and
    (update-status comp393)
    (rebooted comp393)
  )
  :effect (and
    (not (update-status comp393))
    (update-status comp394)
    (not (rebooted comp393))
    (probabilistic 9/10 (and (running comp393)) 1/10 (and))
  )
)
(:action update-status-comp393-all-on
  :parameters ()
  :precondition (and
    (update-status comp393)
    (not (rebooted comp393))
    (all-on comp393)
  )
  :effect (and
    (not (update-status comp393))
    (update-status comp394)
    (not (all-on comp393))
    (probabilistic 1/20 (and (not (running comp393))))
  )
)
(:action update-status-comp393-one-off
  :parameters ()
  :precondition (and
    (update-status comp393)
    (not (rebooted comp393))
    (one-off comp393)
  )
  :effect (and
    (not (update-status comp393))
    (update-status comp394)
    (not (one-off comp393))
    (probabilistic 1/4 (and (not (running comp393))))
  )
)
(:action update-status-comp394-rebooted
  :parameters ()
  :precondition (and
    (update-status comp394)
    (rebooted comp394)
  )
  :effect (and
    (not (update-status comp394))
    (update-status comp395)
    (not (rebooted comp394))
    (probabilistic 9/10 (and (running comp394)) 1/10 (and))
  )
)
(:action update-status-comp394-all-on
  :parameters ()
  :precondition (and
    (update-status comp394)
    (not (rebooted comp394))
    (all-on comp394)
  )
  :effect (and
    (not (update-status comp394))
    (update-status comp395)
    (not (all-on comp394))
    (probabilistic 1/20 (and (not (running comp394))))
  )
)
(:action update-status-comp394-one-off
  :parameters ()
  :precondition (and
    (update-status comp394)
    (not (rebooted comp394))
    (one-off comp394)
  )
  :effect (and
    (not (update-status comp394))
    (update-status comp395)
    (not (one-off comp394))
    (probabilistic 1/4 (and (not (running comp394))))
  )
)
(:action update-status-comp395-rebooted
  :parameters ()
  :precondition (and
    (update-status comp395)
    (rebooted comp395)
  )
  :effect (and
    (not (update-status comp395))
    (update-status comp396)
    (not (rebooted comp395))
    (probabilistic 9/10 (and (running comp395)) 1/10 (and))
  )
)
(:action update-status-comp395-all-on
  :parameters ()
  :precondition (and
    (update-status comp395)
    (not (rebooted comp395))
    (all-on comp395)
  )
  :effect (and
    (not (update-status comp395))
    (update-status comp396)
    (not (all-on comp395))
    (probabilistic 1/20 (and (not (running comp395))))
  )
)
(:action update-status-comp395-one-off
  :parameters ()
  :precondition (and
    (update-status comp395)
    (not (rebooted comp395))
    (one-off comp395)
  )
  :effect (and
    (not (update-status comp395))
    (update-status comp396)
    (not (one-off comp395))
    (probabilistic 1/4 (and (not (running comp395))))
  )
)
(:action update-status-comp396-rebooted
  :parameters ()
  :precondition (and
    (update-status comp396)
    (rebooted comp396)
  )
  :effect (and
    (not (update-status comp396))
    (update-status comp397)
    (not (rebooted comp396))
    (probabilistic 9/10 (and (running comp396)) 1/10 (and))
  )
)
(:action update-status-comp396-all-on
  :parameters ()
  :precondition (and
    (update-status comp396)
    (not (rebooted comp396))
    (all-on comp396)
  )
  :effect (and
    (not (update-status comp396))
    (update-status comp397)
    (not (all-on comp396))
    (probabilistic 1/20 (and (not (running comp396))))
  )
)
(:action update-status-comp396-one-off
  :parameters ()
  :precondition (and
    (update-status comp396)
    (not (rebooted comp396))
    (one-off comp396)
  )
  :effect (and
    (not (update-status comp396))
    (update-status comp397)
    (not (one-off comp396))
    (probabilistic 1/4 (and (not (running comp396))))
  )
)
(:action update-status-comp397-rebooted
  :parameters ()
  :precondition (and
    (update-status comp397)
    (rebooted comp397)
  )
  :effect (and
    (not (update-status comp397))
    (update-status comp398)
    (not (rebooted comp397))
    (probabilistic 9/10 (and (running comp397)) 1/10 (and))
  )
)
(:action update-status-comp397-all-on
  :parameters ()
  :precondition (and
    (update-status comp397)
    (not (rebooted comp397))
    (all-on comp397)
  )
  :effect (and
    (not (update-status comp397))
    (update-status comp398)
    (not (all-on comp397))
    (probabilistic 1/20 (and (not (running comp397))))
  )
)
(:action update-status-comp397-one-off
  :parameters ()
  :precondition (and
    (update-status comp397)
    (not (rebooted comp397))
    (one-off comp397)
  )
  :effect (and
    (not (update-status comp397))
    (update-status comp398)
    (not (one-off comp397))
    (probabilistic 1/4 (and (not (running comp397))))
  )
)
(:action update-status-comp398-rebooted
  :parameters ()
  :precondition (and
    (update-status comp398)
    (rebooted comp398)
  )
  :effect (and
    (not (update-status comp398))
    (update-status comp399)
    (not (rebooted comp398))
    (probabilistic 9/10 (and (running comp398)) 1/10 (and))
  )
)
(:action update-status-comp398-all-on
  :parameters ()
  :precondition (and
    (update-status comp398)
    (not (rebooted comp398))
    (all-on comp398)
  )
  :effect (and
    (not (update-status comp398))
    (update-status comp399)
    (not (all-on comp398))
    (probabilistic 1/20 (and (not (running comp398))))
  )
)
(:action update-status-comp398-one-off
  :parameters ()
  :precondition (and
    (update-status comp398)
    (not (rebooted comp398))
    (one-off comp398)
  )
  :effect (and
    (not (update-status comp398))
    (update-status comp399)
    (not (one-off comp398))
    (probabilistic 1/4 (and (not (running comp398))))
  )
)
(:action update-status-comp399-rebooted
  :parameters ()
  :precondition (and
    (update-status comp399)
    (rebooted comp399)
  )
  :effect (and
    (not (update-status comp399))
    (update-status comp400)
    (not (rebooted comp399))
    (probabilistic 9/10 (and (running comp399)) 1/10 (and))
  )
)
(:action update-status-comp399-all-on
  :parameters ()
  :precondition (and
    (update-status comp399)
    (not (rebooted comp399))
    (all-on comp399)
  )
  :effect (and
    (not (update-status comp399))
    (update-status comp400)
    (not (all-on comp399))
    (probabilistic 1/20 (and (not (running comp399))))
  )
)
(:action update-status-comp399-one-off
  :parameters ()
  :precondition (and
    (update-status comp399)
    (not (rebooted comp399))
    (one-off comp399)
  )
  :effect (and
    (not (update-status comp399))
    (update-status comp400)
    (not (one-off comp399))
    (probabilistic 1/4 (and (not (running comp399))))
  )
)
(:action update-status-comp400-rebooted
  :parameters ()
  :precondition (and
    (update-status comp400)
    (rebooted comp400)
  )
  :effect (and
    (not (update-status comp400))
    (update-status comp401)
    (not (rebooted comp400))
    (probabilistic 9/10 (and (running comp400)) 1/10 (and))
  )
)
(:action update-status-comp400-all-on
  :parameters ()
  :precondition (and
    (update-status comp400)
    (not (rebooted comp400))
    (all-on comp400)
  )
  :effect (and
    (not (update-status comp400))
    (update-status comp401)
    (not (all-on comp400))
    (probabilistic 1/20 (and (not (running comp400))))
  )
)
(:action update-status-comp400-one-off
  :parameters ()
  :precondition (and
    (update-status comp400)
    (not (rebooted comp400))
    (one-off comp400)
  )
  :effect (and
    (not (update-status comp400))
    (update-status comp401)
    (not (one-off comp400))
    (probabilistic 1/4 (and (not (running comp400))))
  )
)
(:action update-status-comp401-rebooted
  :parameters ()
  :precondition (and
    (update-status comp401)
    (rebooted comp401)
  )
  :effect (and
    (not (update-status comp401))
    (update-status comp402)
    (not (rebooted comp401))
    (probabilistic 9/10 (and (running comp401)) 1/10 (and))
  )
)
(:action update-status-comp401-all-on
  :parameters ()
  :precondition (and
    (update-status comp401)
    (not (rebooted comp401))
    (all-on comp401)
  )
  :effect (and
    (not (update-status comp401))
    (update-status comp402)
    (not (all-on comp401))
    (probabilistic 1/20 (and (not (running comp401))))
  )
)
(:action update-status-comp401-one-off
  :parameters ()
  :precondition (and
    (update-status comp401)
    (not (rebooted comp401))
    (one-off comp401)
  )
  :effect (and
    (not (update-status comp401))
    (update-status comp402)
    (not (one-off comp401))
    (probabilistic 1/4 (and (not (running comp401))))
  )
)
(:action update-status-comp402-rebooted
  :parameters ()
  :precondition (and
    (update-status comp402)
    (rebooted comp402)
  )
  :effect (and
    (not (update-status comp402))
    (update-status comp403)
    (not (rebooted comp402))
    (probabilistic 9/10 (and (running comp402)) 1/10 (and))
  )
)
(:action update-status-comp402-all-on
  :parameters ()
  :precondition (and
    (update-status comp402)
    (not (rebooted comp402))
    (all-on comp402)
  )
  :effect (and
    (not (update-status comp402))
    (update-status comp403)
    (not (all-on comp402))
    (probabilistic 1/20 (and (not (running comp402))))
  )
)
(:action update-status-comp402-one-off
  :parameters ()
  :precondition (and
    (update-status comp402)
    (not (rebooted comp402))
    (one-off comp402)
  )
  :effect (and
    (not (update-status comp402))
    (update-status comp403)
    (not (one-off comp402))
    (probabilistic 1/4 (and (not (running comp402))))
  )
)
(:action update-status-comp403-rebooted
  :parameters ()
  :precondition (and
    (update-status comp403)
    (rebooted comp403)
  )
  :effect (and
    (not (update-status comp403))
    (update-status comp404)
    (not (rebooted comp403))
    (probabilistic 9/10 (and (running comp403)) 1/10 (and))
  )
)
(:action update-status-comp403-all-on
  :parameters ()
  :precondition (and
    (update-status comp403)
    (not (rebooted comp403))
    (all-on comp403)
  )
  :effect (and
    (not (update-status comp403))
    (update-status comp404)
    (not (all-on comp403))
    (probabilistic 1/20 (and (not (running comp403))))
  )
)
(:action update-status-comp403-one-off
  :parameters ()
  :precondition (and
    (update-status comp403)
    (not (rebooted comp403))
    (one-off comp403)
  )
  :effect (and
    (not (update-status comp403))
    (update-status comp404)
    (not (one-off comp403))
    (probabilistic 1/4 (and (not (running comp403))))
  )
)
(:action update-status-comp404-rebooted
  :parameters ()
  :precondition (and
    (update-status comp404)
    (rebooted comp404)
  )
  :effect (and
    (not (update-status comp404))
    (update-status comp405)
    (not (rebooted comp404))
    (probabilistic 9/10 (and (running comp404)) 1/10 (and))
  )
)
(:action update-status-comp404-all-on
  :parameters ()
  :precondition (and
    (update-status comp404)
    (not (rebooted comp404))
    (all-on comp404)
  )
  :effect (and
    (not (update-status comp404))
    (update-status comp405)
    (not (all-on comp404))
    (probabilistic 1/20 (and (not (running comp404))))
  )
)
(:action update-status-comp404-one-off
  :parameters ()
  :precondition (and
    (update-status comp404)
    (not (rebooted comp404))
    (one-off comp404)
  )
  :effect (and
    (not (update-status comp404))
    (update-status comp405)
    (not (one-off comp404))
    (probabilistic 1/4 (and (not (running comp404))))
  )
)
(:action update-status-comp405-rebooted
  :parameters ()
  :precondition (and
    (update-status comp405)
    (rebooted comp405)
  )
  :effect (and
    (not (update-status comp405))
    (update-status comp406)
    (not (rebooted comp405))
    (probabilistic 9/10 (and (running comp405)) 1/10 (and))
  )
)
(:action update-status-comp405-all-on
  :parameters ()
  :precondition (and
    (update-status comp405)
    (not (rebooted comp405))
    (all-on comp405)
  )
  :effect (and
    (not (update-status comp405))
    (update-status comp406)
    (not (all-on comp405))
    (probabilistic 1/20 (and (not (running comp405))))
  )
)
(:action update-status-comp405-one-off
  :parameters ()
  :precondition (and
    (update-status comp405)
    (not (rebooted comp405))
    (one-off comp405)
  )
  :effect (and
    (not (update-status comp405))
    (update-status comp406)
    (not (one-off comp405))
    (probabilistic 1/4 (and (not (running comp405))))
  )
)
(:action update-status-comp406-rebooted
  :parameters ()
  :precondition (and
    (update-status comp406)
    (rebooted comp406)
  )
  :effect (and
    (not (update-status comp406))
    (update-status comp407)
    (not (rebooted comp406))
    (probabilistic 9/10 (and (running comp406)) 1/10 (and))
  )
)
(:action update-status-comp406-all-on
  :parameters ()
  :precondition (and
    (update-status comp406)
    (not (rebooted comp406))
    (all-on comp406)
  )
  :effect (and
    (not (update-status comp406))
    (update-status comp407)
    (not (all-on comp406))
    (probabilistic 1/20 (and (not (running comp406))))
  )
)
(:action update-status-comp406-one-off
  :parameters ()
  :precondition (and
    (update-status comp406)
    (not (rebooted comp406))
    (one-off comp406)
  )
  :effect (and
    (not (update-status comp406))
    (update-status comp407)
    (not (one-off comp406))
    (probabilistic 1/4 (and (not (running comp406))))
  )
)
(:action update-status-comp407-rebooted
  :parameters ()
  :precondition (and
    (update-status comp407)
    (rebooted comp407)
  )
  :effect (and
    (not (update-status comp407))
    (update-status comp408)
    (not (rebooted comp407))
    (probabilistic 9/10 (and (running comp407)) 1/10 (and))
  )
)
(:action update-status-comp407-all-on
  :parameters ()
  :precondition (and
    (update-status comp407)
    (not (rebooted comp407))
    (all-on comp407)
  )
  :effect (and
    (not (update-status comp407))
    (update-status comp408)
    (not (all-on comp407))
    (probabilistic 1/20 (and (not (running comp407))))
  )
)
(:action update-status-comp407-one-off
  :parameters ()
  :precondition (and
    (update-status comp407)
    (not (rebooted comp407))
    (one-off comp407)
  )
  :effect (and
    (not (update-status comp407))
    (update-status comp408)
    (not (one-off comp407))
    (probabilistic 1/4 (and (not (running comp407))))
  )
)
(:action update-status-comp408-rebooted
  :parameters ()
  :precondition (and
    (update-status comp408)
    (rebooted comp408)
  )
  :effect (and
    (not (update-status comp408))
    (update-status comp409)
    (not (rebooted comp408))
    (probabilistic 9/10 (and (running comp408)) 1/10 (and))
  )
)
(:action update-status-comp408-all-on
  :parameters ()
  :precondition (and
    (update-status comp408)
    (not (rebooted comp408))
    (all-on comp408)
  )
  :effect (and
    (not (update-status comp408))
    (update-status comp409)
    (not (all-on comp408))
    (probabilistic 1/20 (and (not (running comp408))))
  )
)
(:action update-status-comp408-one-off
  :parameters ()
  :precondition (and
    (update-status comp408)
    (not (rebooted comp408))
    (one-off comp408)
  )
  :effect (and
    (not (update-status comp408))
    (update-status comp409)
    (not (one-off comp408))
    (probabilistic 1/4 (and (not (running comp408))))
  )
)
(:action update-status-comp409-rebooted
  :parameters ()
  :precondition (and
    (update-status comp409)
    (rebooted comp409)
  )
  :effect (and
    (not (update-status comp409))
    (update-status comp410)
    (not (rebooted comp409))
    (probabilistic 9/10 (and (running comp409)) 1/10 (and))
  )
)
(:action update-status-comp409-all-on
  :parameters ()
  :precondition (and
    (update-status comp409)
    (not (rebooted comp409))
    (all-on comp409)
  )
  :effect (and
    (not (update-status comp409))
    (update-status comp410)
    (not (all-on comp409))
    (probabilistic 1/20 (and (not (running comp409))))
  )
)
(:action update-status-comp409-one-off
  :parameters ()
  :precondition (and
    (update-status comp409)
    (not (rebooted comp409))
    (one-off comp409)
  )
  :effect (and
    (not (update-status comp409))
    (update-status comp410)
    (not (one-off comp409))
    (probabilistic 1/4 (and (not (running comp409))))
  )
)
(:action update-status-comp410-rebooted
  :parameters ()
  :precondition (and
    (update-status comp410)
    (rebooted comp410)
  )
  :effect (and
    (not (update-status comp410))
    (update-status comp411)
    (not (rebooted comp410))
    (probabilistic 9/10 (and (running comp410)) 1/10 (and))
  )
)
(:action update-status-comp410-all-on
  :parameters ()
  :precondition (and
    (update-status comp410)
    (not (rebooted comp410))
    (all-on comp410)
  )
  :effect (and
    (not (update-status comp410))
    (update-status comp411)
    (not (all-on comp410))
    (probabilistic 1/20 (and (not (running comp410))))
  )
)
(:action update-status-comp410-one-off
  :parameters ()
  :precondition (and
    (update-status comp410)
    (not (rebooted comp410))
    (one-off comp410)
  )
  :effect (and
    (not (update-status comp410))
    (update-status comp411)
    (not (one-off comp410))
    (probabilistic 1/4 (and (not (running comp410))))
  )
)
(:action update-status-comp411-rebooted
  :parameters ()
  :precondition (and
    (update-status comp411)
    (rebooted comp411)
  )
  :effect (and
    (not (update-status comp411))
    (update-status comp412)
    (not (rebooted comp411))
    (probabilistic 9/10 (and (running comp411)) 1/10 (and))
  )
)
(:action update-status-comp411-all-on
  :parameters ()
  :precondition (and
    (update-status comp411)
    (not (rebooted comp411))
    (all-on comp411)
  )
  :effect (and
    (not (update-status comp411))
    (update-status comp412)
    (not (all-on comp411))
    (probabilistic 1/20 (and (not (running comp411))))
  )
)
(:action update-status-comp411-one-off
  :parameters ()
  :precondition (and
    (update-status comp411)
    (not (rebooted comp411))
    (one-off comp411)
  )
  :effect (and
    (not (update-status comp411))
    (update-status comp412)
    (not (one-off comp411))
    (probabilistic 1/4 (and (not (running comp411))))
  )
)
(:action update-status-comp412-rebooted
  :parameters ()
  :precondition (and
    (update-status comp412)
    (rebooted comp412)
  )
  :effect (and
    (not (update-status comp412))
    (update-status comp413)
    (not (rebooted comp412))
    (probabilistic 9/10 (and (running comp412)) 1/10 (and))
  )
)
(:action update-status-comp412-all-on
  :parameters ()
  :precondition (and
    (update-status comp412)
    (not (rebooted comp412))
    (all-on comp412)
  )
  :effect (and
    (not (update-status comp412))
    (update-status comp413)
    (not (all-on comp412))
    (probabilistic 1/20 (and (not (running comp412))))
  )
)
(:action update-status-comp412-one-off
  :parameters ()
  :precondition (and
    (update-status comp412)
    (not (rebooted comp412))
    (one-off comp412)
  )
  :effect (and
    (not (update-status comp412))
    (update-status comp413)
    (not (one-off comp412))
    (probabilistic 1/4 (and (not (running comp412))))
  )
)
(:action update-status-comp413-rebooted
  :parameters ()
  :precondition (and
    (update-status comp413)
    (rebooted comp413)
  )
  :effect (and
    (not (update-status comp413))
    (update-status comp414)
    (not (rebooted comp413))
    (probabilistic 9/10 (and (running comp413)) 1/10 (and))
  )
)
(:action update-status-comp413-all-on
  :parameters ()
  :precondition (and
    (update-status comp413)
    (not (rebooted comp413))
    (all-on comp413)
  )
  :effect (and
    (not (update-status comp413))
    (update-status comp414)
    (not (all-on comp413))
    (probabilistic 1/20 (and (not (running comp413))))
  )
)
(:action update-status-comp413-one-off
  :parameters ()
  :precondition (and
    (update-status comp413)
    (not (rebooted comp413))
    (one-off comp413)
  )
  :effect (and
    (not (update-status comp413))
    (update-status comp414)
    (not (one-off comp413))
    (probabilistic 1/4 (and (not (running comp413))))
  )
)
(:action update-status-comp414-rebooted
  :parameters ()
  :precondition (and
    (update-status comp414)
    (rebooted comp414)
  )
  :effect (and
    (not (update-status comp414))
    (update-status comp415)
    (not (rebooted comp414))
    (probabilistic 9/10 (and (running comp414)) 1/10 (and))
  )
)
(:action update-status-comp414-all-on
  :parameters ()
  :precondition (and
    (update-status comp414)
    (not (rebooted comp414))
    (all-on comp414)
  )
  :effect (and
    (not (update-status comp414))
    (update-status comp415)
    (not (all-on comp414))
    (probabilistic 1/20 (and (not (running comp414))))
  )
)
(:action update-status-comp414-one-off
  :parameters ()
  :precondition (and
    (update-status comp414)
    (not (rebooted comp414))
    (one-off comp414)
  )
  :effect (and
    (not (update-status comp414))
    (update-status comp415)
    (not (one-off comp414))
    (probabilistic 1/4 (and (not (running comp414))))
  )
)
(:action update-status-comp415-rebooted
  :parameters ()
  :precondition (and
    (update-status comp415)
    (rebooted comp415)
  )
  :effect (and
    (not (update-status comp415))
    (update-status comp416)
    (not (rebooted comp415))
    (probabilistic 9/10 (and (running comp415)) 1/10 (and))
  )
)
(:action update-status-comp415-all-on
  :parameters ()
  :precondition (and
    (update-status comp415)
    (not (rebooted comp415))
    (all-on comp415)
  )
  :effect (and
    (not (update-status comp415))
    (update-status comp416)
    (not (all-on comp415))
    (probabilistic 1/20 (and (not (running comp415))))
  )
)
(:action update-status-comp415-one-off
  :parameters ()
  :precondition (and
    (update-status comp415)
    (not (rebooted comp415))
    (one-off comp415)
  )
  :effect (and
    (not (update-status comp415))
    (update-status comp416)
    (not (one-off comp415))
    (probabilistic 1/4 (and (not (running comp415))))
  )
)
(:action update-status-comp416-rebooted
  :parameters ()
  :precondition (and
    (update-status comp416)
    (rebooted comp416)
  )
  :effect (and
    (not (update-status comp416))
    (update-status comp417)
    (not (rebooted comp416))
    (probabilistic 9/10 (and (running comp416)) 1/10 (and))
  )
)
(:action update-status-comp416-all-on
  :parameters ()
  :precondition (and
    (update-status comp416)
    (not (rebooted comp416))
    (all-on comp416)
  )
  :effect (and
    (not (update-status comp416))
    (update-status comp417)
    (not (all-on comp416))
    (probabilistic 1/20 (and (not (running comp416))))
  )
)
(:action update-status-comp416-one-off
  :parameters ()
  :precondition (and
    (update-status comp416)
    (not (rebooted comp416))
    (one-off comp416)
  )
  :effect (and
    (not (update-status comp416))
    (update-status comp417)
    (not (one-off comp416))
    (probabilistic 1/4 (and (not (running comp416))))
  )
)
(:action update-status-comp417-rebooted
  :parameters ()
  :precondition (and
    (update-status comp417)
    (rebooted comp417)
  )
  :effect (and
    (not (update-status comp417))
    (update-status comp418)
    (not (rebooted comp417))
    (probabilistic 9/10 (and (running comp417)) 1/10 (and))
  )
)
(:action update-status-comp417-all-on
  :parameters ()
  :precondition (and
    (update-status comp417)
    (not (rebooted comp417))
    (all-on comp417)
  )
  :effect (and
    (not (update-status comp417))
    (update-status comp418)
    (not (all-on comp417))
    (probabilistic 1/20 (and (not (running comp417))))
  )
)
(:action update-status-comp417-one-off
  :parameters ()
  :precondition (and
    (update-status comp417)
    (not (rebooted comp417))
    (one-off comp417)
  )
  :effect (and
    (not (update-status comp417))
    (update-status comp418)
    (not (one-off comp417))
    (probabilistic 1/4 (and (not (running comp417))))
  )
)
(:action update-status-comp418-rebooted
  :parameters ()
  :precondition (and
    (update-status comp418)
    (rebooted comp418)
  )
  :effect (and
    (not (update-status comp418))
    (update-status comp419)
    (not (rebooted comp418))
    (probabilistic 9/10 (and (running comp418)) 1/10 (and))
  )
)
(:action update-status-comp418-all-on
  :parameters ()
  :precondition (and
    (update-status comp418)
    (not (rebooted comp418))
    (all-on comp418)
  )
  :effect (and
    (not (update-status comp418))
    (update-status comp419)
    (not (all-on comp418))
    (probabilistic 1/20 (and (not (running comp418))))
  )
)
(:action update-status-comp418-one-off
  :parameters ()
  :precondition (and
    (update-status comp418)
    (not (rebooted comp418))
    (one-off comp418)
  )
  :effect (and
    (not (update-status comp418))
    (update-status comp419)
    (not (one-off comp418))
    (probabilistic 1/4 (and (not (running comp418))))
  )
)
(:action update-status-comp419-rebooted
  :parameters ()
  :precondition (and
    (update-status comp419)
    (rebooted comp419)
  )
  :effect (and
    (not (update-status comp419))
    (update-status comp420)
    (not (rebooted comp419))
    (probabilistic 9/10 (and (running comp419)) 1/10 (and))
  )
)
(:action update-status-comp419-all-on
  :parameters ()
  :precondition (and
    (update-status comp419)
    (not (rebooted comp419))
    (all-on comp419)
  )
  :effect (and
    (not (update-status comp419))
    (update-status comp420)
    (not (all-on comp419))
    (probabilistic 1/20 (and (not (running comp419))))
  )
)
(:action update-status-comp419-one-off
  :parameters ()
  :precondition (and
    (update-status comp419)
    (not (rebooted comp419))
    (one-off comp419)
  )
  :effect (and
    (not (update-status comp419))
    (update-status comp420)
    (not (one-off comp419))
    (probabilistic 1/4 (and (not (running comp419))))
  )
)
(:action update-status-comp420-rebooted
  :parameters ()
  :precondition (and
    (update-status comp420)
    (rebooted comp420)
  )
  :effect (and
    (not (update-status comp420))
    (update-status comp421)
    (not (rebooted comp420))
    (probabilistic 9/10 (and (running comp420)) 1/10 (and))
  )
)
(:action update-status-comp420-all-on
  :parameters ()
  :precondition (and
    (update-status comp420)
    (not (rebooted comp420))
    (all-on comp420)
  )
  :effect (and
    (not (update-status comp420))
    (update-status comp421)
    (not (all-on comp420))
    (probabilistic 1/20 (and (not (running comp420))))
  )
)
(:action update-status-comp420-one-off
  :parameters ()
  :precondition (and
    (update-status comp420)
    (not (rebooted comp420))
    (one-off comp420)
  )
  :effect (and
    (not (update-status comp420))
    (update-status comp421)
    (not (one-off comp420))
    (probabilistic 1/4 (and (not (running comp420))))
  )
)
(:action update-status-comp421-rebooted
  :parameters ()
  :precondition (and
    (update-status comp421)
    (rebooted comp421)
  )
  :effect (and
    (not (update-status comp421))
    (update-status comp422)
    (not (rebooted comp421))
    (probabilistic 9/10 (and (running comp421)) 1/10 (and))
  )
)
(:action update-status-comp421-all-on
  :parameters ()
  :precondition (and
    (update-status comp421)
    (not (rebooted comp421))
    (all-on comp421)
  )
  :effect (and
    (not (update-status comp421))
    (update-status comp422)
    (not (all-on comp421))
    (probabilistic 1/20 (and (not (running comp421))))
  )
)
(:action update-status-comp421-one-off
  :parameters ()
  :precondition (and
    (update-status comp421)
    (not (rebooted comp421))
    (one-off comp421)
  )
  :effect (and
    (not (update-status comp421))
    (update-status comp422)
    (not (one-off comp421))
    (probabilistic 1/4 (and (not (running comp421))))
  )
)
(:action update-status-comp422-rebooted
  :parameters ()
  :precondition (and
    (update-status comp422)
    (rebooted comp422)
  )
  :effect (and
    (not (update-status comp422))
    (update-status comp423)
    (not (rebooted comp422))
    (probabilistic 9/10 (and (running comp422)) 1/10 (and))
  )
)
(:action update-status-comp422-all-on
  :parameters ()
  :precondition (and
    (update-status comp422)
    (not (rebooted comp422))
    (all-on comp422)
  )
  :effect (and
    (not (update-status comp422))
    (update-status comp423)
    (not (all-on comp422))
    (probabilistic 1/20 (and (not (running comp422))))
  )
)
(:action update-status-comp422-one-off
  :parameters ()
  :precondition (and
    (update-status comp422)
    (not (rebooted comp422))
    (one-off comp422)
  )
  :effect (and
    (not (update-status comp422))
    (update-status comp423)
    (not (one-off comp422))
    (probabilistic 1/4 (and (not (running comp422))))
  )
)
(:action update-status-comp423-rebooted
  :parameters ()
  :precondition (and
    (update-status comp423)
    (rebooted comp423)
  )
  :effect (and
    (not (update-status comp423))
    (update-status comp424)
    (not (rebooted comp423))
    (probabilistic 9/10 (and (running comp423)) 1/10 (and))
  )
)
(:action update-status-comp423-all-on
  :parameters ()
  :precondition (and
    (update-status comp423)
    (not (rebooted comp423))
    (all-on comp423)
  )
  :effect (and
    (not (update-status comp423))
    (update-status comp424)
    (not (all-on comp423))
    (probabilistic 1/20 (and (not (running comp423))))
  )
)
(:action update-status-comp423-one-off
  :parameters ()
  :precondition (and
    (update-status comp423)
    (not (rebooted comp423))
    (one-off comp423)
  )
  :effect (and
    (not (update-status comp423))
    (update-status comp424)
    (not (one-off comp423))
    (probabilistic 1/4 (and (not (running comp423))))
  )
)
(:action update-status-comp424-rebooted
  :parameters ()
  :precondition (and
    (update-status comp424)
    (rebooted comp424)
  )
  :effect (and
    (not (update-status comp424))
    (update-status comp425)
    (not (rebooted comp424))
    (probabilistic 9/10 (and (running comp424)) 1/10 (and))
  )
)
(:action update-status-comp424-all-on
  :parameters ()
  :precondition (and
    (update-status comp424)
    (not (rebooted comp424))
    (all-on comp424)
  )
  :effect (and
    (not (update-status comp424))
    (update-status comp425)
    (not (all-on comp424))
    (probabilistic 1/20 (and (not (running comp424))))
  )
)
(:action update-status-comp424-one-off
  :parameters ()
  :precondition (and
    (update-status comp424)
    (not (rebooted comp424))
    (one-off comp424)
  )
  :effect (and
    (not (update-status comp424))
    (update-status comp425)
    (not (one-off comp424))
    (probabilistic 1/4 (and (not (running comp424))))
  )
)
(:action update-status-comp425-rebooted
  :parameters ()
  :precondition (and
    (update-status comp425)
    (rebooted comp425)
  )
  :effect (and
    (not (update-status comp425))
    (update-status comp426)
    (not (rebooted comp425))
    (probabilistic 9/10 (and (running comp425)) 1/10 (and))
  )
)
(:action update-status-comp425-all-on
  :parameters ()
  :precondition (and
    (update-status comp425)
    (not (rebooted comp425))
    (all-on comp425)
  )
  :effect (and
    (not (update-status comp425))
    (update-status comp426)
    (not (all-on comp425))
    (probabilistic 1/20 (and (not (running comp425))))
  )
)
(:action update-status-comp425-one-off
  :parameters ()
  :precondition (and
    (update-status comp425)
    (not (rebooted comp425))
    (one-off comp425)
  )
  :effect (and
    (not (update-status comp425))
    (update-status comp426)
    (not (one-off comp425))
    (probabilistic 1/4 (and (not (running comp425))))
  )
)
(:action update-status-comp426-rebooted
  :parameters ()
  :precondition (and
    (update-status comp426)
    (rebooted comp426)
  )
  :effect (and
    (not (update-status comp426))
    (update-status comp427)
    (not (rebooted comp426))
    (probabilistic 9/10 (and (running comp426)) 1/10 (and))
  )
)
(:action update-status-comp426-all-on
  :parameters ()
  :precondition (and
    (update-status comp426)
    (not (rebooted comp426))
    (all-on comp426)
  )
  :effect (and
    (not (update-status comp426))
    (update-status comp427)
    (not (all-on comp426))
    (probabilistic 1/20 (and (not (running comp426))))
  )
)
(:action update-status-comp426-one-off
  :parameters ()
  :precondition (and
    (update-status comp426)
    (not (rebooted comp426))
    (one-off comp426)
  )
  :effect (and
    (not (update-status comp426))
    (update-status comp427)
    (not (one-off comp426))
    (probabilistic 1/4 (and (not (running comp426))))
  )
)
(:action update-status-comp427-rebooted
  :parameters ()
  :precondition (and
    (update-status comp427)
    (rebooted comp427)
  )
  :effect (and
    (not (update-status comp427))
    (update-status comp428)
    (not (rebooted comp427))
    (probabilistic 9/10 (and (running comp427)) 1/10 (and))
  )
)
(:action update-status-comp427-all-on
  :parameters ()
  :precondition (and
    (update-status comp427)
    (not (rebooted comp427))
    (all-on comp427)
  )
  :effect (and
    (not (update-status comp427))
    (update-status comp428)
    (not (all-on comp427))
    (probabilistic 1/20 (and (not (running comp427))))
  )
)
(:action update-status-comp427-one-off
  :parameters ()
  :precondition (and
    (update-status comp427)
    (not (rebooted comp427))
    (one-off comp427)
  )
  :effect (and
    (not (update-status comp427))
    (update-status comp428)
    (not (one-off comp427))
    (probabilistic 1/4 (and (not (running comp427))))
  )
)
(:action update-status-comp428-rebooted
  :parameters ()
  :precondition (and
    (update-status comp428)
    (rebooted comp428)
  )
  :effect (and
    (not (update-status comp428))
    (update-status comp429)
    (not (rebooted comp428))
    (probabilistic 9/10 (and (running comp428)) 1/10 (and))
  )
)
(:action update-status-comp428-all-on
  :parameters ()
  :precondition (and
    (update-status comp428)
    (not (rebooted comp428))
    (all-on comp428)
  )
  :effect (and
    (not (update-status comp428))
    (update-status comp429)
    (not (all-on comp428))
    (probabilistic 1/20 (and (not (running comp428))))
  )
)
(:action update-status-comp428-one-off
  :parameters ()
  :precondition (and
    (update-status comp428)
    (not (rebooted comp428))
    (one-off comp428)
  )
  :effect (and
    (not (update-status comp428))
    (update-status comp429)
    (not (one-off comp428))
    (probabilistic 1/4 (and (not (running comp428))))
  )
)
(:action update-status-comp429-rebooted
  :parameters ()
  :precondition (and
    (update-status comp429)
    (rebooted comp429)
  )
  :effect (and
    (not (update-status comp429))
    (update-status comp430)
    (not (rebooted comp429))
    (probabilistic 9/10 (and (running comp429)) 1/10 (and))
  )
)
(:action update-status-comp429-all-on
  :parameters ()
  :precondition (and
    (update-status comp429)
    (not (rebooted comp429))
    (all-on comp429)
  )
  :effect (and
    (not (update-status comp429))
    (update-status comp430)
    (not (all-on comp429))
    (probabilistic 1/20 (and (not (running comp429))))
  )
)
(:action update-status-comp429-one-off
  :parameters ()
  :precondition (and
    (update-status comp429)
    (not (rebooted comp429))
    (one-off comp429)
  )
  :effect (and
    (not (update-status comp429))
    (update-status comp430)
    (not (one-off comp429))
    (probabilistic 1/4 (and (not (running comp429))))
  )
)
(:action update-status-comp430-rebooted
  :parameters ()
  :precondition (and
    (update-status comp430)
    (rebooted comp430)
  )
  :effect (and
    (not (update-status comp430))
    (update-status comp431)
    (not (rebooted comp430))
    (probabilistic 9/10 (and (running comp430)) 1/10 (and))
  )
)
(:action update-status-comp430-all-on
  :parameters ()
  :precondition (and
    (update-status comp430)
    (not (rebooted comp430))
    (all-on comp430)
  )
  :effect (and
    (not (update-status comp430))
    (update-status comp431)
    (not (all-on comp430))
    (probabilistic 1/20 (and (not (running comp430))))
  )
)
(:action update-status-comp430-one-off
  :parameters ()
  :precondition (and
    (update-status comp430)
    (not (rebooted comp430))
    (one-off comp430)
  )
  :effect (and
    (not (update-status comp430))
    (update-status comp431)
    (not (one-off comp430))
    (probabilistic 1/4 (and (not (running comp430))))
  )
)
(:action update-status-comp431-rebooted
  :parameters ()
  :precondition (and
    (update-status comp431)
    (rebooted comp431)
  )
  :effect (and
    (not (update-status comp431))
    (update-status comp432)
    (not (rebooted comp431))
    (probabilistic 9/10 (and (running comp431)) 1/10 (and))
  )
)
(:action update-status-comp431-all-on
  :parameters ()
  :precondition (and
    (update-status comp431)
    (not (rebooted comp431))
    (all-on comp431)
  )
  :effect (and
    (not (update-status comp431))
    (update-status comp432)
    (not (all-on comp431))
    (probabilistic 1/20 (and (not (running comp431))))
  )
)
(:action update-status-comp431-one-off
  :parameters ()
  :precondition (and
    (update-status comp431)
    (not (rebooted comp431))
    (one-off comp431)
  )
  :effect (and
    (not (update-status comp431))
    (update-status comp432)
    (not (one-off comp431))
    (probabilistic 1/4 (and (not (running comp431))))
  )
)
(:action update-status-comp432-rebooted
  :parameters ()
  :precondition (and
    (update-status comp432)
    (rebooted comp432)
  )
  :effect (and
    (not (update-status comp432))
    (update-status comp433)
    (not (rebooted comp432))
    (probabilistic 9/10 (and (running comp432)) 1/10 (and))
  )
)
(:action update-status-comp432-all-on
  :parameters ()
  :precondition (and
    (update-status comp432)
    (not (rebooted comp432))
    (all-on comp432)
  )
  :effect (and
    (not (update-status comp432))
    (update-status comp433)
    (not (all-on comp432))
    (probabilistic 1/20 (and (not (running comp432))))
  )
)
(:action update-status-comp432-one-off
  :parameters ()
  :precondition (and
    (update-status comp432)
    (not (rebooted comp432))
    (one-off comp432)
  )
  :effect (and
    (not (update-status comp432))
    (update-status comp433)
    (not (one-off comp432))
    (probabilistic 1/4 (and (not (running comp432))))
  )
)
(:action update-status-comp433-rebooted
  :parameters ()
  :precondition (and
    (update-status comp433)
    (rebooted comp433)
  )
  :effect (and
    (not (update-status comp433))
    (update-status comp434)
    (not (rebooted comp433))
    (probabilistic 9/10 (and (running comp433)) 1/10 (and))
  )
)
(:action update-status-comp433-all-on
  :parameters ()
  :precondition (and
    (update-status comp433)
    (not (rebooted comp433))
    (all-on comp433)
  )
  :effect (and
    (not (update-status comp433))
    (update-status comp434)
    (not (all-on comp433))
    (probabilistic 1/20 (and (not (running comp433))))
  )
)
(:action update-status-comp433-one-off
  :parameters ()
  :precondition (and
    (update-status comp433)
    (not (rebooted comp433))
    (one-off comp433)
  )
  :effect (and
    (not (update-status comp433))
    (update-status comp434)
    (not (one-off comp433))
    (probabilistic 1/4 (and (not (running comp433))))
  )
)
(:action update-status-comp434-rebooted
  :parameters ()
  :precondition (and
    (update-status comp434)
    (rebooted comp434)
  )
  :effect (and
    (not (update-status comp434))
    (update-status comp435)
    (not (rebooted comp434))
    (probabilistic 9/10 (and (running comp434)) 1/10 (and))
  )
)
(:action update-status-comp434-all-on
  :parameters ()
  :precondition (and
    (update-status comp434)
    (not (rebooted comp434))
    (all-on comp434)
  )
  :effect (and
    (not (update-status comp434))
    (update-status comp435)
    (not (all-on comp434))
    (probabilistic 1/20 (and (not (running comp434))))
  )
)
(:action update-status-comp434-one-off
  :parameters ()
  :precondition (and
    (update-status comp434)
    (not (rebooted comp434))
    (one-off comp434)
  )
  :effect (and
    (not (update-status comp434))
    (update-status comp435)
    (not (one-off comp434))
    (probabilistic 1/4 (and (not (running comp434))))
  )
)
(:action update-status-comp435-rebooted
  :parameters ()
  :precondition (and
    (update-status comp435)
    (rebooted comp435)
  )
  :effect (and
    (not (update-status comp435))
    (update-status comp436)
    (not (rebooted comp435))
    (probabilistic 9/10 (and (running comp435)) 1/10 (and))
  )
)
(:action update-status-comp435-all-on
  :parameters ()
  :precondition (and
    (update-status comp435)
    (not (rebooted comp435))
    (all-on comp435)
  )
  :effect (and
    (not (update-status comp435))
    (update-status comp436)
    (not (all-on comp435))
    (probabilistic 1/20 (and (not (running comp435))))
  )
)
(:action update-status-comp435-one-off
  :parameters ()
  :precondition (and
    (update-status comp435)
    (not (rebooted comp435))
    (one-off comp435)
  )
  :effect (and
    (not (update-status comp435))
    (update-status comp436)
    (not (one-off comp435))
    (probabilistic 1/4 (and (not (running comp435))))
  )
)
(:action update-status-comp436-rebooted
  :parameters ()
  :precondition (and
    (update-status comp436)
    (rebooted comp436)
  )
  :effect (and
    (not (update-status comp436))
    (update-status comp437)
    (not (rebooted comp436))
    (probabilistic 9/10 (and (running comp436)) 1/10 (and))
  )
)
(:action update-status-comp436-all-on
  :parameters ()
  :precondition (and
    (update-status comp436)
    (not (rebooted comp436))
    (all-on comp436)
  )
  :effect (and
    (not (update-status comp436))
    (update-status comp437)
    (not (all-on comp436))
    (probabilistic 1/20 (and (not (running comp436))))
  )
)
(:action update-status-comp436-one-off
  :parameters ()
  :precondition (and
    (update-status comp436)
    (not (rebooted comp436))
    (one-off comp436)
  )
  :effect (and
    (not (update-status comp436))
    (update-status comp437)
    (not (one-off comp436))
    (probabilistic 1/4 (and (not (running comp436))))
  )
)
(:action update-status-comp437-rebooted
  :parameters ()
  :precondition (and
    (update-status comp437)
    (rebooted comp437)
  )
  :effect (and
    (not (update-status comp437))
    (update-status comp438)
    (not (rebooted comp437))
    (probabilistic 9/10 (and (running comp437)) 1/10 (and))
  )
)
(:action update-status-comp437-all-on
  :parameters ()
  :precondition (and
    (update-status comp437)
    (not (rebooted comp437))
    (all-on comp437)
  )
  :effect (and
    (not (update-status comp437))
    (update-status comp438)
    (not (all-on comp437))
    (probabilistic 1/20 (and (not (running comp437))))
  )
)
(:action update-status-comp437-one-off
  :parameters ()
  :precondition (and
    (update-status comp437)
    (not (rebooted comp437))
    (one-off comp437)
  )
  :effect (and
    (not (update-status comp437))
    (update-status comp438)
    (not (one-off comp437))
    (probabilistic 1/4 (and (not (running comp437))))
  )
)
(:action update-status-comp438-rebooted
  :parameters ()
  :precondition (and
    (update-status comp438)
    (rebooted comp438)
  )
  :effect (and
    (not (update-status comp438))
    (update-status comp439)
    (not (rebooted comp438))
    (probabilistic 9/10 (and (running comp438)) 1/10 (and))
  )
)
(:action update-status-comp438-all-on
  :parameters ()
  :precondition (and
    (update-status comp438)
    (not (rebooted comp438))
    (all-on comp438)
  )
  :effect (and
    (not (update-status comp438))
    (update-status comp439)
    (not (all-on comp438))
    (probabilistic 1/20 (and (not (running comp438))))
  )
)
(:action update-status-comp438-one-off
  :parameters ()
  :precondition (and
    (update-status comp438)
    (not (rebooted comp438))
    (one-off comp438)
  )
  :effect (and
    (not (update-status comp438))
    (update-status comp439)
    (not (one-off comp438))
    (probabilistic 1/4 (and (not (running comp438))))
  )
)
(:action update-status-comp439-rebooted
  :parameters ()
  :precondition (and
    (update-status comp439)
    (rebooted comp439)
  )
  :effect (and
    (not (update-status comp439))
    (update-status comp440)
    (not (rebooted comp439))
    (probabilistic 9/10 (and (running comp439)) 1/10 (and))
  )
)
(:action update-status-comp439-all-on
  :parameters ()
  :precondition (and
    (update-status comp439)
    (not (rebooted comp439))
    (all-on comp439)
  )
  :effect (and
    (not (update-status comp439))
    (update-status comp440)
    (not (all-on comp439))
    (probabilistic 1/20 (and (not (running comp439))))
  )
)
(:action update-status-comp439-one-off
  :parameters ()
  :precondition (and
    (update-status comp439)
    (not (rebooted comp439))
    (one-off comp439)
  )
  :effect (and
    (not (update-status comp439))
    (update-status comp440)
    (not (one-off comp439))
    (probabilistic 1/4 (and (not (running comp439))))
  )
)
(:action update-status-comp440-rebooted
  :parameters ()
  :precondition (and
    (update-status comp440)
    (rebooted comp440)
  )
  :effect (and
    (not (update-status comp440))
    (update-status comp441)
    (not (rebooted comp440))
    (probabilistic 9/10 (and (running comp440)) 1/10 (and))
  )
)
(:action update-status-comp440-all-on
  :parameters ()
  :precondition (and
    (update-status comp440)
    (not (rebooted comp440))
    (all-on comp440)
  )
  :effect (and
    (not (update-status comp440))
    (update-status comp441)
    (not (all-on comp440))
    (probabilistic 1/20 (and (not (running comp440))))
  )
)
(:action update-status-comp440-one-off
  :parameters ()
  :precondition (and
    (update-status comp440)
    (not (rebooted comp440))
    (one-off comp440)
  )
  :effect (and
    (not (update-status comp440))
    (update-status comp441)
    (not (one-off comp440))
    (probabilistic 1/4 (and (not (running comp440))))
  )
)
(:action update-status-comp441-rebooted
  :parameters ()
  :precondition (and
    (update-status comp441)
    (rebooted comp441)
  )
  :effect (and
    (not (update-status comp441))
    (update-status comp442)
    (not (rebooted comp441))
    (probabilistic 9/10 (and (running comp441)) 1/10 (and))
  )
)
(:action update-status-comp441-all-on
  :parameters ()
  :precondition (and
    (update-status comp441)
    (not (rebooted comp441))
    (all-on comp441)
  )
  :effect (and
    (not (update-status comp441))
    (update-status comp442)
    (not (all-on comp441))
    (probabilistic 1/20 (and (not (running comp441))))
  )
)
(:action update-status-comp441-one-off
  :parameters ()
  :precondition (and
    (update-status comp441)
    (not (rebooted comp441))
    (one-off comp441)
  )
  :effect (and
    (not (update-status comp441))
    (update-status comp442)
    (not (one-off comp441))
    (probabilistic 1/4 (and (not (running comp441))))
  )
)
(:action update-status-comp442-rebooted
  :parameters ()
  :precondition (and
    (update-status comp442)
    (rebooted comp442)
  )
  :effect (and
    (not (update-status comp442))
    (update-status comp443)
    (not (rebooted comp442))
    (probabilistic 9/10 (and (running comp442)) 1/10 (and))
  )
)
(:action update-status-comp442-all-on
  :parameters ()
  :precondition (and
    (update-status comp442)
    (not (rebooted comp442))
    (all-on comp442)
  )
  :effect (and
    (not (update-status comp442))
    (update-status comp443)
    (not (all-on comp442))
    (probabilistic 1/20 (and (not (running comp442))))
  )
)
(:action update-status-comp442-one-off
  :parameters ()
  :precondition (and
    (update-status comp442)
    (not (rebooted comp442))
    (one-off comp442)
  )
  :effect (and
    (not (update-status comp442))
    (update-status comp443)
    (not (one-off comp442))
    (probabilistic 1/4 (and (not (running comp442))))
  )
)
(:action update-status-comp443-rebooted
  :parameters ()
  :precondition (and
    (update-status comp443)
    (rebooted comp443)
  )
  :effect (and
    (not (update-status comp443))
    (update-status comp444)
    (not (rebooted comp443))
    (probabilistic 9/10 (and (running comp443)) 1/10 (and))
  )
)
(:action update-status-comp443-all-on
  :parameters ()
  :precondition (and
    (update-status comp443)
    (not (rebooted comp443))
    (all-on comp443)
  )
  :effect (and
    (not (update-status comp443))
    (update-status comp444)
    (not (all-on comp443))
    (probabilistic 1/20 (and (not (running comp443))))
  )
)
(:action update-status-comp443-one-off
  :parameters ()
  :precondition (and
    (update-status comp443)
    (not (rebooted comp443))
    (one-off comp443)
  )
  :effect (and
    (not (update-status comp443))
    (update-status comp444)
    (not (one-off comp443))
    (probabilistic 1/4 (and (not (running comp443))))
  )
)
(:action update-status-comp444-rebooted
  :parameters ()
  :precondition (and
    (update-status comp444)
    (rebooted comp444)
  )
  :effect (and
    (not (update-status comp444))
    (update-status comp445)
    (not (rebooted comp444))
    (probabilistic 9/10 (and (running comp444)) 1/10 (and))
  )
)
(:action update-status-comp444-all-on
  :parameters ()
  :precondition (and
    (update-status comp444)
    (not (rebooted comp444))
    (all-on comp444)
  )
  :effect (and
    (not (update-status comp444))
    (update-status comp445)
    (not (all-on comp444))
    (probabilistic 1/20 (and (not (running comp444))))
  )
)
(:action update-status-comp444-one-off
  :parameters ()
  :precondition (and
    (update-status comp444)
    (not (rebooted comp444))
    (one-off comp444)
  )
  :effect (and
    (not (update-status comp444))
    (update-status comp445)
    (not (one-off comp444))
    (probabilistic 1/4 (and (not (running comp444))))
  )
)
(:action update-status-comp445-rebooted
  :parameters ()
  :precondition (and
    (update-status comp445)
    (rebooted comp445)
  )
  :effect (and
    (not (update-status comp445))
    (update-status comp446)
    (not (rebooted comp445))
    (probabilistic 9/10 (and (running comp445)) 1/10 (and))
  )
)
(:action update-status-comp445-all-on
  :parameters ()
  :precondition (and
    (update-status comp445)
    (not (rebooted comp445))
    (all-on comp445)
  )
  :effect (and
    (not (update-status comp445))
    (update-status comp446)
    (not (all-on comp445))
    (probabilistic 1/20 (and (not (running comp445))))
  )
)
(:action update-status-comp445-one-off
  :parameters ()
  :precondition (and
    (update-status comp445)
    (not (rebooted comp445))
    (one-off comp445)
  )
  :effect (and
    (not (update-status comp445))
    (update-status comp446)
    (not (one-off comp445))
    (probabilistic 1/4 (and (not (running comp445))))
  )
)
(:action update-status-comp446-rebooted
  :parameters ()
  :precondition (and
    (update-status comp446)
    (rebooted comp446)
  )
  :effect (and
    (not (update-status comp446))
    (update-status comp447)
    (not (rebooted comp446))
    (probabilistic 9/10 (and (running comp446)) 1/10 (and))
  )
)
(:action update-status-comp446-all-on
  :parameters ()
  :precondition (and
    (update-status comp446)
    (not (rebooted comp446))
    (all-on comp446)
  )
  :effect (and
    (not (update-status comp446))
    (update-status comp447)
    (not (all-on comp446))
    (probabilistic 1/20 (and (not (running comp446))))
  )
)
(:action update-status-comp446-one-off
  :parameters ()
  :precondition (and
    (update-status comp446)
    (not (rebooted comp446))
    (one-off comp446)
  )
  :effect (and
    (not (update-status comp446))
    (update-status comp447)
    (not (one-off comp446))
    (probabilistic 1/4 (and (not (running comp446))))
  )
)
(:action update-status-comp447-rebooted
  :parameters ()
  :precondition (and
    (update-status comp447)
    (rebooted comp447)
  )
  :effect (and
    (not (update-status comp447))
    (update-status comp448)
    (not (rebooted comp447))
    (probabilistic 9/10 (and (running comp447)) 1/10 (and))
  )
)
(:action update-status-comp447-all-on
  :parameters ()
  :precondition (and
    (update-status comp447)
    (not (rebooted comp447))
    (all-on comp447)
  )
  :effect (and
    (not (update-status comp447))
    (update-status comp448)
    (not (all-on comp447))
    (probabilistic 1/20 (and (not (running comp447))))
  )
)
(:action update-status-comp447-one-off
  :parameters ()
  :precondition (and
    (update-status comp447)
    (not (rebooted comp447))
    (one-off comp447)
  )
  :effect (and
    (not (update-status comp447))
    (update-status comp448)
    (not (one-off comp447))
    (probabilistic 1/4 (and (not (running comp447))))
  )
)
(:action update-status-comp448-rebooted
  :parameters ()
  :precondition (and
    (update-status comp448)
    (rebooted comp448)
  )
  :effect (and
    (not (update-status comp448))
    (update-status comp449)
    (not (rebooted comp448))
    (probabilistic 9/10 (and (running comp448)) 1/10 (and))
  )
)
(:action update-status-comp448-all-on
  :parameters ()
  :precondition (and
    (update-status comp448)
    (not (rebooted comp448))
    (all-on comp448)
  )
  :effect (and
    (not (update-status comp448))
    (update-status comp449)
    (not (all-on comp448))
    (probabilistic 1/20 (and (not (running comp448))))
  )
)
(:action update-status-comp448-one-off
  :parameters ()
  :precondition (and
    (update-status comp448)
    (not (rebooted comp448))
    (one-off comp448)
  )
  :effect (and
    (not (update-status comp448))
    (update-status comp449)
    (not (one-off comp448))
    (probabilistic 1/4 (and (not (running comp448))))
  )
)
(:action update-status-comp449-rebooted
  :parameters ()
  :precondition (and
    (update-status comp449)
    (rebooted comp449)
  )
  :effect (and
    (not (update-status comp449))
    (update-status comp450)
    (not (rebooted comp449))
    (probabilistic 9/10 (and (running comp449)) 1/10 (and))
  )
)
(:action update-status-comp449-all-on
  :parameters ()
  :precondition (and
    (update-status comp449)
    (not (rebooted comp449))
    (all-on comp449)
  )
  :effect (and
    (not (update-status comp449))
    (update-status comp450)
    (not (all-on comp449))
    (probabilistic 1/20 (and (not (running comp449))))
  )
)
(:action update-status-comp449-one-off
  :parameters ()
  :precondition (and
    (update-status comp449)
    (not (rebooted comp449))
    (one-off comp449)
  )
  :effect (and
    (not (update-status comp449))
    (update-status comp450)
    (not (one-off comp449))
    (probabilistic 1/4 (and (not (running comp449))))
  )
)
(:action update-status-comp450-rebooted
  :parameters ()
  :precondition (and
    (update-status comp450)
    (rebooted comp450)
  )
  :effect (and
    (not (update-status comp450))
    (update-status comp451)
    (not (rebooted comp450))
    (probabilistic 9/10 (and (running comp450)) 1/10 (and))
  )
)
(:action update-status-comp450-all-on
  :parameters ()
  :precondition (and
    (update-status comp450)
    (not (rebooted comp450))
    (all-on comp450)
  )
  :effect (and
    (not (update-status comp450))
    (update-status comp451)
    (not (all-on comp450))
    (probabilistic 1/20 (and (not (running comp450))))
  )
)
(:action update-status-comp450-one-off
  :parameters ()
  :precondition (and
    (update-status comp450)
    (not (rebooted comp450))
    (one-off comp450)
  )
  :effect (and
    (not (update-status comp450))
    (update-status comp451)
    (not (one-off comp450))
    (probabilistic 1/4 (and (not (running comp450))))
  )
)
(:action update-status-comp451-rebooted
  :parameters ()
  :precondition (and
    (update-status comp451)
    (rebooted comp451)
  )
  :effect (and
    (not (update-status comp451))
    (update-status comp452)
    (not (rebooted comp451))
    (probabilistic 9/10 (and (running comp451)) 1/10 (and))
  )
)
(:action update-status-comp451-all-on
  :parameters ()
  :precondition (and
    (update-status comp451)
    (not (rebooted comp451))
    (all-on comp451)
  )
  :effect (and
    (not (update-status comp451))
    (update-status comp452)
    (not (all-on comp451))
    (probabilistic 1/20 (and (not (running comp451))))
  )
)
(:action update-status-comp451-one-off
  :parameters ()
  :precondition (and
    (update-status comp451)
    (not (rebooted comp451))
    (one-off comp451)
  )
  :effect (and
    (not (update-status comp451))
    (update-status comp452)
    (not (one-off comp451))
    (probabilistic 1/4 (and (not (running comp451))))
  )
)
(:action update-status-comp452-rebooted
  :parameters ()
  :precondition (and
    (update-status comp452)
    (rebooted comp452)
  )
  :effect (and
    (not (update-status comp452))
    (update-status comp453)
    (not (rebooted comp452))
    (probabilistic 9/10 (and (running comp452)) 1/10 (and))
  )
)
(:action update-status-comp452-all-on
  :parameters ()
  :precondition (and
    (update-status comp452)
    (not (rebooted comp452))
    (all-on comp452)
  )
  :effect (and
    (not (update-status comp452))
    (update-status comp453)
    (not (all-on comp452))
    (probabilistic 1/20 (and (not (running comp452))))
  )
)
(:action update-status-comp452-one-off
  :parameters ()
  :precondition (and
    (update-status comp452)
    (not (rebooted comp452))
    (one-off comp452)
  )
  :effect (and
    (not (update-status comp452))
    (update-status comp453)
    (not (one-off comp452))
    (probabilistic 1/4 (and (not (running comp452))))
  )
)
(:action update-status-comp453-rebooted
  :parameters ()
  :precondition (and
    (update-status comp453)
    (rebooted comp453)
  )
  :effect (and
    (not (update-status comp453))
    (update-status comp454)
    (not (rebooted comp453))
    (probabilistic 9/10 (and (running comp453)) 1/10 (and))
  )
)
(:action update-status-comp453-all-on
  :parameters ()
  :precondition (and
    (update-status comp453)
    (not (rebooted comp453))
    (all-on comp453)
  )
  :effect (and
    (not (update-status comp453))
    (update-status comp454)
    (not (all-on comp453))
    (probabilistic 1/20 (and (not (running comp453))))
  )
)
(:action update-status-comp453-one-off
  :parameters ()
  :precondition (and
    (update-status comp453)
    (not (rebooted comp453))
    (one-off comp453)
  )
  :effect (and
    (not (update-status comp453))
    (update-status comp454)
    (not (one-off comp453))
    (probabilistic 1/4 (and (not (running comp453))))
  )
)
(:action update-status-comp454-rebooted
  :parameters ()
  :precondition (and
    (update-status comp454)
    (rebooted comp454)
  )
  :effect (and
    (not (update-status comp454))
    (update-status comp455)
    (not (rebooted comp454))
    (probabilistic 9/10 (and (running comp454)) 1/10 (and))
  )
)
(:action update-status-comp454-all-on
  :parameters ()
  :precondition (and
    (update-status comp454)
    (not (rebooted comp454))
    (all-on comp454)
  )
  :effect (and
    (not (update-status comp454))
    (update-status comp455)
    (not (all-on comp454))
    (probabilistic 1/20 (and (not (running comp454))))
  )
)
(:action update-status-comp454-one-off
  :parameters ()
  :precondition (and
    (update-status comp454)
    (not (rebooted comp454))
    (one-off comp454)
  )
  :effect (and
    (not (update-status comp454))
    (update-status comp455)
    (not (one-off comp454))
    (probabilistic 1/4 (and (not (running comp454))))
  )
)
(:action update-status-comp455-rebooted
  :parameters ()
  :precondition (and
    (update-status comp455)
    (rebooted comp455)
  )
  :effect (and
    (not (update-status comp455))
    (update-status comp456)
    (not (rebooted comp455))
    (probabilistic 9/10 (and (running comp455)) 1/10 (and))
  )
)
(:action update-status-comp455-all-on
  :parameters ()
  :precondition (and
    (update-status comp455)
    (not (rebooted comp455))
    (all-on comp455)
  )
  :effect (and
    (not (update-status comp455))
    (update-status comp456)
    (not (all-on comp455))
    (probabilistic 1/20 (and (not (running comp455))))
  )
)
(:action update-status-comp455-one-off
  :parameters ()
  :precondition (and
    (update-status comp455)
    (not (rebooted comp455))
    (one-off comp455)
  )
  :effect (and
    (not (update-status comp455))
    (update-status comp456)
    (not (one-off comp455))
    (probabilistic 1/4 (and (not (running comp455))))
  )
)
(:action update-status-comp456-rebooted
  :parameters ()
  :precondition (and
    (update-status comp456)
    (rebooted comp456)
  )
  :effect (and
    (not (update-status comp456))
    (update-status comp457)
    (not (rebooted comp456))
    (probabilistic 9/10 (and (running comp456)) 1/10 (and))
  )
)
(:action update-status-comp456-all-on
  :parameters ()
  :precondition (and
    (update-status comp456)
    (not (rebooted comp456))
    (all-on comp456)
  )
  :effect (and
    (not (update-status comp456))
    (update-status comp457)
    (not (all-on comp456))
    (probabilistic 1/20 (and (not (running comp456))))
  )
)
(:action update-status-comp456-one-off
  :parameters ()
  :precondition (and
    (update-status comp456)
    (not (rebooted comp456))
    (one-off comp456)
  )
  :effect (and
    (not (update-status comp456))
    (update-status comp457)
    (not (one-off comp456))
    (probabilistic 1/4 (and (not (running comp456))))
  )
)
(:action update-status-comp457-rebooted
  :parameters ()
  :precondition (and
    (update-status comp457)
    (rebooted comp457)
  )
  :effect (and
    (not (update-status comp457))
    (update-status comp458)
    (not (rebooted comp457))
    (probabilistic 9/10 (and (running comp457)) 1/10 (and))
  )
)
(:action update-status-comp457-all-on
  :parameters ()
  :precondition (and
    (update-status comp457)
    (not (rebooted comp457))
    (all-on comp457)
  )
  :effect (and
    (not (update-status comp457))
    (update-status comp458)
    (not (all-on comp457))
    (probabilistic 1/20 (and (not (running comp457))))
  )
)
(:action update-status-comp457-one-off
  :parameters ()
  :precondition (and
    (update-status comp457)
    (not (rebooted comp457))
    (one-off comp457)
  )
  :effect (and
    (not (update-status comp457))
    (update-status comp458)
    (not (one-off comp457))
    (probabilistic 1/4 (and (not (running comp457))))
  )
)
(:action update-status-comp458-rebooted
  :parameters ()
  :precondition (and
    (update-status comp458)
    (rebooted comp458)
  )
  :effect (and
    (not (update-status comp458))
    (update-status comp459)
    (not (rebooted comp458))
    (probabilistic 9/10 (and (running comp458)) 1/10 (and))
  )
)
(:action update-status-comp458-all-on
  :parameters ()
  :precondition (and
    (update-status comp458)
    (not (rebooted comp458))
    (all-on comp458)
  )
  :effect (and
    (not (update-status comp458))
    (update-status comp459)
    (not (all-on comp458))
    (probabilistic 1/20 (and (not (running comp458))))
  )
)
(:action update-status-comp458-one-off
  :parameters ()
  :precondition (and
    (update-status comp458)
    (not (rebooted comp458))
    (one-off comp458)
  )
  :effect (and
    (not (update-status comp458))
    (update-status comp459)
    (not (one-off comp458))
    (probabilistic 1/4 (and (not (running comp458))))
  )
)
(:action update-status-comp459-rebooted
  :parameters ()
  :precondition (and
    (update-status comp459)
    (rebooted comp459)
  )
  :effect (and
    (not (update-status comp459))
    (update-status comp460)
    (not (rebooted comp459))
    (probabilistic 9/10 (and (running comp459)) 1/10 (and))
  )
)
(:action update-status-comp459-all-on
  :parameters ()
  :precondition (and
    (update-status comp459)
    (not (rebooted comp459))
    (all-on comp459)
  )
  :effect (and
    (not (update-status comp459))
    (update-status comp460)
    (not (all-on comp459))
    (probabilistic 1/20 (and (not (running comp459))))
  )
)
(:action update-status-comp459-one-off
  :parameters ()
  :precondition (and
    (update-status comp459)
    (not (rebooted comp459))
    (one-off comp459)
  )
  :effect (and
    (not (update-status comp459))
    (update-status comp460)
    (not (one-off comp459))
    (probabilistic 1/4 (and (not (running comp459))))
  )
)
(:action update-status-comp460-rebooted
  :parameters ()
  :precondition (and
    (update-status comp460)
    (rebooted comp460)
  )
  :effect (and
    (not (update-status comp460))
    (update-status comp461)
    (not (rebooted comp460))
    (probabilistic 9/10 (and (running comp460)) 1/10 (and))
  )
)
(:action update-status-comp460-all-on
  :parameters ()
  :precondition (and
    (update-status comp460)
    (not (rebooted comp460))
    (all-on comp460)
  )
  :effect (and
    (not (update-status comp460))
    (update-status comp461)
    (not (all-on comp460))
    (probabilistic 1/20 (and (not (running comp460))))
  )
)
(:action update-status-comp460-one-off
  :parameters ()
  :precondition (and
    (update-status comp460)
    (not (rebooted comp460))
    (one-off comp460)
  )
  :effect (and
    (not (update-status comp460))
    (update-status comp461)
    (not (one-off comp460))
    (probabilistic 1/4 (and (not (running comp460))))
  )
)
(:action update-status-comp461-rebooted
  :parameters ()
  :precondition (and
    (update-status comp461)
    (rebooted comp461)
  )
  :effect (and
    (not (update-status comp461))
    (update-status comp462)
    (not (rebooted comp461))
    (probabilistic 9/10 (and (running comp461)) 1/10 (and))
  )
)
(:action update-status-comp461-all-on
  :parameters ()
  :precondition (and
    (update-status comp461)
    (not (rebooted comp461))
    (all-on comp461)
  )
  :effect (and
    (not (update-status comp461))
    (update-status comp462)
    (not (all-on comp461))
    (probabilistic 1/20 (and (not (running comp461))))
  )
)
(:action update-status-comp461-one-off
  :parameters ()
  :precondition (and
    (update-status comp461)
    (not (rebooted comp461))
    (one-off comp461)
  )
  :effect (and
    (not (update-status comp461))
    (update-status comp462)
    (not (one-off comp461))
    (probabilistic 1/4 (and (not (running comp461))))
  )
)
(:action update-status-comp462-rebooted
  :parameters ()
  :precondition (and
    (update-status comp462)
    (rebooted comp462)
  )
  :effect (and
    (not (update-status comp462))
    (update-status comp463)
    (not (rebooted comp462))
    (probabilistic 9/10 (and (running comp462)) 1/10 (and))
  )
)
(:action update-status-comp462-all-on
  :parameters ()
  :precondition (and
    (update-status comp462)
    (not (rebooted comp462))
    (all-on comp462)
  )
  :effect (and
    (not (update-status comp462))
    (update-status comp463)
    (not (all-on comp462))
    (probabilistic 1/20 (and (not (running comp462))))
  )
)
(:action update-status-comp462-one-off
  :parameters ()
  :precondition (and
    (update-status comp462)
    (not (rebooted comp462))
    (one-off comp462)
  )
  :effect (and
    (not (update-status comp462))
    (update-status comp463)
    (not (one-off comp462))
    (probabilistic 1/4 (and (not (running comp462))))
  )
)
(:action update-status-comp463-rebooted
  :parameters ()
  :precondition (and
    (update-status comp463)
    (rebooted comp463)
  )
  :effect (and
    (not (update-status comp463))
    (update-status comp464)
    (not (rebooted comp463))
    (probabilistic 9/10 (and (running comp463)) 1/10 (and))
  )
)
(:action update-status-comp463-all-on
  :parameters ()
  :precondition (and
    (update-status comp463)
    (not (rebooted comp463))
    (all-on comp463)
  )
  :effect (and
    (not (update-status comp463))
    (update-status comp464)
    (not (all-on comp463))
    (probabilistic 1/20 (and (not (running comp463))))
  )
)
(:action update-status-comp463-one-off
  :parameters ()
  :precondition (and
    (update-status comp463)
    (not (rebooted comp463))
    (one-off comp463)
  )
  :effect (and
    (not (update-status comp463))
    (update-status comp464)
    (not (one-off comp463))
    (probabilistic 1/4 (and (not (running comp463))))
  )
)
(:action update-status-comp464-rebooted
  :parameters ()
  :precondition (and
    (update-status comp464)
    (rebooted comp464)
  )
  :effect (and
    (not (update-status comp464))
    (update-status comp465)
    (not (rebooted comp464))
    (probabilistic 9/10 (and (running comp464)) 1/10 (and))
  )
)
(:action update-status-comp464-all-on
  :parameters ()
  :precondition (and
    (update-status comp464)
    (not (rebooted comp464))
    (all-on comp464)
  )
  :effect (and
    (not (update-status comp464))
    (update-status comp465)
    (not (all-on comp464))
    (probabilistic 1/20 (and (not (running comp464))))
  )
)
(:action update-status-comp464-one-off
  :parameters ()
  :precondition (and
    (update-status comp464)
    (not (rebooted comp464))
    (one-off comp464)
  )
  :effect (and
    (not (update-status comp464))
    (update-status comp465)
    (not (one-off comp464))
    (probabilistic 1/4 (and (not (running comp464))))
  )
)
(:action update-status-comp465-rebooted
  :parameters ()
  :precondition (and
    (update-status comp465)
    (rebooted comp465)
  )
  :effect (and
    (not (update-status comp465))
    (update-status comp466)
    (not (rebooted comp465))
    (probabilistic 9/10 (and (running comp465)) 1/10 (and))
  )
)
(:action update-status-comp465-all-on
  :parameters ()
  :precondition (and
    (update-status comp465)
    (not (rebooted comp465))
    (all-on comp465)
  )
  :effect (and
    (not (update-status comp465))
    (update-status comp466)
    (not (all-on comp465))
    (probabilistic 1/20 (and (not (running comp465))))
  )
)
(:action update-status-comp465-one-off
  :parameters ()
  :precondition (and
    (update-status comp465)
    (not (rebooted comp465))
    (one-off comp465)
  )
  :effect (and
    (not (update-status comp465))
    (update-status comp466)
    (not (one-off comp465))
    (probabilistic 1/4 (and (not (running comp465))))
  )
)
(:action update-status-comp466-rebooted
  :parameters ()
  :precondition (and
    (update-status comp466)
    (rebooted comp466)
  )
  :effect (and
    (not (update-status comp466))
    (update-status comp467)
    (not (rebooted comp466))
    (probabilistic 9/10 (and (running comp466)) 1/10 (and))
  )
)
(:action update-status-comp466-all-on
  :parameters ()
  :precondition (and
    (update-status comp466)
    (not (rebooted comp466))
    (all-on comp466)
  )
  :effect (and
    (not (update-status comp466))
    (update-status comp467)
    (not (all-on comp466))
    (probabilistic 1/20 (and (not (running comp466))))
  )
)
(:action update-status-comp466-one-off
  :parameters ()
  :precondition (and
    (update-status comp466)
    (not (rebooted comp466))
    (one-off comp466)
  )
  :effect (and
    (not (update-status comp466))
    (update-status comp467)
    (not (one-off comp466))
    (probabilistic 1/4 (and (not (running comp466))))
  )
)
(:action update-status-comp467-rebooted
  :parameters ()
  :precondition (and
    (update-status comp467)
    (rebooted comp467)
  )
  :effect (and
    (not (update-status comp467))
    (update-status comp468)
    (not (rebooted comp467))
    (probabilistic 9/10 (and (running comp467)) 1/10 (and))
  )
)
(:action update-status-comp467-all-on
  :parameters ()
  :precondition (and
    (update-status comp467)
    (not (rebooted comp467))
    (all-on comp467)
  )
  :effect (and
    (not (update-status comp467))
    (update-status comp468)
    (not (all-on comp467))
    (probabilistic 1/20 (and (not (running comp467))))
  )
)
(:action update-status-comp467-one-off
  :parameters ()
  :precondition (and
    (update-status comp467)
    (not (rebooted comp467))
    (one-off comp467)
  )
  :effect (and
    (not (update-status comp467))
    (update-status comp468)
    (not (one-off comp467))
    (probabilistic 1/4 (and (not (running comp467))))
  )
)
(:action update-status-comp468-rebooted
  :parameters ()
  :precondition (and
    (update-status comp468)
    (rebooted comp468)
  )
  :effect (and
    (not (update-status comp468))
    (update-status comp469)
    (not (rebooted comp468))
    (probabilistic 9/10 (and (running comp468)) 1/10 (and))
  )
)
(:action update-status-comp468-all-on
  :parameters ()
  :precondition (and
    (update-status comp468)
    (not (rebooted comp468))
    (all-on comp468)
  )
  :effect (and
    (not (update-status comp468))
    (update-status comp469)
    (not (all-on comp468))
    (probabilistic 1/20 (and (not (running comp468))))
  )
)
(:action update-status-comp468-one-off
  :parameters ()
  :precondition (and
    (update-status comp468)
    (not (rebooted comp468))
    (one-off comp468)
  )
  :effect (and
    (not (update-status comp468))
    (update-status comp469)
    (not (one-off comp468))
    (probabilistic 1/4 (and (not (running comp468))))
  )
)
(:action update-status-comp469-rebooted
  :parameters ()
  :precondition (and
    (update-status comp469)
    (rebooted comp469)
  )
  :effect (and
    (not (update-status comp469))
    (update-status comp470)
    (not (rebooted comp469))
    (probabilistic 9/10 (and (running comp469)) 1/10 (and))
  )
)
(:action update-status-comp469-all-on
  :parameters ()
  :precondition (and
    (update-status comp469)
    (not (rebooted comp469))
    (all-on comp469)
  )
  :effect (and
    (not (update-status comp469))
    (update-status comp470)
    (not (all-on comp469))
    (probabilistic 1/20 (and (not (running comp469))))
  )
)
(:action update-status-comp469-one-off
  :parameters ()
  :precondition (and
    (update-status comp469)
    (not (rebooted comp469))
    (one-off comp469)
  )
  :effect (and
    (not (update-status comp469))
    (update-status comp470)
    (not (one-off comp469))
    (probabilistic 1/4 (and (not (running comp469))))
  )
)
(:action update-status-comp470-rebooted
  :parameters ()
  :precondition (and
    (update-status comp470)
    (rebooted comp470)
  )
  :effect (and
    (not (update-status comp470))
    (update-status comp471)
    (not (rebooted comp470))
    (probabilistic 9/10 (and (running comp470)) 1/10 (and))
  )
)
(:action update-status-comp470-all-on
  :parameters ()
  :precondition (and
    (update-status comp470)
    (not (rebooted comp470))
    (all-on comp470)
  )
  :effect (and
    (not (update-status comp470))
    (update-status comp471)
    (not (all-on comp470))
    (probabilistic 1/20 (and (not (running comp470))))
  )
)
(:action update-status-comp470-one-off
  :parameters ()
  :precondition (and
    (update-status comp470)
    (not (rebooted comp470))
    (one-off comp470)
  )
  :effect (and
    (not (update-status comp470))
    (update-status comp471)
    (not (one-off comp470))
    (probabilistic 1/4 (and (not (running comp470))))
  )
)
(:action update-status-comp471-rebooted
  :parameters ()
  :precondition (and
    (update-status comp471)
    (rebooted comp471)
  )
  :effect (and
    (not (update-status comp471))
    (update-status comp472)
    (not (rebooted comp471))
    (probabilistic 9/10 (and (running comp471)) 1/10 (and))
  )
)
(:action update-status-comp471-all-on
  :parameters ()
  :precondition (and
    (update-status comp471)
    (not (rebooted comp471))
    (all-on comp471)
  )
  :effect (and
    (not (update-status comp471))
    (update-status comp472)
    (not (all-on comp471))
    (probabilistic 1/20 (and (not (running comp471))))
  )
)
(:action update-status-comp471-one-off
  :parameters ()
  :precondition (and
    (update-status comp471)
    (not (rebooted comp471))
    (one-off comp471)
  )
  :effect (and
    (not (update-status comp471))
    (update-status comp472)
    (not (one-off comp471))
    (probabilistic 1/4 (and (not (running comp471))))
  )
)
(:action update-status-comp472-rebooted
  :parameters ()
  :precondition (and
    (update-status comp472)
    (rebooted comp472)
  )
  :effect (and
    (not (update-status comp472))
    (update-status comp473)
    (not (rebooted comp472))
    (probabilistic 9/10 (and (running comp472)) 1/10 (and))
  )
)
(:action update-status-comp472-all-on
  :parameters ()
  :precondition (and
    (update-status comp472)
    (not (rebooted comp472))
    (all-on comp472)
  )
  :effect (and
    (not (update-status comp472))
    (update-status comp473)
    (not (all-on comp472))
    (probabilistic 1/20 (and (not (running comp472))))
  )
)
(:action update-status-comp472-one-off
  :parameters ()
  :precondition (and
    (update-status comp472)
    (not (rebooted comp472))
    (one-off comp472)
  )
  :effect (and
    (not (update-status comp472))
    (update-status comp473)
    (not (one-off comp472))
    (probabilistic 1/4 (and (not (running comp472))))
  )
)
(:action update-status-comp473-rebooted
  :parameters ()
  :precondition (and
    (update-status comp473)
    (rebooted comp473)
  )
  :effect (and
    (not (update-status comp473))
    (update-status comp474)
    (not (rebooted comp473))
    (probabilistic 9/10 (and (running comp473)) 1/10 (and))
  )
)
(:action update-status-comp473-all-on
  :parameters ()
  :precondition (and
    (update-status comp473)
    (not (rebooted comp473))
    (all-on comp473)
  )
  :effect (and
    (not (update-status comp473))
    (update-status comp474)
    (not (all-on comp473))
    (probabilistic 1/20 (and (not (running comp473))))
  )
)
(:action update-status-comp473-one-off
  :parameters ()
  :precondition (and
    (update-status comp473)
    (not (rebooted comp473))
    (one-off comp473)
  )
  :effect (and
    (not (update-status comp473))
    (update-status comp474)
    (not (one-off comp473))
    (probabilistic 1/4 (and (not (running comp473))))
  )
)
(:action update-status-comp474-rebooted
  :parameters ()
  :precondition (and
    (update-status comp474)
    (rebooted comp474)
  )
  :effect (and
    (not (update-status comp474))
    (update-status comp475)
    (not (rebooted comp474))
    (probabilistic 9/10 (and (running comp474)) 1/10 (and))
  )
)
(:action update-status-comp474-all-on
  :parameters ()
  :precondition (and
    (update-status comp474)
    (not (rebooted comp474))
    (all-on comp474)
  )
  :effect (and
    (not (update-status comp474))
    (update-status comp475)
    (not (all-on comp474))
    (probabilistic 1/20 (and (not (running comp474))))
  )
)
(:action update-status-comp474-one-off
  :parameters ()
  :precondition (and
    (update-status comp474)
    (not (rebooted comp474))
    (one-off comp474)
  )
  :effect (and
    (not (update-status comp474))
    (update-status comp475)
    (not (one-off comp474))
    (probabilistic 1/4 (and (not (running comp474))))
  )
)
(:action update-status-comp475-rebooted
  :parameters ()
  :precondition (and
    (update-status comp475)
    (rebooted comp475)
  )
  :effect (and
    (not (update-status comp475))
    (update-status comp476)
    (not (rebooted comp475))
    (probabilistic 9/10 (and (running comp475)) 1/10 (and))
  )
)
(:action update-status-comp475-all-on
  :parameters ()
  :precondition (and
    (update-status comp475)
    (not (rebooted comp475))
    (all-on comp475)
  )
  :effect (and
    (not (update-status comp475))
    (update-status comp476)
    (not (all-on comp475))
    (probabilistic 1/20 (and (not (running comp475))))
  )
)
(:action update-status-comp475-one-off
  :parameters ()
  :precondition (and
    (update-status comp475)
    (not (rebooted comp475))
    (one-off comp475)
  )
  :effect (and
    (not (update-status comp475))
    (update-status comp476)
    (not (one-off comp475))
    (probabilistic 1/4 (and (not (running comp475))))
  )
)
(:action update-status-comp476-rebooted
  :parameters ()
  :precondition (and
    (update-status comp476)
    (rebooted comp476)
  )
  :effect (and
    (not (update-status comp476))
    (update-status comp477)
    (not (rebooted comp476))
    (probabilistic 9/10 (and (running comp476)) 1/10 (and))
  )
)
(:action update-status-comp476-all-on
  :parameters ()
  :precondition (and
    (update-status comp476)
    (not (rebooted comp476))
    (all-on comp476)
  )
  :effect (and
    (not (update-status comp476))
    (update-status comp477)
    (not (all-on comp476))
    (probabilistic 1/20 (and (not (running comp476))))
  )
)
(:action update-status-comp476-one-off
  :parameters ()
  :precondition (and
    (update-status comp476)
    (not (rebooted comp476))
    (one-off comp476)
  )
  :effect (and
    (not (update-status comp476))
    (update-status comp477)
    (not (one-off comp476))
    (probabilistic 1/4 (and (not (running comp476))))
  )
)
(:action update-status-comp477-rebooted
  :parameters ()
  :precondition (and
    (update-status comp477)
    (rebooted comp477)
  )
  :effect (and
    (not (update-status comp477))
    (update-status comp478)
    (not (rebooted comp477))
    (probabilistic 9/10 (and (running comp477)) 1/10 (and))
  )
)
(:action update-status-comp477-all-on
  :parameters ()
  :precondition (and
    (update-status comp477)
    (not (rebooted comp477))
    (all-on comp477)
  )
  :effect (and
    (not (update-status comp477))
    (update-status comp478)
    (not (all-on comp477))
    (probabilistic 1/20 (and (not (running comp477))))
  )
)
(:action update-status-comp477-one-off
  :parameters ()
  :precondition (and
    (update-status comp477)
    (not (rebooted comp477))
    (one-off comp477)
  )
  :effect (and
    (not (update-status comp477))
    (update-status comp478)
    (not (one-off comp477))
    (probabilistic 1/4 (and (not (running comp477))))
  )
)
(:action update-status-comp478-rebooted
  :parameters ()
  :precondition (and
    (update-status comp478)
    (rebooted comp478)
  )
  :effect (and
    (not (update-status comp478))
    (update-status comp479)
    (not (rebooted comp478))
    (probabilistic 9/10 (and (running comp478)) 1/10 (and))
  )
)
(:action update-status-comp478-all-on
  :parameters ()
  :precondition (and
    (update-status comp478)
    (not (rebooted comp478))
    (all-on comp478)
  )
  :effect (and
    (not (update-status comp478))
    (update-status comp479)
    (not (all-on comp478))
    (probabilistic 1/20 (and (not (running comp478))))
  )
)
(:action update-status-comp478-one-off
  :parameters ()
  :precondition (and
    (update-status comp478)
    (not (rebooted comp478))
    (one-off comp478)
  )
  :effect (and
    (not (update-status comp478))
    (update-status comp479)
    (not (one-off comp478))
    (probabilistic 1/4 (and (not (running comp478))))
  )
)
(:action update-status-comp479-rebooted
  :parameters ()
  :precondition (and
    (update-status comp479)
    (rebooted comp479)
  )
  :effect (and
    (not (update-status comp479))
    (all-updated)
    (not (rebooted comp479))
    (probabilistic 9/10 (and (running comp479)) 1/10 (and))
  )
)
(:action update-status-comp479-all-on
  :parameters ()
  :precondition (and
    (update-status comp479)
    (not (rebooted comp479))
    (all-on comp479)
  )
  :effect (and
    (not (update-status comp479))
    (all-updated)
    (not (all-on comp479))
    (probabilistic 1/20 (and (not (running comp479))))
  )
)
(:action update-status-comp479-one-off
  :parameters ()
  :precondition (and
    (update-status comp479)
    (not (rebooted comp479))
    (one-off comp479)
  )
  :effect (and
    (not (update-status comp479))
    (all-updated)
    (not (one-off comp479))
    (probabilistic 1/4 (and (not (running comp479))))
  )
)
)