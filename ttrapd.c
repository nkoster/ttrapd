/*

   Tiny Trap Daemon - Bugs by Niels Koster

*/

// DO NOT FORGET TO CHANGE THE system() CALL BELOW!!!

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/types.h>

#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

#define EXIT_INOTIFY 600

int main(int argc, char *argv[])
{
    pid_t pid, sid;
    pid = fork();
    long counter = 0;
    int length, i = 0;
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];
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
    printf("\nAt your service,\n");
    printf("\n%s loaded into background by (uid=%d, gid=%d)\n\n", argv[0], getuid(), getgid());
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }
    fd = inotify_init();
    if (fd < 0) {
        exit(EXIT_FAILURE);
    }
    wd = inotify_add_watch(fd, "/", IN_ACCESS);
    length = read(fd, buffer, EVENT_BUF_LEN);
    if (length < 0) {
        exit(EXIT_FAILURE);
    }
    // Just respond to all events.
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];
        i += EVENT_SIZE + event->len;
    }
    inotify_rm_watch(fd, wd);
    close(fd);
    // Extremely suspicious: collect evidence, do the system() call.
    syslog(LOG_ALERT, "Please make well-considered decisions.");
    system("(ps faxuwww; echo; netstat -n; echo) >/tmp/ttrapd ; cat /tmp/ttrapd | mail -s 'ttrapd ALERT' someone@example.com");
    sleep(2);
    // Execute myself (or even something else!) and exit this process.
    system(argv[0]);
    exit(EXIT_INOTIFY);
}
