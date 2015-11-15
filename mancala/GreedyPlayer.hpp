//
//  GreedyPlayer.hpp
//  mancala
//
//  Created by Saurabh Mishra on 10/10/15.
//  Copyright © 2015. All rights reserved.
//

#ifndef GreedyPlayer_hpp
#define GreedyPlayer_hpp

#include <iostream>
#include "MancalaBoard.hpp"

class GreedyPlayer
{
public:
    GreedyPlayer();
    ~GreedyPlayer();
    
    deque<int> getPitWithMaximumGain(MancalaBoard* board);
    int getExpectedValue(MancalaBoard* board);
    void initExpectedValue(MancalaBoard *board);
    void percolateBonusMovesUp(MancalaBoard* predecessor, MancalaBoard* successor);
};


#endif /* GreedyPlayer_hpp */
