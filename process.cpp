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
        if (cmd_argument_.size() > 0)
        {
            cmd_name_ = cmd_argument_[0];
        }
        if (cmd_argument_.size() > 1)
        {
            for (auto i = 1; i < cmd_argument_.size(); i++)
            {
                checkInOutArgs(cmd_argument_[i]);
                args_.push_back(cmd_argument_[i]);
            }
        }
        chooseProgram();
        isBackend();
    }
    void Process::updateInputArgs(const std::string &args)
    {
        args_.clear();
        std::istringstream iss(args);
        std::string arg;
        while(iss >> arg)
        {
            args_.push_back(arg);
        }
        std::string cmd = cmd_argument_[0];
        cmd_argument_.resize(args_.size() + 1);
        cmd_argument_[0] = cmd;
        for(auto i = 1 ; i < cmd_argument_.size() ; i++) {
            cmd_argument_[i] = args_[i-1];
        }
    }
    void Process::chooseProgram()
    {
        if (cmd_name_ == "sleep")
        {
            type_ = ProcessType::SLEEP;
        }
        else if (cmd_name_ == "wait")
        {
            type_ = ProcessType::WAIT;
        }
        else if (cmd_name_ == "jobs")
        {
            type_ = ProcessType::JOBS;
        }
        else if (cmd_name_ == "suspend")
        {
            type_ = ProcessType::SUSPEND;
        }
        else if (cmd_name_ == "resume")
        {
            type_ = ProcessType::RESUME;
        }
        else if (cmd_name_ == "kill")
        {
            type_ = ProcessType::KILL;
        }
        else if (cmd_name_ == "exit")
        {
            type_ = ProcessType::EXIT;
        }
        else
        {
            type_ = ProcessType::SYS_CMD;
        }
    }
    void Process::isBackend()
    {
        if (cmd_argument_.back() == "&")
        {
            is_backend_ = true;
        }
        // else if (cmd_argument_.back()[0] == '>')
        // {
        //     is_fileoutput_ = true;
        //     std::string out_file = cmd_argument_.back();
        //     file_name_ = out_file.substr(1, out_file.size() - 1);
        // }
        // else if (cmd_argument_.back()[0] == '<')
        // {
        //     is_fileinput_ = true;
        //     std::string in_file = cmd_argument_.back();
        //     file_name_ = in_file.substr(1, in_file.size() - 1);
        // }
        // else
        // {
        // }
        // is_backend_ = false;
    }
    void Process::checkInOutArgs(const std::string &arg)
    {
        if (arg[0] == '>')
        {
            is_fileoutput_ = true;
            out_file_name_ = arg.substr(1, arg.size() - 1);
        }
        if (arg[0] == '<')
        {
            is_fileinput_ = true;
            in_file_name_ = arg.substr(1, arg.size() - 1);
        }
    }

    void Process::setSystemInfo(pid_t pid, std::chrono::system_clock::time_point current_time)
    {
        id_ = pid;
        start_time_ = current_time;
        status_ = 'R';
    }
}
