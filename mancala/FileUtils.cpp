//
//  FileUtils.cpp
//  mancala
//
//  Created by Saurabh Mishra on 10/14/15.
//  Copyright © 2015. All rights reserved.
//
#include <iostream>
#include <fstream>
#include "FileUtils.hpp"

using namespace std;

namespace FileUtils {
    
    ofstream                m_stateWriter;
    ofstream                m_logger;
    ofstream                m_moveLogger;
    ofstream                m_infoLogger;
    
    void createStateOutputFile()
    {
        m_stateWriter.open("next_state.txt", std::ofstream::out | std::ofstream::trunc);
        if ( !m_stateWriter.is_open() ) {
            std::cerr << "Cannot create state output file...";
        }
    }
    
    void createLogOutputFile()
    {
        m_logger.open("traverse_log.txt", std::ofstream::out | std::ofstream::trunc);
        if ( !m_logger.is_open() ) {
            std::cerr << "Cannot create log output file...";
        }
    }
    
    void createMoveOutputFile()
    {
        m_moveLogger.open("output.txt", std::ofstream::out | std::ofstream::trunc);
        if ( !m_moveLogger.is_open() ) {
            std::cerr << "Cannot create log output file...";
        }
    }

    void createInfoOutputFile()
    {
        m_infoLogger.open("info.txt", std::ofstream::out | std::ofstream::trunc);
        if ( !m_infoLogger.is_open() ) {
            std::cerr << "Cannot create log output file...";
        }
    }
    
    void writeStateOutput(const std::string& output)
    {
        m_stateWriter << output << endl;
    }
    
    void writeLogOutput(const std::string& output)
    {
        m_logger << output << endl;
    }
    
    void writeMoveOutput(const std::string& output)
    {
        m_moveLogger << output << endl;
    }
    
    void writeInfoOutput(const std::string& output)
    {
        m_infoLogger << output << endl;
    }
    
    void exitInstance()
    {
        m_stateWriter.close();
        m_logger.close();
        m_moveLogger.close();
        m_infoLogger.close();
    }
}