#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

class RxShell {
private:
    std::string prompt = "rxsh$ ";
    bool running = true;

    std::vector<std::string> split_command(const std::string& cmd) {
        std::vector<std::string> tokens;
        std::istringstream iss(cmd);
        std::string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void execute_command(const std::vector<std::string>& args) {
        if (args.empty()) return;

        if (args[0] == "exit") {
            running = false;
            return;
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            std::vector<char*> c_args;
            for (const auto& arg : args) {
                c_args.push_back(const_cast<char*>(arg.c_str()));
            }
            c_args.push_back(nullptr);

            execvp(args[0].c_str(), c_args.data());
            std::cerr << "rxsh: command not found: " << args[0] << std::endl;
            exit(1);
        } else if (pid > 0) {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        } else {
            std::cerr << "rxsh: fork failed" << std::endl;
        }
    }

public:
    void run() {
        char* input;
        while (running && (input = readline(prompt.c_str()))) {
            if (strlen(input) > 0) {
                add_history(input);
                std::string cmd(input);
                auto args = split_command(cmd);
                execute_command(args);
            }
            free(input);
        }
    }
};

int main() {
    RxShell shell;
    shell.run();
    return 0;
}