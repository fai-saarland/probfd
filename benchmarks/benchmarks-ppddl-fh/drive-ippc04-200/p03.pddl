(define (problem a-drive-problem286)
(:domain drive)
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 - horizon-value c0 c1 c2  - coord)
(:init (horizon horzn12) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) 
       (heading north)
       (at c0 c0)
       (alive)
       (light_color unknown)
       (clockwise north east)
       (clockwise east south)
       (clockwise south west)
       (clockwise west north)
       (nextx c0 c1 east)
       (nextx c1 c0 west)
       (nextx c0 c0 north)
       (nextx c0 c0 south)
       (nextx c1 c1 north)
       (nextx c1 c1 south)
       (nexty c0 c1 north)
       (nexty c1 c0 south)
       (nexty c1 c2 north)
       (nexty c2 c1 south)
       (nexty c0 c0 east)
       (nexty c0 c0 west)
       (nexty c1 c1 east)
       (nexty c1 c1 west)
       (nexty c2 c2 east)
       (nexty c2 c2 west)
       (light_delay c0 c0 quick)
       (light_delay c0 c1 slow)
       (light_delay c0 c2 slow)
       (light_delay c1 c0 quick)
       (light_delay c1 c1 quick)
       (light_delay c1 c2 quick)
       (light_preference c0 c0 east_west)
       (light_preference c0 c1 none)
       (light_preference c0 c2 none)
       (light_preference c1 c0 north_south)
       (light_preference c1 c1 east_west)
       (light_preference c1 c2 north_south)
       (road-length c0 c0 c0 c1 long)
       (road-length c0 c1 c0 c0 long)
       (road-length c0 c1 c0 c2 long)
       (road-length c0 c2 c0 c1 long)
       (road-length c1 c0 c1 c1 short)
       (road-length c1 c1 c1 c0 short)
       (road-length c1 c1 c1 c2 long)
       (road-length c1 c2 c1 c1 long)
       (road-length c0 c0 c1 c0 short)
       (road-length c1 c0 c0 c0 short)
       (road-length c0 c1 c1 c1 short)
       (road-length c1 c1 c0 c1 short)
       (road-length c0 c2 c1 c2 short)
       (road-length c1 c2 c0 c2 short)
)
(:goal (and (alive) (at c1 c2)))
(:metric minimize (total-cost))
)
