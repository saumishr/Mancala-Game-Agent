//
//  mancala.cpp
//  mancala
//
//  Created by Saurabh Mishra on 10/9/15.
//  Copyright © 2015. All rights reserved.
//

#include <iostream>
#include "FileUtils.hpp"
#include "MancalaManager.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    if(argv[2])
    {
        std::string inputFilePath(argv[2]);
        FileUtils::createStateOutputFile();
        FileUtils::createLogOutputFile();
        FileUtils::createMoveOutputFile();
        
        MancalaManager* mancalaManager = MancalaManager::getInstance();
        mancalaManager->processTestFile(inputFilePath);
        mancalaManager->startPlay();
        FileUtils::exitInstance();
        MancalaManager::exitInstance();
    }
    return 0;
}
