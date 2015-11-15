//
//  FileUtils.hpp
//  mancala
//
//  Created by Saurabh Mishra on 10/14/15.
//  Copyright © 2015. All rights reserved.
//

#ifndef FileUtils_hpp
#define FileUtils_hpp

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

namespace std_patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

namespace FileUtils {
    extern ofstream                m_stateWriter;
    extern ofstream                m_logger;
    extern ofstream                m_moveLogger;
    extern ofstream                m_infoLogger;
    
    void createStateOutputFile();
    void createLogOutputFile();
    void createMoveOutputFile();
    void createInfoOutputFile();
    
    void writeStateOutput(const std::string& output);
    void writeLogOutput(const std::string& output);
    void writeMoveOutput(const std::string& output);
    void writeInfoOutput(const std::string& output);
    
    void exitInstance();
}
#endif /* FileUtils_hpp */
