#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <sys/wait.h>

#include "process_manager.hpp"
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
            // std::cout << " child process pid : " << getpid() << std::endl;
            executeFunc(proc);
            ::exit(1);
        }
        else
        {
            int status;
            process_pool_.insert(std::pair<pid_t, Process>(proc.id(), proc));
            if (proc.type() == ProcessType::KILL)
            {
                process_pool_.erase(stoi(proc.first_arg()));
            }
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
    std::thread ProcessManager::spawn()
    {
        return std::thread([&]
                           { execute(); });
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
        case ProcessType::SUSPEND:
            suspend(stoi(proc.first_arg()));
            break;
        case ProcessType::RESUME:
            resume(stoi(proc.first_arg()));
            break;
        case ProcessType::KILL:
            kill(stoi(proc.first_arg()));
            break;
        case ProcessType::EXIT:
            exit_shell();
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
                if (!it->second.isCompleted())
                {
                    int status;
                    pid_t ret = waitpid(it->first, &status, WNOHANG | WUNTRACED | WCONTINUED);
                    if (ret < 0)
                    {
                        // fprintf(stderr, "%s\n", explain_waitpid(it->first, status, WNOHANG));
                        std::cout << "waitpid failed " << errno << std::endl;
                    }
                    if (status != NULL)
                    {
                        if (WIFEXITED(status))
                        {
                            std::cout << "Closing " << it->first << std::endl;
                            process_pool_[it->first].complete();
                        }
                        else if (WIFSTOPPED(status))
                        {
                            it->second.stop();
                            it->second.setStatus('S');
                        }
                        else if (WIFCONTINUED(status))
                        {
                            it->second.setStatus('R');
                        }
                        // else if (WIFSIGNALED(status))
                        // {
                        //     std::cout << "Process: " << it->first << " is terminated by SIGTERM" << std::endl;
                        //     process_pool_.erase(it);
                        // }
                        else
                        {
                            // printf("status: %d\n", status);
                        }
                    }
                }
                it++;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
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
        std::cout << "#" << std::setw(10) << "PID" << std::setw(10) << "S" << std::setw(20) << "SEC" << std::setw(20) << "COMMAND" << std::endl;
        int active_index = 0;
        int complete_index = 0;
        int active_num = 0;
        for (auto p : process_pool_)
        {
            if (!p.second.isCompleted())
            {
                std::cout << active_index << std::setw(10) << p.second.id() << std::setw(10) << p.second.status() << std::setw(20) << p.second.command() << std::endl;
                active_num++;
                active_index++;
            }
        }
        std::cout << "Processes = " << active_num << " active" << std::endl;
        std::cout << "Completed processes:" << std::endl;
        for (auto p : process_pool_)
        {
            if (p.second.isCompleted())
            {
                std::cout << complete_index << std::setw(10) << p.second.id() << std::setw(10) << p.second.status() << std::setw(20) << p.second.command() << std::endl;
                complete_index++;
            }
        }
    }

    void ProcessManager::suspend(int id)
    {
        if (!process_pool_.count(id))
        {
            std::cerr << "Process: " << id << " does not exist" << std::endl;
        }
        ::kill(id, SIGTSTP);
    }

    void ProcessManager::resume(int id)
    {
        if (!process_pool_.count(id))
        {
            std::cerr << "Process: " << id << " does not exist" << std::endl;
        }
        ::kill(id, SIGCONT);
    }

    void ProcessManager::kill(int id)
    {
        if (!process_pool_.count(id))
        {
            std::cerr << "Process: " << id << " does not exist" << std::endl;
        }
        ::kill(id, SIGTERM);
    }

    void ProcessManager::wait(int id) {
       while(!process_pool_[id].isCompleted()){
           std::cout << "waiting pid: " << id << std::endl;
       }  
    }

    void ProcessManager::exit_shell()
    {
        for(auto p : process_pool_) {
            if(!p.second.isCompleted()) {
                wait(p.first);
            }
        }
        jobs();
        ::exit(0);

    }

    void ProcessManager::deleteProcess(pid_t pid)
    {
        process_pool_[pid].complete();
    }

}