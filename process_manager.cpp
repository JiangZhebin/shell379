#include "process_manager.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <sys/wait.h>
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
           proc.setSystemInfo(pid, std::chrono::system_clock::now());
           if (pid == -1) {
            perror("fork failed");
            ::exit(EXIT_FAILURE);
           }
           else if (pid == 0) {
            // enrichProcess(proc);
            std::cout << " child process pid : " << getpid() << std::endl;     
            executeFunc(proc);
            ::exit(EXIT_SUCCESS);
           }
           else {
            int status;
            std::cout << "main pid: " << getpid() << "  child pid:" << getpid() << "process pid " << proc.id() << std::endl;
            process_pool_.insert(std::pair<pid_t, Process> (proc.id(), proc));
            return pid;
           }
        }
        return -1;
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

    void ProcessManager::execute() {
        while(1) {
            // printf("executing\n");
            auto it = process_pool_.begin();
            while(it != process_pool_.end())
            {
                int status;
                std::cout << "wait for process" << std::endl;
                waitpid(it->first, &status, WNOHANG);
                std::cout <<"after wait" << std::endl;
                if(WIFEXITED(status) || WIFSIGNALED(status)) {
                  process_pool_.erase(it);  
                } 
                else if(WIFSTOPPED(status))
                {
                    it->second.stop();
                }
                else
                {
                    //TODO
                }
                it ++;

            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        }
    }

}