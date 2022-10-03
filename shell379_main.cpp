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
        std::cout<<"Input your command :" << std::endl;
        Process p1(command);
        process_manager.createProcess(p1, false);
        if(!p1.is_backend()) {
            int status;
            waitpid(p1.id(), &status, 0);
            process_manager.deleteProcess(p1.id());
        }
        else {
            continue;
        }

    }

    return 0;
}