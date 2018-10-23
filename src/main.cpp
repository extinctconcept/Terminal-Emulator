#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <chrono>
#include <iomanip>


void sh_loop();
void getInput(char* command, char* cmd[], char input[]);
void getptime(std::vector<double> timeArray, int counter);
std::vector<std::string> history;
std::vector<double> timeArray;
unsigned int counter = 0;

int main() {
	sh_loop();
	return 0;

}

void clean(char* cmd[]) {
	for(int i = 0; i < 256; i++) {
		cmd[i] = NULL;
	}
}

void sh_loop() {
	bool status = true;
	char *command = NULL;
	char *cmd[256];
	char input[256];
	pid_t child_pid;

	while(status) {
		clean(cmd);
		getInput(command, cmd, input);

		auto before = std::chrono::high_resolution_clock::now();

		if(strcmp("ptime", cmd[0]) == 0) {
			double val = timeArray[counter - 1];
			std::cout << "\nTime spent executing child processes: " << std::fixed << std::setprecision(4)  << val  << " seconds." << std::endl;
			clean(cmd);
		} else if (strcmp("exit", cmd[0]) == 0) {
			exit(0);
		} else if (strcmp("history",cmd[0]) == 0) {
			std::cout << "\n-- Command History --" << std::endl;
			for(unsigned int i = 0; i < history.size(); i++) {
				std::cout << history[i] << std::endl;
			}
		} else if(!strcmp("ptime", cmd[0]) == 0) {
			child_pid = fork();
			if(child_pid == 0) {
				execvp(cmd[0], cmd);
				std::cout << "Invalid input" << std::endl;
				exit(0);
			} else {
				wait(NULL);
			}
			

		}
		
		counter++;
		auto after = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> dur = after - before;
		timeArray.push_back(dur.count());
	}
}

void getInput(char* command, char* cmd[], char input[]) {
	std::cout << "[cmd]:  ";
	std::cin.getline(input,256);
	std::string str(input);	
	history.push_back(str);
	command = strtok(input, " ");
	int i = 0;

	while(command != NULL) {
		cmd[i] = command;
		i++;
		command = strtok(NULL, " ");
	}
}
