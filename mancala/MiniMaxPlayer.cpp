//
//  MiniMaxPlayer.cpp
//  mancala
//
//  Created by Saurabh Mishra on 10/10/15.
//  Copyright © 2015. All rights reserved.
//
#include <limits.h>
#include <algorithm>
#include "MancalaManager.hpp"
#include "MiniMaxPlayer.hpp"
#include "FileUtils.hpp"

MiniMaxPlayer::MiniMaxPlayer() {}
MiniMaxPlayer::~MiniMaxPlayer() {}

deque<int> MiniMaxPlayer::getPitWithMaximumGain(MancalaBoard* board)
{
    FileUtils::writeLogOutput("Node,Depth,Value");
    serializeTraversal(std::string("root"), board->getGameDepth(), board->m_expectedValue);
    
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
        
        initIsGrouped(clonedBoard);
        
        clonedBoard->setGameDepth(board->getGameDepth() + 1);
        
        initExpectedValue(clonedBoard);
        
        int gain = getExpectedValue(clonedBoard);
        
        if (gain > board->m_expectedValue)
        {
            board->m_expectedValue = gain;
            maxGainPitIndex = pitIndex;
            moves = clonedBoard->m_bonusMoves;
            moves.push_front(maxGainPitIndex);
        }
        
        serializeTraversal(std::string("root"), board->getGameDepth(), board->m_expectedValue);
        
        delete clonedBoard;
        clonedBoard = NULL;
    }
    
    return moves;
}

void MiniMaxPlayer::initIsGrouped(MancalaBoard *board)
{
    if (board->getCurrentPlayer() == board->getPreviousPlayer())
    {
        board->m_isGrouped = true;
    }
}

int MiniMaxPlayer::getExpectedValue(MancalaBoard* board)
{
    serializeTraversal(board->getPitIdentifier(board->getGameIndex(), board->getPreviousPlayer()),
                       board->getGameDepth(),
                       board->m_expectedValue);
    
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
        }
        else
        {
            if (clonedNodeExpectedValue < board->m_expectedValue) {
                board->m_expectedValue = clonedNodeExpectedValue;
                percolateBonusMovesUp(board, clonedBoard);
            }
        }
        
        serializeTraversal(board->getPitIdentifier(board->getGameIndex(), board->getPreviousPlayer()),
                           board->getGameDepth(),
                           board->m_expectedValue);
        
        delete clonedBoard;
        clonedBoard = NULL;
    }
    
    return board->m_expectedValue;
}

void MiniMaxPlayer::percolateBonusMovesUp(MancalaBoard* predecessor, MancalaBoard* successor)
{
    predecessor->m_bonusMoves.clear();
    if (predecessor->getGameDepth() == 1 && predecessor->getGameDepth() == successor->getGameDepth())
    {
        predecessor->m_bonusMoves.push_front(successor->getGameIndex());
        while (!successor->m_bonusMoves.empty()) {
            predecessor->m_bonusMoves.push_back(successor->m_bonusMoves.front());
            successor->m_bonusMoves.pop_front();
        }
    }
    else
    {
        predecessor->m_bonusMoves = successor->m_bonusMoves;
    }
}

void MiniMaxPlayer::initGameDepth(MancalaBoard *predecessor, MancalaBoard *successor)
{
    if (predecessor->m_isGrouped)
    {
        successor->setGameDepth(predecessor->getGameDepth());
    }
    else
    {
        successor->setGameDepth(predecessor->getGameDepth() + 1);
    }
}

void MiniMaxPlayer::initExpectedValue(MancalaBoard *board)
{
    if (board->isGameOver())
    {
        board->adjustStonesOnGameOver();
        board->m_expectedValue = board->evalBoard();
    }
    else if (board->getGameDepth() == board->getCutOffDepth() &&
        board->getPreviousPlayer() != board->getCurrentPlayer())
    {
        board->m_expectedValue = board->evalBoard();
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

std::string MiniMaxPlayer::getValueIdentifier(int value)
{
    if (value == INT_MIN)
    {
        return std::string("-Infinity");
    }
    else if (value == INT_MAX)
    {
        return std::string("Infinity");
    }
    
    return std_patch::to_string(value);
}

PlayerType MiniMaxPlayer::getMinOrMaxPlayer(Player player)
{
    if (player == MancalaManager::getInstance()->getPlayer())
        return MAX;
    
    return MIN;
}

void MiniMaxPlayer::serializeTraversal(std::string node, int depth, int expectedValue)
{
    std::string separator = std::string(",");
    std::string outputStr = node + separator + std_patch::to_string(depth) + separator + getValueIdentifier(expectedValue);
    FileUtils::writeLogOutput(outputStr);
}