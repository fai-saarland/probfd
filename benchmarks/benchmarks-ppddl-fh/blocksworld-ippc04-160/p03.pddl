(define (problem bw_5_30495)
  (:domain blocks-domain)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 - horizon-value b1 b2 b3 b4 b5 - block)
  (:init (horizon horzn14) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (emptyhand) (on-table b1) (on b2 b1) (on b3 b2) (on-table b4) (on-table b5) (clear b3) (clear b4) (clear b5))
  (:goal (and (emptyhand) (on b1 b4) (on b2 b5) (on-table b3) (on b4 b2) (on b5 b3) (clear b1)))
  (:metric minimize (total-cost))
)
