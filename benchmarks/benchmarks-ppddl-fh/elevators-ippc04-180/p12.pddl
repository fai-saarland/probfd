(define (problem elev_3_12_3_9_29838)
  (:domain elevators)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 horzn25 horzn26 horzn27 horzn28 horzn29 horzn30 horzn31 horzn32 horzn33 horzn34 horzn35 horzn36 horzn37 horzn38 horzn39 horzn40 horzn41 horzn42 horzn43 horzn44 horzn45 horzn46 horzn47 horzn48 horzn49 horzn50 horzn51 horzn52 horzn53 horzn54 horzn55 horzn56 - horizon-value f2 f3 - floor p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 - pos e1 e2 e3 - elevator c1 c2 c3 c4 c5 c6 c7 c8 c9 - coin)
  (:init (horizon horzn56) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (horizon-decrement horzn23 horzn22) (horizon-decrement horzn24 horzn23) (horizon-decrement horzn25 horzn24) (horizon-decrement horzn26 horzn25) (horizon-decrement horzn27 horzn26) (horizon-decrement horzn28 horzn27) (horizon-decrement horzn29 horzn28) (horizon-decrement horzn30 horzn29) (horizon-decrement horzn31 horzn30) (horizon-decrement horzn32 horzn31) (horizon-decrement horzn33 horzn32) (horizon-decrement horzn34 horzn33) (horizon-decrement horzn35 horzn34) (horizon-decrement horzn36 horzn35) (horizon-decrement horzn37 horzn36) (horizon-decrement horzn38 horzn37) (horizon-decrement horzn39 horzn38) (horizon-decrement horzn40 horzn39) (horizon-decrement horzn41 horzn40) (horizon-decrement horzn42 horzn41) (horizon-decrement horzn43 horzn42) (horizon-decrement horzn44 horzn43) (horizon-decrement horzn45 horzn44) (horizon-decrement horzn46 horzn45) (horizon-decrement horzn47 horzn46) (horizon-decrement horzn48 horzn47) (horizon-decrement horzn49 horzn48) (horizon-decrement horzn50 horzn49) (horizon-decrement horzn51 horzn50) (horizon-decrement horzn52 horzn51) (horizon-decrement horzn53 horzn52) (horizon-decrement horzn54 horzn53) (horizon-decrement horzn55 horzn54) (horizon-decrement horzn56 horzn55) (at f1 p1) (dec_f f2 f1) (dec_f f3 f2) (dec_p p2 p1) (dec_p p3 p2) (dec_p p4 p3) (dec_p p5 p4) (dec_p p6 p5) (dec_p p7 p6) (dec_p p8 p7) (dec_p p9 p8) (dec_p p10 p9) (dec_p p11 p10) (dec_p p12 p11) (shaft e1 p2) (in e1 f3) (shaft e2 p2) (in e2 f3) (shaft e3 p8) (in e3 f2) (coin-at c1 f2 p10) (coin-at c2 f2 p4) (coin-at c3 f2 p3) (coin-at c4 f3 p4) (coin-at c5 f3 p2) (coin-at c6 f2 p9) (coin-at c7 f1 p1) (coin-at c8 f1 p1) (coin-at c9 f2 p9) (gate f2 p8) (gate f3 p6) (gate f3 p9))
  (:goal (and (have c1) (have c2) (have c3) (have c4) (have c5) (have c6) (have c7) (have c8) (have c9)))
  (:metric minimize (total-cost))
)
