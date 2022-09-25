#include "process.hpp"
#include "process_manager.hpp"
#include <iostream>
int main()
{
    std::string command;
    std::cout<< "Hello World" << std::endl;
    std::getline(std::cin, command); 
    shell::Process p1(command);

    return 0;
}