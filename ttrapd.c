/* Tiny Trap Daemon */
 
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
        char * filename = "/etc/shadow";
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
 
                /* check every 20 seconds */
                sleep(20);
 
                if (lstat(filename, &st) < 0) {
                        syslog (LOG_ERR, "%s is not accessable", filename);
                } else {
                        if ( (long) st.st_atime != bookmark) {
                                syslog(LOG_ALERT, "ALERT!");
                                syslog(LOG_ALERT, "%ld != %ld %ld", (long) st.st_atime, bookmark, counter);
                                /* send email alert every 5 minutes  */
                                if (counter == 0) {
                                        syslog(LOG_ALERT, "Sending alert notification...");
					/* here you have to specify the email address for alerts */
                                        system ("echo 'Possible integrity violation detected!' | mail -s 'ttrapd ALERT' ttrapd@paranoidsecurity.nl");
                                }
                                /* reset counter after (15 times 20 secs) 5 minutes */
                                if (counter < 15) { counter += 1; } else { counter = 0; }
                        }
                }
        }
 
        exit(EXIT_SUCCESS);
}
