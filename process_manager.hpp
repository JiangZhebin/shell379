#ifndef PROCESS_MANAGER_HPP 
#define PROCESS_MANAGER_HPP 
#include <unordered_map>
#include <signal.h>
#include <thread>
#include "process.hpp"

namespace shell
{
    class ProcessManager 
    {
    public:
        void execute();
        int createProcess(Process &proc, bool is_backend);
        void deleteProcess(pid_t pid);
        std::thread spawn();
        void chooseProgram(Process proc);
        void executeFunc(const Process &proc);
        void enrichProcess(Process &proc);
        void jobs();
        void sleep(int secs);
        void resume(int id);
        void kill(int id);
        // void exit();
        void suspend(int id);
        void wait(int pid);
        void exit_shell();
        void system_cmd(const Process &proc);

    private:
        std::unordered_map<int, Process> process_pool_;


    };

}
#endif