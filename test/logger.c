#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "ics-common.h"

int main() {
	int fdm;
	int rc;
	int n;
	char buffer[10];
	fd_set readset;
	struct timeval timeout;
	char pts_name[50];
	int len;

	fdm = posix_openpt(O_RDWR);
	ICS_EXIT_IF_TRUE(fdm < 0, "Cannot create pseudo-terminal master\n");

	rc = grantpt(fdm);
	ICS_EXIT_IF_TRUE(rc != 0, "Cannot grantpt (change access right)\n");

	rc = unlockpt(fdm);
	ICS_EXIT_IF_TRUE(rc != 0, "Cannot unlockpt (unlock slave side)\n");

	bzero(pts_name, sizeof(pts_name));
	len = ptsname_r(fdm, pts_name, sizeof(pts_name));
	printf("Pseudo terminal file: %s\n", pts_name);

	timeout.tv_sec = 0;
	timeout.tv_usec = 10*1000;
	while(1) {
		FD_ZERO(&readset);
		FD_SET(fdm, &readset);
		rc = select(fdm + 1, &readset, NULL, NULL, &timeout);
		if( rc > 0 ) {
			if( FD_ISSET(fdm, &readset) ) {
				n = read(fdm, buffer, sizeof(buffer));
				buffer[n] = '\0';
				printf("%s",buffer);
			}
		}
		usleep(100*1000);
	}
	return 0;
}
