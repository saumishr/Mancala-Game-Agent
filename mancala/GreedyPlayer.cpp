//
//  GreedyPlayer.cpp
//  mancala
//
//  Created by Saurabh Mishra on 10/10/15.
//  Copyright © 2015. All rights reserved.
//

#include "GreedyPlayer.hpp"
#include "MancalaManager.hpp"
#include <limits.h>

GreedyPlayer::GreedyPlayer() {}
GreedyPlayer::~GreedyPlayer() {}

deque<int> GreedyPlayer::getPitWithMaximumGain(MancalaBoard* board)
{
    int maxGainPitIndex = -1;
    deque<int> moves;

    for(int pitIndex = 0; pitIndex < board->getNumPitsPerPlayer(); pitIndex++)
    {
        if (!board->isPitValid(pitIndex))
            continue;
        
        MancalaBoard* clonedBoard = board->clone();
        clonedBoard->play(pitIndex);
        clonedBoard->setGameIndex(pitIndex);
        clonedBoard->setPreviousPlayer( board->getCurrentPlayer() );
        
        initExpectedValue(clonedBoard);
        
        int gain = getExpectedValue(clonedBoard);
        
        if (gain > board->m_expectedValue)
        {
            board->m_expectedValue = gain;
            maxGainPitIndex = pitIndex;
            moves = clonedBoard->m_bonusMoves;
            moves.push_front(maxGainPitIndex);
        }
        
        delete clonedBoard;
        clonedBoard = NULL;
    }
    
    return moves;
}

int GreedyPlayer::getExpectedValue(MancalaBoard* board)
{    
    if (board->isGameOver() || board->getPreviousPlayer() != board->getCurrentPlayer())
    {
        return board->m_expectedValue;
    }
    else
    {
        for(int pitIndex = 0; pitIndex < board->getNumPitsPerPlayer(); pitIndex++)
        {
            if (!board->isPitValid(pitIndex))
                continue;
            
            MancalaBoard* clonedBoard = board->clone();
            clonedBoard->play(pitIndex);
            
            clonedBoard->setGameIndex(pitIndex);
            clonedBoard->setPreviousPlayer( board->getCurrentPlayer() );
            
            initExpectedValue(clonedBoard);
        
            int clonedNodeExpectedValue = getExpectedValue(clonedBoard);
            if (clonedNodeExpectedValue > board->m_expectedValue)
            {
                board->m_expectedValue = clonedNodeExpectedValue;
                percolateBonusMovesUp(board, clonedBoard);
            }
            delete clonedBoard;
            clonedBoard = NULL;
        }
    }
        
    return board->m_expectedValue;
}

void GreedyPlayer::initExpectedValue(MancalaBoard *board)
{
    if (board->isGameOver())
    {
        board->adjustStonesOnGameOver();
        board->m_expectedValue = board->evalBoard();
    }
    else if(board->getPreviousPlayer() != board->getCurrentPlayer())
    {
        board->m_expectedValue = board->evalBoard();
    }
    else
    {
        board->m_expectedValue = INT_MIN;
    }
}

void GreedyPlayer::percolateBonusMovesUp(MancalaBoard* predecessor, MancalaBoard* successor)
{
    predecessor->m_bonusMoves.clear();
    predecessor->m_bonusMoves.push_front(successor->getGameIndex());
    while (!successor->m_bonusMoves.empty()) {
        predecessor->m_bonusMoves.push_back(successor->m_bonusMoves.front());
        successor->m_bonusMoves.pop_front();
    }
}