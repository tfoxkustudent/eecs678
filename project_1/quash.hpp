#ifndef QUASH_HPP
#define QUASH_HPP

#include <iostream>

struct job {
	int jobid;
	pid_t pid;
	char *cmd;
};

bool is_running();

void terminate();

char** parse(char* cmd, int* numCmds);

void pwd();

void cd(char* target);

void flush_jobs();

void jobs();

void echo(char** args, int argCount);

void set(char** args, int argCount);

char* get_path(char* cmd);

void pipe(char* args, int argCount);

void run_in_background(char* cmd, char** args, int argCount);

void run(char** args, int argCount);

void ioRedirect(char *cmd, char **args, int argCount);

void killProcess(char** args, int argCount);

void handle(char* cmd);

void rm_whitespace(char* cmd);

#endif
