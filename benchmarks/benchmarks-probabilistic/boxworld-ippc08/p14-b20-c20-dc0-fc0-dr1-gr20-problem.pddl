;; Generated by boxworld generator
;; http://www.cs.rutgers.edu/~jasmuth/boxworld.tar.gz
;; by John Asmuth (jasmuth@cs.rutgers.edu)

(define
 (problem box-p14)
  (:domain boxworld)
  (:objects box0 - box
            box1 - box
            box2 - box
            box3 - box
            box4 - box
            box5 - box
            box6 - box
            box7 - box
            box8 - box
            box9 - box
            box10 - box
            box11 - box
            box12 - box
            box13 - box
            box14 - box
            box15 - box
            box16 - box
            box17 - box
            box18 - box
            box19 - box
            truck0 - truck
            truck1 - truck
            plane0 - plane
            truck2 - truck
            truck3 - truck
            plane1 - plane
            city0 - city
            city1 - city
            city2 - city
            city3 - city
            city4 - city
            city5 - city
            city6 - city
            city7 - city
            city8 - city
            city9 - city
            city10 - city
            city11 - city
            city12 - city
            city13 - city
            city14 - city
            city15 - city
            city16 - city
            city17 - city
            city18 - city
            city19 - city
  )
  (:init (= (total-cost) 0) (box-at-city box0 city5)
         (destination box0 city13)
         (box-at-city box1 city6)
         (destination box1 city1)
         (box-at-city box2 city13)
         (destination box2 city12)
         (box-at-city box3 city16)
         (destination box3 city10)
         (box-at-city box4 city18)
         (destination box4 city12)
         (box-at-city box5 city7)
         (destination box5 city17)
         (box-at-city box6 city1)
         (destination box6 city7)
         (box-at-city box7 city18)
         (destination box7 city8)
         (box-at-city box8 city10)
         (destination box8 city15)
         (box-at-city box9 city9)
         (destination box9 city0)
         (box-at-city box10 city5)
         (destination box10 city1)
         (box-at-city box11 city6)
         (destination box11 city11)
         (box-at-city box12 city9)
         (destination box12 city16)
         (box-at-city box13 city4)
         (destination box13 city1)
         (box-at-city box14 city8)
         (destination box14 city11)
         (box-at-city box15 city19)
         (destination box15 city13)
         (box-at-city box16 city9)
         (destination box16 city12)
         (box-at-city box17 city6)
         (destination box17 city2)
         (box-at-city box18 city5)
         (destination box18 city15)
         (box-at-city box19 city12)
         (destination box19 city15)
         (truck-at-city truck0 city0)
         (truck-at-city truck1 city0)
         (plane-at-city plane0 city0)
         (truck-at-city truck2 city1)
         (truck-at-city truck3 city1)
         (plane-at-city plane1 city1)
         (can-drive city0 city14)
         (can-drive city0 city19)
         (can-drive city0 city17)
         (can-drive city0 city5)
         (wrong-drive1 city0 city14)
         (wrong-drive2 city0 city19)
         (wrong-drive3 city0 city17)
         (can-fly city0 city1)
         (can-drive city1 city3)
         (can-drive city1 city10)
         (can-drive city1 city19)
         (wrong-drive1 city1 city3)
         (wrong-drive2 city1 city10)
         (wrong-drive3 city1 city19)
         (can-fly city1 city0)
         (can-drive city2 city9)
         (can-drive city2 city10)
         (can-drive city2 city19)
         (wrong-drive1 city2 city9)
         (wrong-drive2 city2 city10)
         (wrong-drive3 city2 city19)
         (can-drive city3 city1)
         (can-drive city3 city10)
         (can-drive city3 city19)
         (wrong-drive1 city3 city1)
         (wrong-drive2 city3 city10)
         (wrong-drive3 city3 city19)
         (can-drive city4 city18)
         (can-drive city4 city12)
         (can-drive city4 city6)
         (can-drive city4 city7)
         (wrong-drive1 city4 city18)
         (wrong-drive2 city4 city12)
         (wrong-drive3 city4 city6)
         (can-drive city5 city17)
         (can-drive city5 city8)
         (can-drive city5 city0)
         (wrong-drive1 city5 city17)
         (wrong-drive2 city5 city8)
         (wrong-drive3 city5 city0)
         (can-drive city6 city4)
         (can-drive city6 city18)
         (can-drive city6 city16)
         (wrong-drive1 city6 city4)
         (wrong-drive2 city6 city18)
         (wrong-drive3 city6 city16)
         (can-drive city7 city11)
         (can-drive city7 city15)
         (can-drive city7 city4)
         (wrong-drive1 city7 city11)
         (wrong-drive2 city7 city15)
         (wrong-drive3 city7 city4)
         (can-drive city8 city5)
         (can-drive city8 city11)
         (can-drive city8 city15)
         (wrong-drive1 city8 city5)
         (wrong-drive2 city8 city11)
         (wrong-drive3 city8 city15)
         (can-drive city9 city2)
         (can-drive city9 city10)
         (can-drive city9 city19)
         (wrong-drive1 city9 city2)
         (wrong-drive2 city9 city10)
         (wrong-drive3 city9 city19)
         (can-drive city10 city1)
         (can-drive city10 city2)
         (can-drive city10 city3)
         (can-drive city10 city9)
         (wrong-drive1 city10 city1)
         (wrong-drive2 city10 city2)
         (wrong-drive3 city10 city3)
         (can-drive city11 city7)
         (can-drive city11 city8)
         (can-drive city11 city15)
         (wrong-drive1 city11 city7)
         (wrong-drive2 city11 city8)
         (wrong-drive3 city11 city15)
         (can-drive city12 city4)
         (can-drive city12 city18)
         (can-drive city12 city13)
         (can-drive city12 city16)
         (wrong-drive1 city12 city4)
         (wrong-drive2 city12 city18)
         (wrong-drive3 city12 city13)
         (can-drive city13 city12)
         (can-drive city13 city16)
         (can-drive city13 city18)
         (wrong-drive1 city13 city12)
         (wrong-drive2 city13 city16)
         (wrong-drive3 city13 city18)
         (can-drive city14 city0)
         (can-drive city14 city19)
         (can-drive city14 city17)
         (wrong-drive1 city14 city0)
         (wrong-drive2 city14 city19)
         (wrong-drive3 city14 city17)
         (can-drive city15 city7)
         (can-drive city15 city8)
         (can-drive city15 city11)
         (wrong-drive1 city15 city7)
         (wrong-drive2 city15 city8)
         (wrong-drive3 city15 city11)
         (can-drive city16 city6)
         (can-drive city16 city13)
         (can-drive city16 city18)
         (can-drive city16 city12)
         (wrong-drive1 city16 city6)
         (wrong-drive2 city16 city13)
         (wrong-drive3 city16 city18)
         (can-drive city17 city0)
         (can-drive city17 city5)
         (can-drive city17 city14)
         (wrong-drive1 city17 city0)
         (wrong-drive2 city17 city5)
         (wrong-drive3 city17 city14)
         (can-drive city18 city4)
         (can-drive city18 city6)
         (can-drive city18 city12)
         (can-drive city18 city13)
         (can-drive city18 city16)
         (wrong-drive1 city18 city4)
         (wrong-drive2 city18 city6)
         (wrong-drive3 city18 city12)
         (can-drive city19 city0)
         (can-drive city19 city1)
         (can-drive city19 city2)
         (can-drive city19 city3)
         (can-drive city19 city9)
         (can-drive city19 city14)
         (wrong-drive1 city19 city0)
         (wrong-drive2 city19 city1)
         (wrong-drive3 city19 city2)
  )
  (:goal (and
      (box-at-city box0 city13)
      (box-at-city box1 city1)
      (box-at-city box2 city12)
      (box-at-city box3 city10)
      (box-at-city box4 city12)
      (box-at-city box5 city17)
      (box-at-city box6 city7)
      (box-at-city box7 city8)
      (box-at-city box8 city15)
      (box-at-city box9 city0)
      (box-at-city box10 city1)
      (box-at-city box11 city11)
      (box-at-city box12 city16)
      (box-at-city box13 city1)
      (box-at-city box14 city11)
      (box-at-city box15 city13)
      (box-at-city box16 city12)
      (box-at-city box17 city2)
      (box-at-city box18 city15)
      (box-at-city box19 city15)
  ))
  (:metric minimize (total-cost))
)
