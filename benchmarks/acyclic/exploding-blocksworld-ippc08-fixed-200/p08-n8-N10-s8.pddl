(define (problem ex_bw_10_p08)
  (:domain exploding-blocksworld)
  (:objects horzn0 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 horzn25 horzn26 horzn27 horzn28 horzn29 horzn30 horzn31 horzn32 horzn33 horzn34 horzn35 horzn36 horzn37 horzn38 horzn39 horzn40 horzn41 horzn42 horzn43 horzn44 horzn45 horzn46 horzn47 horzn48 - horizon-value b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 - block)
  (:init (horizon horzn48) (horizon-decrement horzn1 horzn1 horzn0) (horizon-decrement horzn2 horzn1 horzn1) (horizon-decrement horzn3 horzn1 horzn2) (horizon-decrement horzn4 horzn1 horzn3) (horizon-decrement horzn5 horzn1 horzn4) (horizon-decrement horzn6 horzn1 horzn5) (horizon-decrement horzn7 horzn1 horzn6) (horizon-decrement horzn8 horzn1 horzn7) (horizon-decrement horzn9 horzn1 horzn8) (horizon-decrement horzn10 horzn1 horzn9) (horizon-decrement horzn11 horzn1 horzn10) (horizon-decrement horzn12 horzn1 horzn11) (horizon-decrement horzn13 horzn1 horzn12) (horizon-decrement horzn14 horzn1 horzn13) (horizon-decrement horzn15 horzn1 horzn14) (horizon-decrement horzn16 horzn1 horzn15) (horizon-decrement horzn17 horzn1 horzn16) (horizon-decrement horzn18 horzn1 horzn17) (horizon-decrement horzn19 horzn1 horzn18) (horizon-decrement horzn20 horzn1 horzn19) (horizon-decrement horzn21 horzn1 horzn20) (horizon-decrement horzn22 horzn1 horzn21) (horizon-decrement horzn23 horzn1 horzn22) (horizon-decrement horzn24 horzn1 horzn23) (horizon-decrement horzn25 horzn1 horzn24) (horizon-decrement horzn26 horzn1 horzn25) (horizon-decrement horzn27 horzn1 horzn26) (horizon-decrement horzn28 horzn1 horzn27) (horizon-decrement horzn29 horzn1 horzn28) (horizon-decrement horzn30 horzn1 horzn29) (horizon-decrement horzn31 horzn1 horzn30) (horizon-decrement horzn32 horzn1 horzn31) (horizon-decrement horzn33 horzn1 horzn32) (horizon-decrement horzn34 horzn1 horzn33) (horizon-decrement horzn35 horzn1 horzn34) (horizon-decrement horzn36 horzn1 horzn35) (horizon-decrement horzn37 horzn1 horzn36) (horizon-decrement horzn38 horzn1 horzn37) (horizon-decrement horzn39 horzn1 horzn38) (horizon-decrement horzn40 horzn1 horzn39) (horizon-decrement horzn41 horzn1 horzn40) (horizon-decrement horzn42 horzn1 horzn41) (horizon-decrement horzn43 horzn1 horzn42) (horizon-decrement horzn44 horzn1 horzn43) (horizon-decrement horzn45 horzn1 horzn44) (horizon-decrement horzn46 horzn1 horzn45) (horizon-decrement horzn47 horzn1 horzn46) (horizon-decrement horzn48 horzn1 horzn47) (emptyhand) (on b1 b8) (on b2 b4) (on b3 b10) (on b4 b5) (on b5 b6) (on b6 b9) (on b7 b2) (on-table b8) (on b9 b1) (on-table b10) (clear b3) (clear b7) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-detonated b5) (no-destroyed b5) (no-detonated b6) (no-destroyed b6) (no-detonated b7) (no-destroyed b7) (no-detonated b8) (no-destroyed b8) (no-detonated b9) (no-destroyed b9) (no-detonated b10) (no-destroyed b10) (no-destroyed-table))
  (:goal (and (on b1 b6) (on b3 b1) (on b5 b8) (on b6 b4) (on b7 b3) (on b8 b7) (on-table b9) (on b10 b5)  )
)(:metric minimize (total-cost))
)
