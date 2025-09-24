; https://www.sokobanonline.com/build/edit/18625#start=play

(define (problem l2)
    (:domain sokoban)
    (:objects
    v1 v2 v3 v4 v5 v6 v7 v8 v9 v10
    )
    
    (:init 
    (inc v1 v2)
    (inc v2 v3) 
    (inc v3 v4)
    (inc v4 v5)
    (inc v5 v6)
    (inc v6 v7)
    (inc v7 v8)
    (inc v8 v9)
    (inc v9 v10)
    
    (dec v10 v9)
    (dec v9 v8)
    (dec v8 v7)
    (dec v7 v6)
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
    (wall v7 v1)
    (wall v8 v1)
    (wall v9 v1)
    (wall v10 v1)
    
    (wall v1 v2)
    (wall v1 v3)
    (wall v1 v4)
    (wall v1 v5)
    (wall v1 v6)
    (wall v1 v7)
    (wall v1 v8)
    (wall v1 v9)
    
    (wall v10 v2)
    (wall v10 v3)
    (wall v10 v4)
    (wall v10 v5)
    (wall v10 v6)
    (wall v10 v7)
    (wall v10 v8)
    (wall v10 v9)
    
    (wall v1 v10)
    (wall v2 v10)
    (wall v3 v10)
    (wall v4 v10)
    (wall v5 v10)
    (wall v6 v10)
    (wall v7 v10)
    (wall v8 v10)
    (wall v9 v10)
    (wall v10 v10)
    
    
    (wall v5 v3)
    (wall v6 v3)
    
    (wall v3 v5)
    (wall v3 v6)
    
    (wall v5 v8)
    (wall v6 v8)
    
    (wall v8 v5)
    (wall v8 v6)
    
    
    (box v6 v4)
    (box v4 v5)
    (box v4 v7)
    (box v6 v7)
    
    (at v5 v6)
    
)

    (:goal 
        (and (box v2 v5)
             (box v2 v6)
             (box v6 v2)
             (box v9 v6)
        )
    )
)