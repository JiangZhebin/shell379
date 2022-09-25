#include "process_manager.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <iomanip>
#include <thread>
namespace shell
{
    void ProcessManager::sleep(int secs){
        std::cout << "start sleep" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(secs));
        std::cout<<"end sleep" << std::endl;
    };
    int ProcessManager::createProcess(Process &proc, bool is_backend)
    {
        if(!proc.is_backend()) {
           pid_t pid = fork();
           if (pid == -1) {
            perror("fork failed");
            ::exit(EXIT_FAILURE);
           }
           else if (pid == 0) {
            proc.setpid(getpid());            
            executeFunc(proc);
            ::exit(EXIT_SUCCESS);
           }
           else {
            int status;
            waitpid(pid, &status, 0 );
           }
           return EXIT_SUCCESS;
            
        }
       return 0; 
    }

    void ProcessManager::chooseProgram(Process proc) {
        
    }
    void ProcessManager::executeFunc(const Process &proc) {
       switch(proc.type()) {
        case ProcessType::SLEEP :
            sleep(std::stoi(proc.first_arg()));
            break;
        default:
            std::cout<< "Other commands" << std::endl;
       } 
    }
    void ProcessManager::enrichProcess(Process &proc) {
        proc.setSystemInfo(getpid(), std::chrono::system_clock::now());
    }

}