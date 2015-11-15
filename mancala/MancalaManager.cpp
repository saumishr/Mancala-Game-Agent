//
//  MancalaManager.cpp
//  mancala
//
//  Created by Saurabh Mishra on 10/9/15.
//  Copyright © 2015. All rights reserved.
//
#include <fstream>
#include <sstream>
#include <limits.h>
#include "MancalaManager.hpp"

using namespace std;

MancalaManager *MancalaManager::m_spInstance = NULL;

MancalaManager::MancalaManager() : m_testFilePath("")
{
    m_mancalaBoard = new MancalaBoard();
}

MancalaManager::~MancalaManager()
{
    if(m_mancalaBoard)
    {
        delete m_mancalaBoard;
        m_mancalaBoard = NULL;
    }
}

MancalaManager* MancalaManager::getInstance() {
    if (m_spInstance == NULL)
        m_spInstance = new MancalaManager();
    
    return m_spInstance;
}

void MancalaManager::exitInstance() {
    if (m_spInstance)
        delete m_spInstance;
    
    m_spInstance = NULL;
}

MancalaBoard* MancalaManager::getMancalaBoard()
{
    return m_mancalaBoard;
}

void MancalaManager::startPlay()
{
    if (m_mancalaBoard)
        m_mancalaBoard->run();
}

void MancalaManager::processTestFile(std::string testFile) {
    m_testFilePath = testFile;
    
    std::ifstream fileReader(m_testFilePath.c_str());
    
    std::string line;
    std::getline(fileReader, line);
    std::istringstream iss(line);
    
    int strategy = 0;
    if (!(iss >> strategy)) {
        cout <<  "Unexpected Input Test File Format!!";
    }
 
    iss.clear();
    std::getline(fileReader, line);
    iss.str(line);
    int currentPlayer = 0;
    if (!(iss >> currentPlayer)) {
        cout <<  "Unexpected Input Test File Format!!";
    }
    
    iss.clear();
    std::getline(fileReader, line);
    iss.str(line);
    int cutoffDepth = 0;
    double cutOffTime;
    
    if (strategy == 4)
    {
        if (!(iss >> cutOffTime)) {
            cout <<  "Unexpected Input Test File Format!!";
        }
    }
    else
    {
        if (!(iss >> cutoffDepth)) {
            cout <<  "Unexpected Input Test File Format!!";
        }
    }
    
    iss.clear();
    std::getline(fileReader, line);
    iss.str(line);
    int player2StoneCount;
    std::vector<int> player2Stones;
    int totalPitsPerPlayer = 0;
    while(iss >> player2StoneCount) {
        player2Stones.push_back(player2StoneCount);
        totalPitsPerPlayer++;
    }

    iss.clear();
    std::getline(fileReader, line);
    iss.str(line);
    int player1StoneCount;
    std::vector<int> player1Stones;
    while(iss >> player1StoneCount) {
        player1Stones.push_back(player1StoneCount);
    }
    
    iss.clear();
    std::getline(fileReader, line);
    iss.str(line);
    int mancalaStones2 = 0;
    if (!(iss >> mancalaStones2)) {
        cout <<  "Unexpected Input Test File Format!!";
    }
    
    iss.clear();
    std::getline(fileReader, line);
    iss.str(line);
    int mancalaStones1 = 0;
    if (!(iss >> mancalaStones1)) {
        cout <<  "Unexpected Input Test File Format!!";
    }
    
    MancalaBoard* mancalaBoard = getMancalaBoard();
    mancalaBoard->initializeBoard(player1Stones, player2Stones, mancalaStones1, mancalaStones2, strategy, cutoffDepth);
    mancalaBoard->setCurrentPlayer(static_cast<Player>(currentPlayer - 1));
    setPlayer(static_cast<Player>(currentPlayer - 1));
    mancalaBoard->setNumPitsPerPlayer(totalPitsPerPlayer);
    mancalaBoard->setGameIndex(0);
    mancalaBoard->setGameDepth(0);
    mancalaBoard->m_expectedValue = INT_MIN;
    
    if (strategy == 4)
    {
        mancalaBoard->setCutOffTime(cutOffTime);
    }
    
    fileReader.close();
}

void MancalaManager::setPlayer(Player player)
{
    m_player = player;
}

Player MancalaManager::getPlayer()
{
    return m_player;
}
