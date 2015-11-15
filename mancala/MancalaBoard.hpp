//
//  MancalaBoard.hpp
//  mancala
//
//  Created by Saurabh Mishra on 10/9/15.
//  Copyright © 2015. All rights reserved.
//

#ifndef MancalaBoard_hpp
#define MancalaBoard_hpp

#include <iostream>
#include <vector>
#include <deque>

using namespace std;

enum Player {PLAYER_NONE = -1, PLAYER_1, PLAYER_2};
enum Strategy {GREEDY = 1, MINI_MAX, ALPHA_BETA, COMPETITION};

class MancalaBoard
{
public:
    MancalaBoard();
    ~MancalaBoard();
    
    MancalaBoard*   clone();
    void            initializeBoard(std::vector<int>& firstPlayerStones, std::vector<int>& secondPlayerStones,
                         int mancalaStones1, int mancalaStones_2, int strategy, int cutOffDepth);
    void            setMancalaPit(int numStones, Player player);
    void            setCurrentPlayer(Player currentPlayer);
    Player          getCurrentPlayer();
    void            setPreviousPlayer(Player currentPlayer);
    Player          getPreviousPlayer();
    void            setCutOffDepth(int depth);
    int             getCutOffDepth();
    void            play(int pitIndex);
    int             getGlobalPitIndex(int pitLocalIndex, Player player);
    static void     setNumPitsPerPlayer(int numPits);
    static int      getNumPitsPerPlayer();
    int             getMancalaPitIndex(Player player);
    void            capturePit(int pitGlobalIndex);
    int             getOppositePitIndex(int pitIndex);
    void            updatePits(vector<int>& pits);
    int             getStonesInPit(int pitIndex);
    Strategy        getGameStrategy() const;
    void            run();
    bool            isGameOver();
    void            adjustStonesOnGameOver();
    void            serializeBoardState();
    std::string     getPitIdentifier(int index, Player player);
    bool            isPitValid(int pitLocalIndex);
    int             evalBoard();
    int             evalBoardEx();
    int             getGameIndex() const;
    void            setGameIndex(int gameIndex);
    int             getGameDepth() const;
    void            setGameDepth(int depth);
    double          getCutOffTime();
    void            setCutOffTime(double& cutOffTime);
    int             getTotalStoneCount();
    
    int             m_expectedValue;
    bool            m_isGrouped;
    int             m_alpha;
    int             m_beta;
    deque<int>      m_bonusMoves;
private:
    vector<int>     m_pits;
    Player          m_currentPlayer;
    Player          m_prevPlayer;
    int             m_cutOffDepth;
    double          m_cutOffTime;
    int             m_gameIndex;
    int             m_gameDepth;
    static int      m_numPitsPerPlayer;
    Strategy        m_strategy;
};


#endif /* MancalaBoard_hpp */
