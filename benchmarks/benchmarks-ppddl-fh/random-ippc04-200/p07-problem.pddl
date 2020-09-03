(define (problem random-problem830) 
 (:domain prob_domain) 
 
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 - horizon-value)
(:init (horizon horzn6) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) 
(not-clear)
(GO YJ) (WI DN TR) (QG WR) (QG KG) (WI OK KG) (LS TR) (XG ED KG) (QG YJ) (GO ED) (LS JQ) (LS WR) (QG OK) (XG KG OK) (LS ED) (QG CI) (XG FJ YJ) (XG TR CI) (LS DN) (XG TR DN) (WI OK DN)  
)
 (:goal (and 
(QG TR ) 
(GO TR ) 
))
(:metric minimize (total-cost))
)
