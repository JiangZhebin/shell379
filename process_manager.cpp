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
    int ProcessManager::createProcess(Process &proc, bool is_backend)
    {
        // if(!proc.is_backend()) {
        pid_t pid = fork();
        proc.setSystemInfo(pid, std::chrono::system_clock::now());
        if (pid == -1)
        {
            perror("fork failed");
            ::exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // enrichProcess(proc);
            std::cout << " child process pid : " << getpid() << std::endl;
            executeFunc(proc);
            ::exit(EXIT_SUCCESS);
        }
        else
        {
            int status;
            process_pool_.insert(std::pair<pid_t, Process>(proc.id(), proc));
            return pid;
        }
        // }
        // else {
        //     pid_t pid = fork();
        //     proc.setSystemInfo(pid, std::chrono::system_clock::now());
        //     if (pid == -1) {
        //         perror("fork failed");
        //         ::exit(EXIT_FAILURE);
        //     }
        //     else if(pid == 0) {
        //         executeFunc(proc);
        //     }
        return 0;
    }

    void ProcessManager::chooseProgram(Process proc)
    {
    }
    std::thread ProcessManager::spawn() {
        return std::thread([&] {execute();});
    }
    void ProcessManager::executeFunc(const Process &proc)
    {
        switch (proc.type())
        {
        case ProcessType::SLEEP:
            sleep(std::stoi(proc.first_arg()));
            break;
        case ProcessType::JOBS:
            jobs();
            break;
        default:
            std::cout << "Other commands" << std::endl;
        }
    }
    void ProcessManager::enrichProcess(Process &proc)
    {
        proc.setSystemInfo(getpid(), std::chrono::system_clock::now());
    }

    void ProcessManager::execute()
    {
        while (1)
        {
            // printf("executing\n");
            auto it = process_pool_.begin();
            while (it != process_pool_.end())
            {
                int status;
                // std::cout << "wait for process" << std::endl;
                pid_t ret = waitpid(it->first, &status, WNOHANG);
                // printf("status: %d\n", status);
                if (ret != 0)
                {
                    process_pool_.erase(it);
                    std::cout << "Closing " << it->first << std::endl;
                }
                else if (WIFSTOPPED(status))
                {
                    it->second.stop();
                    it->second.setStatus('S');
                }
                else
                {
                    // TODO
                }
                it++;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void ProcessManager::sleep(int secs)
    {
        std::cout << "start sleep" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(secs));
        std::cout << "end sleep" << std::endl;
    };

    void ProcessManager::jobs()
    {
        std::cout << "Running processes:" << std::endl;
        std::cout << "#" << std::setw(5) << "PID" << std::setw(6) << "S" << std::setw(3)  << "SEC" << std::setw(5) << "COMMAND" << std::endl;
        int num = 0;
        for(auto p : process_pool_)
        {
            std::cout << num << std::setw(5) << p.second.id() << std::setw(6) << p.second.status() << std::setw(3) << p.second.running_time() << std::setw(5) << p.second.command() << std::endl;  
        }
        std::cout << "Processes = " << process_pool_.size() << " active" << std::endl;
        std::cout << "Completed processes:" << std::endl;
        // std::cout << "User time = " << 
    }

    void ProcessManager::deleteProcess(pid_t pid) {
        process_pool_.erase(pid);
    }

}