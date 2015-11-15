//
//  MancalaBoard.cpp
//  mancala
//
//  Created by Saurabh Mishra on 10/9/15.
//  Copyright © 2015. All rights reserved.
//
#include <limits.h>
#include "MancalaBoard.hpp"
#include "MancalaManager.hpp"
#include "GreedyPlayer.hpp"
#include "MiniMaxPlayer.hpp"
#include "AlphaBetaPlayer.hpp"
#include "TimedAlphaBetaPlayer.hpp"
#include "FileUtils.hpp"

#define PLAYER_2_ID "A"
#define PLAYER_1_ID "B"

int MancalaBoard::m_numPitsPerPlayer;

MancalaBoard::MancalaBoard() : m_currentPlayer(PLAYER_NONE),
m_cutOffDepth(-1),
m_gameDepth(-1),
m_gameIndex(-1),
m_expectedValue(INT_MIN),
m_prevPlayer(PLAYER_NONE),
m_isGrouped(false),
m_alpha(INT_MIN),
m_beta(INT_MAX)
{
}

MancalaBoard::~MancalaBoard()
{
    m_pits.clear();
}

void MancalaBoard::initializeBoard(std::vector<int>& firstPlayerStones, std::vector<int>& secondPlayerStones, int mancalaStones1, int mancalaStones2, int strategy, int cutOffDepth)
{
    std::vector<int>::iterator player1Itr = firstPlayerStones.begin();
    for(; player1Itr != firstPlayerStones.end(); player1Itr++)
    {
        m_pits.push_back(*player1Itr);
    }

    m_pits.push_back(mancalaStones1);
    
    std::vector<int>::reverse_iterator player2Itr = secondPlayerStones.rbegin();
    for(; player2Itr != secondPlayerStones.rend(); player2Itr++)
    {
        m_pits.push_back(*player2Itr);
    }
    
    m_pits.push_back(mancalaStones2);
    
    m_strategy = static_cast<Strategy>(strategy);
    m_cutOffDepth = cutOffDepth;
}

Strategy MancalaBoard::getGameStrategy() const
{
    return m_strategy;
}

void MancalaBoard::setCurrentPlayer(Player currentPlayer)
{
    m_currentPlayer = currentPlayer;
}

Player MancalaBoard::getCurrentPlayer()
{
    return m_currentPlayer;
}

void MancalaBoard::setPreviousPlayer(Player currentPlayer)
{
    m_prevPlayer = currentPlayer;
}

Player MancalaBoard::getPreviousPlayer()
{
    return m_prevPlayer;
}

void MancalaBoard::setCutOffDepth(int depth)
{
    m_cutOffDepth = depth;
}

int MancalaBoard::getCutOffDepth()
{
    return m_cutOffDepth;
}

int MancalaBoard::getGameIndex() const
{
    return m_gameIndex;
}

void MancalaBoard::setGameIndex(int gameIndex)
{
    m_gameIndex = gameIndex;
}

int MancalaBoard::getGameDepth() const
{
    return m_gameDepth;
}

void MancalaBoard::setGameDepth(int depth)
{
    m_gameDepth = depth;
}

void MancalaBoard::setMancalaPit(int numStones, Player player)
{
    if (m_currentPlayer == PLAYER_1)
    {
        long player1MancalaPitIndex = getMancalaPitIndex(PLAYER_1);
        m_pits[player1MancalaPitIndex] = numStones;
    }
    else if (m_currentPlayer == PLAYER_2)
    {
        long player2MancalaPitIndex = getMancalaPitIndex(PLAYER_2);
        m_pits[player2MancalaPitIndex] = numStones;
    }
}

int MancalaBoard::getMancalaPitIndex(Player player)
{
    if (player == PLAYER_1) {
        return static_cast<int>(m_pits.size() / 2) - 1;
    }

    return static_cast<int>(m_pits.size() - 1);
}

void MancalaBoard::setNumPitsPerPlayer(int numPits)
{
    m_numPitsPerPlayer = numPits;
}

int MancalaBoard::getNumPitsPerPlayer()
{
    return m_numPitsPerPlayer;
}

int MancalaBoard::getGlobalPitIndex(int pitLocalIndex, Player player)
{
    if (player == PLAYER_2) {
        return int(m_pits.size() - 1) - 1 - pitLocalIndex;
    }
    return pitLocalIndex;
}

int MancalaBoard::getStonesInPit(int pitGlobalIndex)
{
    return m_pits.at(pitGlobalIndex);
}

void MancalaBoard::play(int pitLocalIndex)
{
    int currentPitGlobalIndex = getGlobalPitIndex(pitLocalIndex, m_currentPlayer);
    int numStones = m_pits.at(currentPitGlobalIndex);
    m_pits.at(currentPitGlobalIndex) = 0;
    long opponentMancalaPitIndex = -1;
    if (m_currentPlayer == PLAYER_1)
    {
        opponentMancalaPitIndex = getMancalaPitIndex(PLAYER_2);
    }
    else if (m_currentPlayer == PLAYER_2)
    {
        opponentMancalaPitIndex = getMancalaPitIndex(PLAYER_1);
    }
    
    while (numStones > 0) {
        currentPitGlobalIndex = (currentPitGlobalIndex + 1) % (m_pits.size());
        if (currentPitGlobalIndex != opponentMancalaPitIndex) {
            int currentStones = m_pits.at(currentPitGlobalIndex);
            m_pits.at(currentPitGlobalIndex) = ++currentStones;
            numStones--;
        }
    }
    
    if (currentPitGlobalIndex != opponentMancalaPitIndex &&
        currentPitGlobalIndex != getMancalaPitIndex(m_currentPlayer))
        capturePit(currentPitGlobalIndex);
    
    if (currentPitGlobalIndex != getMancalaPitIndex(m_currentPlayer))
        m_currentPlayer = m_currentPlayer == PLAYER_1 ? PLAYER_2 : PLAYER_1;
}

int MancalaBoard::getOppositePitIndex(int pitGlobalIndex)
{
    int opponentLocalIndex = pitGlobalIndex;
    if (m_currentPlayer == PLAYER_2) {
        opponentLocalIndex = (int)m_pits.size() - 1 - 1 - pitGlobalIndex; //extra -1 to skip mancala pit for player 2
    }
    Player opponent = m_currentPlayer == PLAYER_1 ? PLAYER_2 : PLAYER_1;
    return getGlobalPitIndex(opponentLocalIndex, opponent);
}

void MancalaBoard::capturePit(int pitGlobalIndex)
{
    if((m_currentPlayer == PLAYER_1 && pitGlobalIndex >= 0 && pitGlobalIndex < getMancalaPitIndex(PLAYER_1)) ||
       (m_currentPlayer == PLAYER_2 && pitGlobalIndex >= (getMancalaPitIndex(PLAYER_1) + 1) && pitGlobalIndex < getMancalaPitIndex(PLAYER_2)))
    {
        int oppositePitIndex = getOppositePitIndex(pitGlobalIndex);
        if (m_pits.at(pitGlobalIndex) == 1 &&
            (m_pits.at(oppositePitIndex) > 0))
        {
            long currentMancalaPitIndex = getMancalaPitIndex(m_currentPlayer);
            m_pits.at(currentMancalaPitIndex) += 1 + m_pits.at(oppositePitIndex);
            m_pits.at(oppositePitIndex) = 0;
            m_pits.at(pitGlobalIndex) = 0;
        }
    }
}

void MancalaBoard::updatePits(vector<int>& pits)
{
    m_pits = pits;
}

MancalaBoard* MancalaBoard::clone()
{
    MancalaBoard* clonedBoard = new MancalaBoard();
    
    clonedBoard->updatePits(m_pits);
    clonedBoard->setCurrentPlayer(getCurrentPlayer());
    clonedBoard->setNumPitsPerPlayer(getNumPitsPerPlayer());
    clonedBoard->setCutOffDepth(getCutOffDepth());
    
    return clonedBoard;
}

void MancalaBoard::run()
{
    switch (m_strategy) {
        case GREEDY:
        {
            GreedyPlayer* greedyAgent = new GreedyPlayer();
            deque<int> pitIndexes = greedyAgent->getPitWithMaximumGain(this);
            while (!pitIndexes.empty()) {
                int pitIndex = pitIndexes.front();
                pitIndexes.pop_front();
                play(pitIndex);
            }
            
            if (isGameOver())
            {
                adjustStonesOnGameOver();
            }
            
            serializeBoardState();
            
            delete greedyAgent;
            greedyAgent = NULL;
        
        }
            break;
        case MINI_MAX:
        {
            MiniMaxPlayer* miniMaxAgent = new MiniMaxPlayer();

            deque<int> pitIndexes = miniMaxAgent->getPitWithMaximumGain(this);
            while (!pitIndexes.empty()) {
                int pitIndex = pitIndexes.front();
                pitIndexes.pop_front();
                cout << pitIndex << endl;
                play(pitIndex);
            }
            
            if (isGameOver())
            {
                adjustStonesOnGameOver();
            }
            
            serializeBoardState();
            
            delete miniMaxAgent;
            miniMaxAgent = NULL;
        
        }
            break;
        case ALPHA_BETA:
        {
            AlphaBetaPlayer* alphaBetaAgent = new AlphaBetaPlayer();
            
            deque<int> pitIndexes = alphaBetaAgent->getPitWithMaximumGain(this);
            while (!pitIndexes.empty()) {
                int pitIndex = pitIndexes.front();
                pitIndexes.pop_front();
                cout << pitIndex << endl;
                play(pitIndex);
            }
            
            if (isGameOver())
            {
                adjustStonesOnGameOver();
            }
            
            serializeBoardState();
            
            delete alphaBetaAgent;
            alphaBetaAgent = NULL;
        }
            break;
            
        case COMPETITION:
        {
            TimedAlphaBetaPlayer* timedAlphaBetaAgent = new TimedAlphaBetaPlayer();
            deque<int> pitIndexes = timedAlphaBetaAgent->getPitWithMaximumGain(this);
            
            while (!pitIndexes.empty()) {
                int pitIndex = pitIndexes.front();
                pitIndexes.pop_front();
                FileUtils::writeMoveOutput(std_patch::to_string(pitIndex));
            }
            
            delete timedAlphaBetaAgent;
            timedAlphaBetaAgent = NULL;
        }
            break;
        default:
            break;
    }
}

bool MancalaBoard::isGameOver()
{
    for (int playerIndex = 0; playerIndex < 2; playerIndex++)
    {
        Player player = static_cast<Player>(playerIndex);
        bool noStonesLeft = true;
        for (int pitIndex = 0; pitIndex < m_numPitsPerPlayer; pitIndex++) {
            if (m_pits.at(getGlobalPitIndex(pitIndex, player)) > 0) {
                noStonesLeft = false;
            }
        }
        if (noStonesLeft) {
            return true;
        }
    }
    return false;
}

void MancalaBoard::adjustStonesOnGameOver()
{
    for (int playerIndex = 0; playerIndex < 2; playerIndex++)
    {
        Player player = static_cast<Player>(playerIndex);

        for (int pitIndex = 0; pitIndex < m_numPitsPerPlayer; pitIndex++) {
            int mancalaPitIndex = getMancalaPitIndex(player);
            int globalPitIndex = getGlobalPitIndex(pitIndex, player);
            m_pits.at(mancalaPitIndex) += m_pits.at(globalPitIndex);
            m_pits.at(globalPitIndex) = 0;
        }

    }
}

void MancalaBoard::serializeBoardState()
{
    int player2MancalaIndex = getMancalaPitIndex(PLAYER_2);
    int player1MancalaIndex = getMancalaPitIndex(PLAYER_1);
    
    string player1Output = "";
    int index = 0;
    for(; index < player1MancalaIndex; index++)
    {
        player1Output += std_patch::to_string(m_pits.at(index)) + string(" ");
    }
    
    player1Output = player1Output.substr(0, player1Output.length() - 1); //remove the trailing whitespace.
    
    string player2Output = "";
    
    for(int rIndex = player2MancalaIndex - 1; rIndex > player1MancalaIndex; rIndex--)
    {
        player2Output += std_patch::to_string(m_pits.at(rIndex)) + string(" ");
    }
    
    player2Output = player2Output.substr(0, player2Output.length() - 1); //remove the trailing whitespace.
    
    int player2Score = getStonesInPit(player2MancalaIndex);
    int player1Score = getStonesInPit(player1MancalaIndex);

    FileUtils::writeStateOutput(player2Output);
    FileUtils::writeStateOutput(player1Output);
    FileUtils::writeStateOutput(std_patch::to_string(player2Score));
    FileUtils::writeStateOutput(std_patch::to_string(player1Score));
}

std::string MancalaBoard::getPitIdentifier(int index, Player player)
{
    std::string id;
    if (player == PLAYER_1)
    {
        id = PLAYER_1_ID + std_patch::to_string(index + 2);
    }
    else
    {
        id = PLAYER_2_ID + std_patch::to_string(index + 2);
    }
    return id;
}

int MancalaBoard::evalBoard()
{
    int player1MancalaIndex = getMancalaPitIndex(PLAYER_1);
    int player2MancalaIndex = getMancalaPitIndex(PLAYER_2);
    
    if (MancalaManager::getInstance()->getPlayer() == PLAYER_1)
    {
        return getStonesInPit(player1MancalaIndex) - getStonesInPit(player2MancalaIndex);
    }
    
    return getStonesInPit(player2MancalaIndex) - getStonesInPit(player1MancalaIndex);
}

int MancalaBoard::getTotalStoneCount()
{
    int totalStones = 0;
    vector<int>::iterator itr = m_pits.begin();
    for (; itr != m_pits.end(); ++itr)
    {
        totalStones += (*itr);
    }
    return totalStones;
}

int MancalaBoard::evalBoardEx()
{
    int H1 = 0, H2 = 0, H3 = 0, H4 = 0, H5 = 0, H6 = 0;
    
    int player1MancalaIndex = getMancalaPitIndex(PLAYER_1);
    int player2MancalaIndex = getMancalaPitIndex(PLAYER_2);
    
    Player player = MancalaManager::getInstance()->getPlayer();
    
    if (player == PLAYER_1)
    {
        H1 = getStonesInPit(player1MancalaIndex) - getStonesInPit(player2MancalaIndex);
        H2 = (getTotalStoneCount() / 2) - getStonesInPit(player1MancalaIndex);
        H2 = H2 ? H2 > 0 : 0;
        H3 = (getTotalStoneCount() / 2) - getStonesInPit(player2MancalaIndex);
        H3 = H3 ? H3 > 0 : 0;
    }
    else
    {
        H1 = getStonesInPit(player2MancalaIndex) - getStonesInPit(player1MancalaIndex);
        H2 = (getTotalStoneCount() / 2) - getStonesInPit(player2MancalaIndex);
        H2 = H2 ? H2 > 0 : 0;
        H3 = (getTotalStoneCount() / 2) - getStonesInPit(player1MancalaIndex);
        H3 = H3 ? H3 > 0 : 0;
    }
    
    int totalPits = getNumPitsPerPlayer();
    for (int i = 0; i < totalPits; ++i)
    {
        if (i < totalPits / 3)
        {
            H5 += m_pits.at(getGlobalPitIndex(i, player));
        }
        else if (i >= totalPits / 3 && i < (totalPits * 2) / 3)
        {
            H6 += m_pits.at(getGlobalPitIndex(i, player));
        }
        else
        {
            H4 += m_pits.at(getGlobalPitIndex(i, player));
        }
    }
    
    return 27 * H1 + 18 * H2 + 18 * H3 + 16 * H4 + 15 * H5 + 16 * H6;
}

bool MancalaBoard::isPitValid(int pitLocalIndex)
{
    Player player = getCurrentPlayer();
    int pitIndex = getGlobalPitIndex(pitLocalIndex, player);
    if (getStonesInPit(pitIndex) == 0) {
        return false;
    }
    return true;
}

double MancalaBoard::getCutOffTime()
{
    return m_cutOffTime;
}

void MancalaBoard::setCutOffTime(double& cutOffTime)
{
    m_cutOffTime = cutOffTime;
}
