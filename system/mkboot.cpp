#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace fs = std::filesystem;

class SystemBoot {
private:
    std::string root_path;
    
    bool mount_virtual_filesystems() {
        std::vector<std::pair<std::string, std::string>> mounts = {
            {"/proc", "proc"},
            {"/sys", "sysfs"},
            {"/dev/pts", "devpts"}
        };

        for (const auto& [mount_point, fs_type] : mounts) {
            std::string full_path = root_path + mount_point;
            if (mount(fs_type.c_str(), full_path.c_str(), fs_type.c_str(), 0, nullptr) != 0) {
                std::cerr << "Failed to mount " << fs_type << " at " << full_path << std::endl;
                return false;
            }
        }
        return true;
    }

    bool start_microkernel() {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (chroot(root_path.c_str()) != 0) {
                std::cerr << "Failed to chroot" << std::endl;
                exit(1);
            }
            chdir("/");
            execl("/bin/unixMicroKernel", "unixMicroKernel", nullptr);
            std::cerr << "Failed to start microkernel" << std::endl;
            exit(1);
        } else if (pid < 0) {
            std::cerr << "Fork failed" << std::endl;
            return false;
        }
        return true;
    }

    bool start_init_process() {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (chroot(root_path.c_str()) != 0) {
                std::cerr << "Failed to chroot" << std::endl;
                exit(1);
            }
            chdir("/");
            execl("/bin/rxsh", "rxsh", nullptr);
            std::cerr << "Failed to start init process" << std::endl;
            exit(1);
        } else if (pid < 0) {
            std::cerr << "Fork failed" << std::endl;
            return false;
        }
        
        // Wait for init process
        int status;
        waitpid(pid, &status, 0);
        return true;
    }

public:
    SystemBoot(const std::string& path) : root_path(path) {}

    bool boot() {
        std::cout << "Starting Unix V Clone boot process..." << std::endl;
        
        if (!fs::exists(root_path)) {
            std::cerr << "Root path does not exist: " << root_path << std::endl;
            return false;
        }

        std::cout << "Mounting virtual filesystems..." << std::endl;
        if (!mount_virtual_filesystems()) {
            return false;
        }

        std::cout << "Starting microkernel..." << std::endl;
        if (!start_microkernel()) {
            return false;
        }

        std::cout << "Starting init process..." << std::endl;
        if (!start_init_process()) {
            return false;
        }

        return true;
    }

    void shutdown() {
        std::cout << "Shutting down..." << std::endl;
        
        // Unmount virtual filesystems in reverse order
        std::vector<std::string> unmounts = {
            "/dev/pts",
            "/sys",
            "/proc"
        };

        for (const auto& mount_point : unmounts) {
            std::string full_path = root_path + mount_point;
            umount(full_path.c_str());
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <chroot_path>" << std::endl;
        return 1;
    }

    SystemBoot boot(argv[1]);
    
    if (!boot.boot()) {
        std::cerr << "Boot process failed" << std::endl;
        return 1;
    }

    boot.shutdown();
    return 0;
}