(define (problem triangle-tire-5)
                   (:domain triangle-tire)
                   (:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 - horizon-value l-1-1 l-1-2 l-1-3 l-1-4 l-1-5 l-1-6 l-1-7 l-1-8 l-1-9 l-1-10 l-1-11 l-2-1 l-2-2 l-2-3 l-2-4 l-2-5 l-2-6 l-2-7 l-2-8 l-2-9 l-2-10 l-2-11 l-3-1 l-3-2 l-3-3 l-3-4 l-3-5 l-3-6 l-3-7 l-3-8 l-3-9 l-3-10 l-3-11 l-4-1 l-4-2 l-4-3 l-4-4 l-4-5 l-4-6 l-4-7 l-4-8 l-4-9 l-4-10 l-4-11 l-5-1 l-5-2 l-5-3 l-5-4 l-5-5 l-5-6 l-5-7 l-5-8 l-5-9 l-5-10 l-5-11 l-6-1 l-6-2 l-6-3 l-6-4 l-6-5 l-6-6 l-6-7 l-6-8 l-6-9 l-6-10 l-6-11 l-7-1 l-7-2 l-7-3 l-7-4 l-7-5 l-7-6 l-7-7 l-7-8 l-7-9 l-7-10 l-7-11 l-8-1 l-8-2 l-8-3 l-8-4 l-8-5 l-8-6 l-8-7 l-8-8 l-8-9 l-8-10 l-8-11 l-9-1 l-9-2 l-9-3 l-9-4 l-9-5 l-9-6 l-9-7 l-9-8 l-9-9 l-9-10 l-9-11 l-10-1 l-10-2 l-10-3 l-10-4 l-10-5 l-10-6 l-10-7 l-10-8 l-10-9 l-10-10 l-10-11 l-11-1 l-11-2 l-11-3 l-11-4 l-11-5 l-11-6 l-11-7 l-11-8 l-11-9 l-11-10 l-11-11 - location)
                   (:init (horizon horzn16) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (vehicle-at l-1-1)(road l-1-1 l-1-2)(road l-1-2 l-1-3)(road l-1-3 l-1-4)(road l-1-4 l-1-5)(road l-1-5 l-1-6)(road l-1-6 l-1-7)(road l-1-7 l-1-8)(road l-1-8 l-1-9)(road l-1-9 l-1-10)(road l-1-10 l-1-11)(road l-1-1 l-2-1)(road l-1-2 l-2-2)(road l-1-3 l-2-3)(road l-1-4 l-2-4)(road l-1-5 l-2-5)(road l-1-6 l-2-6)(road l-1-7 l-2-7)(road l-1-8 l-2-8)(road l-1-9 l-2-9)(road l-1-10 l-2-10)(road l-2-1 l-1-2)(road l-2-2 l-1-3)(road l-2-3 l-1-4)(road l-2-4 l-1-5)(road l-2-5 l-1-6)(road l-2-6 l-1-7)(road l-2-7 l-1-8)(road l-2-8 l-1-9)(road l-2-9 l-1-10)(road l-2-10 l-1-11)(spare-in l-2-1)(spare-in l-2-2)(spare-in l-2-3)(spare-in l-2-4)(spare-in l-2-5)(spare-in l-2-6)(spare-in l-2-7)(spare-in l-2-8)(spare-in l-2-9)(spare-in l-2-10)(road l-3-1 l-3-2)(road l-3-2 l-3-3)(road l-3-3 l-3-4)(road l-3-4 l-3-5)(road l-3-5 l-3-6)(road l-3-6 l-3-7)(road l-3-7 l-3-8)(road l-3-8 l-3-9)(road l-2-1 l-3-1)(road l-2-3 l-3-3)(road l-2-5 l-3-5)(road l-2-7 l-3-7)(road l-2-9 l-3-9)(road l-3-1 l-2-2)(road l-3-3 l-2-4)(road l-3-5 l-2-6)(road l-3-7 l-2-8)(road l-3-9 l-2-10)(spare-in l-3-1)(spare-in l-3-9)(road l-3-1 l-4-1)(road l-3-2 l-4-2)(road l-3-3 l-4-3)(road l-3-4 l-4-4)(road l-3-5 l-4-5)(road l-3-6 l-4-6)(road l-3-7 l-4-7)(road l-3-8 l-4-8)(road l-4-1 l-3-2)(road l-4-2 l-3-3)(road l-4-3 l-3-4)(road l-4-4 l-3-5)(road l-4-5 l-3-6)(road l-4-6 l-3-7)(road l-4-7 l-3-8)(road l-4-8 l-3-9)(spare-in l-4-1)(spare-in l-4-2)(spare-in l-4-3)(spare-in l-4-4)(spare-in l-4-5)(spare-in l-4-6)(spare-in l-4-7)(spare-in l-4-8)(road l-5-1 l-5-2)(road l-5-2 l-5-3)(road l-5-3 l-5-4)(road l-5-4 l-5-5)(road l-5-5 l-5-6)(road l-5-6 l-5-7)(road l-4-1 l-5-1)(road l-4-3 l-5-3)(road l-4-5 l-5-5)(road l-4-7 l-5-7)(road l-5-1 l-4-2)(road l-5-3 l-4-4)(road l-5-5 l-4-6)(road l-5-7 l-4-8)(spare-in l-5-1)(spare-in l-5-7)(road l-5-1 l-6-1)(road l-5-2 l-6-2)(road l-5-3 l-6-3)(road l-5-4 l-6-4)(road l-5-5 l-6-5)(road l-5-6 l-6-6)(road l-6-1 l-5-2)(road l-6-2 l-5-3)(road l-6-3 l-5-4)(road l-6-4 l-5-5)(road l-6-5 l-5-6)(road l-6-6 l-5-7)(spare-in l-6-1)(spare-in l-6-2)(spare-in l-6-3)(spare-in l-6-4)(spare-in l-6-5)(spare-in l-6-6)(road l-7-1 l-7-2)(road l-7-2 l-7-3)(road l-7-3 l-7-4)(road l-7-4 l-7-5)(road l-6-1 l-7-1)(road l-6-3 l-7-3)(road l-6-5 l-7-5)(road l-7-1 l-6-2)(road l-7-3 l-6-4)(road l-7-5 l-6-6)(spare-in l-7-1)(spare-in l-7-5)(road l-7-1 l-8-1)(road l-7-2 l-8-2)(road l-7-3 l-8-3)(road l-7-4 l-8-4)(road l-8-1 l-7-2)(road l-8-2 l-7-3)(road l-8-3 l-7-4)(road l-8-4 l-7-5)(spare-in l-8-1)(spare-in l-8-2)(spare-in l-8-3)(spare-in l-8-4)(road l-9-1 l-9-2)(road l-9-2 l-9-3)(road l-8-1 l-9-1)(road l-8-3 l-9-3)(road l-9-1 l-8-2)(road l-9-3 l-8-4)(spare-in l-9-1)(spare-in l-9-3)(road l-9-1 l-10-1)(road l-9-2 l-10-2)(road l-10-1 l-9-2)(road l-10-2 l-9-3)(spare-in l-10-1)(spare-in l-10-2)(road l-10-1 l-11-1)(road l-11-1 l-10-2)(spare-in l-11-1)(spare-in l-11-1)(not-flattire))
                   (:goal (vehicle-at l-1-11))(:metric minimize (total-cost)))

