#include "process.hpp"
#include <iostream>

namespace shell
{
    Process::Process(std::string cmd)
    {
        total_cmd_ = cmd;
        parseCommand(cmd);
    }

    void Process::parseCommand(const std::string &cmd)
    {
        cmd_argument_.clear();
        std::istringstream iss(cmd);
        std::string arg;
        while (iss >> arg)
        {
            cmd_argument_.push_back(arg);
            // std::cout << arg << std::endl;
        }
        if (arg.size() > 0)
        {
            cmd_name_ = cmd_argument_[0];
        }
        else
        {
            std::cerr << "No input command" << std::endl;
        }
        chooseProgram();
        checkBackend();
    }
    void Process::chooseProgram() {
        if( cmd_name_ == "sleep") {
            type_ = ProcessType::SLEEP;
            
        } else if (cmd_name_ == "wait") {
           type_ = ProcessType::WAIT; 
        }
        else if (cmd_name_ == "jobs") {
            type_ = ProcessType::JOBS;
        }
        else if (cmd_name_ == "suspend" ){
            type_ = ProcessType::SUSPEND;
        } 
        else if (cmd_name_ == "resume"){
            type_ = ProcessType::RESUME;
        }
        else {
            type_ = ProcessType::NONE;
        }
    }
    void Process::checkBackend() {
        if(cmd_argument_.back() == "&")
            is_backend_ = true;
        else 
            is_backend_ = false;
    }
    void Process::setSystemInfo(pid_t pid, std::chrono::system_clock::time_point current_time) {
        id_ = pid;
        start_time_ = current_time; 
        status_ = 'R';
    }
}
