(define (problem tpp-m4-g4-s359314)
(:domain dom--tpp-m4-g4-s359314)
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 - horizon-value
		truck0 - truck
	goods0 goods1 goods2 goods3 - goods
)
(:init (horizon horzn22) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21)
	(= (total-cost) 0)
	(road_isunknown road0)
	(road_isunknown road1)
	(road_isunknown road2)
	(road_isunknown road3)
	(road_isunknown road4)
	(on-sale goods0 market3)
	(= (price goods0 market3 ) 19)
	(on-sale goods1 market2)
	(= (price goods1 market2 ) 1)
	(on-sale goods1 market4)
	(= (price goods1 market4 ) 23)
	(on-sale goods2 market1)
	(= (price goods2 market1 ) 7)
	(on-sale goods2 market2)
	(= (price goods2 market2 ) 20)
	(on-sale goods2 market3)
	(= (price goods2 market3 ) 11)
	(on-sale goods3 market1)
	(= (price goods3 market1 ) 23)
	(on-sale goods3 market2)
	(= (price goods3 market2 ) 11)
	(on-sale goods3 market3)
	(= (price goods3 market3 ) 12)
	(at truck0 depot0)
)
(:goal (and
	(stored goods0)
	(stored goods1)
	(stored goods2)
	(stored goods3)
))
(:metric minimize (total-cost))
)
