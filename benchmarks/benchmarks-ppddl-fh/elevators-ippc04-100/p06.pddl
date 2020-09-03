(define (problem elev_3_8_2_6_17938)
  (:domain elevators)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 - horizon-value f2 f3 - floor p2 p3 p4 p5 p6 p7 p8 - pos e1 e2 - elevator c1 c2 c3 c4 c5 c6 - coin)
  (:init (horizon horzn20) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (at f1 p1) (dec_f f2 f1) (dec_f f3 f2) (dec_p p2 p1) (dec_p p3 p2) (dec_p p4 p3) (dec_p p5 p4) (dec_p p6 p5) (dec_p p7 p6) (dec_p p8 p7) (shaft e1 p4) (in e1 f2) (shaft e2 p8) (in e2 f2) (coin-at c1 f1 p4) (coin-at c2 f1 p8) (coin-at c3 f2 p6) (coin-at c4 f2 p5) (coin-at c5 f1 p6) (coin-at c6 f1 p8) (gate f2 p2) (gate f2 p3) (gate f2 p8) (gate f3 p2) (gate f3 p4))
  (:goal (and (have c1) (have c2) (have c3) (have c4) (have c5) (have c6)))
  (:metric minimize (total-cost))
)
