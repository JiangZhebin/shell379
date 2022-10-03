#include "process.hpp"
#include "process_manager.hpp"
#include <iostream>
#include <thread>
#include <sys/types.h>
#include <sys/wait.h>
using namespace shell;
int main()
{
    ProcessManager process_manager;
    std::thread process_manager_daemon(&ProcessManager::execute, &process_manager);
    // process_manager.spawn();
    std::string command;
    std::cout << "Enter shell379" << std::endl;
    while (std::getline(std::cin, command))
    {
        std::cout << "Input your command :" << std::endl;
        Process p1(command);
        if (p1.type() == ProcessType::EXIT)
        {
            process_manager.exit_shell();
        }
        else if (p1.type() == ProcessType::WAIT) {
            process_manager.wait(stoi(p1.first_arg()));
        }
        else
        {
            process_manager.createProcess(p1, false);
            if (!p1.is_backend())
            {
                int status;
                waitpid(p1.id(), &status, 0);
                process_manager.deleteProcess(p1.id());
                process_manager.using_time_ += process_manager.process_pool_[p1.id()].running_time(); 
            }
            else
            {
                continue;
            }
        }
    }

    return 0;
}