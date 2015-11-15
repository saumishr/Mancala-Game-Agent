//
//  AlphaBetaPlayer.hpp
//  mancala
//
//  Created by Saurabh Mishra on 10/13/15.
//  Copyright © 2015. All rights reserved.
//

#ifndef AlphaBetaPlayer_hpp
#define AlphaBetaPlayer_hpp

#include <iostream>
#include <deque>
#include "MancalaBoard.hpp"
#include "MiniMaxPlayer.hpp"

class AlphaBetaPlayer: public MiniMaxPlayer
{
public:
    AlphaBetaPlayer();
    ~AlphaBetaPlayer();
    
    virtual deque<int> getPitWithMaximumGain(MancalaBoard* board);
    virtual int getExpectedValue(MancalaBoard* board);
    void serializeTraversal(std::string node, int depth, int expectedValue, int alpha, int beta);
};

#endif /* AlphaBetaPlayer_hpp */
