#ifndef QUASH_HPP
#define QUASH_HPP

#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> parse(std::string cmd);

void pwd();

void cd(std::string target);

void flush_jobs();

void jobs();

void echo(std::vector<std::string> args);

void set(std::vector<std::string> args);

std::string get_path(std::string cmd);

void pipe(std::string args);

void run_in_background(std::string cmd, std::vector<std::string> args);

void run(std::vector<std::string> args);

void ioRedirect(std::string cmd, std::vector<std::string> args);

void killProcess(std::vector<std::string> args);

void handle(std::string cmd);

void rm_whitespace(std::string &cmd);

#endif
