(define (problem ex_bw_8_p06)
  (:domain exploding-blocksworld)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 - horizon-value b1 b2 b3 b4 b5 b6 b7 b8 - block)
  (:init (horizon horzn14) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (emptyhand) (on-table b1) (on b2 b3) (on-table b3) (on b4 b8) (on b5 b7) (on b6 b2) (on b7 b6) (on b8 b1) (clear b4) (clear b5) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-detonated b5) (no-destroyed b5) (no-detonated b6) (no-destroyed b6) (no-detonated b7) (no-destroyed b7) (no-detonated b8) (no-destroyed b8) (no-destroyed-table))
  (:goal (and (on-table b1) (on-table b3) (on-table b4) (on b5 b2) (on b6 b4) (on b7 b1)  )
)(:metric minimize (total-cost))
)
