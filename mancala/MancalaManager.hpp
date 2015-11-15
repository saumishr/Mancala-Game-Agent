//
//  MancalaManager.hpp
//  mancala
//
//  Created by Saurabh Mishra on 10/9/15.
//  Copyright © 2015. All rights reserved.
//

#ifndef MancalaManager_hpp
#define MancalaManager_hpp

#include <iostream>
#include "MancalaBoard.hpp"

class MancalaManager
{
public:
    MancalaManager();
    ~MancalaManager();

    void                    processTestFile(std::string testFile);
    static MancalaManager*  getInstance();
    static void             exitInstance();
    MancalaBoard*           getMancalaBoard();
    Player                  getCurrentPlayer();
    void                    startPlay();
    void                    setPlayer(Player player);
    Player                  getPlayer();
private:
    std::string             m_testFilePath;
    static MancalaManager  *m_spInstance;
    MancalaBoard           *m_mancalaBoard;
    Player                  m_player;
};

#endif /* MancalaManager_hpp */
