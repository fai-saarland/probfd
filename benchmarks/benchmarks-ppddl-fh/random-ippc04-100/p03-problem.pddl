(define (problem random-problem538) 
 (:domain prob_domain) 
 
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 - horizon-value)
(:init (horizon horzn4) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) 
(not-clear)
(DJ VA SY) (KB YS) (KB WE) (DL SY YS) (DL NO VA) (QF NO) (DJ UQ UQ) (WY NO) (KB NO) (DL UQ SZ) (DJ YS VA) (WY WE) (KB UQ) (QF UQ) (KB SY) (QF WE) (DJ EV SF) (DJ SZ SY) (QF SF) (DL SZ WE)  
)
 (:goal (and 
(DL UQ  YS ) 
(DL SZ  NO ) 
(DL NO  UQ ) 
(DL SY  SY ) 
))
(:metric minimize (total-cost))
)
