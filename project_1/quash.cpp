#include "quash.hpp"
#include "jobs.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cerrno>
#include <sys/wait.h>
#include <sys/param.h>
#include <readline/readline.h>
#include <vector>
#include <signal.h>
#include <inttypes.h>
#include <algorithm>

static int numJobs;
std::vector<Job> jobList;


std::vector<std::string> parse(std::string cmd_line) {
	std::vector<std::string> parsed_string;
	std::string token;
	std::string delimiter = " ";
	int pos = 0;
	while ((pos = cmd_line.find(delimiter)) != std::string::npos) {
    		token = cmd_line.substr(0, pos);
    		parsed_string.push_back(token);
		cmd_line.erase(0, pos + delimiter.length());
	}	
	return parsed_string;
}

void pwd() {
	char buff[1024];
	getcwd(buff, 1024);
	std::cout << buff << "\n";
}


void cd(std::string dest) {
	if(dest == "") {
		if(chdir(getenv("HOME")) == -1) {
			std::cout << "NO HOME DIRECTORY AT PATH : " << getenv("HOME") << "\n";
		}
		pwd();
	}
	else if(chdir(dest.c_str()) == -1) {
		std::cout << "ENTER A BETTER PATH DUMMY " << "\n";
	}
	else {
		pwd();
	}
}

void flush_jobs() {
	for (int i = 0; i < jobList.size(); i++) {
		if (jobList[i].running == false) {
			jobList.erase (jobList.begin()+i);	
		}
	}
}

void jobs() {
	flush_jobs();
	std::cout << "JOB ID" << "     " <<  "PID" << "        " << "cmd" << "\n";
	for (auto job : jobList) {
		std::cout << job.pjobid << "     " << job.pid << "     " << job.cmd << "\n";
	}	

}

void set(std::string cmd_line) {
	std::vector<std::string> commandList = parse(cmd_line);
	std::string command = commandList[1];
	std::string delimiter = "=";
	std::string env = command.substr(0, command.find(delimiter));
	std::string loc = command.substr(command.find(delimiter)+1, command.size());
	
	if ((getenv("HOME") != env) && (getenv("PATH") != env)) {
		std::cout << "We can only set HOME and PATH environments" << "\n";
		return;
	}
	
	if (getenv(env.c_str()) == loc) {
		std::cout << "The environment is already set to " << loc << "\n";
	}
	else {
		setenv(env.c_str(), loc.c_str(), 1);
		std::cout << "Successfully set " << env << " variable to " << loc << "\n";
	}	
}

std::string get_path(std::string cmd) {
	std::vector<std::string> path_string;
	std::string path = getenv("PATH");
        std::string token;
        std::string delimiter = ":";
        int pos = 0;
        while ((pos = path.find(delimiter)) != std::string::npos) {
                token = path.substr(0, pos);
                path_string.push_back(token);
                path.erase(0, pos + delimiter.length());
        }
	for( auto path : path_string) {
		path += cmd;
		if (access(path.c_str(),X_OK) == 0) {
			return path;
		}
	}
	return NULL;
}

void pipe(std::string cmd) {
	std::vector<std::string> args;
	std::string before_pipe, after_pipe;
	before_pipe = cmd.substr(0,cmd.find("|"));
	cmd.erase(0, cmd.find("|")+1);
	after_pipe = cmd;
	args = parse(before_pipe);
	before_pipe = get_path(args[0]);

	if(before_pipe == "") {
		if(access(args[0].c_str(), X_OK) == 0) {
			before_pipe = args[0];
		}
		else {
			std::cout << "Command was not found " << "\n";
			return;
		}
	}
	char* arr[args.size()];
	
	for (int i = 0; i < args.size(); i++ ) {
		char cstr[args[i].size() + 1];
		strcpy(cstr, args[i].c_str());
		arr[i] = cstr;
	}
	pid_t pid_1, pid_2;
	
	int fds[2], status;
	pipe(fds);

	pid_1 = fork();
	if (pid_1==0) {
		dup2(fds[1], STDOUT_FILENO);
    		close(fds[0]);
    		close(fds[1]);

    		if(execv(before_pipe.c_str(), arr) < 0) {
      			std::cout << "Unnable to execute commands before pipe. ERROR\n";
		}

   		exit(0);
	}

	pid_2 = fork();
	if (pid_2==0) {
		dup2(fds[0], STDIN_FILENO);
    		close(fds[0]);
    		close(fds[1]);

    		fflush(stdout);

    		handle(after_pipe);

    		exit(0);
	}

	close(fds[0]);
  	close(fds[1]);

  	if((waitpid(pid_1, &status, 0)) == -1) {
    		std::cout << "Process 1 encountered an error. ERROR";
  	}
  	if((waitpid(pid_2, &status, 0)) == -1) {
    		std::cout << "Process 2 encountered an error. ERROR";
  	}
}

void run_in_background(std::string cmd, std::vector<std::string> args) {
	cmd.pop_back();
	
	pid_t pid;
	pid = fork();
	
	if (pid == 0) {
		handle(cmd);
	}
	else {
		Job job = Job();
		job.pid = pid;
		jobList.push_back(job);
	}
	//while(waitpid(pid, NULL, WEXITED | WNOHANG) > 0) {}
}

void run(std::vector<std::string> args) {
	int status;
	std::string temp = NULL;
	if (args[0][0] == '.') {
		if(access(args[0].c_str(), X_OK) == 0)
      			temp = args[0];
	}
	else {
		temp = get_path(args[0]);
	}
	
	if (temp != "") {
		pid_t pid;
		pid = fork();

		if (pid == 0){
			char* arr[args.size()];
 
         		for (int i = 0; i < args.size(); i++ ) {
                 		char cstr[args[i].size() + 1];
                 		strcpy(cstr, args[i].c_str());
                 		arr[i] = cstr;
         		}

			if (execv(temp.c_str(),arr) < 0) {
				std::cout << "Error running the command\n";
			}
		}
		else if ((waitpid(pid, &status, 0)) == -1) {
			std::cout << "First process encountered error\n";
		}
	}
	else {
		std::cout << "Command not found\n";
	}
}
/*
void ioRedirect(std::string cmd, std::vector<std::string> args) {
	if (strchr(cmd.c_str(), '<') != NULL && strchr(cmd.c_str(), '>') != NULL) {
		int status;
		std::string inFileName;
    		getcwd(inFileName);
    		inFileName.push_back("/");

    		std::string outFileName;
    		getcwd(outFileName);
    		outFileName.push_back("/");
		
		tokenizedCmd = strtok(cmd, " ");

    		char temp[1024] = "";

    		while (tokenizedCmd != NULL) {
      			if (strcmp(tokenizedCmd, "<") == 0) {
        			tokenizedCmd = strtok(NULL, " ");
        			strcat (inFileName, tokenizedCmd);
        			tokenizedCmd = strtok(NULL, " ");
        			tokenizedCmd = strtok(NULL, " ");
        			strcat(outFileName, tokenizedCmd);
        			tokenizedCmd = NULL;
      			}
      			else {
        			strcat(temp, tokenizedCmd);
        			strcat(temp, " ");
        			tokenizedCmd = strtok(NULL, " ");
      			}
    		}
		
		pid_t pid;
		pid = fork();
		if (pid == 0)
    		{
      			freopen(outFileName, "w", stdout);
			freopen(inFileName, "r", stdin);
			handle(temp);
			fclose(stdin);
			fclose(stdout);
      			exit(0);
    		}
    		else if((waitpid(pid, &status, 0)) == -1) {
      			std::cout << "Process 1 encountered an error. ERROR";
    		}
		}
}
*/
void killProcess(std::vector<std::string> args) {
	std::string temp;
	for (int i = 0;  i < args.size(); i++) {
		temp = jobList[i].pid;
		if(args[2] == temp) {
      			if (kill(jobList[i].pid, 0) == 0) {
        			if (kill(jobList[i].pid, strtoumax(args[1].c_str(), NULL, 10)) == -1)
          				std::cout << "Killing encountered an error: ERROR";
        			else
      	  				std::cout << "Killed process " << jobList[i].pid << " " << "(jobid: " << jobList[i].pjobid << ")\n";
      			}
      			else
      				std::cout << "The requested job does not exist.\n";
    			}
  	}
	
}

void handle(std::string cmd) {
	return;
}

void rm_whitespace(std::string cmd) {
	if (cmd.empty()) {
		return;
	}
	cmd.erase(std::remove_if(cmd.begin(), cmd.end(), ::isspace), cmd.end());
}

int main (int argc, char **argv, char **envp) {
	return 0;
}
