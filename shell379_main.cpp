#include "process.hpp"
#include "process_manager.hpp"
#include <iostream>
using namespace shell;
int main()
{
    ProcessManager process_manager; 
    std::string command;
    std::cout<< "Hello World" << std::endl;
    std::getline(std::cin, command); 
    Process p1(command);
    process_manager.createProcess(p1, false);

    return 0;
}