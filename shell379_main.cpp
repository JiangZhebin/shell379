#include "process.hpp"
#include "process_manager.hpp"
#include <iostream>
#include <thread>
using namespace shell;
int main()
{
    ProcessManager process_manager;
    std::thread process_manager_daemon(&ProcessManager::execute, process_manager);
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
            std::cout << "After waitpid" << std::endl;
        }
    }

    return 0;
}