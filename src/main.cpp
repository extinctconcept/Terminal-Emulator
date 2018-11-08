#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <cstring>
#include <vector>
#include <chrono>
#include <iomanip>
#include <error.h>


void sh_loop();
void getInput(char* command, char* cmd[], char input[]);
void getptime(std::vector<double> timeArray, int counter);
int main();
std::vector<std::string> history;
std::vector<double> timeArray;
unsigned int counter = 0;
bool status = true;

void sigintHandler(int sig_num __attribute__((unused))) {
	signal(SIGINT, sigintHandler);
	status = false;
	main();
} 

int main() {
	sh_loop();
	return 0;

}

void clean(char* cmd[], char input[256]) {
	for(int i = 0; i < 256; i++) {
		cmd[i] = NULL;
		input[i] = ' ';
	}
}

void sh_loop() {
	char *command = NULL;
	char *cmd[256];
	char input[256];
	pid_t child_pid;

	while(status) {
		signal(SIGINT, sigintHandler);
		clean(cmd, input);
		getInput(command, cmd, input);

		auto before = std::chrono::high_resolution_clock::now();

		if (strcmp("^", cmd[0]) == 0) {
			int val = (atoi(cmd[1]) - 1);
			std::string str(history[val]);	
			history.push_back(str);
			clean(cmd, input);

			for(unsigned int i = 0; i <= str.size(); i++) {
				input[i] = str[i];
			}
			command = strtok(input, " ");
			int i = 0;

			while(command != NULL) {
				cmd[i] = command;
				i++;
				command = strtok(NULL, " ");
			}
		
			if(strcmp("cd", cmd[0]) == 0 && i == 1) {
				std::cout << "Error: The cd command must have parameters I.E. cd ../..\n" 
					  << "if you are trying to access the HOME directory use cd ~ \n";
				clean(cmd, input);
				getInput(command, cmd, input);
			}

		}

		if(strcmp("ptime", cmd[0]) == 0) {
			double val = timeArray[counter - 1];
			std::cout << "\nTime spent executing child processes: " << std::fixed << std::setprecision(4)  << val  << " seconds." << std::endl;
			clean(cmd, input);
		} else if (strcmp("exit", cmd[0]) == 0) {
			exit(0);
		} else if (strcmp("history",cmd[0]) == 0) {
			std::cout << "\n-- Command History --" << std::endl;
			for(unsigned int i = 0; i < history.size(); i++) {
				std::cout << history[i] << std::endl;
			}
		} else if(strcmp("cd",cmd[0]) == 0) {
			try {
				if(strcmp("~", cmd[1]) == 0) {
					chdir(getenv("HOME"));
				} else {
					chdir(cmd[1]);
				}
			} catch (...) {
				std::cerr << "Exception Caught : note \"cd\" alone is not valid to access the home directory use \"cd ~\""  << std::endl;
			}
		} else if (strcmp("cwd", cmd[0]) == 0) {
			char cwd[4096];
			if (getcwd(cwd, sizeof(cwd)) != NULL) {
				std::cout << cwd << std::endl;
			} else {
				perror("getcwd() error");
			}
		} else if (!strcmp("ptime", cmd[0]) == 0) {
			const int READ = 0;
			const int WRITE = 1;
			char *tmp[256];
			int val = 0;
			int wstatus;
			int p[2];
			if(pipe(p) != 0) {
				std::cerr << strerror(errno) << std::endl;
			}

			for(int i = 0; cmd[i] != '\0'; i++) {
				if(strcmp("|", cmd[i]) != 0) {
					tmp[i] = cmd[i];
				} else {
					break;
				}
			}

			pid_t child = fork();
			if(child == 0) {
				close(p[READ]);
				dup2(p[WRITE], STDOUT_FILENO);
				execvp(tmp[0], tmp);
			} 


			// split commands if pipe char exists
			while(cmd[val] != '\0') {
				if(strcmp("|", cmd[val]) == 0) {
					char *tmp[256];
					int curr = 0;
					for(int j = val + 1; cmd[j] != '\0' && (strcmp("|", cmd[j]) != 0); j++) {
						tmp[val] = cmd[j];
						curr++;
					}

					pid_t inside = fork();	
					if(inside == 0) {
						dup2(p[READ], STDIN_FILENO);
						execvp(tmp[0], tmp);
					}

					val++;
				}
				val++;
			}

			wait(NULL);
			close(p[READ]);
			close(p[WRITE]);
		}

		
		counter++;
		auto after = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> dur = after - before;
		timeArray.push_back(dur.count());
	}

	if(!status) {
		status = true;
		std::cout << '\n';
		sh_loop();
	}


}

void getInput(char* command, char* cmd[], char input[]) {
	char cwd[4096];
	std::cout <<  getcwd(cwd, sizeof(cwd))<< " $  ";
	std::cin.getline(input,256);
	std::string str(input);
	if(str.find_first_not_of(' ') != std::string::npos) {
		history.push_back(str);
		command = strtok(input, " ");
		int i = 0;
		

		while(command != NULL) {
			cmd[i] = command;
			i++;
			command = strtok(NULL, " ");
		}
		if(strcmp("cd", cmd[0]) == 0 && i == 1) {
			std::cout << "Error: The cd command must have parameters I.E. cd ../..\n" 
				  << "if you are trying to access the HOME directory use cd ~ \n";
			clean(cmd, input);
			getInput(command, cmd, input);
		}
	} else {
		std::cout << "input cannot be NULL" << std::endl;
		clean(cmd, input);
		getInput(command, cmd, input);
	}
}

