; https://www.sokobanonline.com/build/edit/18618#start=play 

(define (problem l1)
    (:domain sokoban)
    (:objects
    v1 v2 v3 v4 v5 v6
    )
    
    (:init 
    (inc v1 v2)
    (inc v2 v3) 
    (inc v3 v4)
    (inc v4 v5)
    (inc v5 v6)
    
    (dec v6 v5)
    (dec v5 v4)
    (dec v4 v3)
    (dec v3 v2)
    (dec v2 v1)
    
    (wall v1 v1)
    (wall v2 v1)
    (wall v3 v1)
    (wall v4 v1)
    (wall v5 v1)
    (wall v6 v1)
    
    (wall v1 v2)
    (wall v1 v3)
    (wall v1 v4)
    (wall v1 v5)
    
    (wall v6 v2)
    (wall v6 v3)
    (wall v6 v4)
    (wall v6 v5)
    
    (wall v1 v6)
    (wall v2 v6)
    (wall v3 v6)
    (wall v4 v6)
    (wall v5 v6)
    (wall v6 v6)
    
    
    (wall v4 v3)
    (wall v5 v3)
    
    (wall v4 v4)
    (wall v5 v4)
    
    (box v3 v2)
    (box v3 v4)
    
    (at v2 v3)
    
)

    (:goal 
        (and (box v5 v2)
             (box v5 v5)
        )
    )
)