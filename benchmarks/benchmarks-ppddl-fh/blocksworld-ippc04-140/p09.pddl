(define (problem bw_10_17042)
  (:domain blocks-domain)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 - horizon-value b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block)
  (:init (horizon horzn20) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (emptyhand) (on b1 b10) (on b2 b8) (on b3 b9) (on-table b4) (on b5 b7) (on-table b6) (on-table b7) (on b8 b4) (on b9 b1) (on b10 b2) (clear b3) (clear b5) (clear b6))
  (:goal (and (emptyhand) (on b1 b4) (on b2 b1) (on-table b3) (on b4 b6) (on b5 b2) (on b6 b9) (on-table b7) (on-table b8) (on-table b9) (on-table b10) (clear b3) (clear b5) (clear b7) (clear b8) (clear b10)))
  (:metric minimize (total-cost))
)
