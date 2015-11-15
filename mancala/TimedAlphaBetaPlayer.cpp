//
//  TimedAlphaBetaPlayer.cpp
//  mancala
//
//  Created by Saurabh Mishra on 11/6/15.
//  Copyright © 2015. All rights reserved.
//

#include <iostream>
#include <limits.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include "MancalaManager.hpp"
#include "FileUtils.hpp"
#include "TimedAlphaBetaPlayer.hpp"

static unsigned long long k_timeLimitPerMove = 10000;
static const int k_maxGameDepth = 8;

TimedAlphaBetaPlayer::TimedAlphaBetaPlayer():m_maxDepth(k_maxGameDepth), m_timeTolerance(50) {}
TimedAlphaBetaPlayer::~TimedAlphaBetaPlayer() {}

deque<int> TimedAlphaBetaPlayer::getPitWithMaximumGain(MancalaBoard* board)
{
    deque<int> movesGlobal;
    int depthCovered = 0;
    unsigned long long depthCoveredTime = k_timeLimitPerMove;
    unsigned long long lastDepthCoveredTime = k_timeLimitPerMove;
    
    int maxGainPitIndexLocal = -1;
    deque<int> movesLocal;
    
    m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    for (int depth = 1; depth < m_maxDepth && getRemainingTime() > m_timeTolerance; depth++)
    {
        board->setCutOffDepth(depth);
        board->m_alpha = INT_MIN;
        board->m_beta = INT_MAX;
        board->m_expectedValue = INT_MIN;
        
        movesGlobal = movesLocal;
        depthCovered = depth - 1;
        lastDepthCoveredTime = depthCoveredTime;
        depthCoveredTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - m_startTime;
        
        for(int pitIndex = 0; pitIndex < board->getNumPitsPerPlayer() && getRemainingTime() > m_timeTolerance; pitIndex++)
        {
            if (!board->isPitValid(pitIndex))
                continue;
            
            MancalaBoard* clonedBoard = board->clone();
            clonedBoard->play(pitIndex);
            clonedBoard->setGameIndex(pitIndex);
            clonedBoard->setPreviousPlayer( board->getCurrentPlayer() );
            clonedBoard->m_alpha = board->m_alpha;
            clonedBoard->m_beta = board->m_beta;
            
            initIsGrouped(clonedBoard);
            
            clonedBoard->setGameDepth(board->getGameDepth() + 1);
            
            initExpectedValue(clonedBoard);
            
            int gain = getExpectedValue(clonedBoard);
            
            if (gain > board->m_expectedValue)
            {
                board->m_expectedValue = gain;
                if (getMinOrMaxPlayer(board->getCurrentPlayer()) == MAX)
                {
                    if (board->m_expectedValue >= board->m_beta)
                    {
                        movesLocal = clonedBoard->m_bonusMoves;
                        movesLocal.push_front(maxGainPitIndexLocal);
                        
                        delete clonedBoard;
                        clonedBoard = NULL;
                        
                        break;
                    }
                    board->m_alpha = std::max(board->m_alpha, board->m_expectedValue);
                }
                else
                {
                    if (board->m_expectedValue <= board->m_alpha)
                    {
                        movesLocal = clonedBoard->m_bonusMoves;
                        movesLocal.push_front(maxGainPitIndexLocal);
                        
                        delete clonedBoard;
                        clonedBoard = NULL;
                        
                        break;
                    }
                    board->m_beta = std::min(board->m_beta, board->m_expectedValue);
                }
                
                maxGainPitIndexLocal = pitIndex;
                movesLocal = clonedBoard->m_bonusMoves;
                movesLocal.push_front(maxGainPitIndexLocal);
            }
            
            delete clonedBoard;
            clonedBoard = NULL;
        }
    }
    return movesGlobal;
}


int TimedAlphaBetaPlayer::getExpectedValue(MancalaBoard* board)
{
    if ((board->getGameDepth() == board->getCutOffDepth() &&
         board->getPreviousPlayer() != board->getCurrentPlayer()) ||
        board->isGameOver()) {
        return board->m_expectedValue;
    }
    
    for(int pitIndex = 0; pitIndex < board->getNumPitsPerPlayer() && getRemainingTime() > m_timeTolerance; pitIndex++)
    {
        if (!board->isPitValid(pitIndex))
            continue;
        
        MancalaBoard* clonedBoard = board->clone();
        clonedBoard->play(pitIndex);
        clonedBoard->setGameIndex(pitIndex);
        clonedBoard->setPreviousPlayer( board->getCurrentPlayer() );
        clonedBoard->m_alpha = board->m_alpha;
        clonedBoard->m_beta = board->m_beta;
        
        initIsGrouped(clonedBoard);
        
        initGameDepth(board, clonedBoard);
        
        initExpectedValue(clonedBoard);
        
        int clonedNodeExpectedValue = getExpectedValue(clonedBoard);
        
        if (getMinOrMaxPlayer(board->getCurrentPlayer()) == MAX)
        {
            if (clonedNodeExpectedValue > board->m_expectedValue)
            {
                board->m_expectedValue = clonedNodeExpectedValue;
                percolateBonusMovesUp(board, clonedBoard);
            }
            
            if (board->m_expectedValue >= board->m_beta)
            {
                delete clonedBoard;
                clonedBoard = NULL;
                
                return board->m_expectedValue;
            }
            board->m_alpha = std::max(board->m_alpha, board->m_expectedValue);
        }
        else
        {
            if (clonedNodeExpectedValue < board->m_expectedValue) {
                board->m_expectedValue = clonedNodeExpectedValue;
                percolateBonusMovesUp(board, clonedBoard);
            }
            
            if (board->m_expectedValue <= board->m_alpha)
            {
                delete clonedBoard;
                clonedBoard = NULL;
                return board->m_expectedValue;
            }
            board->m_beta = std::min(board->m_beta, board->m_expectedValue);
        }
        
        delete clonedBoard;
        clonedBoard = NULL;
    }
    
    return board->m_expectedValue;
}

void TimedAlphaBetaPlayer::initExpectedValue(MancalaBoard *board)
{
    if (board->isGameOver())
    {
        board->adjustStonesOnGameOver();
        board->m_expectedValue = board->evalBoardEx();
    }
    else if (board->getGameDepth() == board->getCutOffDepth() &&
             board->getPreviousPlayer() != board->getCurrentPlayer())
    {
        board->m_expectedValue = board->evalBoardEx();
    }
    else
    {
        if (getMinOrMaxPlayer(board->getCurrentPlayer()) == MAX)
        {
            board->m_expectedValue = INT_MIN;
        }
        else
        {
            board->m_expectedValue = INT_MAX;
        }
    }
}

unsigned long long TimedAlphaBetaPlayer::getRemainingTime()
{
    unsigned long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    return k_timeLimitPerMove - (milliseconds_since_epoch - m_startTime);
}