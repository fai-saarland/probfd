(define (problem ex_bw_16_p14)
  (:domain exploding-blocksworld)
  (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 horzn25 horzn26 horzn27 horzn28 horzn29 horzn30 horzn31 horzn32 horzn33 horzn34 horzn35 horzn36 horzn37 horzn38 horzn39 horzn40 horzn41 horzn42 horzn43 horzn44 horzn45 horzn46 horzn47 horzn48 - horizon-value b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 - block)
  (:init (horizon horzn48) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (horizon-decrement horzn23 horzn22) (horizon-decrement horzn24 horzn23) (horizon-decrement horzn25 horzn24) (horizon-decrement horzn26 horzn25) (horizon-decrement horzn27 horzn26) (horizon-decrement horzn28 horzn27) (horizon-decrement horzn29 horzn28) (horizon-decrement horzn30 horzn29) (horizon-decrement horzn31 horzn30) (horizon-decrement horzn32 horzn31) (horizon-decrement horzn33 horzn32) (horizon-decrement horzn34 horzn33) (horizon-decrement horzn35 horzn34) (horizon-decrement horzn36 horzn35) (horizon-decrement horzn37 horzn36) (horizon-decrement horzn38 horzn37) (horizon-decrement horzn39 horzn38) (horizon-decrement horzn40 horzn39) (horizon-decrement horzn41 horzn40) (horizon-decrement horzn42 horzn41) (horizon-decrement horzn43 horzn42) (horizon-decrement horzn44 horzn43) (horizon-decrement horzn45 horzn44) (horizon-decrement horzn46 horzn45) (horizon-decrement horzn47 horzn46) (horizon-decrement horzn48 horzn47) (emptyhand) (on-table b1) (on-table b2) (on b3 b10) (on b4 b9) (on b5 b11) (on-table b6) (on b7 b1) (on-table b8) (on-table b9) (on b10 b5) (on b11 b8) (on b12 b13) (on b13 b16) (on b14 b4) (on b15 b7) (on b16 b3) (clear b2) (clear b6) (clear b12) (clear b14) (clear b15) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-detonated b5) (no-destroyed b5) (no-detonated b6) (no-destroyed b6) (no-detonated b7) (no-destroyed b7) (no-detonated b8) (no-destroyed b8) (no-detonated b9) (no-destroyed b9) (no-detonated b10) (no-destroyed b10) (no-detonated b11) (no-destroyed b11) (no-detonated b12) (no-destroyed b12) (no-detonated b13) (no-destroyed b13) (no-detonated b14) (no-destroyed b14) (no-detonated b15) (no-destroyed b15) (no-detonated b16) (no-destroyed b16) (no-destroyed-table))
  (:goal (and (on b1 b10) (on b2 b5) (on b3 b7) (on b4 b3) (on-table b5) (on-table b6) (on b8 b14) (on b9 b12) (on-table b10) (on b11 b4) (on b13 b11) (on b14 b13) (on b15 b6) (on-table b16)  )
)(:metric minimize (total-cost))
)
