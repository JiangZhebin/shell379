#include "process.hpp"
#include <iostream>

namespace shell
{
    Process::Process(std::string cmd)
    {
        total_cmd_ = cmd;
        parseCommand(cmd);
    }

    bool Process::parseCommand(const std::string &cmd)
    {
        std::istringstream iss(cmd);
        std::string arg;
    while(iss >> arg) {
            cmd_argument_.push_back(arg);
            std::cout << arg << std::endl;
    }
    return true;
    }
}
