//
//  AlphaBetaPlayer.cpp
//  mancala
//
//  Created by Saurabh Mishra on 10/13/15.
//  Copyright © 2015. All rights reserved.
//
#include <iostream>
#include <limits.h>
#include <algorithm>
#include "AlphaBetaPlayer.hpp"
#include "MancalaManager.hpp"
#include "FileUtils.hpp"

AlphaBetaPlayer::AlphaBetaPlayer() {}
AlphaBetaPlayer::~AlphaBetaPlayer() {}

deque<int> AlphaBetaPlayer::getPitWithMaximumGain(MancalaBoard* board)
{
    FileUtils::writeLogOutput("Node,Depth,Value,Alpha,Beta");
    serializeTraversal(std::string("root"), board->getGameDepth(), board->m_expectedValue, board->m_alpha, board->m_beta);

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
                    serializeTraversal(board->getPitIdentifier(board->getGameIndex(), board->getPreviousPlayer()),
                                       board->getGameDepth(),
                                       board->m_expectedValue,
                                       board->m_alpha,
                                       board->m_beta);
                 
                    moves = clonedBoard->m_bonusMoves;
                    moves.push_front(maxGainPitIndex);
                    
                    delete clonedBoard;
                    clonedBoard = NULL;
                    
                    return moves;
                }
                board->m_alpha = std::max(board->m_alpha, board->m_expectedValue);
            }
            else
            {
                if (board->m_expectedValue <= board->m_alpha)
                {
                    serializeTraversal(board->getPitIdentifier(board->getGameIndex(), board->getPreviousPlayer()),
                                       board->getGameDepth(),
                                       board->m_expectedValue,
                                       board->m_alpha,
                                       board->m_beta);

                    moves = clonedBoard->m_bonusMoves;
                    moves.push_front(maxGainPitIndex);
                    
                    delete clonedBoard;
                    clonedBoard = NULL;
                    
                    return moves;
                }
                board->m_beta = std::min(board->m_beta, board->m_expectedValue);
            }
            maxGainPitIndex = pitIndex;
            moves = clonedBoard->m_bonusMoves;
            moves.push_front(maxGainPitIndex);
        }
        
        serializeTraversal(std::string("root"), board->getGameDepth(), board->m_expectedValue, board->m_alpha, board->m_beta);
        
        delete clonedBoard;
        clonedBoard = NULL;
    }
    
    return moves;
}


int AlphaBetaPlayer::getExpectedValue(MancalaBoard* board)
{
    serializeTraversal(board->getPitIdentifier(board->getGameIndex(), board->getPreviousPlayer()),
                       board->getGameDepth(),
                       board->m_expectedValue,
                       board->m_alpha,
                       board->m_beta);
    
    if ((board->getGameDepth() == board->getCutOffDepth() &&
         board->getPreviousPlayer() != board->getCurrentPlayer()) ||
        board->isGameOver()) {
        return board->m_expectedValue;
    }
    
    for(int pitIndex = 0; pitIndex < board->getNumPitsPerPlayer(); pitIndex++)
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
                serializeTraversal(board->getPitIdentifier(board->getGameIndex(), board->getPreviousPlayer()),
                                   board->getGameDepth(),
                                   board->m_expectedValue,
                                   board->m_alpha,
                                   board->m_beta);
                
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
                serializeTraversal(board->getPitIdentifier(board->getGameIndex(), board->getPreviousPlayer()),
                                   board->getGameDepth(),
                                   board->m_expectedValue,
                                   board->m_alpha,
                                   board->m_beta);
                
                delete clonedBoard;
                clonedBoard = NULL;
                return board->m_expectedValue;
            }
            board->m_beta = std::min(board->m_beta, board->m_expectedValue);
        }
        
        serializeTraversal(board->getPitIdentifier(board->getGameIndex(), board->getPreviousPlayer()),
                           board->getGameDepth(),
                           board->m_expectedValue,
                           board->m_alpha,
                           board->m_beta);
        
        delete clonedBoard;
        clonedBoard = NULL;
    }
    
    return board->m_expectedValue;
}

void AlphaBetaPlayer::serializeTraversal(std::string node, int depth, int expectedValue, int alpha, int beta)
{
    std::string separator = std::string(",");
    std::string outputStr = node + separator + std_patch::to_string(depth) + separator + getValueIdentifier(expectedValue) + separator + getValueIdentifier(alpha) + separator + getValueIdentifier(beta);
    
    FileUtils::writeLogOutput(outputStr);
}
