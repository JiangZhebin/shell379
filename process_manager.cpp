#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <sys/wait.h>
#include <fstream>
#include <cstring>

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
            if (proc.isFileOutput())
            {
                std::ofstream out(proc.outFileName());
                std::streambuf *cont_buf = std::cout.rdbuf();
                std::cout.rdbuf(out.rdbuf());
                executeFunc(proc);
            }
            else if (proc.isFileInput())
            {
                std::ifstream in(proc.inFileName());
                std::streambuf *cin_buf = std::cin.rdbuf();
                std::cin.rdbuf(in.rdbuf());
                std::string input_args;
                std::getline(std::cin, input_args);
                proc.updateInputArgs(input_args);
                executeFunc(proc);
            }
            else
            {
                executeFunc(proc);
            }
            ::exit(1);
        }
        else
        {
            int status;
            process_pool_.insert(std::pair<pid_t, Process>(proc.id(), proc));
            if (proc.type() == ProcessType::KILL)
            {
                deleteProcess(stoi(proc.first_arg()));
                using_time_ += process_pool_[stoi(proc.first_arg())].running_time();
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
        case ProcessType::SYS_CMD:
            system_cmd(proc);
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
                    // update running time
                    int status;
                    if (0 == ::kill(it->first, 0))
                    {
                        it->second.updateRunningTime(getProcessTime(it->first));
                    }
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
                            using_time_ += it->second.running_time();
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

    double ProcessManager::getProcessTime(pid_t pid)
    {
        std::string result;
        std::array<char, 10> buffer;
        std::string time_cmd = "ps -p " + std::to_string(pid) + " -o etimes=";
        FILE *pipe = popen(time_cmd.data(), "r");
        if (!pipe)
        {
            std::cerr << "Couldn't start command." << std::endl;
        }
        while (fgets(buffer.data(), 10, pipe) != NULL)
        {
            result += buffer.data();
        }
        if(result == ""){
            std::cerr<< "empty string time"<< std::endl;
            return 0.0;
        }
        else {
            return std::stod(result);
        }
    }

    void ProcessManager::addProcessTime(pid_t pid)
    {
        using_time_ += getProcessTime(pid);
    }

    void ProcessManager::updateRunningTime(double t)
    {
    }

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
                // get running time
                double consumed_time = getProcessTime(p.first);
                std::cout << active_index << std::setw(10) << p.second.id() << std::setw(10) << p.second.status() << std::setw(20) << consumed_time << std::setw(20) << p.second.command() << std::endl;
                active_num++;
                active_index++;
            }
        }
        std::cout << "Processes = " << active_num << " active" << std::endl;
        std::cout << "Completed processes:" << std::endl;
        std::cout << "User time =" << std::setw(20) << using_time_ << std::setw(10) << "seconds" << std::endl;
        // for (auto p : process_pool_)
        // {
        //     if (p.second.isCompleted())
        //     {
        //         std::cout << complete_index << std::setw(10) << p.second.id() << std::setw(10) << p.second.status() << std::setw(20) << p.second.command() << std::endl;
        //         complete_index++;
        //     }
        // }
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

    void ProcessManager::wait(int id)
    {
        while (!process_pool_[id].isCompleted())
        {
            std::cout << "waiting pid: " << id << std::endl;
        }
    }

    void ProcessManager::exit_shell()
    {
        for (auto p : process_pool_)
        {
            if (!p.second.isCompleted())
            {
                wait(p.first);
            }
        }
        std::cout << "Resources used" << std::endl;
        std::cout << "User time = " << std::setw(5) << using_time_ << " seconds" << std::endl;
        ::exit(0);
    }

    void ProcessManager::system_cmd(const Process &proc)
    {
        printf("Using system cmd %s", proc.cmd_name().data());
        const char *cmd = proc.cmd_name().data();
        // char arguments[proc.args().size() + 1][MAX_LENGTH];

        char **args = (char **)malloc(proc.args().size() + 1 * MAX_LENGTH);
        for (auto i = 0; i < proc.full_cmd().size(); i++)
        {
            args[i] = (char *)proc.full_cmd()[i].data();
        }
        // strcpy(arguments[proc.args().size()], NULL);
        args[proc.full_cmd().size()] = NULL;
        int ret = execv(cmd, args);
        if (ret < 0)
        {
            std::cout << "execv error: " << errno << std::endl;
        }
    }

    void ProcessManager::deleteProcess(pid_t pid)
    {
        process_pool_[pid].complete();
    }
}