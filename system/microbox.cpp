#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>

namespace fs = std::filesystem;

class MicroBox {
private:
    using CommandFunc = std::function<int(const std::vector<std::string>&)>;
    std::map<std::string, CommandFunc> commands;
    std::string program_name;

    // Command implementations
    int cmd_ls(const std::vector<std::string>& args) {
        std::string path = args.size() > 0 ? args[0] : ".";
        try {
            for (const auto& entry : fs::directory_iterator(path)) {
                std::cout << entry.path().filename().string() << "  ";
            }
            std::cout << std::endl;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "ls: cannot access '" << path << "': " << e.what() << std::endl;
            return 1;
        }
        return 0;
    }

    int cmd_cat(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "cat: missing operand" << std::endl;
            return 1;
        }

        for (const auto& file : args) {
            std::ifstream ifs(file);
            if (!ifs) {
                std::cerr << "cat: " << file << ": No such file" << std::endl;
                return 1;
            }
            std::cout << ifs.rdbuf();
        }
        return 0;
    }

    int cmd_pwd(const std::vector<std::string>& args) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            std::cout << cwd << std::endl;
            return 0;
        }
        std::cerr << "pwd: error getting current directory" << std::endl;
        return 1;
    }

    int cmd_mkdir(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "mkdir: missing operand" << std::endl;
            return 1;
        }

        for (const auto& dir : args) {
            if (mkdir(dir.c_str(), 0755) != 0) {
                std::cerr << "mkdir: cannot create directory '" << dir << "': "
                         << strerror(errno) << std::endl;
                return 1;
            }
        }
        return 0;
    }

    int cmd_rm(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "rm: missing operand" << std::endl;
            return 1;
        }

        for (const auto& path : args) {
            if (std::remove(path.c_str()) != 0) {
                std::cerr << "rm: cannot remove '" << path << "': "
                         << strerror(errno) << std::endl;
                return 1;
            }
        }
        return 0;
    }

    int cmd_whoami(const std::vector<std::string>& args) {
        uid_t uid = geteuid();
        struct passwd *pw = getpwuid(uid);
        if (pw) {
            std::cout << pw->pw_name << std::endl;
            return 0;
        }
        std::cerr << "whoami: cannot find name for user ID " << uid << std::endl;
        return 1;
    }

public:
    MicroBox() {
        commands["ls"] = [this](const auto& args) { return cmd_ls(args); };
        commands["cat"] = [this](const auto& args) { return cmd_cat(args); };
        commands["pwd"] = [this](const auto& args) { return cmd_pwd(args); };
        commands["mkdir"] = [this](const auto& args) { return cmd_mkdir(args); };
        commands["rm"] = [this](const auto& args) { return cmd_rm(args); };
        commands["whoami"] = [this](const auto& args) { return cmd_whoami(args); };
    }

    int run(int argc, char* argv[]) {
        if (argc < 1) return 1;
        
        program_name = fs::path(argv[0]).filename().string();
        std::string cmd = program_name;
        
        // If called as 'microbox', first argument is the command
        if (cmd == "microbox" && argc > 1) {
            cmd = argv[1];
            argv++;
            argc--;
        }

        // Convert remaining args to vector
        std::vector<std::string> args;
        for (int i = 1; i < argc; i++) {
            args.push_back(argv[i]);
        }

        auto it = commands.find(cmd);
        if (it != commands.end()) {
            return it->second(args);
        }

        std::cerr << cmd << ": command not found" << std::endl;
        return 127;
    }
};

int main(int argc, char* argv[]) {
    MicroBox mb;
    return mb.run(argc, argv);
}