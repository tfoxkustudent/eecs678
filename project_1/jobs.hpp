#ifndef JOBS_HPP
#define JOBS_HPP
#include "unistd.h"
#include <string>
#include <vector>
class Job{

private:
	int jobid = 0;
	
        
public:
	int pjobid;
	pid_t pid;
       	std::vector<std::string>* cmd;
	bool running;

	Job() {
		pjobid = jobid;
		jobid++;
		running = true;
	}
	~Job() {
		delete cmd;
	}
	
	void start() {
        	running = true;
	}

	bool is_running() {
        	return running;
	}

	void terminate() {
        	running = false;
	}
};

#endif
