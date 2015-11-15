//
//  TimedAlphaBetaPlayer.hpp
//  mancala
//
//  Created by Saurabh Mishra on 11/6/15.
//  Copyright © 2015. All rights reserved.
//

#ifndef TimedAlphaBetaPlayer_hpp
#define TimedAlphaBetaPlayer_hpp

#include <iostream>
#include <deque>
#include "AlphaBetaPlayer.hpp"
#include "MancalaBoard.hpp"
#include "MiniMaxPlayer.hpp"

class TimedAlphaBetaPlayer: public AlphaBetaPlayer
{
public:
    TimedAlphaBetaPlayer();
    ~TimedAlphaBetaPlayer();
    virtual deque<int> getPitWithMaximumGain(MancalaBoard* board);
    virtual int getExpectedValue(MancalaBoard* board);
    virtual void initExpectedValue(MancalaBoard *board);
    
private:
    unsigned long long  getRemainingTime();
    unsigned long long  m_startTime;
    int                 m_maxDepth;
    unsigned long       m_timeTolerance;
    unsigned long long  m_timeLimit;
};
#endif /* TimedAlphaBetaPlayer_hpp */
