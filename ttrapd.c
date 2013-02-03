/* Tiny Trap Daemon - Bugs by Niels Koster */
 
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <strings.h>
 
int main(int argc, char *argv[]) {
 
        struct stat st;
 
        /* here you have to specify the trap file */
        char * filename = "/etc/ssl/private/root.key";
        long bookmark;
 
        if (lstat(filename, &st) < 0) {
                printf("%s is not accessable", filename);
                exit(EXIT_FAILURE);
        } else {
                bookmark = (long) st.st_atime;
        }
 
        pid_t pid, sid;
 
        /* fork into background */
        pid = fork();
 
        if (pid < 0) {
                exit(EXIT_FAILURE);
        } else {
                if (pid > 0) {
                        exit(EXIT_SUCCESS);
                }
        }
 
        umask(0);
        sid = setsid();
 
        if (sid < 0) {
                exit(EXIT_FAILURE);
        }
 
        syslog (LOG_NOTICE, "[%s] loaded into background by (uid=%d, gid=%d)", argv[0], getuid(), getgid());
 
        printf("\nTiny Trap Daemon\n");
        printf("\n%s loaded into background by (uid=%d, gid=%d)\n\n", argv[0], getuid(), getgid());
 
        if ((chdir("/")) < 0) {
                exit(EXIT_FAILURE);
        }
 
        long counter = 0;
 
        while (1) {
 
                /* test every 10 seconds */
                sleep(10);
 
                if (lstat(filename, &st) < 0) {
                        syslog (LOG_ERR, "%s is not accessable", filename);
                } else {
                        if ( (long) st.st_atime != bookmark) {
                                /* send alerts every 5 minutes  */
                                if (counter == 0) {
	                                syslog(LOG_ALERT, "ALERT!");
        	                        syslog(LOG_ALERT, "%ld != %ld %ld", (long) st.st_atime, bookmark, counter);
                                        syslog(LOG_ALERT, "Sending alert notification...");
					/* here you have to adapt the alert email address (and maybe even more..) */
                                        system ("lsof > /tmp/ttrapd ; netstat -n >> /tmp/ttrapd ; cat /tmp/ttrapd | mail -s 'ttrapd ALERT' important@example.com ; rm -f /tmp/ttrapd");
                                } 
                                /* reset counter after (30 times 10 secs) 5 minutes */
                                if (counter < 30) { counter += 1; } else { counter = 0; }
                        }
                }
        }
 
        exit(EXIT_SUCCESS);
}
