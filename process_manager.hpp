#ifndef PROCESS_MANAGER_HPP 
#define PROCESS_MANAGER_HPP 
#include <unordered_map>
#include "process.hpp"
namespace shell
{
    class ProcessManager 
    {
    public:
        void jobs();
        void sleep(int secs);
        void resume(int id);
        void kill(int id);
        void exit();
        void suspend(int id);
        void wait(int pid);

    private:
        std::unordered_map<int, Process> process_pool_;


    };

}
#endif