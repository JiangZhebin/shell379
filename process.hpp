#ifndef PROCESS_HPP
#define PROCESS_HPP
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <iostream>
namespace shell
{
    enum class ProcessType
    {
        NONE = 0,
        SLEEP,
        WAIT,
        SUSPEND
    };
    class Process
    {
    public:
        Process(std::string command);
        void parseCommand(const std::string &cmd);
        void chooseProgram();
        void setpid(pid_t pid)
        {
            id_ = pid;
        }
        std::string cmd_name()
        {
            return cmd_name_;
        }
        bool is_backend()
        {
            return is_backend_;
        }
        ProcessType type() const
        {
            return type_;
        }
        void setSystemInfo(pid_t pid, std::chrono::system_clock::time_point current_time);

        std::string first_arg() const
        {
            if (cmd_argument_.size() >= 2)
                return cmd_argument_[1];
            else
                std::cerr << "No first argument available" << std::endl;
        }
        std::string second_arg() const
        {
            if (cmd_argument_.size() >= 3)
                return cmd_argument_[2];
            else
                std::cerr << "No second argument available" << std::endl;
        }

    private:
        std::string name_;
        pid_t id_;
        ProcessType type_;
        uint64_t running_time_;
        std::chrono::system_clock::time_point start_time_;
        uint64_t end_time_;
        bool is_backend_;
        std::string total_cmd_;
        std::string cmd_name_;
        std::vector<std::string> cmd_argument_;
    };

}
#endif