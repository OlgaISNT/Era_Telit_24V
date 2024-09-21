#include "softdog.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "files.h"
#include "log.h"


#define NAMEDPIPE_NAME "/tmp/softdog_pipe"

static int softdog_fd;


UINT8 initSoftdog(int timeout_ms) {
	softdog_fd = 0;
	//LOG_DEBUG("initSoftdog");
	if (mkfifo(NAMEDPIPE_NAME, 0777) ) {
		//LOG_CRITICAL("can't create pipe");
		//return ERR_SOFTDOG_UNABLE_CREATE_PIPE;
	}
	int pid_ = fork();
	//LOG_DEBUG("fork ok %d", pid_);
	if (pid_ == -1) {
		//LOG_CRITICAL("can't fork");
		return ERR_SOFTDOG_UNABLE_FORK;
	}
	if (pid_ == 0) {
		char path[] = "/data/azc/mod/softdog";
		char file[128];
		memset(file, 0, 128);
		sprintf(file, "%s", path);
		int size = get_file_size(file);
		//LOG_INFO("file:%s, size:%i", file, size);
		if (size == -1) {
			char *e;
			e = file + strlen(path);
			strcpy(e, ".bin");
			size = get_file_size(file);
			//LOG_INFO("file:%s, size:%i", file, size);
		}
		if (size == -1) return ERR_NOT_SOFTDOG_FILE;
		else {
			char data[128];
			memset(data,0,128);
			sprintf(data, "%d", timeout_ms);
			char *argv[] = { file, NAMEDPIPE_NAME, data, NULL }; // "/data/azc/mod/softdog"
			char *environ[] = { NULL };
			int r = execve(argv[0], argv, environ);
			LOG_INFO("sd execve r:%i", r);
			abort();
		}
	} else {
		softdog_fd = open(NAMEDPIPE_NAME, O_WRONLY);
		if (!softdog_fd) {
			//LOG_CRITICAL("can't open pipe");
			return ERR_SOFTDOG_UNABLE_OPEN_PIPE;
		}
	}
	return ERR_SOFTDOG_NO_ERROR;
}


void resetSoftdog(void) {
	//LOG_DEBUG("resetSoftdog");
	int x = 0;
	if (softdog_fd) write(softdog_fd, &x, sizeof(x));
}
