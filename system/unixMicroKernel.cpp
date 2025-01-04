
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

class UnixMicroKernel {
private:
    struct Process {
        pid_t pid;
        std::string name;
        int priority;
        bool running;
    };
    
    std::map<pid_t, std::shared_ptr<Process>> processes;
    std::string root_path;

public:
    UnixMicroKernel(const std::string& chroot_path) : root_path(chroot_path) {}

    int sys_fork() {
        pid_t pid = fork();
        if (pid > 0) {
            // Parent process
            auto proc = std::make_shared<Process>();
            proc->pid = pid;
            proc->running = true;
            processes[pid] = proc;
        }
        return pid;
    }

    int sys_exec(const std::string& path, const std::vector<std::string>& args) {
        std::vector<char*> c_args;
        for (const auto& arg : args) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        return execvp(path.c_str(), c_args.data());
    }

    int sys_chroot() {
        return chroot(root_path.c_str());
    }

    void init_system() {
        if (sys_chroot() != 0) {
            std::cerr << "Failed to chroot to " << root_path << std::endl;
            exit(1);
        }

        // Change to root directory after chroot
        chdir("/");
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <chroot_path>" << std::endl;
        return 1;
    }

    UnixMicroKernel kernel(argv[1]);
    kernel.init_system();
    
    return 0;
}