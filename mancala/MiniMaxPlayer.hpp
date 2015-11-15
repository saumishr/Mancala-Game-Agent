//
//  MiniMaxPlayer.hpp
//  mancala
//
//  Created by Saurabh Mishra on 10/10/15.
//  Copyright © 2015. All rights reserved.
//

#ifndef MiniMaxPlayer_hpp
#define MiniMaxPlayer_hpp

#include <iostream>
#include <deque>
#include "MancalaBoard.hpp"

enum PlayerType {MAX, MIN};

class MiniMaxPlayer
{
public:
    MiniMaxPlayer();
    ~MiniMaxPlayer();
    
    deque<int> getPitWithMaximumGain(MancalaBoard* board);
    int getExpectedValue(MancalaBoard* board);
    std::string getValueIdentifier(int value);
    PlayerType getMinOrMaxPlayer(Player player);
    void serializeTraversal(std::string node, int depth, int expectedValue);
    void percolateBonusMovesUp(MancalaBoard* predecessor, MancalaBoard* successor);
    void initExpectedValue(MancalaBoard* board);
    void initGameDepth(MancalaBoard* predecessor, MancalaBoard* successor);
    void initIsGrouped(MancalaBoard* board);
};

#endif /* MiniMaxPlayer_hpp */
