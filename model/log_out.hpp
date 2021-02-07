#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <opnet.h>

#define TIME "slot_allocation.csv"
#define RELAY "relay.csv"
#define QUEUE "queue.csv"
#define REACH "reach.csv"


void log_write(const std::string dir, int num, ...)
{
    std::va_list arguments;
    std::ofstream outFile;
    outFile.open(dir, std::ios::app);
    outFile << op_sim_time() << ",";
    va_start(arguments, num);
    for (int i = 0; i < num; i++)
    {
        if (i == num - 1) 
        {
            outFile << va_arg(arguments, double) << std::endl;
        }
        else
        {
            outFile << va_arg(arguments, double) << ",";            
        }
    }
    va_end(arguments);
}