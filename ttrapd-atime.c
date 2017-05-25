/*
     Tiny Trap Daemon - Bugs by Niels Koster
     Do not forget to adjust the TTRAPD_FILE
*/

#define TTRAPD_FILE "/root/.ssh/id_rsa"
#define TTRAPD_SYSTEM_CALL "(ps axuwww;echo;netstat -n;echo;lsof 2>&1)|gzip -9|base64|mail -s 'Daily Cron' root >/dev/null 2>&1"
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
        long bookmark;
        if (lstat(TTRAPD_FILE, &st) < 0) {
                printf("%s is not accessable", TTRAPD_FILE);
                exit(EXIT_FAILURE);
        } else {
                bookmark = (long) st.st_atime;
        }
        pid_t pid, sid;
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
        syslog(LOG_NOTICE, "%s forked by uid=%d, gid=%d", argv[0], getuid(), getgid());
        if ((chdir("/")) < 0) {
                exit(EXIT_FAILURE);
        }
        long counter = 0;
        while (1) {
                /* test every 10 seconds */
                sleep(10);
                if (lstat(TTRAPD_FILE, &st) < 0) {
                        syslog (LOG_ERR, "%s is not accessable", TTRAPD_FILE);
                } else {
                        if ( (long) st.st_atime != bookmark) {
                                if (counter == 0) {
                                        syslog(LOG_ALERT, "Success: %ld != %ld", (long) st.st_atime, bookmark);
                                        system(TTRAPD_SYSTEM_CALL);
                                } 
                                /* reset counter after (30 times 10 secs) 5 minutes */
                                if (counter < 30) { counter += 1; } else { counter = 0; }
                        }
                }
        }
        exit(EXIT_SUCCESS);
}