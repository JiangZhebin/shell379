#ifndef PROCESS_HPP
#define PROCESS_HPP
#include <string>
#include <sstream>
#include <vector>
namespace shell
{
    class Process
    {
    public:
        Process(std::string command);
        bool parseCommand(const std::string &cmd);

    private:
        std::string name_;
        int id_;
        uint64_t running_time_;
        uint64_t start_time_;
        uint64_t end_time_;
        bool is_backend_;
        std::string total_cmd_;
        std::string cmd_name_;
        std::vector<std::string> cmd_argument_;
    };

}
#endif