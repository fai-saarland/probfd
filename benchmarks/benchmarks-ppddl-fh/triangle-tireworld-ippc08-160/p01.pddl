(define (problem triangle-tire-1)
                   (:domain triangle-tire)
                   (:objects horzn0 horzn1 horzn2 horzn3 - horizon-value l-1-1 l-1-2 l-1-3 l-2-1 l-2-2 l-2-3 l-3-1 l-3-2 l-3-3 - location)
                   (:init (horizon horzn3) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (vehicle-at l-1-1)(road l-1-1 l-1-2)(road l-1-2 l-1-3)(road l-1-1 l-2-1)(road l-1-2 l-2-2)(road l-2-1 l-1-2)(road l-2-2 l-1-3)(spare-in l-2-1)(spare-in l-2-2)(road l-2-1 l-3-1)(road l-3-1 l-2-2)(spare-in l-3-1)(spare-in l-3-1)(not-flattire))
                   (:goal (vehicle-at l-1-3))(:metric minimize (total-cost)))

