#ifndef JOBS_HPP
#define JOBS_HPP
#include "unistd.h"
#include <string>
#include <vector>
class Job{

private:
	
        
public:
	int pjobid;
	pid_t pid;
	bool job_running;
	std::string cmd;
	Job(std::string cmd, int pjobid) {
		this->pjobid = pjobid ;
		job_running = true;
		this->cmd = cmd;
	}
	Job(): Job("", 0) {
	}
	
	bool is_running() {
        	return job_running;
	}

	void terminate() {
        	job_running = false;
	}
	
};

#endif
